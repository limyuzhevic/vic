#ifndef BRAIN_HPP
#define BRAIN_HPP

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

// Enhanced Python bindings with advanced features
namespace pybindings {

// Configuration utilities
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    
    // Load from multiple sources with fallback
    bool loadFromMultipleFiles(const std::vector<std::string>& filePaths) {
        bool success = false;
        std::string lastError;
        
        for (const auto& path : filePaths) {
            if (config_->loadFromFile(path)) {
                NLM_LOG_INFO("Loaded configuration from: " + path);
                success = true;
                break;  // Use first successful load
            } else {
                lastError = path + " (not found or invalid)";
            }
        }
        
        if (!success) {
            NLM_LOG_WARN("Could not load any config file. Using defaults. Last tried: " + lastError);
        }
        
        return success;
    }
    
    // Save to JSON with pretty formatting
    bool saveToPrettyJSON(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
            return false;
        }
        
        file << config_->summary();
        file.close();
        return true;
    }
    
    // Get configuration value with type conversion
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        try {
            return config_->getOr<T>(key, defaultValue);
        } catch (...) {
            NLM_LOG_WARN("Failed to get config value for key: " + key);
            return defaultValue;
        }
    }
    
    // Set multiple values at once
    void setValues(const std::map<std::string, ConfigValue>& values) {
        for (const auto& pair : values) {
            config_->set(pair.first, pair.second);
        }
    }
    
    // Validate configuration
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        // Check required keys
        std::vector<std::string> requiredKeys = {
            "neuron_count", "region_count", "simulation_timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config_->has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate values
        try {
            int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errors.push_back("neuron_count must be positive");
            }
            
            int64_t regionCount = config_->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                errors.push_back("region_count must be positive");
            }
            
            double timestep = config_->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            }
            
        } catch (...) {
            errors.push_back("Error parsing configuration values");
        }
        
        return errors;
    }
    
    // Export configuration as JSON string
    std::string exportToJSON() const {
        std::stringstream json;
        json << "{\n";
        auto keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            json << "  \"" << keys[i] << "\": ";
            // Simple value representation - in real implementation would be more sophisticated
            json << "\"" << keys[i] << "\"\n";
            if (i < keys.size() - 1) json << ",\n";
        }
        json << "}\n";
        return json.str();
    }
    
    // Import configuration from JSON string
    bool importFromJSON(const std::string& jsonString) {
        // Simple JSON parsing - in real implementation would use a proper JSON library
        // For now, just clear and load from file as a placeholder
        config_->clear();
        // In real implementation: parse jsonString and load values
        NLM_LOG_WARN("JSON import not fully implemented - using placeholder");
        return true;
    }
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Clone configuration
    EnhancedConfig clone() const {
        EnhancedConfig clone;
        clone.config_ = std::make_shared<Config>(*config_);
        return clone;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced brain utilities with batch operations
class EnhancedBrainUtils {
public:
    // Batch neuron operations
    struct BatchNeuronResult {
        size_t injectedCount;
        size_t firedCount;
        float avgFiringRate;
    };
    
    static BatchNeuronResult batchInjectCurrent(
        std::shared_ptr<Brain> brain,
        const std::vector<NeuronId>& neuronIds,
        MembranePotential current) {
        BatchNeuronResult result{0, 0, 0.0f};
        
        for (const auto& neuronId : neuronIds) {
            brain->injectCurrent(neuronId, current);
            result.injectedCount++;
        }
        
        // Run simulation to see effects
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        result.firedCount = brain->getFiringNeuronCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        
        return result;
    }
    
    // Batch synapse operations
    struct BatchSynapseResult {
        size_t weightChangeCount;
        float totalWeightChange;
        float maxWeightChange;
    };
    
    static BatchSynapseResult batchModifySynapses(
        std::shared_ptr<Brain> brain,
        float weightChange,
        float minWeight = -1.0f,
        float maxWeight = 1.0f) {
        BatchSynapseResult result{0, 0.0f, 0.0f};
        
        auto regions = brain->getRegions();
        for (auto& regionPtr : regions) {
            auto& region = *regionPtr;
            auto& synapses = region.getSynapses();
            
            for (auto& synPtr : synapses) {
                auto& synapse = *synPtr;
                float oldWeight = synapse.getWeight();
                synapse.addToWeight(weightChange);
                float newWeight = synapse.getWeight();
                
                if (std::abs(newWeight - oldWeight) > 0.001f) {
                    result.weightChangeCount++;
                    float delta = newWeight - oldWeight;
                    result.totalWeightChange += delta;
                    result.maxWeightChange = std::max(result.maxWeightChange, std::abs(delta));
                }
            }
        }
        
        return result;
    }
    
    // Network analysis utilities
    struct NetworkAnalysis {
        size_t totalNeurons;
        size_t totalSynapses;
        float connectionDensity;
        float avgWeightMagnitude;
        size_t maxFiringNeurons;
        float avgFiringRate;
        float excitationInhibitionRatio;
        float networkActivity;
    };
    
    static NetworkAnalysis analyzeNetwork(std::shared_ptr<Brain> brain) {
        NetworkAnalysis analysis{};
        
        analysis.totalNeurons = brain->getTotalNeuronCount();
        analysis.totalSynapses = brain->getTotalSynapseCount();
        
        // Calculate connection density
        if (analysis.totalNeurons > 1) {
            size_t maxPossible = analysis.totalNeurons * (analysis.totalNeurons - 1);
            analysis.connectionDensity = static_cast<float>(analysis.totalSynapses) / maxPossible;
        }
        
        // Calculate average weight magnitude
        if (analysis.totalSynapses > 0) {
            // This would need to access all synapses - simplified for now
            analysis.avgWeightMagnitude = 0.5f;  // Placeholder
        }
        
        analysis.maxFiringNeurons = brain->getFiringNeuronCount();
        analysis.avgFiringRate = brain->getAverageFiringRate();
        analysis.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Network activity indicator
        analysis.networkActivity = analysis.avgFiringRate / 100.0f;  // Normalize to 0-1
        if (analysis.networkActivity > 1.0f) analysis.networkActivity = 1.0f;
        
        return analysis;
    }
    
    // Simulation utilities
    static std::vector<std::vector<float>> runMultiStepSimulation(
        std::shared_ptr<Brain> brain,
        SimulationStep steps,
        const std::vector<float>& inputs = std::vector<float>()) {
        
        std::vector<std::vector<float>> history;
        history.reserve(steps);
        
        for (SimulationStep step = 0; step < steps; ++step) {
            // Apply input if provided
            if (!inputs.empty() && step < inputs.size()) {
                // Inject current based on input
                brain->injectCurrentToNeurons(NeuronType::Sensory, inputs[step]);
            }
            
            brain->step(step, step * 0.001);
            
            // Record state
            std::vector<float> state;
            state.push_back(static_cast<float>(brain->getTotalSpikeCount()));
            state.push_back(brain->getAverageFiringRate());
            state.push_back(brain->getActiveNeuronCount());
            state.push_back(brain->getFiringNeuronCount());
            
            history.push_back(state);
        }
        
        return history;
    }
    
    // State management utilities
    static bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }
    
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }
    
    // Checkpoint utilities
    static bool createCheckpoint(std::shared_ptr<Brain> brain, const std::string& prefix) {
        std::ostringstream filename;
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        filename << prefix << "_checkpoint_" 
                 << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") 
                 << ".bin";
        
        return saveBrainState(brain, filename.str());
    }
    
    static bool restoreCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
        return loadBrainState(brain, filepath);
    }
};

// Agent utilities
class EnhancedAgentUtils {
public:
    // Agent configuration management
    struct AgentConfig {
        std::string name;
        std::string strategy;
        float curiosityLevel;
        float noveltyThreshold;
        float rewardScale;
        bool enableReflection;
        bool enableMetaLearning;
    };
    
    // Create agent with configuration
    static std::shared_ptr<AgentBrain> createAgent(
        std::shared_ptr<Brain> brain,
        const AgentConfig& config,
        std::shared_ptr<SimpleWorld> world = nullptr) {
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure agent based on config
        agentBrain->enableRewardModulation(config.enableReflection);
        agentBrain->enableCuriosity(config.enableMetaLearning);
        
        // Set up neural populations for different behaviors
        if (world) {
            agentBrain->initialize(*world);
        }
        
        return agentBrain;
    }
    
    // Run agent episode with learning
    static struct EpisodeResult {
        float totalReward;
        size_t stepsTaken;
        size_t actionsExecuted;
        float curiosityScore;
        float noveltyScore;
        float finalPerformance;
    } runEpisode(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        const std::vector<float>& sensoryInputs = std::vector<float>()) {
        
        EpisodeResult result{};
        world->reset();
        
        for (size_t step = 0; step < sensoryInputs.size() || step < 1000; ++step) {
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process with agent
            agent->processSensoryInput(percept);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action in world
            auto actionResult = world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply neuromodulation based on reward
            agent->applyRewardModulation(actionResult.reward, 0.0f);  // No predicted reward for now
            
            // Update development
            agent->updateDevelopment(world->getSimulationTime());
            
            result.stepsTaken++;
            if (actionResult.success) {
                result.actionsExecuted++;
            }
            result.totalReward += actionResult.reward;
            
            world->update(world->getSimulationTime() + 0.001);
            
            // Break if episode ends
            if (!actionResult.success && actionResult.message.find("dead") != std::string::npos) {
                break;
            }
        }
        
        // Calculate final metrics
        auto brain = agent->getBrain();
        if (brain) {
            result.finalPerformance = brain->getAverageFiringRate();
            result.curiosityScore = agent->getCuriosityLevel();
            result.noveltyScore = agent->getNoveltyLevel();
        }
        
        return result;
    }
    
    // Multi-agent utilities
    static std::vector<struct MultiAgentResult> runMultiAgentTraining(
        const std::vector<std::shared_ptr<AgentBrain>>& agents,
        std::shared_ptr<SimpleWorld> world,
        size_t episodes,
        bool enableCommunication = true) {
        
        std::vector<struct MultiAgentResult> results;
        results.reserve(agents.size());
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            for (auto& agent : agents) {
                auto episodeResult = runEpisode(agent, world);
                
                if (enableCommunication) {
                    // Agents can share information (simplified)
                    agent->applyRewardModulation(episodeResult.totalReward / episodeResult.stepsTaken, 0.0f);
                }
                
                results.push_back({
                    agent->getBrain()->getId(),  // Agent ID
                    episodeResult.totalReward,
                    episodeResult.stepsTaken,
                    episodeResult.finalPerformance
                });
            }
            
            world->reset();
        }
        
        return results;
    }
    
private:
    struct MultiAgentResult {
        uint64_t agentId;
        float totalReward;
        size_t stepsTaken;
        float performance;
    };
};

// Simulation runner utilities
class EnhancedSimulationRunner {
public:
    // Run comparative simulation with multiple configurations
    static std::vector<struct ComparativeResult> runComparativeSimulation(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::string& description = "Comparative Simulation") {
        
        std::vector<struct ComparativeResult> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for config " + std::to_string(i));
                continue;
            }
            
            struct ComparativeResult result {
                i,
                configs[i]->summary(),
                0.0f,  // avgFiringRate
                0.0f,  // totalSpikes
                0.0f,  // executionTime
                "success"
            };
            
            // Run simulation
            for (SimulationStep step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            result.avgFiringRate = brain->getAverageFiringRate();
            result.totalSpikes = static_cast<float>(brain->getTotalSpikeCount());
            
            results.push_back(result);
        }
        
        return results;
    }
    
    // Run parameter sweep
    static std::vector<struct ParameterSweepResult> runParameterSweep(
        const std::string& paramName,
        const std::vector<float>& paramValues,
        const std::shared_ptr<Config>& baseConfig) {
        
        std::vector<struct ParameterSweepResult> results;
        
        for (float value : paramValues) {
            auto config = std::make_shared<Config>(*baseConfig);
            config->set(paramName, value, ConfigSource::Runtime);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for parameter " + std::to_string(value));
                continue;
            }
            
            struct ParameterSweepResult result {
                value,
                0.0f,  // performance
                0.0f,  // executionTime
                "success"
            };
            
            // Run simulation
            for (SimulationStep step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            result.performance = brain->getAverageFiringRate();
            
            results.push_back(result);
        }
        
        return results;
    }
    
    // Real-time monitoring utilities
    class RealTimeMonitor {
    private:
        std::shared_ptr<Brain> brain;
        size_t monitorInterval;
        std::chrono::steady_clock::time_point lastUpdate;
        std::vector<struct MonitoringData> history;
        
    public:
        RealTimeMonitor(std::shared_ptr<Brain> brain, size_t interval = 100) 
            : brain(brain), monitorInterval(interval) {
            lastUpdate = std::chrono::steady_clock::now();
        }
        
        void update() {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
            
            if (elapsed.count() >= monitorInterval * 10) {  // Update every monitorInterval steps
                recordData();
                lastUpdate = now;
            }
        }
        
        void recordData() {
            struct MonitoringData data {
                std::chrono::steady_clock::now(),
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                brain->getActiveNeuronCount(),
                brain->getFiringNeuronCount()
            };
            
            history.push_back(data);
            
            // Log current state
            NLM_LOG_INFO("Monitored state: "
                        "spikes=" + std::to_string(data.spikeCount) +
                        " firing_rate=" + std::to_string(data.firingRate) +
                        " active=" + std::to_string(data.activeNeurons) +
                        " firing=" + std::to_string(data.firingNeurons));
        }
        
        std::vector<struct MonitoringData> getHistory() const { return history; }
        
        // Generate monitoring report
        std::string generateReport() {
            std::stringstream report;
            report << "=== Real-Time Monitoring Report ===\n";
            report << "Total observations: " << history.size() << "\n";
            
            if (history.empty()) return report.str();
            
            // Calculate statistics
            size_t minSpikes = history[0].spikeCount;
            size_t maxSpikes = history[0].spikeCount;
            float minFiring = history[0].firingRate;
            float maxFiring = history[0].firingRate;
            
            for (const auto& data : history) {
                minSpikes = std::min(minSpikes, data.spikeCount);
                maxSpikes = std::max(maxSpikes, data.spikeCount);
                minFiring = std::min(minFiring, data.firingRate);
                maxFiring = std::max(maxFiring, data.firingRate);
            }
            
            report << "Spike count: min=" << minSpikes << ", max=" << maxSpikes << "\n";
            report << "Firing rate: min=" << minFiring << " Hz, max=" << maxFiring << " Hz\n";
            report << "=== End Report ===\n";
            
            return report.str();
        }
    };
    
private:
    struct ComparativeResult {
        size_t configIndex;
        std::string configSummary;
        float avgFiringRate;
        float totalSpikes;
        float executionTime;
        std::string status;
    };
    
    struct ParameterSweepResult {
        float parameterValue;
        float performance;
        float executionTime;
        std::string status;
    };
    
    struct MonitoringData {
        std::chrono::steady_clock::time_point timestamp;
        size_t spikeCount;
        float firingRate;
        size_t activeNeurons;
        size_t firingNeurons;
    };
};

// Visualization utilities
class EnhancedVisualizationUtils {
public:
    // Generate text-based visualization
    static std::string generateTextVisualization(std::shared_ptr<Brain> brain, size_t width = 50) {
        std::stringstream viz;
        
        viz << "=== NLM Network Visualization ===\n";
        viz << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        viz << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        viz << "Activity: " << brain->getActiveNeuronCount() << " active, "
            << brain->getFiringNeuronCount() << " firing\n\n";
        
        // Simple firing rate histogram
        size_t activeNeurons = brain->getActiveNeuronCount();
        if (activeNeurons > 0) {
            viz << "Firing Rate Histogram (bins):\n";
            
            // This would require more detailed neuron data
            // Placeholder visualization
            for (size_t i = 0; i < std::min(size_t(10), width); ++i) {
                viz << "  [";
                // Simple placeholder
                viz << std::string(i % 5 + 1, '=') << std::string(4 - (i % 5 + 1), ' ')
                    << "] " << (i * 5) << " Hz\n";
            }
        }
        
        viz << "\n=== End Visualization ===\n";
        return viz.str();
    }
    
    // Export data for external visualization tools
    static bool exportForExternalTools(std::shared_ptr<Brain> brain, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for export: " + filepath);
            return false;
        }
        
        // Export in simple CSV format
        file << "step,spike_count,active_neurons,firing_neurons,avg_firing_rate\n";
        
        // Run a few steps and export data
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            
            file << step << ","
                 << brain->getTotalSpikeCount() << ","
                 << brain->getActiveNeuronCount() << ","
                 << brain->getFiringNeuronCount() << ","
                 << brain->getAverageFiringRate() << "\n";
        }
        
        file.close();
        return true;
    }
    
    // Generate performance metrics report
    static std::string generatePerformanceReport(std::shared_ptr<Brain> brain) {
        std::stringstream report;
        report << "=== Performance Metrics Report ===\n";
        report << "Brain Performance Analysis:\n\n";
        
        report << "Network Statistics:\n";
        report << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        report << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        report << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        report << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        report << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        report << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        report << "\nActivity Metrics:\n";
        report << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        report << "  Spike System Pending: " << brain->getSpikeSystem()->getPendingSpikeCount() << "\n";
        report << "  Spike System Delayed: " << brain->getSpikeSystem()->getPendingDelayedCount() << "\n";
        
        report << "\n=== End Report ===\n";
        
        return report.str();
    }
};

// Main function to register all enhanced bindings
void registerEnhancedBindings(py::module& m) {
    // Register EnhancedConfig
    py::class_<EnhancedConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("loadFromMultipleFiles", &EnhancedConfig::loadFromMultipleFiles)
        .def("saveToPrettyJSON", &EnhancedConfig::saveToPrettyJSON)
        .def("getValue", [](EnhancedConfig& self, const std::string& key, auto defaultValue) {
            return self.getValue<T>(key, defaultValue);
        })
        .def("setValues", &EnhancedConfig::setValues)
        .def("validate", &EnhancedConfig::validate)
        .def("exportToJSON", &EnhancedConfig::exportToJSON)
        .def("importFromJSON", &EnhancedConfig::importFromJSON);
    
    // Register EnhancedBrainUtils
    py::class_<EnhancedBrainUtils>(m, "EnhancedBrainUtils")
        .def_static("batchInjectCurrent", &EnhancedBrainUtils::batchInjectCurrent)
        .def_static("batchModifySynapses", &EnhancedBrainUtils::batchModifySynapses)
        .def_static("analyzeNetwork", &EnhancedBrainUtils::analyzeNetwork)
        .def_static("runMultiStepSimulation", &EnhancedBrainUtils::runMultiStepSimulation)
        .def_static("saveBrainState", &EnhancedBrainUtils::saveBrainState)
        .def_static("loadBrainState", &EnhancedBrainUtils::loadBrainState)
        .def_static("createCheckpoint", &EnhancedBrainUtils::createCheckpoint)
        .def_static("restoreCheckpoint", &EnhancedBrainUtils::restoreCheckpoint);
    
    // Register EnhancedAgentUtils
    py::class_<EnhancedAgentUtils>(m, "EnhancedAgentUtils")
        .def_static("createAgent", &EnhancedAgentUtils::createAgent)
        .def_static("runEpisode", &EnhancedAgentUtils::runEpisode)
        .def_static("runMultiAgentTraining", &EnhancedAgentUtils::runMultiAgentTraining);
    
    // Register EnhancedSimulationRunner
    py::class_<EnhancedSimulationRunner>(m, "EnhancedSimulationRunner")
        .def_static("runComparativeSimulation", &EnhancedSimulationRunner::runComparativeSimulation)
        .def_static("runParameterSweep", &EnhancedSimulationRunner::runParameterSweep)
        .def("RealTimeMonitor", py::class_<EnhancedSimulationRunner::RealTimeMonitor>(m, "RealTimeMonitor")
            .def(py::init<std::shared_ptr<Brain>, size_t>())
            .def("update", &EnhancedSimulationRunner::RealTimeMonitor::update)
            .def("recordData", &EnhancedSimulationRunner::RealTimeMonitor::recordData)
            .def("getHistory", &EnhancedSimulationRunner::RealTimeMonitor::getHistory)
            .def("generateReport", &EnhancedSimulationRunner::RealTimeMonitor::generateReport));
    
    // Register EnhancedVisualizationUtils
    py::class_<EnhancedVisualizationUtils>(m, "EnhancedVisualizationUtils")
        .def_static("generateTextVisualization", &EnhancedVisualizationUtils::generateTextVisualization)
        .def_static("exportForExternalTools", &EnhancedVisualizationUtils::exportForExternalTools)
        .def_static("generatePerformanceReport", &EnhancedVisualizationUtils::generatePerformanceReport);
    
    // Factory functions for enhanced objects
    m.def("createEnhancedConfig", []() { return EnhancedConfig(); });
    m.def("createEnhancedBrainUtils", []() { return EnhancedBrainUtils(); });
    m.def("createEnhancedAgentUtils", []() { return EnhancedAgentUtils(); });
    m.def("createEnhancedSimulationRunner", []() { return EnhancedSimulationRunner(); });
    m.def("createEnhancedVisualizationUtils", []() { return EnhancedVisualizationUtils(); });
}

} // namespace pybindings

} // namespace nlm

#endif // BRAIN_HPP
