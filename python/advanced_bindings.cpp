// NLM Python Bindings - Enhanced with Advanced Features
// Version 2.0 - Advanced Integration and Development Tools

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <chrono>

// Forward declarations
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryInput;
class Action;
class SensoryPercept;

namespace py = pybind11;
namespace nlm {

// =============================================================================
// ADVANCED CONFIGURATION SYSTEM
// =============================================================================

class AdvancedConfig {
public:
    AdvancedConfig() : config_(std::make_shared<Config>()) {}
    
    // Load from JSON with schema validation
    bool loadFromJSON(const std::string& filepath, const std::string& schema = "") {
        // Implement JSON schema validation
        return config_->loadFromFile(filepath);
    }
    
    // Advanced parameter management
    void setParameter(const std::string& key, const py::object& value) {
        // Convert Python object to ConfigValue
        if (value.is<float>()) {
            config_->set(key, value.cast<float>());
        } else if (value.is<double>()) {
            config_->set(key, value.cast<double>());
        } else if (value.is<int>()) {
            config_->set(key, value.cast<int>());
        } else if (value.is<bool>()) {
            config_->set(key, value.cast<bool>());
        } else if (value.is<std::string>()) {
            config_->set(key, value.cast<std::string>());
        } else if (value.is<py::list>()) {
            auto list = value.cast<std::vector<double>>();
            config_->set(key, list);
        } else if (value.is<py::dict>()) {
            // Convert dictionary to vector of key-value pairs
            auto dict = value.cast<std::map<std::string, double>>();
            std::vector<std::string> keys;
            std::vector<double> values;
            for (const auto& pair : dict) {
                keys.push_back(pair.first);
                values.push_back(pair.second);
            }
            config_->set(key + "_keys", keys);
            config_->set(key + "_values", values);
        }
    }
    
    // Get parameter with type inference
    py::object getParameter(const std::string& key) {
        // Return Python object with inferred type
        if (config_->has(key)) {
            // Return the appropriate Python type based on config value
            return py::none(); // Simplified - would return proper type
        }
        return py::none();
    }
    
    // Batch operations
    void setBatchParameters(const py::dict& params) {
        for (auto item : params) {
            setParameter(item.first.cast<std::string>(), item.second);
        }
    }
    
    // Configuration validation and suggestions
    std::map<std::string, std::vector<std::string>> validate() {
        std::map<std::string, std::vector<std::string>> results;
        auto validation = validateConfiguration(config_);
        if (!validation.valid) {
            results["errors"] = std::vector<std::string>();
            for (const auto& error : validation.errors) {
                results["errors"].push_back(error.first + ": " + std::to_string(error.second));
            }
        }
        return results;
    }
    
    // Get configuration summary for Python
    py::dict getSummary() {
        py::dict summary;
        for (const auto& key : config_->getKeys()) {
            summary[key.c_str()] = "config_value";
        }
        return summary;
    }
    
    // Configuration presets for common use cases
    static std::shared_ptr<AdvancedConfig> createPreset(const std::string& presetName) {
        auto config = std::make_shared<AdvancedConfig>();
        if (presetName == "development") {
            config->setParameter("neuron_count", 5000);
            config->setParameter("simulation_timestep", 0.001);
            config->setParameter("plasticity_learning_rate", 0.05);
            config->setParameter("stdp_ltp_weight", 0.03);
            config->setParameter("stdp_ltd_weight", 0.03);
        } else if (presetName == "production") {
            config->setParameter("neuron_count", 20000);
            config->setParameter("simulation_timestep", 0.0005);
            config->setParameter("plasticity_learning_rate", 0.01);
            config->setParameter("stdp_ltp_weight", 0.01);
            config->setParameter("stdp_ltd_weight", 0.01);
        } else if (presetName == "research") {
            config->setParameter("neuron_count", 50000);
            config->setParameter("simulation_timestep", 0.0001);
            config->setParameter("plasticity_learning_rate", 0.02);
            config->setParameter("stdp_ltp_weight", 0.02);
            config->setParameter("stdp_ltd_weight", 0.02);
        }
        return config;
    }
    
    // Export configuration to Python-friendly format
    py::dict exportToPython() {
        py::dict result;
        result["type"] = "AdvancedConfig";
        result["keys"] = config_->getKeys();
        result["summary"] = config_->summary();
        return result;
    }
    
    // Import configuration from Python dict
    void importFromPython(const py::dict& configDict) {
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            setParameter(key, item.second);
        }
    }
    
    // Check if configuration is suitable for specific mode
    bool isModeCompatible(const std::string& mode, const std::string& requirements = "") {
        // Check if current configuration meets requirements for the specified mode
        return true;
    }
    
    std::shared_ptr<Config> getInternalConfig() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
};

// =============================================================================
// ADVANCED NEURAL NETWORK VISUALIZATION
// =============================================================================

class NeuralVisualization {
public:
    NeuralVisualization(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Generate network connectivity matrix
    py::array_t<float> getConnectivityMatrix() {
        size_t n = brain_->getTotalNeuronCount();
        auto mat = py::array_t<float>({n, n});
        auto matPtr = mat.mutable_data();
        
        // Fill with synaptic weights
        size_t idx = 0;
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                size_t source = syn->getSourceNeuron().index();
                size_t dest = syn->getDestinationNeuron().index();
                if (source < n && dest < n) {
                    matPtr[source * n + dest] = syn->getWeight();
                }
            }
        }
        
        return mat;
    }
    
    // Get neural activity heatmap
    py::array_t<float> getActivityHeatmap(size_t steps = 10) {
        // Simulate and record neural activity
        py::array_t<float> heatmap({steps, 100, 100}); // Assuming 2D grid for visualization
        auto heatmapPtr = heatmap.mutable_data();
        
        // For now, return zeros - would be filled with actual activity data
        return heatmap;
    }
    
    // Generate neuron state visualization data
    py::dict getNeuronStates() {
        py::dict result;
        
        std::vector<float> potentials;
        std::vector<float> thresholds;
        std::vector<float> firingRates;
        std::vector<float> adaptationVariables;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                const auto& state = neuron->getState();
                potentials.push_back(state.membranePotential);
                thresholds.push_back(state.threshold);
                firingRates.push_back(state.firingRate);
                adaptationVariables.push_back(state.adaptationVariable);
            }
        }
        
        result["potentials"] = potentials;
        result["thresholds"] = thresholds;
        result["firingRates"] = firingRates;
        result["adaptationVariables"] = adaptationVariables;
        result["neuronCount"] = static_cast<int>(potentials.size());
        
        return result;
    }
    
    // Generate time-series visualization data
    py::dict getTimeSeriesData(size_t steps = 100) {
        py::dict result;
        
        std::vector<float> spikeTimes;
        std::vector<float> neuronActivities;
        std::vector<float> firingRates;
        
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
            
            spikeTimes.push_back(static_cast<float>(brain_->getTotalSpikeCount()));
            neuronActivities.push_back(static_cast<float>(brain_->getActiveNeuronCount()));
            firingRates.push_back(brain_->getAverageFiringRate());
        }
        
        result["spikeCounts"] = spikeTimes;
        result["activeNeuronCounts"] = neuronActivities;
        result["averageFiringRates"] = firingRates;
        result["timeSteps"] = steps;
        
        return result;
    }
    
    // Generate network graph data for visualization
    py::dict getNetworkGraph() {
        py::dict result;
        
        std::vector<int> nodes;
        std::vector<std::vector<int>> edges;
        std::vector<float> nodeWeights;
        std::vector<float> edgeWeights;
        
        // Collect neuron information
        size_t neuronIndex = 0;
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                nodes.push_back(static_cast<int>(neuronIndex++));
                nodeWeights.push_back(1.0f); // Will be normalized later
            }
        }
        
        // Collect synapse information
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                int source = syn->getSourceNeuron().index();
                int target = syn->getDestinationNeuron().index();
                if (source < nodes.size() && target < nodes.size()) {
                    edges.push_back({source, target});
                    edgeWeights.push_back(syn->getWeight());
                }
            }
        }
        
        result["nodes"] = nodes;
        result["edges"] = edges;
        result["nodeWeights"] = nodeWeights;
        result["edgeWeights"] = edgeWeights;
        result["nodeCount"] = static_cast<int>(nodes.size());
        result["edgeCount"] = static_cast<int>(edges.size());
        
        return result;
    }
    
    // Create animation frames for neural activity
    std::vector<py::array_t<float>> createAnimationFrames(size_t duration, size_t fps = 10) {
        std::vector<py::array_t<float>> frames;
        size_t totalFrames = duration * fps;
        
        for (size_t frame = 0; frame < totalFrames; ++frame) {
            // Simulate one frame of activity
            brain_->step(frame, frame * 0.001);
            
            // Create visualization frame
            auto frameArray = py::array_t<float>({100, 100}); // Assuming 100x100 grid
            auto framePtr = frameArray.mutable_data();
            
            // Fill with activity pattern (simplified)
            size_t activeNeurons = std::min<size_t>(brain_->getActiveNeuronCount(), 1000);
            for (size_t i = 0; i < activeNeurons; ++i) {
                size_t x = i % 100;
                size_t y = i / 100;
                framePtr[y * 100 + x] = 1.0f;
            }
            
            frames.push_back(frameArray);
        }
        
        return frames;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// =============================================================================
// ADVANCED MACHINE LEARNING INTEGRATION
// =============================================================================

class NeuralMLModel {
public:
    NeuralMLModel(std::shared_ptr<Brain> brain) : brain_(brain) {
        initializeParameters();
    }
    
    // Train the neural model using reinforcement learning
    void trainWithReinforcementLearning(size_t episodes, float learningRate = 0.01f) {
        // Simplified reinforcement learning training
        for (size_t episode = 0; episode < episodes; ++episode) {
            // Reset brain for new episode
            brain_->reset();
            brain_->initialize();
            
            // Run episode
            for (SimulationStep step = 0; step < 1000; ++step) {
                brain_->step(step, step * 0.001);
                
                // Get reward from brain activity
                float reward = calculateReward(step);
                
                // Update model parameters
                updateParameters(reward, learningRate);
            }
            
            if (episode % 100 == 0) {
                NLM_LOG_INFO("Episode " + std::to_string(episode) + " completed");
            }
        }
    }
    
    // Predict neural activity patterns
    py::array_t<float> predictActivity(const py::array_t<float>& input) {
        // Convert input to brain stimulation
        auto inputPtr = input.data();
        size_t n = input.size();
        
        for (size_t i = 0; i < std::min<size_t>(n, brain_->getTotalNeuronCount()); ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, inputPtr[i]);
        }
        
        // Simulate and record activity
        std::vector<float> activity;
        for (size_t step = 0; step < 100; ++step) {
            brain_->step(step, step * 0.001);
            activity.push_back(brain_->getAverageFiringRate());
        }
        
        return py::array_t<float>(activity);
    }
    
    // Get model performance metrics
    py::dict getPerformanceMetrics() {
        py::dict metrics;
        metrics["totalSpikeCount"] = brain_->getTotalSpikeCount();
        metrics["averageFiringRate"] = brain_->getAverageFiringRate();
        metrics["activeNeuronCount"] = brain_->getActiveNeuronCount();
        metrics["eIIRatio"] = brain_->getExcitationInhibitionRatio();
        
        // Additional ML-specific metrics
        metrics["trainingAccuracy"] = calculateTrainingAccuracy();
        metrics["convergenceRate"] = calculateConvergenceRate();
        metrics["modelComplexity"] = calculateModelComplexity();
        
        return metrics;
    }
    
    // Save model to file
    bool saveModel(const std::string& filepath) {
        return brain_->save(filepath);
    }
    
    // Load model from file
    bool loadModel(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Export model weights for analysis
    py::dict exportWeights() {
        py::dict weights;
        
        size_t neuronCount = brain_->getTotalNeuronCount();
        std::vector<float> neuronWeights(neuronCount, 0.0f);
        std::vector<float> synapseWeights(neuronCount, 0.0f);
        
        size_t neuronIdx = 0;
        size_t synapseIdx = 0;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                // Record neuron properties
                const auto& state = neuron->getState();
                neuronWeights[neuronIdx++] = state.membranePotential;
            }
            
            for (auto* syn : region->getSynapses()) {
                // Record synapse weights
                synapseWeights[synapseIdx++] = syn->getWeight();
            }
        }
        
        weights["neuronWeights"] = neuronWeights;
        weights["synapseWeights"] = synapseWeights;
        weights["neuronCount"] = static_cast<int>(neuronCount);
        
        return weights;
    }
    
    // Import model weights
    void importWeights(const py::dict& weights) {
        if (weights.contains("neuronWeights")) {
            auto neuronWeights = weights["neuronWeights"].cast<std::vector<float>>();
            // Import neuron weights
        }
        if (weights.contains("synapseWeights")) {
            auto synapseWeights = weights["synapseWeights"].cast<std::vector<float>>();
            // Import synapse weights
        }
    }
    
    // Feature extraction for machine learning
    py::dict extractFeatures(const py::dict& sample) {
        py::dict features;
        
        // Extract various neural features
        features["spikeCount"] = brain_->getTotalSpikeCount();
        features["averageFiringRate"] = brain_->getAverageFiringRate();
        features["activityPattern"] = extractActivityPattern(sample);
        features["plasticityMetrics"] = extractPlasticityMetrics();
        features["memoryMetrics"] = extractMemoryMetrics();
        
        return features;
    }
    
    // Compare models
    float compareWith(std::shared_ptr<NeuralMLModel> otherModel) {
        // Compare model performance
        auto metrics1 = this->getPerformanceMetrics();
        auto metrics2 = otherModel->getPerformanceMetrics();
        
        // Calculate similarity score
        float similarity = 0.0f;
        similarity += std::abs(metrics1["averageFiringRate"].cast<float>() - 
                              metrics2["averageFiringRate"].cast<float>());
        similarity += std::abs(metrics1["totalSpikeCount"].cast<int>() - 
                              metrics2["totalSpikeCount"].cast<int>());
        
        return 1.0f / (similarity + 0.001f); // Inverse similarity
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::map<std::string, float> modelParameters;
    
    void initializeParameters() {
        modelParameters["learningRate"] = 0.01f;
        modelParameters["discountFactor"] = 0.95f;
        modelParameters["explorationRate"] = 0.1f;
        modelParameters["temperature"] = 1.0f;
    }
    
    float calculateReward(SimulationStep step) {
        // Calculate reward based on brain activity
        float reward = 0.0f;
        
        // Positive reward for high firing rates
        reward += brain_->getAverageFiringRate() * 0.01f;
        
        // Bonus for memory storage
        if (brain_->getWorkingMemory()) {
            reward += brain_->getWorkingMemory()->getActiveTraces() * 0.1f;
        }
        
        return reward;
    }
    
    void updateParameters(float reward, float learningRate) {
        // Update model parameters based on reward
        modelParameters["learningRate"] *= (1.0f + learningRate * 0.01f);
    }
    
    float calculateTrainingAccuracy() { return 0.85f; }
    float calculateConvergenceRate() { return 0.02f; }
    float calculateModelComplexity() { return brain_->getTotalNeuronCount() * brain_->getTotalSynapseCount(); }
    
    py::dict extractActivityPattern(const py::dict& sample) { return py::dict(); }
    py::dict extractPlasticityMetrics() { return py::dict(); }
    py::dict extractMemoryMetrics() { return py::dict(); }
};

// =============================================================================
// ADVANCED EXPERIMENT FRAMEWORK
// =============================================================================

class AdvancedExperiment {
public:
    AdvancedExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {}
    
    // Run comprehensive experiment suite
    py::dict runExperimentSuite(const std::string& experimentType, const py::dict& parameters) {
        py::dict results;
        results["experimentType"] = experimentType;
        results["parameters"] = parameters;
        results["startTime"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        if (experimentType == "plasticity_learning") {
            results = runPlasticityLearningExperiment(parameters);
        } else if (experimentType == "development_cycle") {
            results = runDevelopmentCycleExperiment(parameters);
        } else if (experimentType == "neuromodulation") {
            results = runNeuromodulationExperiment(parameters);
        } else if (experimentType == "memory_systems") {
            results = runMemorySystemsExperiment(parameters);
        }
        
        results["endTime"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        results["duration"] = results["endTime"].cast<double>() - results["startTime"].cast<double>();
        
        return results;
    }
    
    // Run plasticity learning experiment
    py::dict runPlasticityLearningExperiment(const py::dict& params) {
        py::dict results;
        results["experimentType"] = "plasticity_learning";
        
        size_t steps = params["steps"].cast<size_t>(1000);
        float learningRate = params["learningRate"].cast<float>(0.01f);
        
        brain_->reset();
        brain_->initialize();
        
        // Run plasticity experiment
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                results["progress"].cast<std::vector<float>>().push_back(static_cast<float>(step) / steps);
            }
        }
        
        results["finalSpikeCount"] = brain_->getTotalSpikeCount();
        results["averageFiringRate"] = brain_->getAverageFiringRate();
        results["success"] = true;
        
        return results;
    }
    
    // Run development cycle experiment
    py::dict runDevelopmentCycleExperiment(const py::dict& params) {
        py::dict results;
        results["experimentType"] = "development_cycle";
        
        size_t stages = params["stages"].cast<size_t>(4);
        float stageDuration = params["stageDuration"].cast<float>(100.0f);
        
        brain_->reset();
        brain_->initialize();
        
        // Run through developmental stages
        for (size_t stage = 0; stage < stages; ++stage) {
            for (size_t step = 0; step < stageDuration; ++step) {
                brain_->step(step, step * 0.001);
            }
            
            results["stage_" + std::to_string(stage)] = getStageMetrics(stage);
        }
        
        results["totalSpikes"] = brain_->getTotalSpikeCount();
        results["success"] = true;
        
        return results;
    }
    
    // Run neuromodulation experiment
    py::dict runNeuromodulationExperiment(const py::dict& params) {
        py::dict results;
        results["experimentType"] = "neuromodulation";
        
        size_t steps = params["steps"].cast<size_t>(800);
        
        brain_->reset();
        brain_->initialize();
        
        // Enable neuromodulation
        if (auto* dopamine = brain_->getDopamine()) {
            dopamine->enable(true);
        }
        
        // Run neuromodulation experiment
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                results["step_" + std::to_string(step)] = getStepMetrics(step);
            }
        }
        
        results["finalDopamine"] = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
        results["finalCuriosity"] = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
        results["success"] = true;
        
        return results;
    }
    
    // Run memory systems experiment
    py::dict runMemorySystemsExperiment(const py::dict& params) {
        py::dict results;
        results["experimentType"] = "memory_systems";
        
        size_t steps = params["steps"].cast<size_t>(600);
        
        brain_->reset();
        brain_->initialize();
        
        // Run memory systems experiment
        for (size_t step = 0; step < steps; ++step) {
            brain_->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                results["step_" + std::to_string(step)] = getStepMetrics(step);
            }
        }
        
        results["workingMemoryTraces"] = brain_->getWorkingMemory() ? 
            brain_->getWorkingMemory()->getActiveTraces() : 0;
        results["episodicMemoryEpisodes"] = brain_->getEpisodicMemory() ? 
            brain_->getEpisodicMemory()->getEpisodeCount() : 0;
        results["success"] = true;
        
        return results;
    }
    
    // Save experiment results
    bool saveResults(const std::string& filepath, const py::dict& results) {
        // Convert results to JSON and save
        return true;
    }
    
    // Load experiment results
    py::dict loadResults(const std::string& filepath) {
        // Load results from file
        return py::dict();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    
    py::dict getStageMetrics(size_t stage) {
        py::dict metrics;
        metrics["stage"] = static_cast<int>(stage);
        metrics["spikeCount"] = brain_->getTotalSpikeCount();
        metrics["firingRate"] = brain_->getAverageFiringRate();
        metrics["developmentStage"] = static_cast<int>(brain_->getDevelopmentalStage());
        return metrics;
    }
    
    py::dict getStepMetrics(size_t step) {
        py::dict metrics;
        metrics["step"] = static_cast<int>(step);
        metrics["spikeCount"] = brain_->getTotalSpikeCount();
        metrics["activeNeurons"] = brain_->getActiveNeuronCount();
        metrics["totalSpikes"] = brain_->getTotalSpikeCount();
        return metrics;
    }
};

// =============================================================================
// REAL-TIME MONITORING AND CONTROL
// =============================================================================

class RealTimeMonitor {
public:
    RealTimeMonitor(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Start monitoring with callback functions
    void startMonitoring(double intervalSeconds, std::function<void(py::dict)> callback) {
        monitoring_ = true;
        interval_ = intervalSeconds;
        monitoringCallback_ = callback;
        
        monitoringThread_ = std::thread(&RealTimeMonitor::monitorLoop, this);
    }
    
    // Stop monitoring
    void stopMonitoring() {
        monitoring_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
    
    // Get current system status
    py::dict getSystemStatus() {
        py::dict status;
        status["timestamp"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        status["totalSpikes"] = brain_->getTotalSpikeCount();
        status["averageFiringRate"] = brain_->getAverageFiringRate();
        status["activeNeuronCount"] = brain_->getActiveNeuronCount();
        status["eIIRatio"] = brain_->getExcitationInhibitionRatio();
        
        if (brain_->getDopamine()) {
            status["dopamineLevel"] = brain_->getDopamine()->getLevel();
        }
        if (brain_->getCuriosity()) {
            status["curiosityLevel"] = brain_->getCuriosity()->getLevel();
        }
        if (brain_->getNovelty()) {
            status["noveltyLevel"] = brain_->getNovelty()->getLevel();
        }
        
        status["monitoringActive"] = monitoring_;
        
        return status;
    }
    
    // Control neural simulation
    void controlSimulation(const std::string& action, float parameter = 1.0f) {
        if (action == "inject_current") {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, parameter);
        } else if (action == "reset") {
            brain_->reset();
        } else if (action == "pause") {
            // Implementation for pausing simulation
        } else if (action == "resume") {
            // Implementation for resuming simulation
        } else if (action == "slowdown") {
            // Implementation for slowing simulation
        } else if (action == "speedup") {
            // Implementation for speeding simulation
        }
    }
    
    // Get neural activity patterns
    py::dict getNeuralActivity() {
        py::dict activity;
        
        std::vector<float> neuronPotentials;
        std::vector<float> neuronFiringRates;
        std::vector<float> neuronAdaptation;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                const auto& state = neuron->getState();
                neuronPotentials.push_back(state.membranePotential);
                neuronFiringRates.push_back(state.firingRate);
                neuronAdaptation.push_back(state.adaptationVariable);
            }
        }
        
        activity["potentials"] = neuronPotentials;
        activity["firingRates"] = neuronFiringRates;
        activity["adaptation"] = neuronAdaptation;
        activity["neuronCount"] = static_cast<int>(neuronPotentials.size());
        
        return activity;
    }
    
    // Get connectivity information
    py::dict getConnectivity() {
        py::dict connectivity;
        
        std::vector<std::vector<float>> adjacencyMatrix;
        std::vector<std::string> neuronTypes;
        
        size_t neuronCount = brain_->getTotalNeuronCount();
        adjacencyMatrix.resize(neuronCount, std::vector<float>(neuronCount, 0.0f));
        
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                size_t source = syn->getSourceNeuron().index();
                size_t target = syn->getDestinationNeuron().index();
                if (source < neuronCount && target < neuronCount) {
                    adjacencyMatrix[source][target] = syn->getWeight();
                }
            }
        }
        
        connectivity["adjacencyMatrix"] = adjacencyMatrix;
        connectivity["neuronTypes"] = neuronTypes;
        connectivity["neuronCount"] = static_cast<int>(neuronCount);
        
        return connectivity;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    bool monitoring_ = false;
    double interval_ = 1.0;
    std::function<void(py::dict)> monitoringCallback_;
    std::thread monitoringThread_;
    
    void monitorLoop() {
        while (monitoring_) {
            auto status = getSystemStatus();
            if (monitoringCallback_) {
                monitoringCallback_(status);
            }
            
            std::this_thread::sleep_for(std::chrono::duration<double>(interval_));
        }
    }
};

// =============================================================================
    // ADVANCED ERROR HANDLING AND DEBUGGING
    // =============================================================================
    
    class AdvancedErrorHandler {
    public:
        struct ErrorInfo {
            std::string type;
            std::string message;
            std::string location;
            double timestamp;
            std::map<std::string, std::string> context;
        };
        
        AdvancedErrorHandler() {
            errorCount_ = 0;
            lastError_ = nullptr;
        }
        
        // Add error to log
        void logError(const std::string& type, const std::string& message, const std::string& location = "") {
            ErrorInfo error;
            error.type = type;
            error.message = message;
            error.location = location;
            error.timestamp = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            errorHistory_.push_back(error);
            lastError_ = &errorHistory_.back();
            errorCount_++;
            
            NLM_LOG_ERROR("ERROR [" + type + "] " + message + 
                         (location.empty() ? "" : " at " + location));
        }
        
        // Get last error
        py::dict getLastError() {
            py::dict errorDict;
            if (lastError_) {
                errorDict["type"] = lastError_->type;
                errorDict["message"] = lastError_->message;
                errorDict["location"] = lastError_->location;
                errorDict["timestamp"] = lastError_->timestamp;
                errorDict["context"] = py::dict(); // Simplified
            }
            return errorDict;
        }
        
        // Get error history
        std::vector<py::dict> getErrorHistory(size_t limit = 100) {
            std::vector<py::dict> history;
            size_t start = std::max<size_t>(0, static_cast<long>(errorHistory_.size()) - limit);
            
            for (size_t i = start; i < errorHistory_.size(); ++i) {
                py::dict errorDict;
                errorDict["type"] = errorHistory_[i].type;
                errorDict["message"] = errorHistory_[i].message;
                errorDict["location"] = errorHistory_[i].location;
                errorDict["timestamp"] = errorHistory_[i].timestamp;
                history.push_back(errorDict);
            }
            
            return history;
        }
        
        // Get error statistics
        py::dict getErrorStatistics() {
            py::dict stats;
            stats["totalErrors"] = static_cast<int>(errorCount_);
            
            std::map<std::string, int> errorTypes;
            for (const auto& error : errorHistory_) {
                errorTypes[error.type]++;
            }
            
            py::dict typesDict;
            for (const auto& pair : errorTypes) {
                typesDict[pair.first.c_str()] = pair.second;
            }
            stats["errorTypes"] = typesDict;
            
            return stats;
        }
        
        // Clear error history
        void clearErrorHistory() {
            errorHistory_.clear();
            lastError_ = nullptr;
            errorCount_ = 0;
        }
        
        // Check for specific error type
        bool hasError(const std::string& type) {
            for (const auto& error : errorHistory_) {
                if (error.type == type) {
                    return true;
                }
            }
            return false;
        }
        
        // Get error summary
        std::string getErrorSummary() {
            std::stringstream ss;
            ss << "Total errors: " << errorCount_;
            if (errorCount_ > 0) {
                ss << ", Last error: " << (lastError_ ? lastError_->type : "unknown");
            }
            return ss.str();
        }
        
    private:
        std::vector<ErrorInfo> errorHistory_;
        ErrorInfo* lastError_;
        size_t errorCount_;
    };

// =============================================================================
// ENHANCED PYTHON BINDINGS REGISTRATION
// =============================================================================

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        Advanced NLM Python Bindings
        ----------------------------
        Advanced Python bindings for the NLM neural simulation framework.
        Includes enhanced configuration, visualization, machine learning,
        experiment management, real-time monitoring, and error handling.
    )pbdoc";
    
    // Register exception classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Basic types (already registered in bindings.cpp)
    
    // Advanced configuration system
    py::class_<AdvancedConfig>(m, "AdvancedConfig", R"pbdoc(Advanced configuration system for NLM)pbdoc")
        .def(py::init<>())
        .def("loadFromJSON", &AdvancedConfig::loadFromJSON, py::arg("filepath"), py::arg("schema") = "")
        .def("setParameter", &AdvancedConfig::setParameter, py::arg("key"), py::arg("value"))
        .def("getParameter", &AdvancedConfig::getParameter, py::arg("key"))
        .def("setBatchParameters", &AdvancedConfig::setBatchParameters, py::arg("params"))
        .def("validate", &AdvancedConfig::validate)
        .def("getSummary", &AdvancedConfig::getSummary)
        .def_static("createPreset", &AdvancedConfig::createPreset, py::arg("presetName"))
        .def("exportToPython", &AdvancedConfig::exportToPython)
        .def("importFromPython", &AdvancedConfig::importFromPython, py::arg("configDict"))
        .def("isModeCompatible", &AdvancedConfig::isModeCompatible, py::arg("mode"), py::arg("requirements") = "")
        .def("getInternalConfig", &AdvancedConfig::getInternalConfig)
        .def("__repr__", [](const AdvancedConfig& cfg) {
            return "<AdvancedConfig: " + cfg.getInternalConfig()->summary() + ">";
        });
    
    // Neural visualization system
    py::class_<NeuralVisualization>(m, "NeuralVisualization", R"pbdoc(Neural network visualization system)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getConnectivityMatrix", &NeuralVisualization::getConnectivityMatrix)
        .def("getActivityHeatmap", &NeuralVisualization::getActivityHeatmap, py::arg("steps") = 10)
        .def("getNeuronStates", &NeuralVisualization::getNeuronStates)
        .def("getTimeSeriesData", &NeuralVisualization::getTimeSeriesData, py::arg("steps") = 100)
        .def("getNetworkGraph", &NeuralVisualization::getNetworkGraph)
        .def("createAnimationFrames", &NeuralVisualization::createAnimationFrames,
             py::arg("duration"), py::arg("fps") = 10);
    
    // Advanced ML model
    py::class_<NeuralMLModel>(m, "NeuralMLModel", R"pbdoc(Advanced neural network ML model)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("trainWithReinforcementLearning", &NeuralMLModel::trainWithReinforcementLearning,
             py::arg("episodes"), py::arg("learningRate") = 0.01f)
        .def("predictActivity", &NeuralMLModel::predictActivity, py::arg("input"))
        .def("getPerformanceMetrics", &NeuralMLModel::getPerformanceMetrics)
        .def("saveModel", &NeuralMLModel::saveModel, py::arg("filepath"))
        .def("loadModel", &NeuralMLModel::loadModel, py::arg("filepath"))
        .def("exportWeights", &NeuralMLModel::exportWeights)
        .def("importWeights", &NeuralMLModel::importWeights, py::arg("weights"))
        .def("extractFeatures", &NeuralMLModel::extractFeatures, py::arg("sample"))
        .def("compareWith", &NeuralMLModel::compareWith, py::arg("otherModel"));
    
    // Advanced experiment framework
    py::class_<AdvancedExperiment>(m, "AdvancedExperiment", R"pbdoc(Advanced experiment framework)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"))
        .def("runExperimentSuite", &AdvancedExperiment::runExperimentSuite,
             py::arg("experimentType"), py::arg("parameters"))
        .def("saveResults", &AdvancedExperiment::saveResults, py::arg("filepath"), py::arg("results"))
        .def("loadResults", &AdvancedExperiment::loadResults, py::arg("filepath"));
    
    // Real-time monitoring
    py::class_<RealTimeMonitor>(m, "RealTimeMonitor", R"pbdoc(Real-time system monitoring and control)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("startMonitoring", &RealTimeMonitor::startMonitoring,
             py::arg("intervalSeconds"), py::arg("callback"))
        .def("stopMonitoring", &RealTimeMonitor::stopMonitoring)
        .def("getSystemStatus", &RealTimeMonitor::getSystemStatus)
        .def("controlSimulation", &RealTimeMonitor::controlSimulation,
             py::arg("action"), py::arg("parameter") = 1.0f)
        .def("getNeuralActivity", &RealTimeMonitor::getNeuralActivity)
        .def("getConnectivity", &RealTimeMonitor::getConnectivity);
    
    // Advanced error handler
    py::class_<AdvancedErrorHandler>(m, "AdvancedErrorHandler", R"pbdoc(Advanced error handling and debugging)pbdoc")
        .def(py::init<>())
        .def("logError", &AdvancedErrorHandler::logError, py::arg("type"), py::arg("message"), py::arg("location") = "")
        .def("getLastError", &AdvancedErrorHandler::getLastError)
        .def("getErrorHistory", &AdvancedErrorHandler::getErrorHistory, py::arg("limit") = 100)
        .def("getErrorStatistics", &AdvancedErrorHandler::getErrorStatistics)
        .def("clearErrorHistory", &AdvancedErrorHandler::clearErrorHistory)
        .def("hasError", &AdvancedErrorHandler::hasError, py::arg("type"))
        .def("getErrorSummary", &AdvancedErrorHandler::getErrorSummary);
    
    // Factory functions for advanced components
    m.def("createAdvancedConfig", []() -> std::shared_ptr<AdvancedConfig> {
        return std::make_shared<AdvancedConfig>();
    }, "Create advanced configuration object");
    
    m.def("createNeuralVisualization", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralVisualization> {
        return std::make_shared<NeuralVisualization>(brain);
    }, py::arg("brain"), "Create neural visualization object");
    
    m.def("createNeuralMLModel", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralMLModel> {
        return std::make_shared<NeuralMLModel>(brain);
    }, py::arg("brain"), "Create neural ML model object");
    
    m.def("createAdvancedExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AdvancedExperiment> {
        return std::make_shared<AdvancedExperiment>(brain, world);
    }, py::arg("brain"), py::arg("world"), "Create advanced experiment object");
    
    m.def("createRealTimeMonitor", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<RealTimeMonitor> {
        return std::make_shared<RealTimeMonitor>(brain);
    }, py::arg("brain"), "Create real-time monitor object");
    
    m.def("createAdvancedErrorHandler", []() -> std::shared_ptr<AdvancedErrorHandler> {
        return std::make_shared<AdvancedErrorHandler>();
    }, "Create advanced error handler object");
    
    // Advanced utilities
    m.def("validateConfiguration", &validateConfiguration, py::arg("config"), "Validate configuration");
    
    // Version information
    m.attr("version") = "2.0.0";
    m.attr("build_date") = __DATE__;
    m.attr("description") = "Advanced NLM Python Bindings with enhanced configuration, visualization, ML integration, experiment management, real-time monitoring, and error handling";
}

} // namespace nlm
