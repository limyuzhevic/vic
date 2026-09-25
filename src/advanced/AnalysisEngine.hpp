#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "<string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <queue>
#include <set>

namespace nlm {

// Analysis result types
enum class AnalysisMetric {
    FiringRate,
    Synchronization,
    Coherence,
    Entanglement,
    PlasticityStrength,
    NetworkEfficiency,
    InformationFlow,
    TopologicalOrder,
    CommunityStructure,
    DynamicStability,
    MemoryCapacity,
    LearningRate,
    AdaptationSpeed
};

// Analysis configuration
struct AnalysisConfig {
    std::string name;
    std::string description;
    bool enable;
    std::map<std::string, float> parameters;
    std::vector<AnalysisMetric> metrics;
    size_t timeWindow;
    float updateInterval;
    
    AnalysisConfig(const std::string& n = "Analysis", const std::string& desc = "")
        : name(n), description(desc), enable(true), timeWindow(100), updateInterval(1.0f) {}
};

// Neural activity pattern
struct NeuralPattern {
    std::string patternId;
    std::vector<NeuronId> neuronIds;
    std::vector<float> activityProfile;
    float coherence;
    float stability;
    float significance;
    Timestamp timestamp;
    SimulationStep step;
    
    NeuralPattern(const std::string& id) : patternId(id), coherence(0.0f), stability(0.0f),
                                            significance(0.0f), timestamp(0.0), step(0) {}
};

// Advanced neural activity analyzer
class NeuralActivityAnalyzer {
public:
    NeuralActivityAnalyzer();
    ~NeuralActivityAnalyzer();
    
    // Activity analysis
    std::vector<float> analyzeActivity(const Neuron* neuron, size_t timeWindow = 100);
    float calculateFiringRate(const Neuron* neuron, size_t timeWindow = 100);
    float calculateFiringCoherence(const std::vector<Neuron*>& neurons, float threshold = 0.5f);
    float calculateSynchronization(const std::vector<Neuron*>& neurons, float timeWindow = 1.0f);
    float calculatePhaseLocking(const std::vector<Neuron*>& neurons, float threshold = 0.1f);
    float calculateCrossCorrelation(const Neuron* neuron1, const Neuron* neuron2, 
                                   float maxLag = 10.0f, size_t timeWindow = 100);
    
    // Advanced dynamics analysis
    float calculateNeuralEntanglement(const std::vector<Neuron*>& neurons);
    float calculateDynamicStability(const NeuralRegion* region, size_t timeWindow = 1000);
    float calculateInformationFlow(const Synapse* synapse, size_t timeWindow = 100);
    float calculateTopologicalOrder(const NeuralRegion* region);
    
    // Pattern detection
    std::vector<NeuralPattern> detectActivityPatterns(const std::vector<Neuron*>& neurons,
                                                     const std::string& patternType = "all_to_all");
    std::vector<NeuralPattern> detectOscillations(const std::vector<Neuron*>& neurons,
                                                 float minFrequency = 0.1f,
                                                 float maxFrequency = 10.0f,
                                                 float minAmplitude = 0.1f);
    std::vector<NeuralPattern> detectBursts(const std::vector<Neuron*>& neurons,
                                           float threshold = 2.0f,
                                           float minDuration = 1.0f,
                                           float maxDuration = 10.0f);
    std::vector<NeuralPattern> detectSynchrony(const std::vector<Neuron*>& neurons,
                                              float coherenceThreshold = 0.8f,
                                              size_t minGroupSize = 3);
    
    // Time-series analysis
    float calculateSpectralPower(const std::vector<float>& signal, float frequency);
    std::vector<float> calculatePowerSpectrum(const std::vector<float>& signal, 
                                             float samplingRate = 1.0f,
                                             size_t fftSize = 1024);
    float calculateHjorthComplexity(const std::vector<float>& signal);
    float calculatePermutationalEntropy(const std::vector<float>& signal);
    std::vector<float> calculateSampleEntropy(const std::vector<float>& signal,
                                             size_t templateLength = 5,
                                             float tolerance = 0.1f);
    
    // Pattern clustering and classification
    std::vector<size_t> clusterNeuronsByActivity(const std::vector<Neuron*>& neurons,
                                                 const std::string& method = "kmeans",
                                                 size_t numClusters = 3);
    float classifyPattern(const std::vector<Neuron*>& neurons,
                         const std::vector<std::vector<float>>& trainingPatterns,
                         const std::vector<std::string>& classLabels);
    
    // Advanced analysis methods
    float analyzeNetworkTopology(const NeuralRegion* region);
    float analyzeCommunityStructure(const NeuralRegion* region);
    float analyzeInformationFlow(const NeuralRegion* region);
    float analyzePlasticityDynamics(const NeuralRegion* region);
    std::vector<float> calculateNeuralMetrics(const std::vector<Neuron*>& neurons);
    
    // Configuration and control
    void setAnalysisWindow(size_t window) { analysisWindow = window; }
    void setUpdateInterval(float interval) { updateInterval = interval; }
    void enableMetric(AnalysisMetric metric, bool enable);
    void setThreshold(AnalysisMetric metric, float threshold);
    
    // Accessors
    size_t getAnalysisWindow() const { return analysisWindow; }
    float getUpdateInterval() const { return updateInterval; }
    bool isMetricEnabled(AnalysisMetric metric) const;
    float getThreshold(AnalysisMetric metric) const;
    
    // Results and reporting
    std::map<AnalysisMetric, float> getCurrentMetrics(const std::vector<Neuron*>& neurons);
    std::string getAnalysisReport(const std::vector<Neuron*>& neurons) const;
    std::string exportAnalysisData(const std::vector<Neuron*>& neurons, 
                                   const std::string& format = "json") const;
    
protected:
    size_t analysisWindow;
    float updateInterval;
    std::map<AnalysisMetric, bool> metricEnabled;
    std::map<AnalysisMetric, float> metricThresholds;
    std::map<std::string, float> algorithmParameters;
};

// Comprehensive neural analysis engine
class NeuralAnalysisEngine {
public:
    NeuralAnalysisEngine();
    ~NeuralAnalysisEngine();
    
    // Multi-scale analysis
    void analyzeMultiScale(const std::vector<NeuralRegion*>& regions);
    void analyzeLocalDynamics(const NeuralRegion* region);
    void analyzeRegionalInteractions(const std::vector<NeuralRegion*>& regions);
    void analyzeGlobalTopology(const std::vector<NeuralRegion*>& regions);
    
    // Pattern discovery and recognition
    std::vector<NeuralPattern> discoverPatterns(const std::vector<NeuralRegion*>& regions,
                                               const std::string& discoveryMethod = "automatic");
    std::vector<NeuralPattern> recognizePatterns(const std::vector<NeuralRegion*>& regions,
                                                const std::vector<NeuralPattern>& knownPatterns);
    std::vector<std::vector<Neuron*>> clusterNeuralActivity(const std::vector<NeuralRegion*>& regions,
                                                            size_t numClusters = 5);
    
    // Statistical analysis
    float calculateStatisticalSignificance(const std::vector<float>& data,
                                          const std::vector<float>& baseline);
    std::map<std::string, float> performStatisticalTests(const std::vector<float>& data);
    std::vector<float> normalizeData(const std::vector<float>& data, 
                                     const std::vector<float>& reference);
    float calculateEffectSize(const std::vector<float>& experimental,
                            const std::vector<float>& control);
    
    // Machine learning integration
    void trainPatternClassifier(const std::vector<NeuralPattern>& trainingPatterns,
                               const std::vector<std::string>& labels);
    std::string classifyPattern(const NeuralPattern& pattern);
    void updateClassifier(const NeuralPattern& pattern, const std::string& trueLabel);
    std::map<std::string, float> getClassifierMetrics() const;
    
    // Dynamic analysis
    void trackActivityDynamics(const std::vector<NeuralRegion*>& regions);
    void trackPlasticityDynamics(const std::vector<NeuralRegion*>& regions);
    void trackNetworkDynamics(const std::vector<NeuralRegion*>& regions);
    void trackDevelopmentDynamics(const std::vector<NeuralRegion*>& regions);
    
    // Advanced analysis features
    std::vector<float> calculateFourierDescriptors(const NeuralPattern& pattern);
    std::vector<float> calculateWaveletCoefficients(const NeuralPattern& pattern,
                                                    const std::string& waveletType = "morlet");
    float calculateLyapunovExponent(const NeuralPattern& pattern);
    float calculateFractalDimension(const NeuralPattern& pattern);
    
    // Configuration and control
    void setAnalysisInterval(float interval) { analysisInterval = interval; }
    void setDiscoveryThreshold(float threshold) { discoveryThreshold = threshold; }
    void enablePatternRecognition(bool enable) { patternRecognitionEnabled = enable; }
    void setLearningRate(float rate) { learningRate = rate; }
    
    // Accessors
    float getAnalysisInterval() const { return analysisInterval; }
    float getDiscoveryThreshold() const { return discoveryThreshold; }
    bool isPatternRecognitionEnabled() const { return patternRecognitionEnabled; }
    float getLearningRate() const { return learningRate; }
    
    // Results and reporting
    std::map<std::string, std::vector<float>> getAnalysisResults() const;
    std::string generateAnalysisReport() const;
    std::string exportAnalysisResults(const std::string& format = "json") const;
    bool saveAnalysisResults(const std::string& filepath);
    
    // Advanced features
    std::vector<NeuralPattern> performTimeSeriesMining(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performSpatialPatternMining(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performTemporalPatternMining(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performHybridPatternMining(const std::vector<NeuralRegion*>& regions);
    
protected:
    float analysisInterval;
    float discoveryThreshold;
    bool patternRecognitionEnabled;
    float learningRate;
    
    std::map<std::string, std::vector<float>> analysisResults;
    std::map<std::string, std::vector<NeuralPattern>> discoveredPatterns;
    std::map<std::string, std::vector<std::string>> patternClassifications;
    
    std::vector<std::vector<float>> classifierTrainingData;
    std::vector<std::string> classifierLabels;
    std::map<std::string, float> classifierMetrics;
};

// Custom metric calculator
class MetricCalculator {
public:
    MetricCalculator();
    ~MetricCalculator();
    
    // Metric types
    enum class MetricType {
        Mean,
        Median,
        StandardDeviation,
        Variance,
        Skewness,
        Kurtosis,
        Range,
        MinMax,
        Percentile,
        InterquartileRange,
        CoefficientOfVariation,
        SignalToNoiseRatio,
        ShannonEntropy,
        BoltzmannEntropy,
        GiniCoefficient,
        HerfindahlIndex
    };
    
    // Metric computation
    float calculateMetric(const std::vector<float>& data, MetricType type);
    std::map<std::string, float> calculateAllMetrics(const std::vector<float>& data);
    
    // Statistical metrics
    float calculateMean(const std::vector<float>& data);
    float calculateMedian(const std::vector<float>& data);
    float calculateStandardDeviation(const std::vector<float>& data);
    float calculateVariance(const std::vector<float>& data);
    float calculateSkewness(const std::vector<float>& data);
    float calculateKurtosis(const std::vector<float>& data);
    
    // Distribution metrics
    float calculateRange(const std::vector<float>& data);
    std::pair<float, float> calculateMinMax(const std::vector<float>& data);
    float calculatePercentile(const std::vector<float>& data, float percentile);
    float calculateInterquartileRange(const std::vector<float>& data);
    
    // Normalized metrics
    float calculateCoefficientOfVariation(const std::vector<float>& data);
    float calculateSignalToNoiseRatio(const std::vector<float>& signal, const std::vector<float>& noise);
    float calculateShannonEntropy(const std::vector<float>& data);
    float calculateBoltzmannEntropy(const std::vector<float>& data);
    float calculateGiniCoefficient(const std::vector<float>& data);
    float calculateHerfindahlIndex(const std::vector<float>& data);
    
    // Network-specific metrics
    float calculateNetworkEfficiency(const NeuralRegion* region);
    float calculateClusteringCoefficient(const NeuralRegion* region);
    float calculatePathLength(const NeuralRegion* region);
    float calculateModularity(const NeuralRegion* region,
                             const std::vector<std::vector<Neuron*>>& communities);
    float calculateBetweennessCentrality(const NeuralRegion* region, NeuronId neuronId);
    float calculateEigenvectorCentrality(const NeuralRegion* region, NeuronId neuronId);
    
    // Temporal metrics
    float calculateTemporalCoherence(const std::vector<float>& timeSeries1,
                                      const std::vector<float>& timeSeries2);
    float calculateTemporalLeadLag(const std::vector<float>& series1,
                                   const std::vector<float>& series2);
    float calculateTemporalAutocorrelation(const std::vector<float>& signal,
                                           float lag = 1);
    std::vector<float> calculateCrossCorrelation(const std::vector<float>& signal1,
                                                 const std::vector<float>& signal2,
                                                 size_t maxLag = 10);
    
    // Advanced metrics
    float calculateLyapunovExponent(const std::vector<float>& timeSeries);
    float calculateCorrelationDimension(const std::vector<float>& timeSeries);
    float calculateKolmogorovSmirnov(const std::vector<float>& data1,
                                    const std::vector<float>& data2);
    float calculateMannWhitneyU(const std::vector<float>& data1,
                               const std::vector<float>& data2);
    
    // Configuration
    void setConfidenceLevel(float level) { confidenceLevel = level; }
    void setSignificanceLevel(float level) { significanceLevel = level; }
    void enableNonParametricTests(bool enable) { nonParametricTestsEnabled = enable; }
    void setBootstrapSamples(size_t samples) { bootstrapSamples = samples; }
    
    // Accessors
    float getConfidenceLevel() const { return confidenceLevel; }
    float getSignificanceLevel() const { return significanceLevel; }
    bool areNonParametricTestsEnabled() const { return nonParametricTestsEnabled; }
    size_t getBootstrapSamples() const { return bootstrapSamples; }
    
    // Statistical testing
    struct StatisticalTestResult {
        std::string testName;
        float statistic;
        float pValue;
        bool significant;
        std::string interpretation;
    };
    
    std::vector<StatisticalTestResult> performStatisticalTests(const std::vector<float>& data1,
                                                               const std::vector<float>& data2);
    bool performTTest(const std::vector<float>& data1, const std::vector<float>& data2);
    bool performMannWhitneyUTest(const std::vector<float>& data1, const std::vector<float>& data2);
    bool performKSmoothTest(const std::vector<float>& data1, const std::vector<float>& data2);
    bool performChiSquareTest(const std::vector<size_t>& observed, const std::vector<size_t>& expected);
    
    // Confidence intervals
    std::pair<float, float> calculateConfidenceInterval(const std::vector<float>& data,
                                                         float confidence = 0.95f);
    std::pair<float, float> calculateBootstrapConfidenceInterval(const std::vector<float>& data,
                                                                  float confidence = 0.95f);
    
    // Effect size calculation
    float calculateCohenD(const std::vector<float>& data1, const std::vector<float>& data2);
    float calculateGlassDelta(const std::vector<float>& data1, const std::vector<float>& data2);
    float calculateHedgesG(const std::vector<float>& data1, const std::vector<float>& data2);
    
    // Reporting
    std::string getMetricReport(const std::vector<float>& data) const;
    std::string getStatisticalTestReport(const std::vector<StatisticalTestResult>& results) const;
    bool exportMetricsToFile(const std::string& filepath);
    
protected:
    float confidenceLevel;
    float significanceLevel;
    bool nonParametricTestsEnabled;
    size_t bootstrapSamples;
    
    std::vector<float> bootstrapSample(const std::vector<float>& data, size_t sampleSize);
    float calculateBootstrapStatistic(const std::vector<float>& data, MetricType type);
};

// Pattern recognition system
class PatternRecognizer {
public:
    PatternRecognizer();
    ~PatternRecognizer();
    
    // Pattern types
    enum class PatternType {
        Temporal,
        Spatial,
        Spectral,
        Topological,
        Dynamic,
        Hybrid,
        Custom
    };
    
    // Pattern recognition
    std::vector<NeuralPattern> recognizePatterns(const std::vector<NeuralRegion*>& regions,
                                                const std::string& patternType = "automatic");
    std::string classifyPattern(const NeuralPattern& pattern,
                                const std::string& patternType = "temporal");
    float assessPatternSimilarity(const NeuralPattern& pattern1, const NeuralPattern& pattern2);
    
    // Temporal pattern recognition
    std::vector<NeuralPattern> recognizeTemporalPatterns(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> recognizeSpatialPatterns(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> recognizeSpectralPatterns(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> recognizeTopologicalPatterns(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> recognizeDynamicPatterns(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> recognizeHybridPatterns(const std::vector<NeuralRegion*>& regions);
    
    // Machine learning for pattern recognition
    void trainPatternRecognizer(const std::vector<NeuralPattern>& trainingPatterns,
                               const std::vector<std::string>& labels,
                               const std::string& algorithm = "random_forest");
    std::string recognizePattern(const NeuralPattern& pattern);
    void updatePatternRecognizer(const NeuralPattern& pattern, const std::string& trueLabel);
    std::map<std::string, float> getPatternRecognizerMetrics() const;
    
    // Pattern matching and clustering
    std::vector<NeuralPattern> findSimilarPatterns(const NeuralPattern& pattern,
                                                  const std::vector<NeuralPattern>& patternDatabase,
                                                  float similarityThreshold = 0.8f);
    std::vector<std::vector<NeuralPattern>> clusterPatterns(const std::vector<NeuralPattern>& patterns,
                                                            size_t numClusters = 3);
    std::vector<NeuralPattern> filterPatternsByType(const std::vector<NeuralPattern>& patterns,
                                                    PatternType type);
    std::vector<NeuralPattern> filterPatternsBySimilarity(const NeuralPattern& pattern,
                                                         const std::vector<NeuralPattern>& patterns,
                                                         float threshold = 0.8f);
    
    // Advanced pattern features
    std::vector<float> extractPatternFeatures(const NeuralPattern& pattern);
    float calculatePatternComplexity(const NeuralPattern& pattern);
    float calculatePatternSignificance(const NeuralPattern& pattern);
    float calculatePatternStability(const NeuralPattern& pattern);
    
    // Configuration
    void setRecognitionAlgorithm(const std::string& algorithm) { recognitionAlgorithm = algorithm; }
    void setSimilarityMetric(const std::string& metric) { similarityMetric = metric; }
    void setPatternLibraryPath(const std::string& path) { patternLibraryPath = path; }
    void enableRealTimeRecognition(bool enable) { realTimeRecognitionEnabled = enable; }
    
    // Accessors
    std::string getRecognitionAlgorithm() const { return recognitionAlgorithm; }
    std::string getSimilarityMetric() const { return similarityMetric; }
    std::string getPatternLibraryPath() const { return patternLibraryPath; }
    bool isRealTimeRecognitionEnabled() const { return realTimeRecognitionEnabled; }
    
    // Reporting and export
    std::string getPatternRecognitionReport() const;
    std::string exportPatternsAsJSON(const std::vector<NeuralPattern>& patterns) const;
    std::string exportPatternsAsGraphML(const std::vector<NeuralPattern>& patterns) const;
    bool savePatternLibrary(const std::vector<NeuralPattern>& patterns);
    bool loadPatternLibrary(std::vector<NeuralPattern>& patterns);
    
    // Advanced features
    std::vector<NeuralPattern> performRealTimePatternRecognition(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performStreamingPatternRecognition(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performAdaptivePatternRecognition(const std::vector<NeuralRegion*>& regions);
    std::vector<NeuralPattern> performTransferLearningPatternRecognition(const std::vector<NeuralRegion*>& regions,
                                                                        const std::string& sourceDomain);
    
protected:
    std::string recognitionAlgorithm;
    std::string similarityMetric;
    std::string patternLibraryPath;
    bool realTimeRecognitionEnabled;
    
    std::vector<std::vector<float>> patternFeatureVectors;
    std::vector<std::string> patternLabels;
    std::map<std::string, float> recognizerMetrics;
};

} // namespace nlm
