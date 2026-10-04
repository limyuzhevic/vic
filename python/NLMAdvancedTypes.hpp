#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/eval.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;

// Enhanced sensory processing with machine learning utilities
class AdvancedSensoryProcessor {
public:
    AdvancedSensoryProcessor() = default;
    
    // Feature extraction and preprocessing
    std::vector<float> extractFeatures(const SensoryInput& input) const;
    std::vector<float> normalizeFeatures(const std::vector<float>& features) const;
    std::vector<float> applyFeatureScaling(const std::vector<float>& features, 
                                           const std::string& method = "zscore") const;
    
    // Pattern recognition utilities
    std::vector<float> computeSobelEdges(const std::vector<float>& image) const;
    std::vector<float> computeGradientMagnitude(const std::vector<float>& image) const;
    std::vector<float> applyHessianLaplace(const std::vector<float>& image) const;
    
    // Statistical analysis
    float computeMean(const std::vector<float>& data) const;
    float computeVariance(const std::vector<float>& data) const;
    float computeStdDev(const std::vector<float>& data) const;
    float computeCorrelation(const std::vector<float>& x, const std::vector<float>& y) const;
    
    // Signal processing
    std::vector<float> applyGaussianFilter(const std::vector<float>& signal, 
                                           float sigma = 1.0f) const;
    std::vector<float> computeFFT(const std::vector<float>& signal) const;
    std::vector<float> applyWaveletTransform(const std::vector<float>& signal, 
                                             const std::string& wavelet = "db4") const;
    
    // Machine learning utilities
    std::vector<float> computePCA(const std::vector<std::vector<float>>& data, 
                                 int n_components = 2) const;
    std::vector<float> tSNE(const std::vector<std::vector<float>>& data, 
                            int n_components = 2, int max_iter = 1000) const;
    std::vector<float> kmeansClustering(const std::vector<std::vector<float>>& data, 
                                        int n_clusters = 3) const;
    
    // Quality metrics
    float computeSignalToNoiseRatio(const std::vector<float>& signal) const;
    float computeSpectral flatness(const std::vector<float>& spectrum) const;
    float computeSharpness(const std::vector<float>& image) const;
};

// Advanced visualization system
class VisualizationSystem {
public:
    VisualizationSystem() = default;
    
    // Neural activity visualization
    void visualizeNeuralActivity(const Brain& brain, 
                                const std::string& outputFile = "neural_activity.png") const;
    void visualizeNeuronalConnectivity(const Brain& brain,
                                       const std::string& outputFile = "connectivity.png") const;
    void visualizePlasticityChanges(const Brain& brain,
                                   const std::string& outputFile = "plasticity.png") const;
    
    // Agent behavior visualization
    void visualizeAgentPath(const AgentBrain& agent, 
                           const SimpleWorld& world,
                           const std::string& outputFile = "agent_path.png") const;
    void visualizeSensoryInput(const SensoryPercept& percept,
                              const std::string& outputFile = "sensory_input.png") const;
    void visualizeMotorOutput(const AgentBrain& agent,
                             const std::string& outputFile = "motor_output.png") const;
    
    // Time series visualization
    void visualizeTimeSeries(const std::vector<double>& data,
                           const std::string& title = "Time Series",
                           const std::string& outputFile = "time_series.png") const;
    void visualizeMultivariateTimeSeries(const std::vector<std::vector<double>>& data,
                                        const std::vector<std::string>& labels,
                                        const std::string& outputFile = "multivariate.png") const;
    
    // Interactive visualization
    void createInteractiveNotebook(const std::string& notebookFile = "nlm_analysis.ipynb") const;
    void createWebDashboard(const std::string& port = "8080",
                           const std::string& outputDir = "dashboard") const;
    
    // Statistical visualization
    void createHistogram(const std::vector<float>& data,
                         const std::string& title = "Distribution",
                         const std::string& outputFile = "histogram.png") const;
    void createScatterPlot(const std::vector<float>& x, const std::vector<float>& y,
                          const std::string& outputFile = "scatter.png") const;
    void createHeatmap(const std::vector<std::vector<float>>& data,
                      const std::string& outputFile = "heatmap.png") const;
    void createContourPlot(const std::vector<std::vector<float>>& data,
                          const std::string& outputFile = "contour.png") const;
};

// Advanced training and experimentation framework
class AdvancedExperimentationFramework {
public:
    AdvancedExperimentationFramework() = default;
    
    // Experimental design utilities
    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::unordered_map<std::string, std::string> parameters;
        std::vector<std::string> conditions;
        std::vector<std::string> metrics;
        double duration;
        int repetitions;
    };
    
    // Automated experiment running
    void runExperiment(const ExperimentConfig& config);
    void runHyperparameterSearch(const std::unordered_map<std::string, std::vector<float>>& paramGrid,
                                const std::function<void(const std::unordered_map<std::string, float>&)>& callback);
    void runEvolutionaryAlgorithm(const std::function<float(const std::vector<float>&)>& fitnessFunction,
                                 int populationSize = 100, int generations = 100);
    
    // Statistical analysis of experiments
    struct ExperimentResult {
        std::string experimentName;
        std::unordered_map<std::string, float> metrics;
        std::unordered_map<std::string, float> parameters;
        double executionTime;
        double timestamp;
    };
    
    void analyzeResults(const std::vector<ExperimentResult>& results);
    void performStatisticalTests(const std::vector<ExperimentResult>& results);
    void generateExperimentReport(const std::vector<ExperimentResult>& results,
                                 const std::string& outputFile = "experiment_report.pdf") const;
    
    // Experiment management
    void saveExperimentConfig(const ExperimentConfig& config, const std::string& filepath) const;
    ExperimentConfig loadExperimentConfig(const std::string& filepath) const;
    std::vector<ExperimentResult> loadExperimentResults(const std::string& filepath) const;
    void saveExperimentResults(const std::vector<ExperimentResult>& results, const std::string& filepath) const;
};

// Expert command-line interface utilities
class CommandLineTools {
public:
    CommandLineTools() = default;
    
    // Advanced configuration tools
    struct ConfigurationBuilder {
        std::unordered_map<std::string, std::string> settings;
        
        ConfigurationBuilder& set(std::string key, std::string value);
        ConfigurationBuilder& loadFromFile(const std::string& filepath);
        ConfigurationBuilder& saveToFile(const std::string& filepath) const;
        ConfigurationBuilder& applyDefaults();
        std::shared_ptr<Config> build() const;
    };
    
    // Model training and optimization tools
    struct TrainingManager {
        std::vector<std::shared_ptr<Brain>> models;
        std::vector<std::vector<float>> datasets;
        
        void addModel(const std::shared_ptr<Brain>& model);
        void addDataset(const std::vector<float>& data);
        void trainModels(const std::string& lossFunction = "mse", 
                        const std::string& optimizer = "adam",
                        int epochs = 100);
        void validateModels();
        void exportModels(const std::string& directory) const;
    };
    
    // Data management tools
    struct DataManager {
        std::string dataDirectory;
        
        void loadTrainingData(const std::string& filename);
        void loadTestData(const std::string& filename);
        void saveProcessedData(const std::string& filename);
        void splitData(float trainRatio = 0.8f, float valRatio = 0.2f);
        void normalizeData(float mean = 0.0f, float stddev = 1.0f);
        void applyFeatureExtraction(const std::shared_ptr<AdvancedSensoryProcessor>& processor);
    };
    
    // Visualization command-line tools
    static void generateReport(const Brain& brain, const std::string& outputFile = "report.pdf");
    static void createDashboard(const std::string& outputDir = "dashboard");
    static void exportResults(const std::string& outputFormat = "json");
    
    // Performance profiling tools
    static void profilePerformance(const std::function<void()>& benchmarkFunction);
    static void memoryProfile(const std::function<void()>& benchmarkFunction);
    static void timeProfile(const std::function<void()>& benchmarkFunction);
};

// Memory management and optimization utilities
class MemoryManagementTools {
public:
    MemoryManagementTools() = default;
    
    // Memory pool management
    struct MemoryPool {
        size_t totalSize;
        size_t allocated;
        size_t peak;
        
        MemoryPool(size_t size = 1024 * 1024 * 1024); // 1GB default
        void* allocate(size_t size);
        void deallocate(void* ptr);
        void clear();
        size_t getUsedMemory() const;
        size_t getAvailableMemory() const;
    };
    
    // Object lifecycle management
    template<typename T>
    class ObjectManager {
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> objects;
        std::unordered_map<std::string, int> referenceCounts;
        
    public:
        std::shared_ptr<T> create(const std::string& id, const T& object);
        std::shared_ptr<T> get(const std::string& id) const;
        void destroy(const std::string& id);
        void clear();
        std::vector<std::string> getAllIds() const;
        void incrementReference(const std::string& id);
        void decrementReference(const std::string& id);
        size_t getReferenceCount(const std::string& id) const;
    };
    
    // Garbage collection utilities
    static void forceGarbageCollection();
    static void enableMemoryTracking(bool enable = true);
    static size_t getTotalMemoryUsage();
    static size_t getPeakMemoryUsage();
    static std::unordered_map<std::string, size_t> getMemoryUsageByType();
    
    // Cache management
    static void enableCaching(bool enable = true);
    template<typename K, typename V>
    class SimpleCache {
    private:
        std::unordered_map<K, V> cache;
        size_t maxSize;
        
    public:
        SimpleCache(size_t maxSize = 1000);
        void put(const K& key, const V& value);
        bool get(const K& key, V& value) const;
        bool contains(const K& key) const;
        void remove(const K& key);
        void clear();
        size_t size() const;
    };
};

// Advanced serialization system
class SerializationManager {
public:
    SerializationManager() = default;
    
    // Binary serialization
    template<typename T>
    static void saveBinary(const T& object, const std::string& filepath);
    
    template<typename T>
    static T loadBinary(const std::string& filepath);
    
    // Compressed serialization
    template<typename T>
    static void saveCompressed(const T& object, const std::string& filepath, 
                               int compressionLevel = 6);
    
    template<typename T>
    static T loadCompressed(const std::string& filepath);
    
    // JSON serialization
    template<typename T>
    static void saveJSON(const T& object, const std::string& filepath);
    
    template<typename T>
    static T loadJSON(const std::string& filepath);
    
    // XML serialization
    template<typename T>
    static void saveXML(const T& object, const std::string& filepath);
    
    template<typename T>
    static T loadXML(const std::string& filepath);
    
    // CUDA serialization for GPU acceleration
    static void saveCUDA(const std::string& objectName, const std::string& filepath);
    static void* loadCUDA(const std::string& filepath, size_t& size);
    
    // Network serialization
    static std::vector<char> serializeForNetwork(const Brain& brain);
    static std::shared_ptr<Brain> deserializeFromNetwork(const std::vector<char>& data);
    
    // Version control for serialized data
    static void enableVersionControl(const std::string& repoPath);
    static void saveWithVersion(const std::string& objectName, const std::string& filepath);
    static std::shared_ptr<Brain> loadWithVersion(const std::string& objectName,
                                                 const std::string& version = "latest");
};

// Debug and analysis tools
class DebugTools {
public:
    DebugTools() = default;
    
    // Runtime debugging
    static void enableDebugMode(bool enable = true);
    static void setBreakPoint(const std::string& condition);
    static void logDebugInfo(const std::string& message);
    static void validateState(const Brain& brain);
    static void checkMemoryLeaks();
    
    // Performance debugging
    static void enablePerformanceProfiling(bool enable = true);
    static void startProfile(const std::string& operation);
    static void endProfile(const std::string& operation);
    static void printProfileResults();
    
    // Neural debugging
    static void visualizeNeuronStates(const Brain& brain);
    static void visualizeSynapticWeights(const Brain& brain);
    static void visualizePlasticityMaps(const Brain& brain);
    static void analyzeNeuralDynamics(const Brain& brain);
    
    // Agent debugging
    static void trackAgentBehavior(const AgentBrain& agent);
    static void logActionSequences(const AgentBrain& agent);
    static void analyzeDecisionMaking(const AgentBrain& agent);
    
    // Data debugging
    static void validateDataIntegrity(const std::vector<float>& data);
    static void checkForNaN(const std::vector<float>& data);
    static void checkForInf(const std::vector<float>& data);
    static void detectAnomalies(const std::vector<float>& data);
    
    // File system debugging
    static void checkFileSystemHealth();
    static void monitorMemoryUsage();
    static void checkNetworkConnections();
    static void logSystemResources();
};

} // namespace nlm