#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

// Advanced Brain Features for Power Users

// Batch processing capabilities
class BrainBatchProcessor {
public:
    struct BatchResult {
        size_t brainId;
        std::shared_ptr<Brain> brain;
        std::chrono::duration<double> executionTime;
        bool success;
        std::string errorMessage;
        std::map<std::string, double> metrics;
    };
    
    using BatchCallback = std::function<void(const BatchResult&)>;
    
    BrainBatchProcessor() = default;
    ~BrainBatchProcessor() = default;
    
    // Process multiple brains in parallel
    std::vector<BatchResult> processBrains(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::vector<std::string>& experimentNames,
        size_t numThreads = std::thread::hardware_concurrency()
    );
    
    // Process brains with custom callback
    void processBrainsAsync(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::vector<std::string>& experimentNames,
        BatchCallback callback,
        size_t numThreads = std::thread::hardware_concurrency()
    );
    
    // Save batch results to file
    static bool saveBatchResults(const std::string& filename, const std::vector<BatchResult>& results);
    
    // Load batch results from file
    static std::vector<BatchResult> loadBatchResults(const std::string& filename);
};

// Advanced analysis tools
class AdvancedBrainAnalysis {
public:
    struct NeuralNetworkMetrics {
        double averageFiringRate;
        double firingRateVariance;
        double excitationInhibitionRatio;
        double coherence;
        double complexity;
        double integration;
        std::vector<double> activityProfile;
        std::map<std::string, double> plasticityMetrics;
    };
    
    struct LearningProgress {
        double learningRate;
        double plasticityGrowth;
        double memoryConsolidation;
        double developmentStage;
        std::vector<double> performanceOverTime;
        std::vector<double> stabilityOverTime;
    };
    
    // Analyze neural network properties
    static NeuralNetworkMetrics analyzeNetworkProperties(const Brain& brain);
    
    // Analyze learning progress over time
    static LearningProgress analyzeLearningProgress(
        const std::vector<std::shared_ptr<Brain>>& brainHistory
    );
    
    // Generate comprehensive report
    static std::string generateAnalysisReport(
        const Brain& brain,
        const std::string& experimentName,
        const std::chrono::duration<double>& executionTime
    );
    
    // Detect network patterns
    static std::vector<std::string> detectNetworkPatterns(const Brain& brain);
    
    // Calculate network efficiency
    static double calculateNetworkEfficiency(const Brain& brain);
    
    // Analyze spike timing patterns
    static std::map<std::string, double> analyzeSpikeTiming(const Brain& brain);
};

// Performance monitoring and profiling
class BrainPerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double simulationSpeed;
        double accuracy;
        double stability;
        std::chrono::duration<double> wallClockTime;
        std::chrono::duration<double> cpuTime;
        size_t memoryAllocated;
        size_t memoryFreed;
    };
    
    struct ProfilingData {
        std::string operationName;
        std::chrono::duration<double> executionTime;
        size_t memoryUsed;
        std::map<std::string, double> parameters;
    };
    
    // Start performance monitoring
    void startMonitoring();
    
    // Stop performance monitoring
    PerformanceMetrics stopMonitoring();
    
    // Record profiling data
    void recordProfiling(const std::string& operation, const std::chrono::duration<double>& time);
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
    // Check if system resources are adequate
    static bool checkSystemResources();
    
    // Optimize for current system
    static void optimizeForSystem();
};

// Advanced configuration management
class AdvancedConfigManager {
public:
    struct ExperimentConfig {
        std::string name;
        std::shared_ptr<Config> config;
        std::string description;
        std::chrono::duration<double> recommendedDuration;
        std::map<std::string, std::string> tags;
        std::vector<std::string> dependencies;
    };
    
    // Save experiment configuration
    static bool saveExperimentConfig(const ExperimentConfig& config, const std::string& filename);
    
    // Load experiment configuration
    static ExperimentConfig loadExperimentConfig(const std::string& filename);
    
    // Get recommended configurations for different use cases
    static std::vector<ExperimentConfig> getRecommendedConfigs(const std::string& useCase);
    
    // Compare configurations
    static std::string compareConfigs(const std::vector<ExperimentConfig>& configs);
    
    // Validate configuration for specific requirements
    static bool validateConfigForRequirements(
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::function<bool(const ConfigValue&)>>& requirements
    );
};

// Advanced visualization tools
class AdvancedVisualization {
public:
    struct VisualizationData {
        std::vector<double> timeSeries;
        std::vector<std::pair<double, double>> spikeTimes;
        std::vector<std::vector<double>> networkActivity;
        std::vector<std::string> labels;
        std::string title;
        std::string xlabel;
        std::string ylabel;
    };
    
    // Generate network visualization
    static std::string generateNetworkVisualization(const Brain& brain, const std::string& format = "graphml");
    
    // Generate activity heatmap
    static std::string generateActivityHeatmap(const Brain& brain, const std::string& format = "png");
    
    // Generate spike train visualization
    static std::string generateSpikeTrainVisualization(const Brain& brain, const std::string& format = "png");
    
    // Generate learning curve
    static std::string generateLearningCurve(const std::vector<std::shared_ptr<Brain>>& brainHistory, const std::string& format = "png");
    
    // Generate 3D neural network visualization
    static std::string generate3DNetworkVisualization(const Brain& brain, const std::string& format = "stl");
    
    // Generate interactive web visualization
    static std::string generateInteractiveVisualization(const Brain& brain, const std::string& filename = "visualization.html");
};

// Plugin system for extensibility
class BrainPluginManager {
public:
    using PluginFunction = std::function<void(Brain& brain, const Config& config)>;
    
    // Register a plugin
    void registerPlugin(const std::string& name, PluginFunction plugin);
    
    // Unregister a plugin
    void unregisterPlugin(const std::string& name);
    
    // Apply all plugins to a brain
    void applyPlugins(Brain& brain, const Config& config);
    
    // Get list of available plugins
    std::vector<std::string> getAvailablePlugins() const;
    
    // Save plugins to configuration
    bool savePlugins(const std::string& filename);
    
    // Load plugins from configuration
    bool loadPlugins(const std::string& filename);
    
    // Create default plugins
    static BrainPluginManager createDefaultPlugins();
};

// Hardware acceleration utilities
class HardwareAccelerator {
public:
    // Check if GPU is available
    static bool isGPUSupported();
    
    // Get available GPU devices
    static std::vector<std::string> getAvailableGPUs();
    
    // Set preferred GPU device
    static bool setGPUPreference(const std::string& deviceName);
    
    // Enable hardware acceleration
    static bool enableHardwareAcceleration();
    
    // Get current hardware configuration
    static std::string getHardwareInfo();
    
    // Optimize for current hardware
    static void optimizeForHardware();
    
    // Get system capabilities
    static std::map<std::string, bool> getSystemCapabilities();
};

// Utility functions for advanced usage
namespace Advanced {
    
    // Create custom configuration based on use case
    std::shared_ptr<Config> createConfigForUseCase(
        const std::string& useCase,
        const std::map<std::string, double>& parameters
    );
    
    // Validate configuration for power user needs
    bool validatePowerUserConfig(const std::shared_ptr<Config>& config);
    
    // Generate unique experiment identifier
    std::string generateExperimentId(const std::string& prefix = "exp");
    
    // Calculate simulation efficiency
    double calculateEfficiency(
        const Brain& brain,
        const std::chrono::duration<double>& time,
        const std::map<std::string, double>& metrics
    );
    
    // Find optimal configuration parameters
    std::shared_ptr<Config> findOptimalConfig(
        const std::vector<std::shared_ptr<Config>>& configs,
        std::function<double(const Brain&)> objectiveFunction
    );
    
    // Batch processing utilities
    template<typename T>
    std::vector<T> parallelProcess(
        const std::vector<T>& items,
        std::function<T(const T&, size_t)> processor,
        size_t numThreads = std::thread::hardware_concurrency()
    );
    
    // Data serialization utilities
    template<typename T>
    bool serializeToFile(const T& data, const std::string& filename);
    
    template<typename T>
    bool deserializeFromFile(T& data, const std::string& filename);
    
    // Performance benchmarking
    class Benchmarker {
    public:
        struct BenchmarkResult {
            std::string operationName;
            std::chrono::duration<double> duration;
            size_t memoryUsed;
            double operationsPerSecond;
            std::map<std::string, double> metrics;
        };
        
        void startBenchmark(const std::string& operation);
        BenchmarkResult stopBenchmark();
        std::string generateReport() const;
        
    private:
        std::map<std::string, std::chrono::high_resolution_clock::time_point> startTimes_;
        std::map<std::string, BenchmarkResult> results_;
    };
}

} // namespace nlm