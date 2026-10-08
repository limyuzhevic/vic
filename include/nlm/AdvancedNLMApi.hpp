#pragma once

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

namespace nlm {

/**
 * @brief Advanced high-performance API for expert users
 * 
 * This API provides granular control over NLM brain operations for advanced users
 * who need fine-grained control over neural simulation parameters, experimental
 * protocols, and performance optimizations.
 */
class AdvancedNLMApi {
public:
    /**
     * @brief High-performance brain configuration with advanced options
     */
    struct AdvancedConfig {
        // Core brain parameters
        size_t neuronCount = 5000;
        size_t regionCount = 4;
        float connectionProbability = 0.15f;
        
        // Performance optimization parameters
        bool enableSIMD = true;
        bool enableVectorization = true;
        bool enableParallelProcessing = true;
        size_t maxConcurrentThreads = std::thread::hardware_concurrency();
        
        // Neuromodulation control
        bool enableDopamine = true;
        bool enableCuriosity = true;
        bool enableNovelty = true;
        bool enablePredictionError = true;
        
        // Plasticity control
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableStructuralPlasticity = true;
        float stdpLTPWeight = 0.02f;
        float stdpLTDWeight = 0.015f;
        float stdpTimeConstant = 20.0f;
        
        // Memory system control
        size_t workingMemoryCapacity = 1000;
        size_t episodicMemoryCapacity = 10000;
        bool enableMemoryReplay = true;
        float replayStrength = 0.5f;
        
        // Development control
        bool enableDevelopment = true;
        DevelopmentalStage initialStage = DevelopmentalStage::CriticalPeriod;
        double developmentTimestep = 0.1;
        
        // Experimental protocol control
        bool enableExperimentalMode = false;
        std::string experimentProtocol;
        double protocolTimestep = 0.001;
        
        // Performance monitoring
        bool enablePerformanceMonitoring = true;
        std::chrono::milliseconds performanceReportInterval{1000};
        std::atomic<size_t> stepCount{0};
        
        // Checkpointing control
        bool enableAutomaticCheckpointing = false;
        std::string checkpointDirectory = "./checkpoints";
        size_t maxCheckpoints = 100;
        double checkpointInterval = 1000.0; // seconds
        
        // Custom callbacks
        std::function<void(const Brain&)> preStepCallback;
        std::function<void(const Brain&)> postStepCallback;
        std::function<void(const std::string&)> errorCallback;
        
        AdvancedConfig() = default;
    };
    
    /**
     * @brief High-performance neural simulation manager
     */
    class NeuralSimulator {
    public:
        NeuralSimulator(std::shared_ptr<Brain> brain, const AdvancedConfig& config);
        ~NeuralSimulator();
        
        // Configuration and setup
        void configure(const AdvancedConfig& config);
        void initialize();
        void reset();
        
        // High-performance simulation loop
        void runSimulation(size_t steps);
        void runForDuration(double duration, double timestep);
        void runWithProtocol(const std::string& protocol);
        
        // Low-level control
        void step(size_t step = 0);
        void stepWithTimestamp(size_t step, double timestamp);
        
        // Performance control
        void enableParallelProcessing(bool enable);
        void setThreadCount(size_t count);
        void enableSIMD(bool enable);
        void enableVectorization(bool enable);
        
        // Memory management
        void optimizeMemoryUsage();
        void forceGarbageCollection();
        void setMemoryLimit(size_t limitBytes);
        
        // Experimental control
        void startExperiment(const std::string& protocol);
        void pauseExperiment();
        void resumeExperiment();
        void stopExperiment();
        
        // Neuromodulation control
        void setDopamineLevel(float level);
        void setCuriosityLevel(float level);
        void setNoveltyThreshold(float threshold);
        void setPredictionError(float error);
        
        // Plasticity control
        void enablePlasticity(bool stdp, bool hebbian, bool structural);
        void setPlasticityParameters(float ltp, float ltd, float tau);
        void setEligibilityTraceDecay(float decay);
        
        // Sensorimotor control
        void injectSensoryInput(const std::vector<float>& input);
        void setMotorCommand(MotorCommand command);
        MotorCommand getDecodedMotorCommand();
        
        // Statistical analysis
        struct SimulationStats {
            size_t totalSteps = 0;
            size_t totalSpikes = 0;
            size_t activeNeurons = 0;
            float averageFiringRate = 0.0f;
            float eIRepeateduceRatio = 0.0f;
            double simulationTime = 0.0;
            size_t memoryUsage = 0;
            float cpuUsage = 0.0f;
        };
        
        SimulationStats getSimulationStats() const;
        std::vector<float> getNeuronActivations() const;
        std::vector<float> getSynapticWeights() const;
        
        // Checkpoint and recovery
        bool saveCheckpoint(const std::string& filepath);
        bool loadCheckpoint(const std::string& filepath);
        void setCheckpointManager(const std::string& directory, size_t maxCount);
        
        // Advanced control
        void setStepCallback(std::function<void(const Brain&)> callback);
        void setErrorCallback(std::function<void(const std::string&)> callback);
        void setPerformanceMonitor(bool enable, std::chrono::milliseconds interval);
        
        // Low-level neural operations
        void injectCurrentToNeurons(NeuronType type, float current);
        void setNeuronParameters(NeuronId id, const NeuronState& state);
        void applyCustomPlasticityRule(Synapse* synapse, float weightChange);
        void scheduleDelayedSpike(NeuronId source, NeuronId destination, Delay delay, float weight);
        
        // Debug and profiling
        void enableDebugMode(bool enable);
        void setLogLevel(LogLevel level);
        std::string getPerformanceProfile() const;
        void exportNeuralState(const std::string& filepath);
        
        // Accessors
        Brain* getBrain() { return brain_.get(); }
        const AdvancedConfig& getConfig() const { return config_; }
        bool isRunning() const { return isRunning_; }
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        std::shared_ptr<Brain> brain_;
        AdvancedConfig config_;
        std::mutex mutex_;
        bool isRunning_ = false;
        std::thread simulationThread_;
        std::atomic<size_t> stepCount_;
        std::chrono::steady_clock::time_point startTime_;
        
        void simulationWorker();
        void processStep(size_t step);
        void updatePerformanceStats();
        void reportPerformance();
    };
    
    /**
     * @brief Experimental protocol manager for advanced research
     */
    class ExperimentManager {
    public:
        ExperimentManager(std::shared_ptr<Brain> brain, std::shared_ptr<AdvancedNLMApi::NeuralSimulator> simulator);
        ~ExperimentManager();
        
        // Protocol management
        void loadProtocol(const std::string& filepath);
        void saveProtocol(const std::string& filepath) const;
        void runProtocol(const std::string& protocolName);
        void pauseProtocol();
        void resumeProtocol();
        void stopProtocol();
        
        // Protocol definition
        struct ProtocolStep {
            double timestamp;
            std::string action;  // "inject_current", "apply_stdp", "record_data", "reward"
            std::vector<float> parameters;
            std::string target;
            float value = 0.0f;
            bool enabled = true;
        };
        
        using Protocol = std::vector<ProtocolStep>;
        using ProtocolMap = std::unordered_map<std::string, Protocol>;
        
        // Data recording
        struct ExperimentalData {
            std::vector<double> timestamps;
            std::vector<std::vector<float>> neuralActivations;
            std::vector<std::vector<float>> synapticWeights;
            std::vector<float> neuromodulationLevels;
            std::vector<MotorCommand> motorCommands;
            std::vector<float> rewards;
            std::vector<std::string> events;
        };
        
        void startDataRecording(const std::string& recordingName);
        void stopDataRecording();
        void pauseDataRecording();
        void resumeDataRecording();
        
        ExperimentalData getRecordedData() const;
        bool saveData(const std::string& filepath) const;
        bool loadData(const std::string& filepath);
        
        // Analysis
        struct AnalysisResults {
            double meanFiringRate = 0.0;
            double stdFiringRate = 0.0;
            double coherence = 0.0;
            double informationContent = 0.0;
            std::vector<float> weightDistribution;
            std::vector<float> activationDistribution;
        };
        
        AnalysisResults analyzeData(const ExperimentalData& data) const;
        
        // Protocol management
        void addProtocol(const std::string& name, const Protocol& protocol);
        void removeProtocol(const std::string& name);
        Protocol getProtocol(const std::string& name) const;
        std::vector<std::string> getProtocolNames() const;
        
        // Real-time control
        void setProtocolSpeed(double speedFactor);
        void seekToTime(double timestamp);
        double getCurrentTime() const;
        double getProtocolDuration() const;
        
        // Statistical analysis
        void enableStatisticalAnalysis(bool enable);
        void setAnalysisWindow(double window);
        struct StatisticalSummary {
            double mean = 0.0;
            double variance = 0.0;
            double stddev = 0.0;
            double min = 0.0;
            double max = 0.0;
            size_t count = 0;
        };
        
        StatisticalSummary analyzeMetric(const std::string& metricName) const;
        
        // Comparison
        bool compareWithBaseline(const std::string& baselineFile) const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<AdvancedNLMApi::NeuralSimulator> simulator_;
        ProtocolMap protocols_;
        ExperimentalData recordedData_;
        bool isRecording_ = false;
        bool isPaused_ = false;
        std::mutex mutex_;
    };
    
    /**
     * @brief Performance optimizer for advanced users
     */
    class PerformanceOptimizer {
    public:
        PerformanceOptimizer(std::shared_ptr<Brain> brain);
        ~PerformanceOptimizer();
        
        // Memory optimization
        void optimizeMemoryLayout();
        void enableMemoryPooling(bool enable);
        void setMemoryPoolSize(size_t size);
        void enableNUMAoptimization(bool enable);
        
        // Computational optimization
        void enableSIMDOptimizations(bool enable);
        void enableLoopUnrolling(bool enable);
        void enableCacheOptimization(bool enable);
        void enableBranchPrediction(bool enable);
        
        // Threading optimization
        void enableThreadAffinity(bool enable);
        void setCpuAffinity(const std::vector<int>& cpuIds);
        void optimizeThreadScheduling();
        void enableLockFreeDataStructures(bool enable);
        
        // I/O optimization
        void enableAsynchronousIO(bool enable);
        void setIOBufferSize(size_t size);
        void enableFileBuffering(bool enable);
        
        // Profile and tune
        struct PerformanceProfile {
            double cpuUtilization = 0.0;
            double memoryBandwidth = 0.0;
            double cacheHitRate = 0.0;
            double branchPredictionAccuracy = 0.0;
            double instructionPipelineEfficiency = 0.0;
            double dataLocality = 0.0;
        };
        
        PerformanceProfile getPerformanceProfile() const;
        std::vector<std::string> getOptimizationRecommendations() const;
        
        // Apply optimizations
        void applyAllOptimizations();
        void applyMemoryOptimizations();
        void applyComputationalOptimizations();
        void applyThreadingOptimizations();
        
        // Monitoring
        void enablePerformanceMonitoring(bool enable);
        double getCurrentCPUUsage() const;
        size_t getMemoryUsage() const;
        size_t getCacheMissRate() const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        std::shared_ptr<Brain> brain_;
        bool optimizationsEnabled_ = false;
    };
    
    /**
     * @brief Main interface for advanced API
     */
    static std::unique_ptr<AdvancedNLMApi> createAdvancedApi(const AdvancedConfig& config = AdvancedConfig());
    static std::unique_ptr<AdvancedNLMApi::NeuralSimulator> createSimulator(
        std::shared_ptr<Brain> brain, const AdvancedConfig& config);
    static std::unique_ptr<AdvancedNLMApi::ExperimentManager> createExperimentManager(
        std::shared_ptr<Brain> brain, std::shared_ptr<AdvancedNLMApi::NeuralSimulator> simulator);
    static std::unique_ptr<AdvancedNLMApi::PerformanceOptimizer> createPerformanceOptimizer(
        std::shared_ptr<Brain> brain);
};

} // namespace nlm