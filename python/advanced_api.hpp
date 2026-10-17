#pragma once

// Advanced NLM Python API - High-level, Pythonic interfaces for NLM functionality
// Provides enhanced features for brain configuration, agent operations, monitoring,
// neural network operations, integration, neuroscience features, and performance optimization

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <optional>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <random>

// Core NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace nlm {
namespace python {

// Advanced Brain Configuration
class AdvancedBrainConfiguration {
public:
    struct BrainPreset {
        std::string name;
        std::string description;
        std::map<std::string, std::variant<int, float, bool, std::string>> parameters;
    };
    
    struct SchemaValidation {
        std::map<std::string, std::string> fieldTypes;
        std::map<std::string, std::pair<float, float>> ranges;
        std::map<std::string, std::string> patterns;
        std::vector<std::string> requiredFields;
    };
    
    // Programmatic configuration creation with schema validation
    static std::shared_ptr<Config> createConfig(const std::map<std::string, std::variant<int, float, bool, std::string>>& params);
    
    // Configuration presets and templates
    static std::vector<std::string> getAvailablePresets();
    static std::shared_ptr<Config> applyPreset(const std::string& presetName);
    static void registerPreset(const BrainPreset& preset);
    
    // Runtime configuration modification
    static bool modifyConfig(std::shared_ptr<Config> config, const std::string& key, 
                           const std::variant<int, float, bool, std::string>& value);
    
    // Configuration inheritance and merging
    static std::shared_ptr<Config> mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs);
    static std::shared_ptr<Config> inheritFrom(const std::shared_ptr<Config>& baseConfig, 
                                             const std::map<std::string, std::variant<int, float, bool, std::string>>& overrides);
    
    // Schema validation
    static bool validateConfig(const std::shared_ptr<Config>& config, const SchemaValidation& schema);
    static SchemaValidation getDefaultBrainSchema();
    static std::map<std::string, std::variant<int, float, bool, std::string>> getConfigSchemaInfo();
};

// Advanced Agent Features
class AdvancedAgent {
public:
    struct Experience {
        double timestamp;
        std::vector<float> state;
        int action;
        double reward;
        std::vector<float> nextState;
        bool done;
    };
    
    struct RewardFunction {
        std::string name;
        std::function<double(const std::vector<float>&, int, const std::vector<float>&)> compute;
    };
    
    // Reinforcement learning integration
    static void enableReinforcedLearning(std::shared_ptr<AgentBrain> agent, double learningRate = 0.01, 
                                       double discountFactor = 0.99);
    
    // Experience replay and learning
    static void addExperience(std::shared_ptr<AgentBrain> agent, const Experience& exp);
    static void replayExperiences(std::shared_ptr<AgentBrain> agent, size_t batchSize = 32);
    static std::vector<Experience> getExperienceBuffer(std::shared_ptr<AgentBrain> agent);
    
    // Custom reward functions
    static void registerRewardFunction(const RewardFunction& rewardFunc);
    static void setRewardFunction(std::shared_ptr<AgentBrain> agent, const std::string& name);
    static double computeCustomReward(std::shared_ptr<AgentBrain> agent, const std::vector<float>& state, 
                                    int action, const std::vector<float>& nextState);
    
    // Advanced perception processing
    static void enableAdvancedPerception(std::shared_ptr<AgentBrain> agent, bool enablePredictions = true,
                                        bool enableNovelty = true, bool enableCuriosity = true);
    static std::vector<float> extractFeatures(const SensoryPercept& percept);
    static std::map<std::string, double> analyzePerception(const SensoryPercept& percept);
};

// Real-time Monitoring and Visualization
class MonitoringSystem {
public:
    struct MetricData {
        std::string name;
        std::string unit;
        std::vector<std::pair<double, double>> timeSeries;
        double currentValue;
        double maxValue;
        double minValue;
        double averageValue;
    };
    
    struct BrainStateVisualization {
        std::map<std::string, double> regionalActivity;
        std::vector<std::pair<double, double>> firingRateHistory;
        std::vector<std::pair<double, double>> connectivityMatrix;
        std::map<std::string, std::vector<double>> neuromodulatorLevels;
    };
    
    // Live metrics monitoring and streaming
    static void startMetricsStreaming(std::shared_ptr<Brain> brain, double interval = 0.1);
    static void stopMetricsStreaming(std::shared_ptr<Brain> brain);
    static std::vector<MetricData> getCurrentMetrics(std::shared_ptr<Brain> brain);
    static void registerMetricCollector(const std::string& name, 
                                       std::function<MetricData(std::shared_ptr<Brain>)> collector);
    
    // Real-time brain state visualization
    static BrainStateVisualization getBrainStateVisualization(std::shared_ptr<Brain> brain);
    static std::string generateBrainStateVisualizationHTML(const BrainStateVisualization& state);
    static void startVisualizationServer(std::shared_ptr<Brain> brain, int port = 8080);
    
    // Performance profiling
    static void enableProfiling(std::shared_ptr<Brain> brain, bool enable = true);
    static struct ProfilingData {
        double totalTime;
        double stepTime;
        double spikeProcessTime;
        double plasticityTime;
        double memoryAccessTime;
        std::map<std::string, double> componentTimes;
    } getProfilingData(std::shared_ptr<Brain> brain);
    
    // Activity patterns analysis
    static std::map<std::string, double> analyzeActivityPatterns(std::shared_ptr<Brain> brain, 
                                                                double windowSeconds = 60.0);
    static std::vector<std::pair<double, double>> getFiringRateHistory(std::shared_ptr<Brain> brain, 
                                                                      int windowSize = 1000);
    static double computeCoherenceIndex(std::shared_ptr<Brain> brain);
};

// Advanced Neural Network Operations
class AdvancedNeuralOperations {
public:
    // Custom neural population creation
    struct PopulationParams {
        std::string name;
        NeuronType type;
        size_t neuronCount;
        float connectionDensity;
        float meanSynapticStrength;
        float stdSynapticStrength;
        std::string connectionPattern;
    };
    
    static std::vector<PopulationId> createCustomPopulation(std::shared_ptr<Brain> brain, 
                                                          const PopulationParams& params);
    static std::vector<PopulationId> createNeuralPopulation(std::shared_ptr<Brain> brain, 
                                                          const std::string& regionName,
                                                          NeuronType type, size_t neuronCount);
    
    // Synapse manipulation and analysis
    static void connectNeurons(std::shared_ptr<Brain> brain, NeuronId source, NeuronId target, 
                             float weight, SynapseType type = SynapseType::Excitatory);
    static std::vector<std::pair<NeuronId, NeuronId>> findStrongestConnections(std::shared_ptr<Brain> brain, 
                                                                             size_t topN = 10);
    static std::vector<std::pair<NeuronId, NeuronId>> findWeakestConnections(std::shared_ptr<Brain> brain, 
                                                                            size_t bottomN = 10);
    static float computeConnectionStrength(NeuronId source, NeuronId target);
    static std::map<NeuronId, std::vector<std::pair<NeuronId, float>>> analyzeNeuronConnectivity(std::shared_ptr<Brain> brain, NeuronId neuronId);
    
    // Plasticity control and tuning
    static void setSTDPParameters(std::shared_ptr<Brain> brain, float ltpWeight = 0.01, 
                                float ltdWeight = 0.012, float learningRate = 0.01);
    static void enableHebbianPlasticity(std::shared_ptr<Brain> brain, float learningRate = 0.005);
    static void enableStructuralPlasticity(std::shared_ptr<Brain> brain, bool enable = true);
    static void tunePlasticityForStability(std::shared_ptr<Brain> brain);
    static void tunePlasticityForLearning(std::shared_ptr<Brain> brain);
    
    // Developmental stage management
    static void triggerCriticalPeriod(std::shared_ptr<Brain> brain);
    static void advanceDevelopmentStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage);
    static DevelopmentalStage getCurrentDevelopmentalStage(std::shared_ptr<Brain> brain);
    static bool isCriticalPeriodActive(std::shared_ptr<Brain> brain);
};

// Integration and Interoperability
class IntegrationSystem {
public:
    // Integration with Python ML libraries (NumPy, SciPy)
    static std::vector<float> convertToNLMFormat(const py::array_t<float>& numpyArray);
    static py::array_t<float> convertToNumPyFormat(const std::vector<float>& nlmData);
    static py::array_t<double> computeSimilarityMatrix(const std::vector<std::vector<float>>& data);
    static std::vector<float> applyNeuralNetwork(std::shared_ptr<Brain> brain, 
                                               const py::array_t<float>& input);
    
    // Data import/export capabilities
    static bool exportBrainToJSON(std::shared_ptr<Brain> brain, const std::string& filepath);
    static std::shared_ptr<Brain> importBrainFromJSON(const std::string& filepath);
    static bool exportDataToCSV(const std::string& filepath, const std::vector<std::vector<double>>& data);
    static std::vector<std::vector<double>> importDataFromCSV(const std::string& filepath);
    static bool exportSimulationToHDF5(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Serialization formats (JSON, pickle, custom)
    static std::string serializeToJSON(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> deserializeFromJSON(const std::string& jsonString);
    static std::string serializeToPickle(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> deserializeFromPickle(const std::string& pickleData);
    
    // Cross-language interoperability
    static py::dict getPythonInterface(std::shared_ptr<Brain> brain);
    static std::shared_ptr<Brain> createBrainFromPythonInterface(const py::dict& interface);
    static void registerForeignFunction(const std::string& name, std::function<py::object(const py::args&)> func);
};

// Advanced Neuroscience Features
class AdvancedNeuroscience {
public:
    // Custom neuromodulator implementations
    struct NeuromodulatorParams {
        std::string name;
        std::function<double(double)> activationFunction;
        std::function<double(double)> decayFunction;
        double baselineLevel;
        double sensitivity;
    };
    
    static void registerCustomNeuromodulator(const NeuromodulatorParams& params);
    static double computeNeuromodulationSignal(const std::string& name, double input);
    static std::map<std::string, double> getAllNeuromodulatorLevels(std::shared_ptr<Brain> brain);
    
    // Novelty and curiosity computation
    static double computeNoveltyScore(std::shared_ptr<Brain> brain, const std::vector<float>& input);
    static double computeCuriosityScore(std::shared_ptr<Brain> brain, const std::vector<float>& state);
    static void updateNoveltyLearning(std::shared_ptr<Brain> brain, const std::vector<float>& prediction,
                                   const std::vector<float>& actual);
    static void updateCuriosityLearning(std::shared_ptr<Brain> brain, const std::vector<float>& state,
                                      double rewardPredictionError);
    
    // Prediction error analysis
    static double computePredictionError(std::shared_ptr<Brain> brain, const std::vector<float>& prediction,
                                        const std::vector<float>& actual);
    static std::vector<double> computeTemporalDifferenceError(std::shared_ptr<Brain> brain, 
                                                             const std::vector<double>& rewards,
                                                             const std::vector<double>& values);
    static void enablePredictionErrorLearning(std::shared_ptr<Brain> brain, bool enable = true);
    
    // Memory consolidation and replay
    static void triggerMemoryConsolidation(std::shared_ptr<Brain> brain, double consolidationThreshold = 0.8);
    static std::vector<std::vector<float>> getConsolidationCandidates(std::shared_ptr<Brain> brain, 
                                                                     size_t maxCandidates = 100);
    static void replayMemories(std::shared_ptr<Brain> brain, const std::vector<std::vector<float>>& memories,
                              double replayStrength = 1.0);
};

// Performance and Optimization
class PerformanceOptimizer {
public:
    // Batch operations for large-scale simulations
    static void enableBatchProcessing(std::shared_ptr<Brain> brain, size_t batchSize = 1000);
    static std::vector<double> processBatchSteps(std::shared_ptr<Brain> brain, 
                                                const std::vector<SimulationStep>& steps);
    static void processBatchNeurons(std::shared_ptr<Brain> brain, 
                                   const std::vector<NeuronId>& neuronIds);
    static std::vector<double> processBatchInputs(std::shared_ptr<Brain> brain,
                                                 const std::vector<std::vector<float>>& inputs);
    
    // Parallel execution support
    static void enableParallelExecution(std::shared_ptr<Brain> brain, bool enable = true);
    static size_t getAvailableParallelThreads(std::shared_ptr<Brain> brain);
    static void setParallelThreads(std::shared_ptr<Brain> brain, size_t threads);
    static std::vector<double> executeInParallel(std::shared_ptr<Brain> brain,
                                                std::function<double(size_t)> task,
                                                size_t taskCount);
    
    // Memory management and optimization
    static void enableMemoryOptimization(std::shared_ptr<Brain> brain, bool enable = true);
    static size_t getMemoryUsage(std::shared_ptr<Brain> brain);
    static size_t getPeakMemoryUsage(std::shared_ptr<Brain> brain);
    static void optimizeMemoryLayout(std::shared_ptr<Brain> brain);
    static void clearUnusedMemory(std::shared_ptr<Brain> brain);
    
    // Resource monitoring and management
    static struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
    } getResourceUsage(std::shared_ptr<Brain> brain);
    static void setResourceLimit(std::shared_ptr<Brain> brain, const std::string& resource, double limit);
    static double getResourceUsage(std::shared_ptr<Brain> brain, const std::string& resource);
};

// Main advanced API class that provides access to all features
class AdvancedAPI {
public:
    static AdvancedAPI& getInstance() {
        static AdvancedAPI instance;
        return instance;
    }
    
    // Get advanced brain configuration
    static std::shared_ptr<Config> createBrainConfig(const std::map<std::string, std::variant<int, float, bool, std::string>>& params);
    
    // Get advanced agent interface
    static std::shared_ptr<AgentBrain> createAdvancedAgent(std::shared_ptr<Brain> brain);
    
    // Get monitoring system
    static std::shared_ptr<MonitoringSystem> getMonitoringSystem();
    
    // Get neural operations interface
    static std::shared_ptr<AdvancedNeuralOperations> getNeuralOperations();
    
    // Get integration system
    static std::shared_ptr<IntegrationSystem> getIntegration();
    
    // Get neuroscience features
    static std::shared_ptr<AdvancedNeuroscience> getNeuroscience();
    
    // Get performance optimizer
    static std::shared_ptr<PerformanceOptimizer> getPerformance();
    
    // Initialize and cleanup
    static bool initialize();
    static void cleanup();
    static bool isInitialized();
    
private:
    AdvancedAPI() = default;
    ~AdvancedAPI() = default;
    AdvancedAPI(const AdvancedAPI&) = delete;
    AdvancedAPI& operator=(const AdvancedAPI&) = delete;
};

} // namespace python
} // namespace nlm