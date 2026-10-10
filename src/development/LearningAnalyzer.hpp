#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../profiling/Profiler.hpp"
#include "../profiling/BenchmarkSuite.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <cmath>

namespace nlm {

class LearningAnalyzer {
public:
    LearningAnalyzer(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                    std::shared_ptr<BrainSimulator> simulator, std::shared_ptr<Profiler> profiler);
    ~LearningAnalyzer();
    
    // Disable copying, enable moving
    LearningAnalyzer(const LearningAnalyzer&) = delete;
    LearningAnalyzer& operator=(const LearningAnalyzer&) = delete;
    LearningAnalyzer(LearningAnalyzer&&) noexcept = default;
    LearningAnalyzer& operator=(LearningAnalyzer&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Learning process analysis
    void analyzeLearningProgress();
    void analyzeLearningProgress(double timeWindow);
    void analyzeLearningProgress(SimulationStep startStep, SimulationStep endStep);
    
    // Learning curve analysis
    void analyzeLearningCurves();
    void analyzeLearningCurves(const std::vector<std::string>& metrics);
    void analyzeLearningCurves(double timeWindow, size_t maxPoints);
    void analyzeLearningCurves(const std::vector<LearningCurve>& learningCurves);
    
    // Performance analysis
    void analyzePerformance();
    void analyzePerformance(const PerformanceMetrics& metrics);
    void analyzePerformance(double timeWindow);
    void analyzePerformanceBenchmark();
    
    // Convergence analysis
    void analyzeConvergence();
    void analyzeConvergence(double convergenceThreshold);
    void analyzeConvergence(size_t minSamples);
    void analyzeConvergence(const ConvergenceConfig& config);
    
    // Learning optimization
    void analyzeOptimization();
    void analyzeOptimization(const OptimizationConfig& config);
    void analyzeOptimization(double learningRate);
    void analyzeOptimization(const LearningParameters& parameters);
    
    // Learning stability analysis
    void analyzeStability();
    void analyzeStability(double stabilityThreshold);
    void analyzeStability(size_t consecutiveSteps);
    void analyzeStability(const StabilityConfig& config);
    
    // Learning metrics collection
    struct LearningMetrics {
        double timestamp;
        SimulationStep step;
        double time;
        
        // Performance metrics
        float accuracy;
        float loss;
        float errorRate;
        float precision;
        float recall;
        float f1Score;
        
        // Learning dynamics
        float learningRate;
        float gradientNorm;
        float parameterChange;
        float convergenceRate;
        float stabilityIndex;
        
        // Training statistics
        size_t totalSamples;
        size_t correctPredictions;
        size_t falsePositives;
        size_t falseNegatives;
        
        // Optimization metrics
        float lossHistory;
        float gradientHistory;
        float parameterHistory;
        float learningCurve;
        
        // Time-based metrics
        float samplesPerSecond;
        float learningSpeed;
        float adaptationRate;
        
        LearningMetrics()
            : timestamp(0.0), step(0), time(0.0), accuracy(0.0f), loss(1.0f),
              errorRate(1.0f), precision(0.0f), recall(0.0f), f1Score(0.0f),
              learningRate(0.01f), gradientNorm(0.0f), parameterChange(0.0f),
              convergenceRate(0.0f), stabilityIndex(1.0f), totalSamples(0),
              correctPredictions(0), falsePositives(0), falseNegatives(0),
              lossHistory(0.0f), gradientHistory(0.0f), parameterHistory(0.0f),
              learningCurve(0.0f), samplesPerSecond(0.0f), learningSpeed(0.0f),
              adaptationRate(0.0f) {}
    };
    
    LearningMetrics getCurrentLearningMetrics() const;
    std::vector<LearningMetrics> getLearningHistory(size_t maxHistory = 1000) const;
    
    // Learning curve data
    struct LearningCurve {
        std::string metricName;
        std::vector<double> timestamps;
        std::vector<float> values;
        std::vector<float> errors;
        std::string units;
        
        LearningCurve() : metricName("unknown"), values(0), errors(0) {}
        LearningCurve(const std::string& name, const std::vector<float>& vals)
            : metricName(name), values(vals), errors(vals.size(), 0.0f) {}
    };
    
    // Convergence analysis
    struct ConvergenceAnalysis {
        bool isConverged;
        double convergenceStep;
        float convergenceValue;
        float convergenceThreshold;
        size_t consecutiveStepsBelowThreshold;
        float convergenceRate;
        float stabilityScore;
        
        ConvergenceAnalysis()
            : isConverged(false), convergenceStep(0), convergenceValue(0.0f),
              convergenceThreshold(0.01f), consecutiveStepsBelowThreshold(0),
              convergenceRate(0.0f), stabilityScore(1.0f) {}
    };
    
    // Optimization analysis
    struct OptimizationAnalysis {
        float optimalLearningRate;
        float optimalBatchSize;
        float optimalRegularization;
        float gradientFlowQuality;
        float lossLandscape smoothness;
        float escapeFromLocalMinima;
        
        OptimizationAnalysis()
            : optimalLearningRate(0.01f), optimalBatchSize(32), optimalRegularization(0.001f),
              gradientFlowQuality(1.0f), smoothness(0.0f), escapeFromLocalMinima(0.0f) {}
    };
    
    // Stability analysis
    struct StabilityAnalysis {
        float stabilityScore;
        float variance;
        float standardDeviation;
        float coefficientOfVariation;
        size_t stableSteps;
        size_t unstableSteps;
        float recoveryRate;
        
        StabilityAnalysis()
            : stabilityScore(1.0f), variance(0.0f), standardDeviation(0.0f),
              coefficientOfVariation(0.0f), stableSteps(0), unstableSteps(0),
              recoveryRate(0.0f) {}
    };
    
    // Learning data management
    void storeLearningData(const std::string& experimentName, const std::vector<LearningMetrics>& data);
    std::vector<LearningMetrics> loadLearningData(const std::string& experimentName) const;
    void clearLearningData(const std::string& experimentName);
    std::vector<std::string> listLearningExperiments() const;
    
    // Learning analysis and reporting
    void generateLearningReport(const std::string& filename = "learning_report.txt");
    void exportLearningData(const std::string& filename, const std::string& format = "csv");
    void generateLearningAnalysis(const std::string& filename = "learning_analysis.txt");
    void generatePerformanceAnalysis(const std::string& filename = "performance_analysis.txt");
    
    // Learning comparison
    void compareLearningExperiments(const std::vector<std::string>& experimentNames);
    void compareLearningCurves(const std::vector<LearningCurve>& curves);
    void comparePerformanceMetrics(const std::vector<LearningMetrics>& metrics);
    
    // Learning prediction and forecasting
    void predictLearningProgress(double forecastTime);
    void predictPerformance(double forecastTime);
    void predictConvergence(double convergenceThreshold);
    
    // Learning optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations() const;
    std::vector<std::string> generateConvergenceRecommendations() const;
    std::vector<std::string> generateStabilityRecommendations() const;
    
    // Integration with other systems
    void syncWithProfiler(std::shared_ptr<Profiler> profiler);
    void syncWithBenchmark(std::shared_ptr<BenchmarkSuite> benchmark);
    void syncWithDebugger(std::shared_ptr<BrainDebugger> debugger);
    void syncWithSimulator(std::shared_ptr<BrainSimulator> simulator);
    
    // Configuration
    void loadLearningConfiguration(const std::string& configFile);
    void saveLearningConfiguration() const;
    void resetLearningConfiguration();
    LearningConfig getCurrentLearningConfiguration() const;
    
    // Thread safety
    void lockLearningAnalyzer() const;
    void unlockLearningAnalyzer() const;
    bool tryLockLearningAnalyzer() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Profiler> profiler_;
    std::shared_ptr<BenchmarkSuite> benchmark_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex analyzerMutex_;
    std::atomic<bool> isInitialized_;
    std::atomic<bool> isAnalyzing_;
    std::thread analysisThread_;
    
    // Learning configuration
    LearningConfig learningConfig_;
    
    // Learning data storage
    std::unordered_map<std::string, std::vector<LearningMetrics>> experimentData_;
    std::deque<LearningMetrics> learningHistory_;
    
    // Performance data
    std::vector<LearningCurve> learningCurves_;
    std::vector<ConvergenceAnalysis> convergenceAnalysis_;
    std::vector<OptimizationAnalysis> optimizationAnalysis_;
    std::vector<StabilityAnalysis> stabilityAnalysis_;
    
    // Analysis results
    std::vector<std::string> optimizationRecommendations_;
    std::vector<std::string> convergenceRecommendations_;
    std::vector<std::string> stabilityRecommendations_;
    
    // Performance monitoring
    double analysisStartTime_;
    double analysisEndTime_;
    double totalAnalysisTime_;
    
    // Internal helper methods
    void cleanup();
    void updateLearningState();
    void collectLearningData();
    void analyzeLearningProgressInternal();
    void analyzeLearningCurvesInternal();
    void analyzePerformanceInternal();
    void analyzeConvergenceInternal();
    void analyzeOptimizationInternal();
    void analyzeStabilityInternal();
    void generateLearningReports();
    void updateLearningHistory();
    LearningMetrics captureCurrentLearningMetrics() const;
    void calculateLearningMetrics(LearningMetrics& metrics) const;
    void calculateLearningCurves();
    void calculateConvergenceAnalysis();
    void calculateOptimizationAnalysis();
    void calculateStabilityAnalysis();
    void validateLearningMetrics(LearningMetrics& metrics) const;
    void logLearningUpdate(const LearningMetrics& metrics);
    void exportLearningDataInternal() const;
    void generateLearningReportInternal() const;
    void generateLearningAnalysisInternal() const;
    void generatePerformanceAnalysisInternal() const;
    bool isLearningConverged(const LearningMetrics& metrics) const;
    std::string generateOptimizationRecommendation(const OptimizationAnalysis& analysis) const;
    std::string generateConvergenceRecommendation(const ConvergenceAnalysis& analysis) const;
    std::string generateStabilityRecommendation(const StabilityAnalysis& analysis) const;
    void syncWithExternalSystems();
};

} // namespace nlm