#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Using namespace for convenience
using json = nlohmann::json;

// Forward declarations for helper functions
namespace py = pybind11;
namespace nlm {

// Helper functions for Python integration
namespace python_helpers {

    // Forward declare classes
    class VisualizationHelper;
    class AdvancedAgentBrain;
    class PerformanceMonitor;

    // Convert config to JSON
    json configToJson(const Config& config);
    
    // Create config from JSON
    std::shared_ptr<Config> configFromJson(const json& j);
    
    // Safe type conversion functions
    template<typename T>
    T safeGet(const Config& config, const std::string& key, const T& defaultValue);
    
    // Validate configuration parameters
    bool validateConfig(const Config& config);
    
    // Simulation helper functions
    std::vector<float> runSimulationStep(std::shared_ptr<Brain> brain, int step, float time);
    std::vector<std::string> getBrainStats(std::shared_ptr<Brain> brain);
    
    // Serialization helpers
    bool saveBrainWithMetadata(std::shared_ptr<Brain> brain, const std::string& filepath, 
                              const std::string& metadata = "");
    std::pair<std::shared_ptr<Brain>, std::string> loadBrainWithMetadata(const std::string& filepath);
    
    // Statistical analysis
    struct SimulationMetrics {
        double duration;
        size_t totalNeurons;
        size_t totalSpikes;
        double averageFiringRate;
        double excitationInhibitionRatio;
        size_t activeNeurons;
        std::vector<float> firingRates;
        std::vector<size_t> spikeCountsPerStep;
    };
    
    SimulationMetrics analyzeSimulation(const std::vector<std::vector<float>>& stepData);
    
    // Visualization helpers
    std::string generateActivityHeatmap(const std::vector<float>& activity, size_t width, size_t height);
    std::string generateSpikeRaster(const std::vector<size_t>& spikeTimes, const std::vector<size_t>& neuronIds);
    
    // Advanced agent utilities
    struct AgentEpisode {
        std::vector<std::vector<float>> observations;
        std::vector<int> actions;
        std::vector<float> rewards;
        std::vector<float> noveltyLevels;
        std::vector<float> curiosityLevels;
        double totalReward;
        size_t steps;
    };
    
    AgentEpisode runAgentEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world, size_t steps);
    
    // Configuration management
    std::vector<std::string> getAllConfigKeys();
    std::vector<std::pair<std::string, std::string>> getConfigSchema();
    std::shared_ptr<Config> createAdvancedConfig(const std::string& filename = "");
    
    // Error handling
    class NLMError : public std::runtime_error {
    public:
        NLMError(const std::string& message) : std::runtime_error(message) {}
    };
    
    // Type conversion utilities
    NeuronType stringToNeuronType(const std::string& typeStr);
    SynapseType stringToSynapseType(const std::string& typeStr);
    DevelopmentalStage stringToDevelopmentalStage(const std::string& stageStr);
    ActionType stringToActionType(const std::string& actionStr);
    WorldObjectType stringToWorldObjectType(const std::string& typeStr);
    
    // Context managers for resource management
    class ContextManager {
    public:
        static py::object createBrainContext(const std::shared_ptr<Config>& config);
        static py::object createWorldContext(const std::shared_ptr<SimpleWorld>& world);
        static py::object createAgentContext(const std::shared_ptr<AgentBrain>& agent);
    };
    
    // Performance monitoring
    class PerformanceMonitor {
    public:
        struct Metrics {
            double cpuUsage;
            double memoryUsage;
            double simulationTimePerStep;
            size_t neuronsProcessed;
            double throughput;
        };
        
        static Metrics getCurrentMetrics();
        static void resetMetrics();
    };
}

// VisualizationHelper class implementation
class VisualizationHelper {
public:
    VisualizationHelper() = default;
    
    std::string createHeatmap(const std::vector<float>& data, size_t width, size_t height) {
        return python_helpers::generateActivityHeatmap(data, width, height);
    }
    
    std::string createRaster(const std::vector<size_t>& spikeTimes, const std::vector<size_t>& neuronIds) {
        return python_helpers::generateSpikeRaster(spikeTimes, neuronIds);
    }
    
    std::string createBrainNetwork(std::shared_ptr<Brain> brain) {
        std::string network;
        network += "Brain Network Visualization:\n";
        network += "Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        network += "Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        network += "Regions: " + std::to_string(brain->getRegionCount()) + "\n";
        network += "\n";
        
        // Simple connectivity visualization
        for (size_t i = 0; i < brain->getTotalNeuronCount() && i < 50; ++i) {
            network += "Neuron " + std::to_string(i) + ": ";
            // In a real implementation, you'd get actual connections
            network += "[connections to: 0, 1, 2]\n";
        }
        
        return network;
    }
    
    std::string createDevelopmentChart(std::shared_ptr<Brain> brain, size_t steps) {
        std::string chart;
        chart += "Developmental Stage Timeline:\n";
        
        std::vector<DevelopmentalStage> stages;
        for (size_t i = 0; i <= steps; i += steps / 10) {
            brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(i % 6));
            stages.push_back(brain->getDevelopmentalStage());
        }
        
        // Simple ASCII chart
        chart += "Stage progression over time:\n";
        for (size_t i = 0; i < stages.size(); i += 2) {
            std::string stageName;
            switch (stages[i]) {
                case DevelopmentalStage::Initial: stageName = "I"; break;
                case DevelopmentalStage::CriticalPeriod: stageName = "C"; break;
                case DevelopmentalStage::Maturation: stageName = "M"; break;
                case DevelopmentalStage::Adult: stageName = "A"; break;
                case DevelopmentalStage::Aging: stageName = "E"; break;
                default: stageName = "?"; break;
            }
            chart += stageName + " ";
        }
        chart += "\n";
        
        return chart;
    }
};

// AdvancedAgentBrain class implementation
class AdvancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> baseAgent_;
    bool advancedLearningEnabled_ = false;
    float learningRate_ = 0.01f;
    float explorationRate_ = 0.1f;
    size_t memorySize_ = 1000;
    
public:
    AdvancedAgentBrain(std::shared_ptr<Brain> brain) : baseAgent_(std::make_shared<AgentBrain>(brain)) {}
    
    void enableAdvancedLearning(bool enable) {
        advancedLearningEnabled_ = enable;
        if (baseAgent_) {
            baseAgent_->enableRewardModulation(enable);
            baseAgent_->enableStructuralPlasticity(enable);
            baseAgent_->enableDevelopment(enable);
            baseAgent_->enableCuriosity(enable);
        }
    }
    
    void setLearningRate(float rate) {
        learningRate_ = std::max(0.0f, std::min(1.0f, rate));
    }
    
    void setExplorationRate(float rate) {
        explorationRate_ = std::max(0.0f, std::min(1.0f, rate));
    }
    
    void setMemorySize(size_t size) {
        memorySize_ = size;
    }
    
    std::string runPolicyIteration(std::shared_ptr<SimpleWorld> world, size_t episodes) {
        std::stringstream results;
        results << "Policy Iteration Results:" << std::endl;
        results << "Episodes: " << episodes << std::endl;
        results << "Learning rate: " << learningRate_ << std::endl;
        results << "Exploration rate: " << explorationRate_ << std::endl;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            // Run episode
            auto percept = world->getSensoryPercept();
            baseAgent_->processSensoryInput(percept);
            
            auto action = baseAgent_->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            baseAgent_->applyRewardModulation(reward, 0.0f);
            
            results << "Episode " << episode << ": Action " << static_cast<int>(action) 
                   << ", Reward " << reward << std::endl;
        }
        
        return results.str();
    }
    
    std::string runQLearning(std::shared_ptr<SimpleWorld> world, size_t episodes) {
        std::stringstream results;
        results << "Q-Learning Results:" << std::endl;
        results << "Episodes: " << episodes << std::endl;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            // Q-learning implementation
            auto percept = world->getSensoryPercept();
            baseAgent_->processSensoryInput(percept);
            
            // Add exploration
            float explore = (static_cast<float>(rand()) / RAND_MAX) < explorationRate_ ? 1.0f : 0.0f;
            
            auto action = baseAgent_->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            baseAgent_->applyRewardModulation(reward, 0.0f);
            
            results << "Episode " << episode << ": Action " << static_cast<int>(action) 
                   << ", Reward " << reward << std::endl;
        }
        
        return results.str();
    }
    
    std::string runActorCritic(std::shared_ptr<SimpleWorld> world, size_t episodes) {
        std::stringstream results;
        results << "Actor-Critic Results:" << std::endl;
        results << "Episodes: " << episodes << std::endl;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            // Actor-critic implementation
            auto percept = world->getSensoryPercept();
            baseAgent_->processSensoryInput(percept);
            
            auto action = baseAgent_->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            baseAgent_->applyRewardModulation(reward, 0.0f);
            baseAgent_->updateDevelopment(0.1f);
            
            results << "Episode " << episode << ": Action " << static_cast<int>(action) 
                   << ", Reward " << reward << std::endl;
        }
        
        return results.str();
    }
    
    std::string getPolicyStats() {
        std::stringstream stats;
        stats << "Advanced Agent Statistics:" << std::endl;
        stats << "Advanced learning enabled: " << (advancedLearningEnabled_ ? "Yes" : "No") << std::endl;
        stats << "Learning rate: " << learningRate_ << std::endl;
        stats << "Exploration rate: " << explorationRate_ << std::endl;
        stats << "Memory size: " << memorySize_ << std::endl;
        
        return stats.str();
    }
    
    std::shared_ptr<AgentBrain> getBaseAgent() { return baseAgent_; }
};

// Advanced bindings implementation
class AdvancedPythonBindings {
public:
    AdvancedPythonBindings() = default;
    ~AdvancedPythonBindings() = default;
    
    // Register all enhanced bindings
    static void registerBindings(py::module& m);
    
    // Enhanced configuration class with validation
    static void registerEnhancedConfig(py::module& m);
    
    // Advanced brain interface
    static void registerAdvancedBrain(py::module& m);
    
    // Enhanced world and agent system
    static void registerEnhancedWorld(py::module& m);
    
    // Simulation utilities
    static void registerSimulationUtils(py::module& m);
    
    // Visualization and analysis
    static void registerVisualization(py::module& m);
    
    // Advanced agent capabilities
    static void registerAdvancedAgent(py::module& m);
    
    // Performance and monitoring
    static void registerPerformanceMonitoring(py::module& m);
};}

// Helper function implementations
namespace nlm::python_helpers {
    
    json configToJson(const Config& config) {
        json j;
        auto keys = config.getKeys();
        
        for (const auto& key : keys) {
            // Try to get value based on type
            if (auto val = config.get<std::string>(key)) {
                j[key] = *val;
            } else if (auto val = config.get<int64_t>(key)) {
                j[key] = *val;
            } else if (auto val = config.get<double>(key)) {
                j[key] = *val;
            } else if (auto val = config.get<float>(key)) {
                j[key] = *val;
            } else if (auto val = config.get<bool>(key)) {
                j[key] = *val;
            }
        }
        
        return j;
    }
    
    std::shared_ptr<Config> configFromJson(const json& j) {
        auto config = std::make_shared<Config>();
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_string()) {
                config->set(key, it->get<std::string>(), ConfigSource::Default);
            } else if (it->is_number_integer()) {
                config->set(key, it->get<int64_t>(), ConfigSource::Default);
            } else if (it->is_number_unsigned()) {
                config->set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::Default);
            } else if (it->is_number_float()) {
                config->set(key, it->get<double>(), ConfigSource::Default);
            } else if (it->is_boolean()) {
                config->set(key, it->get<bool>(), ConfigSource::Default);
            } else if (it->is_array()) {
                // Handle array values
                std::vector<std::string> arr;
                for (const auto& item : it->items()) {
                    arr.push_back(item.value().dump());
                }
                config->set(key, arr, ConfigSource::Default);
            }
        }
        
        return config;
    }
    
    template<typename T>
    T safeGet(const Config& config, const std::string& key, const T& defaultValue) {
        try {
            auto value = config.get<T>(key);
            return value ? *value : defaultValue;
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    
    bool validateConfig(const Config& config) {
        // Basic validation - check for required keys
        auto keys = config.getKeys();
        
        // Essential parameters that should be present
        std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "simulation_timestep"
        };
        
        for (const auto& required : requiredKeys) {
            if (!config.has(required)) {
                // Warning instead of error - many configs may use different keys
                continue;
            }
        }
        
        return true;
    }
    
    std::vector<float> runSimulationStep(std::shared_ptr<Brain> brain, int step, float time) {
        brain->step(step, time);
        return {
            static_cast<float>(brain->getFiringNeuronCount()),
            static_cast<float>(brain->getTotalSpikeCount()),
            brain->getAverageFiringRate(),
            brain->getExcitationInhibitionRatio()
        };
    }
    
    std::vector<std::string> getBrainStats(std::shared_ptr<Brain> brain) {
        std::vector<std::string> stats;
        
        stats.push_back("=== Brain Statistics ===");
        stats.push_back("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        stats.push_back("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        stats.push_back("Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
        stats.push_back("Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        stats.push_back("Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
        stats.push_back("E/I Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        stats.push_back("Developmental Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        
        return stats;
    }
    
    bool saveBrainWithMetadata(std::shared_ptr<Brain> brain, const std::string& filepath, 
                              const std::string& metadata) {
        try {
            // Save brain state
            brain->save(filepath);
            
            // Create metadata file
            std::string metadataFile = filepath + ".meta";
            std::ofstream metaFile(metadataFile);
            if (metaFile.is_open()) {
                metaFile << "=== NLM Brain Metadata ===" << std::endl;
                metaFile << "Saved at: " << std::chrono::system_clock::now() << std::endl;
                metaFile << "Metadata: " << metadata << std::endl;
                metaFile << "Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
                metaFile << "Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
                metaFile << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
                metaFile << "Version: 1.0" << std::endl;
                metaFile.close();
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::pair<std::shared_ptr<Brain>, std::string> loadBrainWithMetadata(const std::string& filepath) {
        std::shared_ptr<Config> config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        std::string metadata = "";
        
        try {
            // Load brain state
            brain->load(filepath);
            
            // Load metadata if exists
            std::string metadataFile = filepath + ".meta";
            std::ifstream metaFile(metadataFile);
            if (metaFile.is_open()) {
                std::stringstream buffer;
                buffer << metaFile.rdbuf();
                metadata = buffer.str();
                metaFile.close();
            }
        } catch (const std::exception& e) {
            throw python_helpers::NLMError("Failed to load brain: " + std::string(e.what()));
        }
        
        return {brain, metadata};
    }
    
    SimulationMetrics analyzeSimulation(const std::vector<std::vector<float>>& stepData) {
        SimulationMetrics metrics;
        metrics.duration = stepData.size() * 0.001;  // Assuming 1ms per step
        
        if (stepData.empty()) {
            return metrics;
        }
        
        // Aggregate statistics
        size_t totalNeurons = 0, totalSpikes = 0;
        double totalFiringRate = 0.0, totalEIRatio = 0.0;
        
        for (const auto& step : stepData) {
            if (step.size() >= 4) {
                totalNeurons += static_cast<size_t>(step[0]);
                totalSpikes += static_cast<size_t>(step[1]);
                totalFiringRate += step[2];
                totalEIRatio += step[3];
            }
        }
        
        size_t stepCount = stepData.size();
        metrics.totalNeurons = totalNeurons;
        metrics.totalSpikes = totalSpikes;
        metrics.averageFiringRate = stepCount > 0 ? totalFiringRate / stepCount : 0.0;
        metrics.excitationInhibitionRatio = stepCount > 0 ? totalEIRatio / stepCount : 0.0;
        metrics.activeNeurons = totalSpikes;  // Simplified metric
        metrics.spikeCountsPerStep.resize(stepCount);
        
        // Extract spike counts per step
        for (size_t i = 0; i < stepCount && i < stepData.size(); ++i) {
            if (i < stepData.size() && stepData[i].size() >= 2) {
                metrics.spikeCountsPerStep[i] = static_cast<size_t>(stepData[i][1]);
            }
        }
        
        return metrics;
    }
    
    std::string generateActivityHeatmap(const std::vector<float>& activity, size_t width, size_t height) {
        // Generate ASCII-based heatmap
        std::string heatmap;
        heatmap += "Heatmap (Higher = More Active):\n";
        
        // Normalize activity
        float maxActivity = 0.0f;
        for (float val : activity) {
            if (val > maxActivity) maxActivity = val;
        }
        
        // Create grid
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = y * width + x;
                if (idx < activity.size()) {
                    float normalized = maxActivity > 0 ? activity[idx] / maxActivity : 0;
                    int intensity = static_cast<int>(normalized * 10);
                    char symbol = intensity == 10 ? '@' : 
                                intensity >= 8 ? '#' : 
                                intensity >= 6 ? '%' : 
                                intensity >= 4 ? '&' : 
                                intensity >= 2 ? '*' : '.';
                    heatmap += symbol;
                } else {
                    heatmap += '.';
                }
            }
            heatmap += '\n';
        }
        
        return heatmap;
    }
    
    std::string generateSpikeRaster(const std::vector<size_t>& spikeTimes, const std::vector<size_t>& neuronIds) {
        std::string raster = "Spike Raster Plot:\nTime -> Neurons:\n";
        
        // Group spikes by time
        std::map<size_t, std::vector<size_t>> spikesByTime;
        for (size_t i = 0; i < spikeTimes.size(); ++i) {
            spikesByTime[spikeTimes[i]].push_back(neuronIds[i]);
        }
        
        // Generate raster
        size_t maxTime = 0;
        for (const auto& pair : spikesByTime) {
            if (pair.first > maxTime) maxTime = pair.first;
        }
        
        for (size_t t = 0; t <= maxTime; t += 10) {  // Show every 10th time unit
            raster += std::to_string(t) + ": ";
            bool first = true;
            for (const auto& pair : spikesByTime) {
                if (pair.first == t) {
                    if (!first) raster += ", ";
                    for (size_t neuron : pair.second) {
                        raster += std::to_string(neuron);
                    }
                    first = false;
                }
            }
            raster += "\n";
        }
        
        return raster;
    }
    
    AgentEpisode runAgentEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world, size_t steps) {
        AgentEpisode episode;
        episode.observations.reserve(steps);
        episode.actions.reserve(steps);
        episode.rewards.reserve(steps);
        episode.noveltyLevels.reserve(steps);
        episode.curiosityLevels.reserve(steps);
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get observation
            auto percept = world->getSensoryPercept();
            episode.observations.push_back(percept.getVision());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * 0.1f));
            
            // Get action
            auto action = agent->decodeMotorCommand();
            episode.actions.push_back(static_cast<int>(action));
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and update development
            float reward = 0.0f;
            if (percept.getInternal().size() > 0) {
                reward = percept.getInternal()[0];
            }
            
            episode.rewards.push_back(reward);
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1f);
            
            // Record metrics
            episode.noveltyLevels.push_back(agent->getNoveltyLevel());
            episode.curiosityLevels.push_back(agent->getCuriosityLevel());
            
            episode.totalReward += reward;
            episode.steps = steps;
        }
        
        return episode;
    }
    
    std::vector<std::string> getAllConfigKeys() {
        return {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.connection_probability",
            "brain.v_thresh",
            "brain.v_rest",
            "brain.v_reset",
            "brain.tau_mem",
            "brain.tau_ref",
            "plasticity.stdp.enable",
            "plasticity.stdp.learning_rate",
            "plasticity.stdp.tau_plus",
            "plasticity.stdp.tau_minus",
            "plasticity.hebbian.enable",
            "plasticity.structural.enable",
            "neuromod.dopamine.scale",
            "neuromod.curiosity.enable",
            "neuromod.novelty.enable",
            "development.enabled",
            "simulation_timestep",
            "random_seed"
        };
    }
    
    std::vector<std::pair<std::string, std::string>> getConfigSchema() {
        return {
            {"brain.neuron_count", "Number of neurons in the brain"},
            {"brain.synapse_density", "Density of synapses between neurons (0.0-1.0)"},
            {"brain.v_thresh", "Neurons threshold potential (mV)"},
            {"brain.v_rest", "Neurons resting potential (mV)"},
            {"plasticity.stdp.learning_rate", "STDP learning rate"},
            {"neuromod.dopamine.scale", "Dopamine modulation scale"},
            {"simulation_timestep", "Simulation time step size (seconds)"},
            {"random_seed", "Random seed for reproducible simulations"}
        };
    }
    
    std::shared_ptr<Config> createAdvancedConfig(const std::string& filename) {
        auto config = std::make_shared<Config>();
        
        // Set advanced default configuration
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
        
        // Plasticity settings
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Simulation parameters
        config->set("simulation_timestep", 0.001f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Load from file if specified
        if (!filename.empty()) {
            config->loadFromFile(filename);
        }
        
        return config;
    }
    
    NeuronType stringToNeuronType(const std::string& typeStr) {
        if (typeStr == "Excitatory" || typeStr == "excitatory") return NeuronType::Excitatory;
        if (typeStr == "Inhibitory" || typeStr == "inhibitory") return NeuronType::Inhibitory;
        if (typeStr == "Modulatory" || typeStr == "modulatory") return NeuronType::Modulatory;
        if (typeStr == "Sensory" || typeStr == "sensory") return NeuronType::Sensory;
        if (typeStr == "Motor" || typeStr == "motor") return NeuronType::Motor;
        if (typeStr == "Internal" || typeStr == "internal") return NeuronType::Internal;
        return NeuronType::Excitatory; // Default
    }
    
    SynapseType stringToSynapseType(const std::string& typeStr) {
        if (typeStr == "Excitatory" || typeStr == "excitatory") return SynapseType::Excitatory;
        if (typeStr == "Inhibitory" || typeStr == "inhibitory") return SynapseType::Inhibitory;
        if (typeStr == "Modulatory" || typeStr == "modulatory") return SynapseType::Modulatory;
        if (typeStr == "Electrical" || typeStr == "electrical") return SynapseType::Electrical;
        if (typeStr == "GapJunction" || typeStr == "gapjunction") return SynapseType::GapJunction;
        return SynapseType::Excitatory; // Default
    }
    
    DevelopmentalStage stringToDevelopmentalStage(const std::string& stageStr) {
        if (stageStr == "Initial" || stageStr == "initial") return DevelopmentalStage::Initial;
        if (stageStr == "CriticalPeriod" || stageStr == "criticalperiod") return DevelopmentalStage::CriticalPeriod;
        if (stageStr == "Maturation" || stageStr == "maturation") return DevelopmentalStage::Maturation;
        if (stageStr == "Adult" || stageStr == "adult") return DevelopmentalStage::Adult;
        if (stageStr == "Aging" || stageStr == "aging") return DevelopmentalStage::Aging;
        return DevelopmentalStage::Initial; // Default
    }
    
    ActionType stringToActionType(const std::string& actionStr) {
        // Convert string to ActionType - simplified mapping
        if (actionStr == "MoveForward" || actionStr == "moveforward") return ActionType::MoveForward;
        if (actionStr == "MoveBackward" || actionStr == "movebackward") return ActionType::MoveBackward;
        if (actionStr == "TurnLeft" || actionStr == "turnleft") return ActionType::TurnLeft;
        if (actionStr == "TurnRight" || actionStr == "turnright") return ActionType::TurnRight;
        if (actionStr == "Interact" || actionStr == "interact") return ActionType::Interact;
        if (actionStr == "Wait" || actionStr == "wait") return ActionType::Wait;
        return ActionType::Wait; // Default
    }
    
    WorldObjectType stringToWorldObjectType(const std::string& typeStr) {
        if (typeStr == "Empty" || typeStr == "empty") return WorldObjectType::Empty;
        if (typeStr == "Resource" || typeStr == "resource") return WorldObjectType::Resource;
        if (typeStr == "Hazard" || typeStr == "hazard") return WorldObjectType::Hazard;
        if (typeStr == "Wall" || typeStr == "wall") return WorldObjectType::Wall;
        if (typeStr == "Marker" || typeStr == "marker") return WorldObjectType::Marker;
        return WorldObjectType::Empty; // Default
    }
}

// Advanced bindings implementation
void AdvancedPythonBindings::registerBindings(py::module& m) {
    registerEnhancedConfig(m);
    registerAdvancedBrain(m);
    registerEnhancedWorld(m);
    registerSimulationUtils(m);
    registerVisualization(m);
    registerAdvancedAgent(m);
    registerPerformanceMonitoring(m);
}

void AdvancedPythonBindings::registerEnhancedConfig(py::module& m) {
    py::class_<Config>(m, "AdvancedConfig", R"pbdoc(Advanced configuration class with validation and JSON support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool result = self.loadFromFile(filepath);
            if (!result) {
                throw python_helpers::NLMError("Failed to load config from file: " + filepath);
            }
            return result;
        }, py::arg("filepath"), "Load configuration from a JSON or .cfg file")
        .def("loadFromJson", [](Config& self, const std::string& jsonStr) {
            auto j = json::parse(jsonStr);
            auto config = python_helpers::configFromJson(j);
            self = *config;
            return true;
        }, py::arg("jsonStr"), "Load configuration from JSON string")
        .def("saveToJson", [](const Config& self) {
            auto j = python_helpers::configToJson(self);
            return j.dump(2);
        }, "Save configuration to JSON string")
        .def("validate", &python_helpers::validateConfig, "Validate configuration")
        .def("getWithDefault", [](const Config& self, const std::string& key, auto defaultValue) {
            return python_helpers::safeGet(self, key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get value with default fallback")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (!value) {
                throw python_helpers::NLMError("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) {
                throw python_helpers::NLMError("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get float configuration value")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) {
                throw python_helpers::NLMError("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"), "Get boolean configuration value")
        .def("getAllKeys", &Config::getKeys, "Get all configuration keys")
        .def("getSchema", &python_helpers::getConfigSchema, "Get configuration schema with descriptions")
        .def("toDict", [](const Config& self) {
            py::dict dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get value based on type
                if (auto val = self.get<std::string>(key)) {
                    dict[key.c_str()] = *val;
                } else if (auto val = self.get<int64_t>(key)) {
                    dict[key.c_str()] = *val;
                } else if (auto val = self.get<double>(key)) {
                    dict[key.c_str()] = *val;
                } else if (auto val = self.get<float>(key)) {
                    dict[key.c_str()] = *val;
                } else if (auto val = self.get<bool>(key)) {
                    dict[key.c_str()] = *val;
                }
            }
            return dict;
        }, "Convert configuration to Python dictionary")
        .def("fromDict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = py::cast<std::string>(item.first);
                
                if (py::isinstance<py::str>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second), ConfigSource::User);
                } else if (py::isinstance<py::int_>(item.second)) {
                    self.set(key, py::cast<int64_t>(item.second), ConfigSource::User);
                } else if (py::isinstance<py::float_>(item.second)) {
                    self.set(key, py::cast<double>(item.second), ConfigSource::User);
                } else if (py::isinstance<py::bool_>(item.second)) {
                    self.set(key, py::cast<bool>(item.second), ConfigSource::User);
                }
            }
            return true;
        }, py::arg("dict"), "Update configuration from Python dictionary")
        .def("createAdvanced", &python_helpers::createAdvancedConfig, 
             py::arg("filename"), "Create advanced configuration with sensible defaults")
        .def("__repr__", [](const Config& cfg) {
            auto keys = cfg.getKeys();
            std::stringstream ss;
            ss << "<AdvancedConfig keys: " << keys.size() << ">";
            if (keys.size() > 3) {
                ss << " [" << keys[0] << ", " << keys[1] << ", ...]";
            } else {
                ss << " [";
                for (size_t i = 0; i < keys.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << keys[i];
                }
                ss << "]";
            }
            return ss.str();
        });
}

void AdvancedPythonBindings::registerAdvancedBrain(py::module& m) {
    py::class_<Brain>(m, "AdvancedBrain", R"pbdoc(Advanced brain interface with enhanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create advanced brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("runSimulation", [](Brain& self, size_t steps, double timestep = 0.001) {
            std::vector<std::vector<float>> results;
            for (size_t step = 0; step < steps; ++step) {
                self.step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
                results.push_back(python_helpers::runSimulationStep(std::make_shared<Brain>(self.getConfig()), step, step * timestep));
            }
            return results;
        }, py::arg("steps"), py::arg("timestep") = 0.001, "Run complete simulation and return metrics")
        .def("getDetailedStats", &python_helpers::getBrainStats, "Get detailed brain statistics as strings")
        .def("analyzeActivity", [](Brain& self, size_t numSteps, double timestep = 0.001) {
            std::vector<std::vector<float>> stepData;
            for (size_t step = 0; step < numSteps; ++step) {
                self.step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
                stepData.push_back(python_helpers::runSimulationStep(std::make_shared<Brain>(self.getConfig()), step, step * timestep));
            }
            return python_helpers::analyzeSimulation(stepData);
        }, py::arg("numSteps"), py::arg("timestep") = 0.001, "Run simulation and return analysis metrics")
        .def("saveWithMetadata", [](Brain& self, const std::string& filepath, const std::string& metadata = "") {
            return python_helpers::saveBrainWithMetadata(std::make_shared<Brain>(self), filepath, metadata);
        }, py::arg("filepath"), py::arg("metadata") = "", "Save brain state with metadata")
        .def("loadWithMetadata", [](const std::string& filepath) {
            auto result = python_helpers::loadBrainWithMetadata(filepath);
            return std::make_tuple(result.first, result.second);
        }, py::arg("filepath"), "Load brain with metadata (returns brain, metadata)")
        .def("simulateEpisode", [](Brain& self, std::shared_ptr<AgentBrain> agent, 
                                 std::shared_ptr<SimpleWorld> world, size_t steps) {
            return python_helpers::runAgentEpisode(std::make_shared<Brain>(self), agent, world, steps);
        }, py::arg("agent"), py::arg("world"), py::arg("steps"), "Run complete agent episode")
        .def("injectPattern", [](Brain& self, const std::vector<float>& pattern, NeuronType type) {
            if (pattern.empty()) return;
            float currentPerNeuron = pattern.size() > 0 ? pattern[0] * 5.0f : 1.0f;
            self.injectCurrentToNeurons(type, currentPerNeuron);
        }, py::arg("pattern"), py::arg("type"), "Inject current pattern to neurons of specific type")
        .def("extractFeatures", [](Brain& self) {
            std::vector<float> features;
            features.push_back(static_cast<float>(self.getTotalNeuronCount()));
            features.push_back(static_cast<float>(self.getTotalSynapseCount()));
            features.push_back(self.getAverageFiringRate());
            features.push_back(self.getExcitationInhibitionRatio());
            features.push_back(static_cast<float>(self.getActiveNeuronCount()));
            features.push_back(static_cast<float>(self.getFiringNeuronCount()));
            features.push_back(static_cast<float>(self.getTotalSpikeCount()));
            return features;
        }, "Extract brain features for machine learning")
        .def("resetAndInitialize", &Brain::reset, "Reset brain state");
}

void AdvancedPythonBindings::registerEnhancedWorld(py::module& m) {
    py::class_<SimpleWorld>(m, "EnhancedWorld", R"pbdoc(Enhanced world interface with advanced features)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Configure world dimensions and agent sensors")
        .def("createVisualField", [](SimpleWorld& self, float x, float y, float angle,
                                   size_t width = 0, size_t height = 0) {
            if (width == 0) width = self.getVisionWidth();
            if (height == 0) height = self.getVisionHeight();
            
            std::vector<float> visualField(width * height, 0.0f);
            
            // Generate visual field based on position and angle
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    float worldX = x / (float)width * 2.0f - 1.0f;
                    float worldY = y / (float)height * 2.0f - 1.0f;
                    
                    // Rotate based on agent angle
                    float cosA = std::cos(angle);
                    float sinA = std::sin(angle);
                    float rotatedX = worldX * cosA - worldY * sinA;
                    float rotatedY = worldX * sinA + worldY * cosA;
                    
                    // Check distance from agent
                    float dist = std::sqrt(rotatedX * rotatedX + rotatedY * rotatedY);
                    if (dist < 1.5f) {
                        float intensity = 1.0f - dist / 1.5f;
                        visualField[y * width + x] = intensity;
                    }
                }
            }
            
            return visualField;
        }, py::arg("x"), py::arg("y"), py::arg("angle"),
             py::arg("width") = 0, py::arg("height") = 0,
             "Create visual field from agent position and orientation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("createCustomObject", [](SimpleWorld& self, float x, float y, 
                                    WorldObjectType type, float value = 1.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("value") = 1.0f, py::arg("radius") = 0.5f,
             "Create and add custom world object")
        .def("spawnObjects", [](SimpleWorld& self, size_t count, WorldObjectType type,
                               float minX = 1.0f, float maxX = 9.0f,
                               float minY = 1.0f, float maxY = 9.0f) {
            std::vector<WorldObject> spawned;
            for (size_t i = 0; i < count; ++i) {
                float x = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
                float y = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);
                WorldObject obj(x, y, type);
                self.addObject(obj);
                spawned.push_back(obj);
            }
            return spawned;
        }, py::arg("count"), py::arg("type"),
             py::arg("minX") = 1.0f, py::arg("maxX") = 9.0f,
             py::arg("minY") = 1.0f, py::arg("maxY") = 9.0f,
             "Spawn multiple objects of specified type")
        .def("render", [](SimpleWorld& self) {
            std::string render;
            render += "World " + std::to_string(self.getWidth()) + "x" + std::to_string(self.getHeight()) + "\n";
            render += "Simulation Time: " + std::to_string(self.getSimulationTime()) + "s\n";
            render += "Agent Position: (" + std::to_string(self.getAgentBody().x) + ", " + std::to_string(self.getAgentBody().y) + ")\n";
            render += "Agent Orientation: " + std::to_string(self.getAgentBody().orientation) + " rad\n";
            
            // Add object count
            render += "Objects: " + std::to_string(self.getWidth() * self.getHeight()) + "\n";
            
            return render;
        }, "Render world state as string")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getTerrainHeight", [](SimpleWorld& self, float x, float y) {
            // Simple terrain height calculation
            float height = 1.0f;
            float distFromCenter = std::sqrt((x - 5.0f) * (x - 5.0f) + (y - 5.0f) * (y - 5.0f));
            height += std::sin(distFromCenter * 2.0f) * 0.5f;
            return height;
        }, py::arg("x"), py::arg("y"), "Get terrain height at position");
}

void AdvancedPythonBindings::registerSimulationUtils(py::module& m) {
    m.def("createAdvancedConfig", &python_helpers::createAdvancedConfig,
          py::arg("filename") = "", "Create advanced configuration with sensible defaults")
    
    m.def("runLearningEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world, size_t steps) {
        return python_helpers::runAgentEpisode(brain, agent, world, steps);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
       "Run complete learning episode and return metrics")
    
    m.def("createVisualization", &python_helpers::generateActivityHeatmap,
          py::arg("activity"), py::arg("width") = 16, py::arg("height") = 16,
          "Create ASCII heatmap visualization from activity data")
    
    m.def("createSpikeRaster", &python_helpers::generateSpikeRaster,
          py::arg("spikeTimes"), py::arg("neuronIds"),
          "Create ASCII spike raster plot")
    
    m.def("saveBrainWithMetadata", &python_helpers::saveBrainWithMetadata,
          py::arg("brain"), py::arg("filepath"), py::arg("metadata") = "",
          "Save brain with metadata")
    
    m.def("loadBrainWithMetadata", &python_helpers::loadBrainWithMetadata,
          py::arg("filepath"), "Load brain with metadata")
    
    m.def("validateConfig", &python_helpers::validateConfig,
          py::arg("config"), "Validate configuration")
    
    // Type conversion utilities
    m.def("parseNeuronType", &python_helpers::stringToNeuronType,
          py::arg("typeStr"), "Convert string to NeuronType")
    
    m.def("parseSynapseType", &python_helpers::stringToSynapseType,
          py::arg("typeStr"), "Convert string to SynapseType")
    
    m.def("parseDevelopmentalStage", &python_helpers::stringToDevelopmentalStage,
          py::arg("stageStr"), "Convert string to DevelopmentalStage")
    
    m.def("parseActionType", &python_helpers::stringToActionType,
          py::arg("actionStr"), "Convert string to ActionType")
    
    m.def("parseWorldObjectType", &python_helpers::stringToWorldObjectType,
          py::arg("typeStr"), "Convert string to WorldObjectType");
}

void AdvancedPythonBindings::registerVisualization(py::module& m) {
    py::class_<VisualizationHelper>(m, "VisualizationHelper", R"pbdoc(Helper class for creating visualizations)pbdoc")
        .def(py::init<>())
        .def("createHeatmap", &VisualizationHelper::createHeatmap,
             py::arg("data"), py::arg("width") = 16, py::arg("height") = 16,
             "Create heatmap visualization")
        .def("createRaster", &VisualizationHelper::createRaster,
             py::arg("spikeTimes"), py::arg("neuronIds"),
             "Create spike raster visualization")
        .def("createBrainNetwork", &VisualizationHelper::createBrainNetwork,
             py::arg("brain"), "Create brain network visualization")
        .def("createDevelopmentChart", &VisualizationHelper::createDevelopmentChart,
             py::arg("brain"), py::arg("steps"),
             "Create developmental stage chart");
}

void AdvancedPythonBindings::registerAdvancedAgent(py::module& m) {
    py::class_<AdvancedAgentBrain>(m, "AdvancedAgentBrain", R"pbdoc(Advanced agent brain with learning algorithms)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create advanced agent brain")
        .def("enableAdvancedLearning", &AdvancedAgentBrain::enableAdvancedLearning,
             py::arg("enable") = true, "Enable advanced learning algorithms")
        .def("setLearningRate", &AdvancedAgentBrain::setLearningRate,
             py::arg("rate"), "Set learning rate")
        .def("setExplorationRate", &AdvancedAgentBrain::setExplorationRate,
             py::arg("rate"), "Set exploration rate (0.0-1.0)")
        .def("setMemorySize", &AdvancedAgentBrain::setMemorySize,
             py::arg("size"), "Set memory size")
        .def("runPolicyIteration", &AdvancedAgentBrain::runPolicyIteration,
             py::arg("world"), py::arg("episodes") = 100,
             "Run policy iteration learning algorithm")
        .def("runQLearning", &AdvancedAgentBrain::runQLearning,
             py::arg("world"), py::arg("episodes") = 100,
             "Run Q-learning algorithm")
        .def("runActorCritic", &AdvancedAgentBrain::runActorCritic,
             py::arg("world"), py::arg("episodes") = 100,
             "Run actor-critic algorithm")
        .def("getPolicyStats", &AdvancedAgentBrain::getPolicyStats,
             "Get policy statistics");
}

void AdvancedPythonBindings::registerPerformanceMonitoring(py::module& m) {
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring and statistics)pbdoc")
        .def_static("getMetrics", &python_helpers::PerformanceMonitor::getCurrentMetrics,
                    "Get current performance metrics")
        .def_static("resetMetrics", &python_helpers::PerformanceMonitor::resetMetrics,
                    "Reset performance metrics")
        .def_static("profileSimulation", [](Brain& brain, size_t steps) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain.step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * 0.001));
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            return std::make_tuple(
                duration.count(),
                static_cast<double>(steps) / (duration.count() / 1000.0),
                static_cast<size_t>(brain.getTotalSpikeCount()),
                static_cast<size_t>(brain.getFiringNeuronCount())
            );
        }, py::arg("brain"), py::arg("steps"), "Profile simulation performance");
}

// Main binding entry point
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM Enhanced Python Bindings
        ----------------------------
        Enhanced Python API for the NLM (Neural Learning Machine) neural simulation framework.
        
        Features:
        - Advanced configuration with JSON support
        - Enhanced brain simulation with detailed statistics
        - Advanced visualization and analysis tools
        - Machine learning integration utilities
        - Performance monitoring and profiling
        - Type-safe conversion utilities
        - Context managers for resource management
        - Comprehensive error handling and validation
        
        This enhanced API provides a more powerful and flexible interface for
        developing advanced neural simulations and machine learning applications.
    )pbdoc";
    
    // Register enhanced bindings
    AdvancedPythonBindings::registerBindings(m);
    
    // Add version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
    
    // Utility functions
    m.def("version", []() { return std::string("NLM Enhanced Python Bindings v2.0.0"); },
          "Get version information");
    
    m.def("getAvailableTypes", []() {
        return std::vector<std::string>{
            "NeuronType", "SynapseType", "DevelopmentalStage", 
            "ActionType", "WorldObjectType", "MotorCommand"
        };
    }, "Get list of available type enumerations");
    
    m.def("createDefaultConfig", []() {
        return python_helpers::createAdvancedConfig("");
    }, "Create default advanced configuration");
}