#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <chrono>
#include <fstream>
#include <sstream>

namespace py = pybind11;
namespace nlm {

// Enhanced error handling with detailed Pythonic error messages
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& msg) : std::runtime_error(msg) {}
    NLMError(const std::string& context, const std::string& operation, const std::string& details)
        : std::runtime_error("NLM " + operation + " failed: " + context + ". " + details) {}
};

// Type checking and validation utilities
namespace validation {
    template<typename T>
    T getValidatedValue(const ConfigValue& value, const std::string& key) {
        try {
            if constexpr (std::is_same_v<T, int>) {
                if (auto* v = std::get_if<int>(&value)) return *v;
                if (auto* v = std::get_if<int64_t>(&value)) return static_cast<int>(*v);
                throw NLMError("Invalid type for key '" + key + "'", "type_check", 
                             "Expected int but got " + std::string(typeid(value).name()));
            }
            else if constexpr (std::is_same_v<T, double>) {
                if (auto* v = std::get_if<double>(&value)) return *v;
                if (auto* v = std::get_if<int>(&value)) return static_cast<double>(*v);
                if (auto* v = std::get_if<int64_t>(&value)) return static_cast<double>(*v);
                throw NLMError("Invalid type for key '" + key + "'", "type_check",
                             "Expected double but got " + std::string(typeid(value).name()));
            }
            else if constexpr (std::is_same_v<T, bool>) {
                if (auto* v = std::get_if<bool>(&value)) return *v;
                if (auto* v = std::get_if<int>(&value)) return *v != 0;
                throw NLMError("Invalid type for key '" + key + "'", "type_check",
                             "Expected bool but got " + std::string(typeid(value).name()));
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                if (auto* v = std::get_if<std::string>(&value)) return *v;
                if (auto* v = std::get_if<int>(&value)) return std::to_string(*v);
                if (auto* v = std::get_if<double>(&value)) return std::to_string(*v);
                throw NLMError("Invalid type for key '" + key + "'", "type_check",
                             "Expected string but got " + std::string(typeid(value).name()));
            }
            else if constexpr (std::is_same_v<T, std::vector<float>>) {
                if (auto* v = std::get_if<std::vector<float>>(&value)) return *v;
                if (auto* v = std::get_if<std::vector<double>>(&value)) {
                    std::vector<float> result(v->begin(), v->end());
                    return result;
                }
                throw NLMError("Invalid type for key '" + key + "'", "type_check",
                             "Expected vector<float> but got " + std::string(typeid(value).name()));
            }
            else {
                if (auto* v = std::get_if<T>(&value)) return *v;
                throw NLMError("Invalid type for key '" + key + "'", "type_check",
                             "Expected " + typeid(T).name() + " but got " + std::string(typeid(value).name()));
            }
        } catch (const NLMError& e) {
            throw;
        } catch (...) {
            throw NLMError("Invalid type for key '" + key + "'", "type_check",
                         "Cannot convert to target type");
        }
    }
    
    double clamp(double value, double min, double max) {
        return std::max(min, std::min(value, max));
    }
    
    std::string toPythonPath(const std::string& path) {
        std::string normalized = path;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        return normalized;
    }
}

// Enhanced data structures for result collection and analysis
class ResultCollector {
public:
    struct EpisodeResult {
        double startTime;
        double endTime;
        size_t steps;
        double totalReward;
        double maxReward;
        double minReward;
        double averageReward;
        double successProbability;
        std::vector<double> rewards;
        std::vector<std::string> actions;
        double explorationRate;
    };
    
    struct TrainingMetrics {
        double trainingAccuracy;
        double validationAccuracy;
        double loss;
        double learningRate;
        double entropy;
        size_t parameterCount;
        std::map<std::string, double> gradientStats;
    };
    
    ResultCollector() : totalEpisodes_(0), totalSteps_(0), totalReward_(0.0) {}
    
    void recordEpisode(const EpisodeResult& episode) {
        episodes_.push_back(episode);
        totalEpisodes_++;
        totalSteps_ += episode.steps;
        totalReward_ += episode.totalReward;
        
        // Update running averages
        if (episodes_.size() > 1) {
            runningAvgReward_ = (runningAvgReward_ * (episodes_.size() - 1) + episode.totalReward) / episodes_.size();
        } else {
            runningAvgReward_ = episode.totalReward;
        }
    }
    
    double getTotalReward() const { return totalReward_; }
    size_t getTotalEpisodes() const { return totalEpisodes_; }
    size_t getTotalSteps() const { return totalSteps_; }
    double getRunningAverageReward() const { return runningAvgReward_; }
    
    std::vector<EpisodeResult> getEpisodes() const { return episodes_; }
    EpisodeResult getLastEpisode() const { return episodes_.empty() ? EpisodeResult() : episodes_.back(); }
    
    // Analysis utilities
    double calculateSuccessRate(double threshold = 0.0) const {
        if (episodes_.empty()) return 0.0;
        size_t successCount = 0;
        for (const auto& episode : episodes_) {
            if (episode.totalReward > threshold) successCount++;
        }
        return static_cast<double>(successCount) / episodes_.size();
    }
    
    double calculateAverageEpisodeLength() const {
        if (episodes_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& episode : episodes_) {
            total += episode.steps;
        }
        return total / episodes_.size();
    }
    
    std::vector<double> getRewardHistory() const {
        std::vector<double> history;
        for (const auto& episode : episodes_) {
            history.push_back(episode.totalReward);
        }
        return history;
    }
    
    // Save results to file
    bool saveToJSON(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"total_episodes\": " << totalEpisodes_ << ",\n";
        file << "  \"total_steps\": " << totalSteps_ << ",\n";
        file << "  \"total_reward\": " << totalReward_ << ",\n";
        file << "  \"running_average_reward\": " << runningAvgReward_ << ",\n";
        file << "  \"episodes\": [\n";
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            const auto& episode = episodes_[i];
            file << "    {\n";
            file << "      \"start_time\": " << episode.startTime << ",\n";
            file << "      \"end_time\": " << episode.endTime << ",\n";
            file << "      \"steps\": " << episode.steps << ",\n";
            file << "      \"total_reward\": " << episode.totalReward << ",\n";
            file << "      \"max_reward\": " << episode.maxReward << ",\n";
            file << "      \"min_reward\": " << episode.minReward << ",\n";
            file << "      \"average_reward\": " << episode.averageReward << ",\n";
            file << "      \"success_probability\": " << episode.successProbability << ",\n";
            file << "      \"exploration_rate\": " << episode.explorationRate << "\n";
            file << "    }";
            if (i < episodes_.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ]\n";
        file << "}\n";
        return true;
    }
    
private:
    std::vector<EpisodeResult> episodes_;
    size_t totalEpisodes_;
    size_t totalSteps_;
    double totalReward_;
    double runningAvgReward_;
};

// Progress monitoring and callbacks
class ProgressMonitor {
public:
    using ProgressCallback = std::function<void(double, const std::string&)>;
    
    ProgressMonitor(double totalSteps) : totalSteps_(totalSteps), currentStep_(0), startTime_(std::chrono::steady_clock::now()) {}
    
    void update(double stepProgress, const std::string& message = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        currentStep_++;
        double overallProgress = static_cast<double>(currentStep_) / totalSteps_;
        
        if (callback_) {
            callback_(overallProgress, message);
        }
    }
    
    double getProgress() const {
        return totalSteps_ > 0 ? static_cast<double>(currentStep_) / totalSteps_ : 0.0;
    }
    
    double getElapsedTime() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
        return duration.count() / 1000.0;
    }
    
    void setCallback(const ProgressCallback& callback) {
        callback_ = callback;
    }
    
    void setUpdateInterval(size_t interval) {
        updateInterval_ = interval;
    }
    
private:
    double totalSteps_;
    size_t currentStep_;
    ProgressCallback callback_;
    size_t updateInterval_;
    std::chrono::steady_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

// Simulation configuration presets
namespace presets {
    std::shared_ptr<Config> createBehavioralPreset() {
        auto config = std::make_shared<Config>();
        
        // Behavioral learning parameters
        config->set("learning.rate", 0.01);
        config->set("learning.momentum", 0.9);
        config->set("exploration.epsilon", 0.1);
        config->set("exploration.epsilon_decay", 0.9995);
        config->set("exploration.epsilon_min", 0.01);
        config->set("reward.scaling", 1.0);
        config->set("reward.discount", 0.99);
        config->set("reward.accumulate", true);
        config->set("reward.clip_min", -1.0);
        config->set("reward.clip_max", 1.0);
        
        // Policy parameters
        config->set("policy.gaussian_noise", 0.1);
        config->set("policy.clip_range", 1.0);
        config->set("policy.entropy_coef", 0.01);
        config->set("policy.value_loss_coef", 0.5);
        
        return config;
    }
    
    std::shared_ptr<Config> createCognitivePreset() {
        auto config = std::make_shared<Config>();
        
        // Cognitive architecture parameters
        config->set("cognition.working_memory_size", 128);
        config->set("cognition.episodic_memory_capacity", 1000);
        config->set("cognition.associative_memory_threshold", 0.7);
        config->set("cognition.concept_learning_rate", 0.05);
        config->set("cognition.attention_span", 20);
        config->set("cognition.innovation_rate", 0.02);
        config->set("cognition.transference_strength", 0.3);
        
        // Prediction system parameters
        config->set("prediction.prediction_horizon", 10);
        config->set("prediction.error_threshold", 0.5);
        config->set("prediction.learning_rate", 0.01);
        
        return config;
    }
    
    std::shared_ptr<Config> createDevelopmentalPreset() {
        auto config = std::make_shared<Config>();
        
        // Developmental stages
        config->set("development.stage_duration.0", 100);
        config->set("development.stage_duration.1", 500);
        config->set("development.stage_duration.2", 1000);
        config->set("development.stage_duration.3", 5000);
        
        // Plasticity dynamics
        config->set("plasticity.initial_strength", 0.1);
        config->set("plasticity.maturation_rate", 0.001);
        config->set("plasticity.pruning_threshold", 0.01);
        config->set("plasticity.inhibition_strength", 0.5);
        config->set("plasticity.reinforcement_threshold", 0.8);
        
        // Synaptogenesis
        config->set("synaptogenesis.rate.initial", 0.05);
        config->set("synaptogenesis.rate.mature", 0.01);
        config->set("synaptogenesis.rate.adaptive", 0.02);
        
        return config;
    }
    
    std::shared_ptr<Config> createMultiEnvPreset() {
        auto config = std::make_shared<Config>();
        
        // Multi-environment parameters
        config->set("multi_env.adaptation_rate", 0.1);
        config->set("multi_env.transfer_threshold", 0.7);
        config->set("multi_env.forgetting_rate", 0.001);
        config->set("multi_env.domain_shift_rate", 0.01);
        config->set("multi_env.context_switch_cost", 0.1);
        
        // Environment-specific parameters
        config->set("env1.rewards", "resource_positive,hazard_negative");
        config->set("env1.difficulty", 0.5);
        config->set("env2.rewards", "predator_positive,resource_negative");
        config->set("env2.difficulty", 0.8);
        
        return config;
    }
}

// Simulation manager for batch processing
class SimulationManager {
public:
    SimulationManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                      std::shared_ptr<AgentBrain> agent)
        : brain_(brain), world_(world), agent_(agent) {}
    
    struct SimulationConfig {
        size_t numEpisodes;
        size_t maxStepsPerEpisode;
        double timeStep;
        bool render;
        std::string outputPath;
        ProgressMonitor* progressMonitor;
    };
    
    std::vector<ResultCollector::EpisodeResult> runBatchSimulation(const SimulationConfig& config) {
        std::vector<ResultCollector::EpisodeResult> allResults;
        ResultCollector collector;
        
        ProgressMonitor monitor(config.numEpisodes * config.maxStepsPerEpisode);
        
        for (size_t episode = 0; episode < config.numEpisodes; ++episode) {
            ResultCollector::EpisodeResult episodeResult;
            episodeResult.startTime = world_->getSimulationTime();
            episodeResult.rewards.clear();
            episodeResult.actions.clear();
            
            // Reset simulation for new episode
            agent_->reset();
            world_->reset();
            
            double episodeReward = 0.0;
            size_t stepsCompleted = 0;
            
            for (size_t step = 0; step < config.maxStepsPerEpisode; ++step) {
                // Get sensory percept
                SensoryPercept percept = agent_->processSensoryInput(world_->getSensoryPercept());
                
                // Update brain
                brain_->step(SimulationStep(step), world_->getSimulationTime());
                
                // Produce action
                MotorCommand action = agent_->decodeMotorCommand();
                
                // Apply action to world
                world_->applyMotorCommand(action, world_->getSimulationTime());
                
                // Record action and reward
                episodeResult.actions.push_back(actionToString(action));
                
                monitor.update(1.0, "Episode " + std::to_string(episode + 1) + 
                              ", Step " + std::to_string(step + 1));
                
                stepsCompleted++;
                
                // Check if episode should end
                if (shouldTerminateEpisode()) {
                    break;
                }
            }
            
            episodeResult.endTime = world_->getSimulationTime();
            episodeResult.steps = stepsCompleted;
            episodeResult.totalReward = episodeReward;
            episodeResult.averageReward = episodeReward / stepsCompleted;
            episodeResult.successProbability = calculateSuccessProbability(episodeResult);
            
            // Store results
            allResults.push_back(episodeResult);
            collector.recordEpisode(episodeResult);
            
            if (config.outputPath != "") {
                std::string episodeFile = config.outputPath + "_episode_" + 
                                         std::to_string(episode) + ".json";
                collector.saveToJSON(episodeFile);
            }
        }
        
        return allResults;
    }
    
private:
    std::string actionToString(MotorCommand action) {
        switch (action) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    bool shouldTerminateEpisode() {
        return brain_->getActiveNeuronCount() < brain_->getTotalNeuronCount() * 0.1 ||
               world_->getSimulationTime() > 1000.0;
    }
    
    double calculateSuccessProbability(const ResultCollector::EpisodeResult& episode) {
        return std::exp(-episode.steps / 100.0);
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
};

// Machine learning utilities
namespace ml {
    // Feature extraction utilities
    class FeatureExtractor {
    public:
        struct NeuralFeatures {
            std::vector<float> firingRates;
            std::vector<float> membranePotentials;
            std::vector<float> synapticWeights;
            std::vector<float> populationActivity;
            std::vector<float> crossCorrelation;
            std::vector<float> sparsityMeasures;
        };
        
        static NeuralFeatures extractFromBrain(const Brain& brain) {
            NeuralFeatures features;
            
            // Get brain regions
            auto regions = brain.getRegions();
            features.firingRates.reserve(regions.size());
            features.synapticWeights.reserve(regions.size());
            features.populationActivity.reserve(regions.size());
            
            for (const auto& region : regions) {
                features.firingRates.push_back(brain.getAverageFiringRate());
                features.synapticWeights.push_back(1.0f); // Placeholder
                features.populationActivity.push_back(1.0f); // Placeholder
            }
            
            return features;
        }
        
        static std::vector<float> normalizeFeatures(const NeuralFeatures& features) {
            std::vector<float> normalized;
            normalized.reserve(features.firingRates.size() + features.synapticWeights.size() +
                             features.populationActivity.size());
            
            // Simple min-max normalization
            float maxFiringRate = *std::max_element(features.firingRates.begin(), features.firingRates.end());
            float maxWeight = *std::max_element(features.synapticWeights.begin(), features.synapticWeights.end());
            
            for (float rate : features.firingRates) {
                normalized.push_back(rate / (maxFiringRate + 1e-8));
            }
            
            for (float weight : features.synapticWeights) {
                normalized.push_back(weight / (maxWeight + 1e-8));
            }
            
            return normalized;
        }
    };
    
    // Pattern recognition helpers
    class PatternRecognizer {
    public:
        struct Pattern {
            std::string name;
            std::vector<float> signature;
            double similarity;
        };
        
        static Pattern recognizePattern(const std::vector<float>& data) {
            Pattern pattern;
            pattern.name = "unknown";
            pattern.similarity = 0.0;
            
            // Simple pattern matching based on average
            double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
            pattern.similarity = avg / (data.size() + 1e-8);
            
            // Simple pattern classification
            if (pattern.similarity > 0.5) {
                if (avg > 0.0) {
                    pattern.name = "positive_pattern";
                } else {
                    pattern.name = "negative_pattern";
                }
            }
            
            return pattern;
        }
        
        static std::vector<Pattern> batchRecognize(const std::vector<std::vector<float>>& datasets) {
            std::vector<Pattern> patterns;
            patterns.reserve(datasets.size());
            
            for (size_t i = 0; i < datasets.size(); ++i) {
                Pattern pattern = recognizePattern(datasets[i]);
                pattern.name += "_sample_" + std::to_string(i);
                patterns.push_back(pattern);
            }
            
            return patterns;
        }
    };
    
    // Behavioral cloning interface
    class BehavioralCloning {
    public:
        struct TrainingData {
            std::vector<std::vector<float>> states;
            std::vector<std::string> actions;
            std::vector<double> rewards;
        };
        
        static TrainingData collectEpisodeData(std::shared_ptr<Brain> brain, 
                                              std::shared_ptr<AgentBrain> agent,
                                              std::shared_ptr<SimpleWorld> world,
                                              size_t maxSteps = 1000) {
            TrainingData data;
            
            agent->reset();
            world->reset();
            
            for (size_t step = 0; step < maxSteps; ++step) {
                // Extract state features
                auto features = FeatureExtractor::extractFromBrain(*brain);
                auto normalized = FeatureExtractor::normalizeFeatures(features);
                data.states.push_back(normalized);
                
                // Record action and reward
                data.actions.push_back(actionToString(agent->decodeMotorCommand()));
                
                // Step simulation
                brain->step(SimulationStep(step), world->getSimulationTime());
                
                if (shouldTerminate()) break;
            }
            
            return data;
        }
        
        static double trainPolicyNetwork(const TrainingData& data) {
            // Placeholder for neural network training
            // In practice, this would implement a policy gradient or value-based method
            double loss = 0.0;
            
            for (size_t i = 0; i < data.actions.size(); ++i) {
                // Simple loss calculation based on action consistency
                loss += 1.0; // Placeholder
            }
            
            return loss / data.actions.size();
        }
        
    private:
        static std::string actionToString(MotorCommand action) {
            switch (action) {
                case MotorCommand::MoveForward: return "MoveForward";
                case MotorCommand::MoveBackward: return "MoveBackward";
                case MotorCommand::TurnLeft: return "TurnLeft";
                case MotorCommand::TurnRight: return "TurnRight";
                case MotorCommand::LookLeft: return "LookLeft";
                case MotorCommand::LookRight: return "LookRight";
                case MotorCommand::Interact: return "Interact";
                case MotorCommand::Wait: return "Wait";
                default: return "Unknown";
            }
        }
        
        static bool shouldTerminate() {
            return false; // Placeholder for actual termination condition
        }
    };
    
    // Reward shaping utilities
    class RewardShaper {
    public:
        struct ShapedReward {
            double baseReward;
            double shapedReward;
            double bonus;
            double penalty;
            std::string shapingMethod;
        };
        
        static ShapedReward applyIntrinsicMotivation(const Brain& brain, double baseReward) {
            ShapedReward result;
            result.baseReward = baseReward;
            result.shapingMethod = "intrinsic_motivation";
            
            // Curiosity-based reward
            double curiosity = brain.getCuriosityLevel();
            result.bonus = curiosity * 0.5;
            result.shapedReward = baseReward + result.bonus;
            
            // Novelty penalty if too predictable
            double novelty = brain.getNoveltyLevel();
            if (novelty < 0.2) {
                result.penalty = (0.2 - novelty) * 0.3;
                result.shapedReward -= result.penalty;
            }
            
            return result;
        }
        
        static ShapedReward applyCurriculumLearning(const Brain& brain, double baseReward,
                                                   size_t currentStep, size_t totalSteps) {
            ShapedReward result;
            result.baseReward = baseReward;
            result.shapingMethod = "curriculum_learning";
            
            // Progressive difficulty
            double difficulty = static_cast<double>(currentStep) / totalSteps;
            result.bonus = difficulty * 0.3;
            result.shapedReward = baseReward + result.bonus;
            
            // Performance-based bonus
            double performance = brain.getAverageFiringRate() / 100.0;
            if (performance > 0.5) {
                result.bonus += (performance - 0.5) * 0.5;
                result.shapedReward = baseReward + result.bonus;
            }
            
            return result;
        }
        
        static ShapedReward applyTransferLearning(const Brain& sourceBrain, const Brain& targetBrain,
                                                 double baseReward, double transferThreshold = 0.7) {
            ShapedReward result;
            result.baseReward = baseReward;
            result.shapingMethod = "transfer_learning";
            
            // Check if transfer is beneficial
            double sourcePerformance = sourceBrain.getAverageFiringRate();
            double targetPerformance = targetBrain.getAverageFiringRate();
            
            if (sourcePerformance > targetPerformance * transferThreshold) {
                result.bonus = (sourcePerformance - targetPerformance) * 0.2;
                result.shapedReward = baseReward + result.bonus;
            }
            
            return result;
        }
    };
}

// Neural network visualization utilities
namespace visualization {
    class NeuralNetworkVisualizer {
    public:
        struct VisualizationConfig {
            std::string outputFormat;
            std::string outputPath;
            double scale;
            bool showWeights;
            bool showActivity;
            bool showSpikes;
            bool showConnections;
        };
        
        static bool exportForAnalysis(const Brain& brain, const VisualizationConfig& config) {
            // Create a simplified representation of the neural network
            std::vector<std::vector<float>> networkGraph;
            
            auto regions = brain.getRegions();
            networkGraph.reserve(regions.size());
            
            for (size_t i = 0; i < regions.size(); ++i) {
                networkGraph.push_back(std::vector<float>(regions.size(), 0.0f));
                
                for (size_t j = 0; j < regions.size(); ++j) {
                    if (i != j) {
                        // Calculate connection strength
                        float weight = 1.0f; // Placeholder
                        networkGraph[i][j] = weight;
                    }
                }
            }
            
            // Save as CSV for further analysis
            std::ofstream file(config.outputPath + ".csv");
            if (!file.is_open()) return false;
            
            for (const auto& row : networkGraph) {
                for (size_t j = 0; j < row.size(); ++j) {
                    file << row[j];
                    if (j < row.size() - 1) file << ",";
                }
                file << "\n";
            }
            
            return true;
        }
        
        static bool generateRasterPlot(const std::vector<SpikeEvent>& spikes, 
                                      const std::string& outputPath) {
            std::ofstream file(outputPath + ".txt");
            if (!file.is_open()) return false;
            
            file << "Spike Raster Plot Data\n";
            file << "Time\tNeuronId\n";
            
            for (const auto& spike : spikes) {
                file << spike.timestamp << "\t" << spike.source_neuron.index() << "\n";
            }
            
            return true;
        }
        
        static bool createActivityHeatmap(const Brain& brain, const std::string& outputPath) {
            // Generate activity heatmap data
            std::vector<std::vector<float>> heatmap;
            
            auto regions = brain.getRegions();
            heatmap.reserve(regions.size());
            
            for (size_t i = 0; i < regions.size(); ++i) {
                heatmap.push_back(std::vector<float>(regions.size(), 0.0f));
                
                for (size_t j = 0; j < regions.size(); ++j) {
                    heatmap[i][j] = static_cast<float>(rand()) / RAND_MAX;
                }
            }
            
            // Save as image (placeholder - in practice use a proper image library)
            std::ofstream file(outputPath + ".csv");
            if (!file.is_open()) return false;
            
            for (const auto& row : heatmap) {
                for (size_t j = 0; j < row.size(); ++j) {
                    file << row[j];
                    if (j < row.size() - 1) file << ",";
                }
                file << "\n";
            }
            
            return true;
        }
    };
}

// Custom recording for episode analysis
class EpisodeRecorder {
public:
    struct EpisodeRecord {
        double startTime;
        double endTime;
        size_t steps;
        std::vector<double> rewards;
        std::vector<std::string> actions;
        std::vector<float> visionData;
        std::vector<float> touchData;
        std::vector<float> internalData;
        std::vector<float> proprioceptionData;
        std::vector<float> audioData;
        std::vector<double> brainActivity;
        std::vector<NeuronId> activeNeurons;
        std::vector<SynapseId> activeSynapses;
    };
    
    EpisodeRecorder(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                   std::shared_ptr<SimpleWorld> world)
        : brain_(brain), agent_(agent), world_(world) {}
    
    EpisodeRecord recordEpisode(size_t maxSteps = 1000) {
        EpisodeRecord record;
        record.startTime = world_->getSimulationTime();
        
        agent_->reset();
        world_->reset();
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Record state before step
            record.startTime = world_->getSimulationTime();
            
            // Get sensory data
            SensoryPercept percept = world_->getSensoryPercept();
            
            // Record sensory data
            record.visionData = percept.getVision();
            record.touchData = percept.getTouch();
            record.internalData = percept.getInternal();
            record.proprioceptionData = percept.getProprioception();
            record.audioData = percept.getAudio();
            
            // Step brain
            brain_->step(SimulationStep(step), world_->getSimulationTime());
            
            // Record brain state
            record.brainActivity.push_back(brain_->getAverageFiringRate());
            
            // Get active neurons and synapses
            record.activeNeurons.clear();
            record.activeSynapses.clear();
            
            // Produce action
            MotorCommand action = agent_->decodeMotorCommand();
            record.actions.push_back(actionToString(action));
            
            // Apply action
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Check termination
            if (shouldTerminateEpisode()) {
                record.endTime = world_->getSimulationTime();
                record.steps = step + 1;
                break;
            }
        }
        
        if (record.steps == 0) {
            record.endTime = record.startTime;
        }
        
        return record;
    }
    
    // Analyze episode data
    double calculateEfficiency(const EpisodeRecord& record) const {
        if (record.steps == 0) return 0.0;
        
        double totalReward = 0.0;
        for (double reward : record.rewards) {
            totalReward += reward;
        }
        
        return totalReward / record.steps;
    }
    
    double calculateExplorationRate(const EpisodeRecord& record) const {
        if (record.actions.empty()) return 0.0;
        
        size_t uniqueActions = 0;
        std::unordered_set<std::string> uniqueActionSet(record.actions.begin(), record.actions.end());
        uniqueActions = uniqueActionSet.size();
        
        return static_cast<double>(uniqueActions) / record.actions.size();
    }
    
    // Save episode record
    bool saveEpisodeRecord(const EpisodeRecord& record, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        file << "Episode Record\n";
        file << "Start Time: " << record.startTime << "\n";
        file << "End Time: " << record.endTime << "\n";
        file << "Steps: " << record.steps << "\n";
        file << "Rewards: ";
        for (size_t i = 0; i < record.rewards.size(); ++i) {
            file << record.rewards[i];
            if (i < record.rewards.size() - 1) file << ",";
        }
        file << "\nActions: ";
        for (size_t i = 0; i < record.actions.size(); ++i) {
            file << record.actions[i];
            if (i < record.actions.size() - 1) file << ",";
        }
        file << "\n";
        
        return true;
    }
    
private:
    std::string actionToString(MotorCommand action) {
        switch (action) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    bool shouldTerminateEpisode() {
        return brain_->getActiveNeuronCount() < brain_->getTotalNeuronCount() * 0.1 ||
               world_->getSimulationTime() > 1000.0;
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
};

// Comparative analysis tools
namespace comparative {
    class ComparativeAnalyzer {
    public:
        struct ComparativeResult {
            std::string experimentName;
            double performanceScore;
            double efficiencyScore;
            double explorationScore;
            double learningSpeed;
            std::map<std::string, double> metrics;
        };
        
        static ComparativeResult compareBrains(const std::vector<std::shared_ptr<Brain>>& brains,
                                              const std::vector<std::string>& names) {
            ComparativeResult result;
            result.experimentName = "brain_comparison";
            
            if (brains.empty()) return result;
            
            double totalPerformance = 0.0;
            double totalEfficiency = 0.0;
            double totalExploration = 0.0;
            
            for (size_t i = 0; i < brains.size(); ++i) {
                auto brain = brains[i];
                
                // Calculate performance metrics
                double performance = brain->getAverageFiringRate() / 100.0;
                double efficiency = brain->getExcitationInhibitionRatio() / 10.0;
                double exploration = 0.5; // Placeholder
                
                result.metrics[names[i] + "_performance"] = performance;
                result.metrics[names[i] + "_efficiency"] = efficiency;
                result.metrics[names[i] + "_exploration"] = exploration;
                
                totalPerformance += performance;
                totalEfficiency += efficiency;
                totalExploration += exploration;
            }
            
            result.performanceScore = totalPerformance / brains.size();
            result.efficiencyScore = totalEfficiency / brains.size();
            result.explorationScore = totalExploration / brains.size();
            
            // Calculate learning speed (placeholder)
            result.learningSpeed = result.performanceScore * 0.5 + result.efficiencyScore * 0.5;
            
            return result;
        }
        
        static ComparativeResult runCurriculumLearning(const std::vector<std::shared_ptr<Brain>>& brains,
                                                      const std::vector<std::string>& names,
                                                      size_t curriculumSteps) {
            ComparativeResult result;
            result.experimentName = "curriculum_learning";
            
            for (size_t step = 0; step < curriculumSteps; ++step) {
                double difficulty = static_cast<double>(step) / curriculumSteps;
                
                for (size_t i = 0; i < brains.size(); ++i) {
                    auto brain = brains[i];
                    
                    // Simulate learning at this difficulty level
                    double performance = brain->getAverageFiringRate() / 100.0;
                    performance *= (1.0 + difficulty * 0.5); // Improve with difficulty
                    
                    result.metrics[names[i] + "_step_" + std::to_string(step)] = performance;
                }
            }
            
            // Calculate final scores
            double totalPerformance = 0.0;
            for (const auto& [key, value] : result.metrics) {
                if (key.find("_step_") != std::string::npos) {
                    totalPerformance += value;
                }
            }
            
            result.performanceScore = totalPerformance / (brains.size() * curriculumSteps);
            result.efficiencyScore = result.performanceScore * 0.8;
            result.explorationScore = result.performanceScore * 0.6;
            result.learningSpeed = result.performanceScore / curriculumSteps;
            
            return result;
        }
    };
}