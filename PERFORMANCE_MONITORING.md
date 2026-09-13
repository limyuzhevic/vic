# NLM Performance Monitoring Framework

This document describes the performance monitoring and profiling tools added to the NLM (Neural Learning Machine) system.

## Overview

Performance monitoring is crucial for understanding the computational characteristics of neural simulations. The NLM performance monitoring framework provides comprehensive profiling capabilities to help users optimize their simulations for different use cases.

## Core Performance Monitoring Components

### 1. Performance Metrics

The NLM performance monitoring system tracks multiple dimensions of performance:

#### Neural Performance Metrics
- **Spike Generation**: Total spikes, average firing rate, spike frequency distribution
- **Network Dynamics**: Excitation/inhibition ratio, synaptic weight statistics
- **Memory Systems**: Working memory trace counts, episodic memory episode counts

#### Neuromodulation Metrics
- **Dopamine Levels**: Baseline, phasic responses, learning modulation
- **Curiosity Levels**: Exploration drive, novelty detection sensitivity
- **Prediction Error**: Learning signal strength and timing

#### Computational Metrics
- **Simulation Speed**: Wall-clock time, CPU time, steps per second
- **Memory Usage**: RAM consumption, memory allocation patterns
- **I/O Operations**: File read/write performance, checkpoint timing

### 2. Performance Monitoring API

```cpp
// Core performance monitoring interface
class PerformanceMonitor {
public:
    // Initialize performance monitoring
    void initialize(const PerformanceConfig& config);
    
    // Start timing a specific operation
    void startTimer(const std::string& operationName);
    
    // End timing and record result
    void stopTimer(const std::string& operationName);
    
    // Record a performance metric
    void recordMetric(const std::string& name, double value);
    
    // Get current performance statistics
    PerformanceStats getStats() const;
    
    // Generate performance report
    void generateReport(const std::string& outputFile);
};

// Configuration for performance monitoring
struct PerformanceConfig {
    bool enableProfiling = true;           // Enable/disable profiling
    bool enableMetrics = true;             // Enable metrics collection
    double samplingInterval = 0.1;         // Sampling interval in seconds
    size_t maxHistorySize = 10000;         // Maximum number of stored metrics
    std::string outputDirectory = ".";   // Output directory for reports
    bool generatePlots = true;            // Generate performance plots
};

// Comprehensive performance statistics
struct PerformanceStats {
    // Neural performance
    struct {
        size_t totalSpikes = 0;
        double averageFiringRate = 0.0;
        double spikeFrequencyHz = 0.0;
        double eiRatio = 0.0;
        std::vector<double> spikeFrequencyDistribution;
    } neural;
    
    // Memory system performance
    struct {
        size_t workingMemoryTraces = 0;
        size_t episodicMemoryEpisodes = 0;
        double memoryUsageMB = 0.0;
        size_t allocationCount = 0;
    } memory;
    
    // Neuromodulation performance
    struct {
        double dopamineLevel = 0.0;
        double curiosityLevel = 0.0;
        double noveltyLevel = 0.0;
        double predictionError = 0.0;
    } neuromodulation;
    
    // Computational performance
    struct {
        double simulationTimeSeconds = 0.0;
        double wallClockTimeSeconds = 0.0;
        double stepsPerSecond = 0.0;
        double cpuTimePercentage = 0.0;
        std::unordered_map<std::string, double> operationTimes;
    } computational;
    
    // Resource usage
    struct {
        double peakMemoryUsageMB = 0.0;
        double averageMemoryUsageMB = 0.0;
        size_t fileHandles = 0;
        size_t networkConnections = 0;
    } resources;
    
    // Time statistics
    struct {
        double minStepTime = 0.0;
        double maxStepTime = 0.0;
        double medianStepTime = 0.0;
        double stddevStepTime = 0.0;
    } timing;
};
```

### 3. Performance Monitoring Implementation

#### 3.1 Real-time Monitoring

```cpp
// Real-time performance monitoring during simulation
class RealTimePerformanceMonitor {
public:
    RealTimePerformanceMonitor(nlm::Brain* brain, 
                               const PerformanceConfig& config);
    
    void update(size_t step, double dt);
    
    PerformanceStats getCurrentStats() const;
    
    void setCallback(std::function<void(const PerformanceStats&)> callback);
    
private:
    void sampleMetrics(size_t step);
    void updateComputationalMetrics(size_t step, double dt);
    void updateNeuralMetrics();
    void updateMemoryMetrics();
    void updateNeuromodulationMetrics();
    
    nlm::Brain* brain_;
    PerformanceConfig config_;
    PerformanceStats currentStats_;
    std::vector<PerformanceStats> history_;
    std::function<void(const PerformanceStats&)> callback_;
    std::chrono::steady_clock::time_point lastSampleTime_;
};
```

#### 3.2 Detailed Profiling

```cpp
// Detailed profiling with operation-specific timing
class DetailedProfiler {
public:
    struct OperationProfile {
        std::string name;
        size_t callCount = 0;
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double avgTime() const { return callCount > 0 ? totalTime / callCount : 0.0; }
        double efficiency() const { return avgTime(); }
    };
    
    void startOperation(const std::string& name);
    void stopOperation(const std::string& name);
    
    const std::vector<OperationProfile>& getOperationProfiles() const;
    
    void reset();
    
    void generateReport(const std::string& outputFile);
    
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeOperations_;
    std::vector<OperationProfile> profiles_;
    std::mutex mutex_;
};
```

### 4. Performance Analysis Tools

#### 4.1 Learning Curve Analysis

```cpp
class LearningCurveAnalyzer {
public:
    struct LearningMetrics {
        size_t step;
        double reward;
        double performance;
        double exploration;
        double consolidation;
    };
    
    LearningCurveAnalyzer(size_t numSamples = 1000);
    
    void addDataPoint(size_t step, double reward, double performance);
    
    std::vector<LearningMetrics> analyzeLearningCurve() const;
    
    double calculateLearningRate() const;
    double calculateSaturationPoint() const;
    double calculateExplorationEfficiency() const;
    
    void plotLearningCurve(const std::string& outputFile);
    
private:
    std::vector<std::pair<size_t, double>> rewardData_;
    std::vector<std::pair<size_t, double>> performanceData_;
    size_t numSamples_;
};
```

#### 4.2 Scalability Analysis

```cpp
class ScalabilityAnalyzer {
public:
    struct ScalabilityMetrics {
        size_t neuronCount;
        size_t steps;
        double wallClockTime;
        double stepsPerSecond;
        double memoryUsageMB;
        double cpuEfficiency;
    };
    
    ScalabilityAnalyzer();
    
    void runBenchmark(size_t neuronCount, size_t steps);
    
    std::vector<ScalabilityMetrics> getBenchmarkResults() const;
    
    void analyzeScalability();
    
    void generateScalabilityReport(const std::string& outputFile);
    
private:
    std::vector<ScalabilityMetrics> benchmarks_;
    nlm::Config benchmarkConfig_;
};
```

### 5. Integration with NLM

#### 5.1 Integration with Main NLM Class

```cpp
// Enhanced NLM class with performance monitoring
class PerformanceEnhancedNLM : public nlm::Brain {
public:
    PerformanceEnhancedNLM(std::shared_ptr<nlm::Config> config);
    ~PerformanceEnhancedNLM() override;
    
    // Override step method to include performance monitoring
    void step(SimulationStep currentStep) override;
    
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Performance monitoring interface
    void enablePerformanceMonitoring(const PerformanceConfig& config);
    void disablePerformanceMonitoring();
    
    PerformanceStats getPerformanceStats() const;
    
    void generatePerformanceReport(const std::string& outputFile);
    
    // Callback for real-time performance updates
    void setPerformanceCallback(std::function<void(const PerformanceStats&)> callback);
    
protected:
    void initializePerformanceMonitor();
    void updatePerformanceMetrics(SimulationStep currentStep, Timestamp currentTime);
    
private:
    std::unique_ptr<RealTimePerformanceMonitor> performanceMonitor_;
    PerformanceConfig performanceConfig_;
    std::function<void(const PerformanceStats&)> performanceCallback_;
    bool performanceMonitoringEnabled_;
};
```

#### 5.2 Command Line Integration

```cpp
// Command-line options for performance monitoring
class PerformanceCLI {
public:
    PerformanceCLI(int argc, char* argv[]);
    
    bool parseArguments();
    
    void applyToNLM(PerformanceEnhancedNLM& nlm);
    
private:
    int argc_;
    char** argv_;
    
    bool enableProfiling_ = false;
    bool enableMetrics_ = false;
    std::string outputDirectory_ = ".";
    bool generatePlots_ = true;
    double samplingInterval_ = 0.1;
    
    std::string performanceReportFile_;
    std::string metricsFile_;
    std::string profileFile_;
    
    struct DetailedProfiler {
        void startOperation(const std::string& name);
        void stopOperation(const std::string& name);
        const std::vector<OperationProfile>& getProfiles() const;
        void reset();
        void generateReport(const std::string& outputFile);
    private:
        std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeOperations_;
        std::vector<OperationProfile> profiles_;
        std::mutex mutex_;
    } profiler_;
};
```

### 6. Performance Report Generation

#### 6.1 Comprehensive Report Generation

```cpp
class PerformanceReportGenerator {
public:
    struct ReportConfig {
        bool includeNeuralMetrics = true;
        bool includeMemoryMetrics = true;
        bool includeNeuromodulationMetrics = true;
        bool includeComputationalMetrics = true;
        bool includeResourceMetrics = true;
        bool includeTimingMetrics = true;
        
        bool generateTextReport = true;
        bool generateJSONReport = true;
        bool generateCSVReport = true;
        bool generateHTMLReport = true;
        bool generatePlot = true;
        
        std::string outputDirectory = ".";
        std::string reportName = "nlm_performance_report";
    };
    
    PerformanceReportGenerator(const PerformanceStats& stats, const ReportConfig& config);
    
    void generate();
    
    std::string getTextReport() const;
    std::string getJSONReport() const;
    std::string getCSVReport() const;
    std::string getHTMLReport() const;
    
    void saveReport(const std::string& filename);
    
private:
    PerformanceStats stats_;
    ReportConfig config_;
    
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
    std::string generateHTMLReport() const;
    std::string generatePlotData() const;
};
```

### 7. Example Usage

#### 7.1 Basic Performance Monitoring

```cpp
// Example: Basic performance monitoring during simulation
int main() {
    // Initialize NLM
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    
    PerformanceEnhancedNLM nlm(config);
    
    // Enable performance monitoring
    PerformanceConfig perfConfig;
    perfConfig.enableProfiling = true;
    perfConfig.enableMetrics = true;
    perfConfig.samplingInterval = 0.05; // Sample every 0.05 seconds
    perfConfig.generatePlots = true;
    
    nlm.enablePerformanceMonitoring(perfConfig);
    
    // Set up callback for real-time updates
    nlm.setPerformanceCallback([](const PerformanceStats& stats) {
        std::cout << "Step " << stats.computational.stepsPerSecond 
                  << " firing rate: " << stats.neural.averageFiringRate 
                  << " E/I ratio: " << stats.neural.eiRatio << std::endl;
    });
    
    // Run simulation
    for (size_t step = 0; step < 5000; ++step) {
        nlm.step(step, step * 0.001);
    }
    
    // Generate performance report
    nlm.generatePerformanceReport("nlm_performance_report.html");
    
    return 0;
}
```

#### 7.2 Detailed Profiling

```cpp
// Example: Detailed profiling of specific operations
class CustomProfilerExample {
public:
    void runProfiledSimulation() {
        // Initialize profiler
        DetailedProfiler profiler;
        
        // Start profiling simulation
        profiler.startOperation("simulation_step");
        
        // Run simulation with profiling
        for (size_t step = 0; step < 1000; ++step) {
            // Profile individual operations
            profiler.startOperation("brain_step");
            brain->step(step);
            profiler.stopOperation("brain_step");
            
            profiler.startOperation("memory_update");
            workingMemory->update(0.001);
            profiler.stopOperation("memory_update");
            
            profiler.startOperation("neuromodulation");
            dopamine->update(0.001);
            profiler.stopOperation("neuromodulation");
        }
        
        profiler.stopOperation("simulation_step");
        
        // Generate profiling report
        profiler.generateReport("detailed_profiling_report.html");
        
        // Analyze results
        analyzeProfilingResults(profiler.getProfiles());
    }
    
    void analyzeProfilingResults(const std::vector<DetailedProfiler::OperationProfile>& profiles) {
        std::cout << "=== Profiling Results ===" << std::endl;
        for (const auto& profile : profiles) {
            std::cout << profile.name << ": " << profile.callCount << " calls, "
                      << profile.avgTime() << "s per call, "
                      << (profile.avgTime() * profile.callCount) << " total time" << std::endl;
        }
    }
};
```

#### 7.3 Learning Curve Analysis

```cpp
// Example: Learning curve analysis
class LearningCurveExample {
public:
    void demonstrateLearningCurveAnalysis() {
        // Run simulation
        runSimulationWithLearningMetrics();
        
        // Analyze learning curve
        LearningCurveAnalyzer analyzer(1000);  // Analyze last 1000 steps
        
        // Add data points from simulation
        for (size_t i = 0; i < simulationData_.size(); ++i) {
            analyzer.addDataPoint(
                simulationData_[i].step,
                simulationData_[i].reward,
                simulationData_[i].performance
            );
        }
        
        // Analyze learning curve
        auto learningMetrics = analyzer.analyzeLearningCurve();
        
        double learningRate = analyzer.calculateLearningRate();
        double saturationPoint = analyzer.calculateSaturationPoint();
        
        std::cout << "=== Learning Curve Analysis ===" << std::endl;
        std::cout << "Learning Rate: " << learningRate << std::endl;
        std::cout << "Saturation Point: " << saturationPoint << std::endl;
        std::cout << "Exploration Efficiency: " 
                  << analyzer.calculateExplorationEfficiency() << std::endl;
        
        // Plot learning curve
        analyzer.plotLearningCurve("learning_curve.png");
    }
    
private:
    struct SimulationData {
        size_t step;
        double reward;
        double performance;
    };
    
    std::vector<SimulationData> simulationData_;
    
    void runSimulationWithLearningMetrics() {
        // Run simulation and collect data
        // This is a placeholder for your actual simulation logic
        for (size_t step = 0; step < 5000; ++step) {
            SimulationData data;
            data.step = step;
            data.reward = calculateReward(step);
            data.performance = calculatePerformance(step);
            simulationData_.push_back(data);
        }
    }
    
    double calculateReward(size_t step) {
        // Your reward calculation logic
        return 0.5 + 0.5 * std::sin(step * 0.01);
    }
    
    double calculatePerformance(size_t step) {
        // Your performance calculation logic
        return 0.3 + 0.7 * (1.0 - std::exp(-step * 0.001));
    }
};
```

### 8. Performance Monitoring Configuration

#### 8.1 Configuration File Example

```ini
# Performance Monitoring Configuration
[performance]
enabled = true
sampling_interval = 0.05
max_history_size = 10000
output_directory = ./performance_reports

[performance.profiling]
enabled = true
profile_level = detailed  # options: basic, detailed, comprehensive

[performance.metrics]
enabled = true
neural_metrics = true
memory_metrics = true
neuromodulation_metrics = true
computational_metrics = true
resource_metrics = true
timing_metrics = true

[performance.analysis]
enabled = true
learning_curve_analysis = true
scalability_analysis = true
performance_benchmarks = true

[performance.reporting]
text_report = true
json_report = true
csv_report = true
html_report = true
plot_report = true
report_name = nlm_performance_report
```

#### 8.2 Command Line Options

```bash
# Command line options for performance monitoring
./nlm --neurons 1000 --seed 12345 
    --enable-performance-monitoring 
    --performance-sampling-interval 0.1
    --performance-output-directory ./reports
    --performance-generate-plots
    --performance-enable-learning-curve-analysis
    --performance-enable-scalability-analysis
    --help

# Performance monitoring specific options
--enable-performance-monitoring          Enable performance monitoring
--disable-performance-monitoring          Disable performance monitoring
--performance-sampling-interval <seconds> Sampling interval for metrics (default: 0.1)
--performance-max-history <count>        Maximum number of stored metrics (default: 10000)
--performance-output-directory <dir>      Output directory for reports (default: ./performance_reports)
--performance-generate-plots              Generate performance plots (default: true)
--performance-text-report                 Generate text report (default: true)
--performance-json-report                Generate JSON report (default: true)
--performance-csv-report                 Generate CSV report (default: true)
--performance-html-report                Generate HTML report (default: true)
--performance-disable-learning-curve-analysis Disable learning curve analysis (default: false)
--performance-disable-scalability-analysis   Disable scalability analysis (default: false)

# Examples
./nlm --neurons 500 --performance-sampling-interval 0.05 --performance-max-history 5000
./nlm --neurons 2000 --enable-performance-monitoring --performance-output-directory ./my_reports
./nlm --neurons 1000 --performance-disable-learning-curve-analysis --performance-disable-scalability-analysis
```

### 9. Performance Monitoring Best Practices

#### 9.1 For Researchers

1. **Use Consistent Sampling**: Maintain consistent sampling intervals for reproducible results
2. **Baseline Comparison**: Always compare against a baseline version to measure improvements
3. **Multi-dimensional Analysis**: Analyze performance from multiple dimensions (neural, memory, computational)
4. **Ablation Studies**: Test performance with different components enabled/disabled
5. **Resource Limits**: Monitor resource usage to identify bottlenecks

#### 9.2 For Developers

1. **Minimal Overhead**: Ensure performance monitoring itself doesn't significantly impact simulation performance
2. **Configurable Monitoring**: Make monitoring levels configurable to balance detail and performance
3. **Non-intrusive**: Design monitoring that doesn't interfere with normal operation
4. **Memory Management**: Properly manage memory usage of monitoring data
5. **Thread Safety**: Ensure monitoring code is thread-safe for parallel simulations

#### 9.3 For Users

1. **Start Simple**: Begin with basic monitoring and add detail as needed
2. **Focus on Key Metrics**: Identify the most important metrics for your use case
3. **Long-term Monitoring**: Use monitoring for long-term performance analysis
4. **Comparative Testing**: Compare performance across different configurations
5. **Documentation**: Document your performance findings for future reference

### 10. Future Enhancements

#### 10.1 Machine Learning Integration

- Integrate machine learning models for performance prediction
- Use reinforcement learning to optimize simulation parameters
- Implement automated performance tuning

#### 10.2 Cloud-Based Profiling

- Distributed performance monitoring across multiple machines
- Cloud-based visualization of performance data
- Collaborative performance analysis

#### 10.3 Real-time Visualization

- Real-time dashboards for monitoring simulation performance
- Interactive visualization of performance metrics
- Web-based performance monitoring interfaces

## Conclusion

The NLM performance monitoring framework provides comprehensive profiling capabilities to help users understand, optimize, and analyze neural simulations. By integrating performance monitoring throughout the simulation pipeline, researchers and developers can gain valuable insights into the computational characteristics of their neural models while maintaining flexibility and configurability for different use cases.

The framework is designed to be extensible, allowing for future enhancements while maintaining backward compatibility with existing NLM code. Whether you're developing new neural models, optimizing simulation performance, or conducting research on neural computation, the NLM performance monitoring system provides the tools needed to make informed decisions about your neural simulations.
