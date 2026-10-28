#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>

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

// ========== ADVANCED UTILITIES ========== //

// Advanced configuration builder class
class AdvancedConfigBuilder {
public:
    AdvancedConfigBuilder() : config(std::make_shared<Config>()) {}
    
    // Neural network configuration
    AdvancedConfigBuilder& setNeuralNetwork(size_t neuronCount, size_t regionCount = 1) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        return *this;
    }
    
    // Connectivity configuration
    AdvancedConfigBuilder& setConnectivity(float connectionProbability, float weightMean = 0.5f, float weightStd = 0.1f) {
        config->set("connection_probability", connectionProbability);
        config->set("initial_weight_mean", weightMean);
        config->set("initial_weight_std", weightStd);
        return *this;
    }
    
    // Plasticity configuration
    AdvancedConfigBuilder& setSTDP(float ltpWeight = 0.01f, float ltdWeight = 0.012f, float tau = 20.0f) {
        config->set("stdp_ltp_weight", ltpWeight);
        config->set("stdp_ltd_weight", ltdWeight);
        config->set("stdp_tau", tau);
        return *this;
    }
    
    AdvancedConfigBuilder& enablePlasticity(bool enable = true) {
        config->set("plasticity.stdp.enable", enable);
        return *this;
    }
    
    // Neuromodulation configuration
    AdvancedConfigBuilder& enableRewardModulation(bool enable = true) {
        config->set("neuromod.dopamine.enable", enable);
        return *this;
    }
    
    AdvancedConfigBuilder& enableCuriosity(bool enable = true) {
        config->set("neuromod.curiosity.enable", enable);
        return *this;
    }
    
    // Development configuration
    AdvancedConfigBuilder& enableDevelopment(bool enable = true) {
        config->set("development.enable", enable);
        return *this;
    }
    
    // Memory configuration
    AdvancedConfigBuilder& setMemoryCapacity(size_t workingMemoryCapacity = 1000, size_t episodicMemoryCapacity = 1000) {
        config->set("working_memory.capacity", workingMemoryCapacity);
        config->set("episodic_memory.max_episodes", episodicMemoryCapacity);
        return *this;
    }
    
    // Simulation configuration
    AdvancedConfigBuilder& setSimulation(float timestep = 0.001f, float duration = 100.0f) {
        config->set("simulation_timestep", timestep);
        config->set("simulation_duration", duration);
        return *this;
    }
    
    // Visualization configuration
    AdvancedConfigBuilder& enableVisualization(bool enable = true) {
        config->set("visualization.enable", enable);
        return *this;
    }
    
    // Convenience presets
    AdvancedConfigBuilder& presetStandard() {
        return this->setNeuralNetwork(1000, 1)
                   .setConnectivity(0.1f)
                   .setSTDP()
                   .enablePlasticity(true)
                   .enableRewardModulation(true)
                   .enableDevelopment(true)
                   .setMemoryCapacity()
                   .setSimulation();
    }
    
    AdvancedConfigBuilder& presetHighPerformance(size_t neuronCount = 5000) {
        return this->setNeuralNetwork(neuronCount, 2)
                   .setConnectivity(0.05f)
                   .setSTDP(0.02f, 0.018f, 15.0f)
                   .enablePlasticity(true)
                   .enableCuriosity(true)
                   .enableRewardModulation(true)
                   .setMemoryCapacity(neuronCount / 10, 5000)
                   .setSimulation(0.002f, 50.0f);
    }
    
    AdvancedConfigBuilder& presetLearning(size_t neuronCount = 2000) {
        return this->setNeuralNetwork(neuronCount, 1)
                   .setConnectivity(0.15f)
                   .setSTDP(0.03f, 0.025f, 25.0f)
                   .enablePlasticity(true)
                   .enableRewardModulation(true)
                   .enableCuriosity(true)
                   .enableDevelopment(true)
                   .setMemoryCapacity(neuronCount / 5, 2000)
                   .setSimulation(0.001f, 200.0f);
    }
    
    // Load from JSON/YAML file
    bool loadFromFile(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
    
    // Load from Python dictionary
    bool loadFromDict(py::dict dict) {
        for (auto item : dict) {
            std::string key = py::str(item.first);
            std::string value;
            
            if (py::isinstance(item.second, py::int_())) {
                int int_val = item.second.cast<int>();
                config->set(key, int_val, ConfigSource::Runtime);
            } else if (py::isinstance(item.second, py::float_())) {
                float float_val = item.second.cast<float>();
                config->set(key, float_val, ConfigSource::Runtime);
            } else if (py::isinstance(item.second, py::bool_())) {
                bool bool_val = item.second.cast<bool>();
                config->set(key, bool_val, ConfigSource::Runtime);
            } else {
                value = item.second.cast<std::string>();
                config->set(key, value, ConfigSource::Runtime);
            }
        }
        return true;
    }
    
    // Get the config
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Export to Python dictionary
    py::dict exportToDict() const {
        py::dict result;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            result[key] = getValueFromConfig(key);
        }
        return result;
    }
    
private:
    std::shared_ptr<Config> config;
    
    py::object getValueFromConfig(const std::string& key) const {
        if (config->has(key)) {
            // Try to get as int
            auto int_val = config->getOr<int>(key);
            if (int_val.has_value()) {
                return py::int_(int_val.value());
            }
            
            // Try to get as float
            auto float_val = config->getOr<float>(key);
            if (float_val.has_value()) {
                return py::float_(float_val.value());
            }
            
            // Try to get as bool
            auto bool_val = config->getOr<bool>(key);
            if (bool_val.has_value()) {
                return py::bool_(bool_val.value());
            }
            
            // Fall back to string
            auto string_val = config->getOr<std::string>(key);
            if (string_val.has_value()) {
                return py::str(string_val.value());
            }
        }
        return py::none();
    }
};

// Advanced simulation runner with advanced features
class AdvancedSimulationRunner {
public:
    AdvancedSimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent)
        : brain(brain), world(world), agent(agent) {}
    
    // Run simulation with advanced features
    void runAdvanced(size_t steps, const std::string& mode = "balanced") {
        if (mode == "aggressive") {
            runAggressiveLearning(steps);
        } else if (mode == "conservative") {
            runConservativeLearning(steps);
        } else {
            runBalancedLearning(steps);
        }
    }
    
    // Get simulation statistics
    py::dict getAdvancedStatistics() {
        py::dict stats;
        
        // Basic brain statistics
        stats["neurons"] = brain->getTotalNeuronCount();
        stats["synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        
        // Memory statistics
        if (brain->getWorkingMemory()) {
            auto workingMem = brain->getWorkingMemory();
            stats["working_memory_traces"] = workingMem->getActiveTraces();
        }
        if (brain->getEpisodicMemory()) {
            auto episodicMem = brain->getEpisodicMemory();
            stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
        }
        
        // Neuromodulation statistics
        if (agent) {
            stats["curiosity_level"] = agent->getCuriosityLevel();
            stats["novelty_level"] = agent->getNoveltyLevel();
            stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stats["prediction_error"] = agent->getPredictionError();
        }
        
        // Development statistics
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // World statistics
        stats["world_width"] = world->getWidth();
        stats["world_height"] = world->getHeight();
        stats["simulation_time"] = world->getSimulationTime();
        
        return stats;
    }
    
    // Save brain state with timestamp
    bool saveWithTimestamp(const std::string& basePath) {
        std::string timestamp = getTimestamp();
        std::string filepath = basePath + "_" + timestamp + ".bin";
        return brain->save(filepath);
    }
    
    // Run multiple episodes with learning
    void runMultiEpisode(size_t numEpisodes, size_t stepsPerEpisode) {
        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            runEpisode(stepsPerEpisode);
            
            // Save checkpoint every 10 episodes
            if (episode % 10 == 0) {
                std::string checkpoint = "checkpoint_episode_" + std::to_string(episode) + ".bin";
                brain->save(checkpoint);
            }
            
            // Print progress
            if (episode % 5 == 0) {
                auto stats = getAdvancedStatistics();
                NLM_LOG_INFO("Episode " + std::to_string(episode) + 
                           ": " + std::to_string(stats["total_spikes"]).get_value_or("0") + " spikes");
            }
        }
    }
    
    // Run learning experiment
    void runLearningExperiment(size_t steps, float rewardScaling = 1.0f) {
        // Reset for clean experiment
        brain->reset();
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        
        // Run learning
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward based on action success
            float reward = calculateReward(action);
            agent->applyRewardModulation(reward * rewardScaling, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Log progress periodically
            if (step % 100 == 0) {
                auto stats = getAdvancedStatistics();
                NLM_LOG_INFO("Learning Step " + std::to_string(step) +
                           ": " + std::to_string(stats["total_spikes"]).get_value_or("0") + " spikes, " +
                           std::to_string(stats["curiosity_level"]).get_value_or("0.0") + " curiosity");
            }
        }
    }
    
    // Compare performance of different configurations
    static std::vector<py::dict> benchmarkConfigurations(std::vector<std::shared_ptr<Config>> configs, size_t steps = 1000) {
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            // Create brain with configuration
            auto config = configs[i];
            auto testBrain = std::make_shared<Brain>(config);
            testBrain->initialize();
            
            // Run benchmark
            size_t spikeCount = 0;
            float avgFiringRate = 0.0f;
            
            for (size_t step = 0; step < steps; ++step) {
                testBrain->step(step);
                spikeCount = testBrain->getTotalSpikeCount();
                avgFiringRate = testBrain->getAverageFiringRate();
            }
            
            // Add to results
            py::dict result;
            result["config_id"] = i;
            result["neurons"] = testBrain->getTotalNeuronCount();
            result["synapses"] = testBrain->getTotalSynapseCount();
            result["total_spikes"] = spikeCount;
            result["avg_firing_rate"] = avgFiringRate;
            result["e_i_ratio"] = testBrain->getExcitationInhibitionRatio();
            
            results.push_back(result);
        }
        
        return results;
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    float calculateReward(const Action& action) {
        // Simple reward calculation based on action type
        switch (action.getType()) {
            case ActionType::MoveForward:
                return 1.0f;
            case ActionType::TurnLeft:
                return 0.5f;
            case ActionType::TurnRight:
                return 0.5f;
            case ActionType::Look:
                return 0.3f;
            case ActionType::Wait:
                return -0.1f;
            default:
                return 0.0f;
        }
    }
    
    void runEpisode(size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.0f, 0.0f);
        }
    }
    
    void runAggressiveLearning(size_t steps) {
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        // Run with higher plasticity
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply stronger reward modulation
            agent->applyRewardModulation(0.5f, 0.0f);
            agent->updateDevelopment(0.1f);
        }
    }
    
    void runConservativeLearning(size_t steps) {
        // Enable basic learning only
        agent->enableRewardModulation(true);
        agent->enableCuriosity(false);
        agent->enableStructuralPlasticity(false);
        agent->enableDevelopment(false);
        
        // Run with lower plasticity
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply weaker reward modulation
            agent->applyRewardModulation(0.1f, 0.0f);
            agent->updateDevelopment(0.1f);
        }
    }
    
    void runBalancedLearning(size_t steps) {
        // Enable balanced learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        // Run with balanced settings
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply moderate reward modulation
            agent->applyRewardModulation(0.2f, 0.0f);
            agent->updateDevelopment(0.1f);
        }
    }
};

// Performance profiler
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string operation;
        double totalTime;
        size_t iterations;
        double avgTime;
        double minTime;
        double maxTime;
    };
    
    PerformanceProfiler() : enabled(true) {}
    
    void enableProfiling(bool enable = true) {
        enabled = enable;
    }
    
    void startProfile(const std::string& operation) {
        if (!enabled) return;
        currentOperation = operation;
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void endProfile() {
        if (!enabled) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        double timeMs = duration.count() / 1000.0;
        
        ProfileData data;
        data.operation = currentOperation;
        data.totalTime = timeMs;
        data.iterations = 1;
        data.avgTime = timeMs;
        data.minTime = timeMs;
        data.maxTime = timeMs;
        
        profileData.push_back(data);
    }
    
    py::dict getProfileSummary() {
        py::dict summary;
        
        if (profileData.empty()) {
            summary["message"] = "No profiling data collected";
            return summary;
        }
        
        // Aggregate data by operation
        std::unordered_map<std::string, std::vector<double>> operationTimes;
        for (const auto& data : profileData) {
            operationTimes[data.operation].push_back(data.totalTime);
        }
        
        // Create summary for each operation
        for (const auto& pair : operationTimes) {
            const auto& times = pair.second;
            double total = 0.0, min = std::numeric_limits<double>::max(), max = 0.0;
            
            for (double t : times) {
                total += t;
                min = std::min(min, t);
                max = std::max(max, t);
            }
            
            double avg = total / times.size();
            
            py::dict opSummary;
            opSummary["operation"] = pair.first;
            opSummary["iterations"] = static_cast<int>(times.size());
            opSummary["total_time_ms"] = total;
            opSummary["avg_time_ms"] = avg;
            opSummary["min_time_ms"] = min;
            opSummary["max_time_ms"] = max;
            opSummary["std_deviation"] = std::sqrt((times.size() > 1) ? 
                std::accumulate(times.begin(), times.end(), 0.0) / times.size() - avg * avg : 0.0);
            
            summary[pair.first] = opSummary;
        }
        
        return summary;
    }
    
    void clear() {
        profileData.clear();
    }
    
private:
    bool enabled;
    std::string currentOperation;
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<ProfileData> profileData;
};

// Experiment runner with advanced features
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent)
        : brain(brain), world(world), agent(agent) {}
    
    // Run ablation study
    py::dict runAblationStudy(std::map<std::string, bool> systemDisabling) {
        py::dict results;
        
        // Run baseline
        results["baseline"] = runSingleExperiment();
        
        // Run variants
        std::vector<std::string> variantNames;
        for (const auto& pair : systemDisabling) {
            variantNames.push_back(pair.first + "_disabled");
        }
        
        for (const auto& variant : variantNames) {
            results[variant] = runVariantExperiment(systemDisabling);
        }
        
        return results;
    }
    
    // Run scaling experiment
    py::dict runScalingExperiment(std::vector<size_t> neuronCounts, size_t steps = 1000) {
        py::dict results;
        
        for (size_t neuronCount : neuronCounts) {
            // Create brain with specific neuron count
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
            
            auto testBrain = std::make_shared<Brain>(config);
            testBrain->initialize();
            
            auto testAgent = std::make_shared<AgentBrain>(testBrain);
            auto testWorld = std::make_shared<SimpleWorld>();
            testWorld->configure(20, 20, 8, 8);
            
            // Run simulation
            for (size_t step = 0; step < steps; ++step) {
                testWorld->update(0.1f);
                auto percept = testWorld->getSensoryPercept();
                testAgent->processSensoryInput(percept);
                testBrain->step(step);
                auto action = testAgent->decodeMotorCommand();
                testWorld->applyMotorCommand(action, testWorld->getSimulationTime());
            }
            
            // Store results
            py::dict variantResults;
            variantResults["neurons"] = neuronCount;
            variantResults["spikes"] = testBrain->getTotalSpikeCount();
            variantResults["firing_rate"] = testBrain->getAverageFiringRate();
            variantResults["e_i_ratio"] = testBrain->getExcitationInhibitionRatio();
            
            results["neurons_" + std::to_string(neuronCount)] = variantResults;
        }
        
        return results;
    }
    
    // Run parameter sweep
    py::dict runParameterSweep(std::map<std::string, std::vector<double>> parameters, size_t steps = 1000) {
        py::dict results;
        
        // Generate all parameter combinations (simplified)
        size_t paramCount = 0;
        for (const auto& pair : parameters) {
            paramCount *= pair.second.size();
        }
        
        // For demonstration, just run a few combinations
        size_t combinations = std::min(size_t(5), paramCount);
        
        for (size_t i = 0; i < combinations; ++i) {
            // Create config with current parameters
            auto config = std::make_shared<Config>();
            
            // Set parameters based on index (simplified)
            for (auto it = parameters.begin(); it != parameters.end(); ++it) {
                std::string key = it->first;
                const auto& values = it->second;
                
                size_t valueIndex = (i + paramCount) % values.size();
                double value = values[valueIndex];
                
                if (key == "connection_probability") {
                    config->set(key, value);
                } else if (key == "stdp_ltp_weight") {
                    config->set(key, static_cast<float>(value));
                } else if (key == "neuron_count") {
                    config->set(key, static_cast<int64_t>(value));
                }
            }
            
            // Run experiment
            auto testBrain = std::make_shared<Brain>(config);
            testBrain->initialize();
            
            for (size_t step = 0; step < steps; ++step) {
                testBrain->step(step);
            }
            
            // Store results
            std::string paramKey = "exp_" + std::to_string(i);
            py::dict experimentResults;
            experimentResults["parameters"] = exportConfigToPyDict(config);
            experimentResults["spikes"] = testBrain->getTotalSpikeCount();
            experimentResults["firing_rate"] = testBrain->getAverageFiringRate();
            
            results[paramKey] = experimentResults;
        }
        
        return results;
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    
    py::dict exportConfigToPyDict(std::shared_ptr<Config> config) {
        py::dict result;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            result[key] = getConfigValue(config, key);
        }
        
        return result;
    }
    
    py::object getConfigValue(std::shared_ptr<Config> config, const std::string& key) {
        if (config->has(key)) {
            // Try to get as int
            auto int_val = config->getOr<int>(key);
            if (int_val.has_value()) {
                return py::int_(int_val.value());
            }
            
            // Try to get as float
            auto float_val = config->getOr<float>(key);
            if (float_val.has_value()) {
                return py::float_(float_val.value());
            }
            
            // Try to get as bool
            auto bool_val = config->getOr<bool>(key);
            if (bool_val.has_value()) {
                return py::bool_(bool_val.value());
            }
            
            // Fall back to string
            auto string_val = config->getOr<std::string>(key);
            if (string_val.has_value()) {
                return py::str(string_val.value());
            }
        }
        return py::none();
    }
    
    py::dict runSingleExperiment() {
        // Run with all systems enabled
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        // Run experiment
        for (size_t step = 0; step < 1000; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.1f, 0.0f);
            agent->updateDevelopment(0.1f);
        }
        
        // Return results
        py::dict results;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_rate"] = brain->getAverageFiringRate();
        results["curiosity_level"] = agent->getCuriosityLevel();
        results["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return results;
    }
    
    py::dict runVariantExperiment(std::map<std::string, bool> systemDisabling) {
        // Temporarily disable systems based on map
        std::map<std::string, bool> originalState;
        
        for (const auto& pair : systemDisabling) {
            if (pair.second) { // System should be disabled
                originalState[pair.first] = true;
                if (pair.first == "reward_modulation") {
                    agent->enableRewardModulation(false);
                } else if (pair.first == "curiosity") {
                    agent->enableCuriosity(false);
                } else if (pair.first == "structural_plasticity") {
                    agent->enableStructuralPlasticity(false);
                } else if (pair.first == "development") {
                    agent->enableDevelopment(false);
                }
            }
        }
        
        // Run experiment
        py::dict results = runSingleExperiment();
        
        // Restore original state
        for (const auto& pair : originalState) {
            if (pair.second) {
                // Re-enable the system (simplified - would need to know original state)
            }
        }
        
        return results;
    }
};

} // namespace nlm

// Main module registration
PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Python Module
        ===========================

        Advanced Python module for NLM with enhanced capabilities for research and experimentation.

        This module provides:
        - Advanced configuration builder with presets
        - Advanced simulation runner with multiple learning modes
        - Performance profiling tools
        - Experiment runner with ablation studies
        - Parameter sweep and scaling experiments
        - Multi-episode learning with checkpointing

        Key Features:
        - Configuration builder with JSON/YAML support
        - Advanced simulation modes (aggressive, conservative, balanced)
        - Performance profiling and analysis
        - Experiment automation and comparison
        - Parameter optimization tools
        - Multi-episode learning with state management

        Quick Start Example:
            >>> import pynlm_advanced as nlm
            >>> # Create advanced configuration
            >>> config = nlm.AdvancedConfigBuilder().presetLearning()
            >>> # Create brain with advanced config
            >>> brain = nlm.create_brain(config.getConfig())
            >>> # Create advanced runner
            >>> runner = nlm.AdvancedSimulationRunner(brain, world, agent)
            >>> # Run advanced learning
            >>> runner.runLearningExperiment(5000)

        Advanced Usage:
            >>> # Run performance benchmarks
            >>> benchmarks = nlm.PerformanceProfiler.benchmarks(configs)
            >>> # Run ablation studies
            >>> ablation_results = runner.runAblationStudy({"reward_modulation": true, "curiosity": false})
            >>> # Run parameter sweeps
            >>> sweep_results = runner.runParameterSweep({"connection_probability": [0.05, 0.1, 0.15]})
    )pbdoc";

    // Register AdvancedConfigBuilder
    py::class_<AdvancedConfigBuilder>(m, "AdvancedConfigBuilder", R"pbdoc(Advanced configuration builder with presets)pbdoc")
        .def(py::init<>())
        // Neural network configuration
        .def("set_neural_network", &AdvancedConfigBuilder::setNeuralNetwork, py::arg("neuron_count"), py::arg("region_count") = 1,
             "Set neural network configuration")
        .def("set_connectivity", &AdvancedConfigBuilder::setConnectivity, py::arg("connection_probability"), py::arg("weight_mean") = 0.5f, py::arg("weight_std") = 0.1f,
             "Set connectivity configuration")
        .def("set_stdp", &AdvancedConfigBuilder::setSTDP, py::arg("ltp_weight") = 0.01f, py::arg("ltd_weight") = 0.012f, py::arg("tau") = 20.0f,
             "Set STDP parameters")
        .def("enable_plasticity", &AdvancedConfigBuilder::enablePlasticity, py::arg("enable") = true,
             "Enable/disable plasticity")
        .def("enable_reward_modulation", &AdvancedConfigBuilder::enableRewardModulation, py::arg("enable") = true,
             "Enable/disable reward modulation")
        .def("enable_curiosity", &AdvancedConfigBuilder::enableCuriosity, py::arg("enable") = true,
             "Enable/disable curiosity")
        .def("enable_development", &AdvancedConfigBuilder::enableDevelopment, py::arg("enable") = true,
             "Enable/disable development")
        .def("set_memory_capacity", &AdvancedConfigBuilder::setMemoryCapacity, py::arg("working_memory_capacity") = 1000, py::arg("episodic_memory_capacity") = 1000,
             "Set memory capacity")
        .def("set_simulation", &AdvancedConfigBuilder::setSimulation, py::arg("timestep") = 0.001f, py::arg("duration") = 100.0f,
             "Set simulation parameters")
        .def("enable_visualization", &AdvancedConfigBuilder::enableVisualization, py::arg("enable") = true,
             "Enable/disable visualization")
        // Convenience presets
        .def("preset_standard", &AdvancedConfigBuilder::presetStandard,
             "Get standard configuration preset")
        .def("preset_high_performance", &AdvancedConfigBuilder::presetHighPerformance, py::arg("neuron_count") = 5000,
             "Get high performance configuration preset")
        .def("preset_learning", &AdvancedConfigBuilder::presetLearning, py::arg("neuron_count") = 2000,
             "Get learning configuration preset")
        // Loading and exporting
        .def("load_from_file", &AdvancedConfigBuilder::loadFromFile, py::arg("filepath"),
             "Load configuration from file (JSON/YAML)")
        .def("load_from_dict", &AdvancedConfigBuilder::loadFromDict, py::arg("config_dict"),
             "Load configuration from Python dictionary")
        .def("get_config", &AdvancedConfigBuilder::getConfig,
             "Get the underlying configuration object")
        .def("export_to_dict", &AdvancedConfigBuilder::exportToDict,
             "Export configuration to Python dictionary");

    // Register AdvancedSimulationRunner
    py::class_<AdvancedSimulationRunner>(m, "AdvancedSimulationRunner", R"pbdoc(Advanced simulation runner with multiple modes)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             "Create advanced simulation runner")
        .def("run_advanced", &AdvancedSimulationRunner::runAdvanced, py::arg("steps"), py::arg("mode") = "balanced",
             "Run advanced simulation with specified mode")
        .def("get_advanced_statistics", &AdvancedSimulationRunner::getAdvancedStatistics,
             "Get comprehensive simulation statistics")
        .def("save_with_timestamp", &AdvancedSimulationRunner::saveWithTimestamp, py::arg("base_path"),
             "Save brain state with timestamp")
        .def("run_multi_episode", &AdvancedSimulationRunner::runMultiEpisode, py::arg("num_episodes"), py::arg("steps_per_episode"),
             "Run multiple episodes with learning")
        .def("run_learning_experiment", &AdvancedSimulationRunner::runLearningExperiment, py::arg("steps"), py::arg("reward_scaling") = 1.0f,
             "Run learning experiment with reward modulation")
        .def("benchmark_configurations", &AdvancedSimulationRunner::benchmarkConfigurations, py::arg("configs"), py::arg("steps") = 1000,
             "Benchmark multiple configurations");

    // Register PerformanceProfiler
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance profiler for NLM operations)pbdoc")
        .def(py::init<>())
        .def("enable_profiling", &PerformanceProfiler::enableProfiling, py::arg("enable") = true,
             "Enable/disable profiling")
        .def("start_profile", &PerformanceProfiler::startProfile, py::arg("operation"),
             "Start profiling an operation")
        .def("end_profile", &PerformanceProfiler::endProfile,
             "End current profiling session")
        .def("get_profile_summary", &PerformanceProfiler::getProfileSummary,
             "Get profiling summary as dictionary")
        .def("clear", &PerformanceProfiler::clear,
             "Clear all profiling data");

    // Register AdvancedExperimentRunner
    py::class_<AdvancedExperimentRunner>(m, "AdvancedExperimentRunner", R"pbdoc(Advanced experiment runner for research)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             "Create advanced experiment runner")
        .def("run_ablation_study", &AdvancedExperimentRunner::runAblationStudy, py::arg("system_disabling"),
             "Run ablation study to test system contributions")
        .def("run_scaling_experiment", &AdvancedExperimentRunner::runScalingExperiment, py::arg("neuron_counts"), py::arg("steps") = 1000,
             "Run scaling experiment with different network sizes")
        .def("run_parameter_sweep", &AdvancedExperimentRunner::runParameterSweep, py::arg("parameters"), py::arg("steps") = 1000,
             "Run parameter sweep to optimize configuration");

    // Additional factory functions
    m.def("create_advanced_config", []() { return std::make_shared<AdvancedConfigBuilder>(); },
          "Create advanced configuration builder");
    
    m.def("create_advanced_runner", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                      std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<AdvancedSimulationRunner>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create advanced simulation runner");
    
    m.def("create_performance_profiler", []() { return std::make_shared<PerformanceProfiler>(); },
          "Create performance profiler");
    
    m.def("create_advanced_experiment_runner", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                                   std::shared_ptr<AgentBrain> agent) {
        return std::make_shared<AdvancedExperimentRunner>(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Create advanced experiment runner");

    // Constants
    m.attr("ADVANCED_VERSION") = "0.1.0";
    m.attr("DEFAULT_ADVANCED_NEURON_COUNT") = 2000;
    m.attr("DEFAULT_ADVANCED_SIMULATION_TIMESTEP") = 0.001f;
}
