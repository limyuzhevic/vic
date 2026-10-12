# NLM Metrics Collection and Analysis Guide

## Overview

This guide provides comprehensive instructions for using the NLM Advanced Research Metrics System. The system provides extensive data collection and analysis capabilities for NLM artificial brain simulations, supporting experimental research, parameter tuning, and statistical analysis.

## System Requirements

### Minimum Requirements
- Operating System: Linux, macOS, or Windows
- C++20 compatible compiler
- Available memory: 4 GB minimum (16 GB recommended for large experiments)
- Disk space: 1 GB minimum for data storage

### Optional Dependencies
- JSON library for data export
- CSV parser for data import
- MATLAB engine for MATLAB export
- Statistical libraries for advanced analysis

## Installation

### Build from Source
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Metrics Examples
```bash
# Run Phase 6 demo with metrics
./nlm_phase6_demo

# Run metrics-specific test
./nlm_metrics_test
```

## Quick Start

### 1. Basic Metrics Collection

```cpp
#include "ResearchMetrics.hpp"

int main() {
    // Initialize metrics system
    ResearchMetrics metrics;
    
    // Record basic metrics
    metrics.record("firing_rate", 12.5f);
    metrics.record("reward", 5.0f);
    metrics.record("learning_progress", 0.75f);
    
    // Record time series data
    metrics.recordTimeSeries("temperature", 25.0f);
    metrics.recordTimeSeries("pressure", 101.3f);
    
    // Calculate statistics
    auto stats = metrics.calculateStatistics("firing_rate");
    
    // Generate report
    std::string report = metrics.generateReport();
    
    return 0;
}
```

### 2. Advanced Metrics Collection

```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Record learning metrics
    metrics.recordLearningProgress("phase1", 0.8f, 0.01f);
    metrics.recordLearningProgress("phase2", 0.9f, 0.005f);
    
    // Record memory metrics
    ResearchMetrics::MemoryMetrics memMetrics;
    memMetrics.workingMemoryCapacity = 100;
    memMetrics.workingMemoryUtilization = 50;
    memMetrics.episodicMemoryCapacity = 1000;
    memMetrics.episodicMemoryStored = 250;
    memMetrics.consolidationEffectiveness = 0.85f;
    metrics.recordMemoryMetrics(memMetrics);
    
    // Record neuromodulation metrics
    ResearchMetrics::NeuromodulationMetrics nmMetrics;
    nmMetrics.dopamineSignalStrength = 0.7f;
    nmMetrics.curiositySignalStrength = 0.5f;
    nmMetrics.noveltyDetectionAccuracy = 0.85f;
    metrics.recordNeuromodulationMetrics(nmMetrics);
    
    // Record cognition metrics
    ResearchMetrics::CognitionMetrics cogMetrics;
    cogMetrics.conceptFormationAccuracy = 0.82f;
    cogMetrics.planningAccuracy = 0.78f;
    cogMetrics.attentionEffectiveness = 0.85f;
    metrics.recordCognitionMetrics(cogMetrics);
    
    // Record development metrics
    ResearchMetrics::DevelopmentMetrics devMetrics;
    devMetrics.developmentalStageProgress = 0.6f;
    devMetrics.plasticityRate = 0.2f;
    devMetrics.maturationProgress = 0.7f;
    metrics.recordDevelopmentMetrics(devMetrics);
    
    return 0;
}
```

### 3. Time Series Analysis

```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Record time series data
    auto currentTime = std::chrono::system_clock::now();
    for (int i = 0; i < 100; ++i) {
        double value = 10.0 + i * 0.1; // Example data
        metrics.recordTimeSeries("temperature", value, currentTime);
        currentTime += std::chrono::seconds(1);
    }
    
    // Analyze time series
    std::vector<std::string> metricNames = metrics.getMetricNames();
    for (const std::string& name : metricNames) {
        double trend = metrics.calculateTrend(name, 10);
        std::cout << "Trend for " << name << ": " << trend << std::endl;
    }
    
    return 0;
}
```

### 4. Pattern Recognition Metrics

```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Record pattern metrics
    ResearchMetrics::PatternMetrics pattern1;
    pattern1.patternName = "spike_pattern_1";
    pattern1.accuracy = 0.85f;
    pattern1.precision = 0.83f;
    pattern1.recall = 0.87f;
    pattern1.f1Score = 0.85f;
    pattern1.confidence = 0.9f;
    pattern1.totalPredictions = 100;
    pattern1.correctPredictions = 85;
    metrics.recordPatternMetrics(pattern1);
    
    // Record prediction quality
    metrics.recordPredictionQuality("spike_prediction", 0.85f, 100);
    
    // Record classification metrics
    std::vector<bool> predictions = {1, 0, 1, 1, 0};
    std::vector<bool> truths = {1, 0, 1, 0, 0};
    metrics.recordClassificationMetrics("spike_class", predictions, truths);
    
    return 0;
}
```

## Metrics Categories

### Performance Metrics
- **Firing Rate**: Average neuron firing rate
- **Simulation Speed**: Time steps per second
- **Memory Usage**: RAM consumption
- **CPU Usage**: Processor utilization
- **Step Count**: Number of simulation steps

### Learning Metrics
- **Learning Progress**: Progress through learning phases
- **Learning Rate**: Rate of learning improvement
- **Retention Factor**: Ability to retain learned information
- **Generalization Score**: Performance on new tasks
- **Epoch Count**: Number of training epochs

### Memory Metrics
- **Working Memory Capacity**: Maximum working memory size
- **Working Memory Utilization**: Current working memory usage
- **Episodic Memory Capacity**: Maximum episodic memory storage
- **Episodic Memory Stored**: Number of stored episodes
- **Consolidation Effectiveness**: Memory consolidation efficiency
- **Replay Events**: Number of memory replay events
- **Memory Decay Rate**: Rate of memory decay

### Neuromodulation Metrics
- **Dopamine Signal Strength**: Dopamine signal amplitude
- **Dopamine Timing Precision**: Accuracy of dopamine signaling timing
- **Curiosity Signal Strength**: Curiosity signal amplitude
- **Novelty Detection Accuracy**: Novelty detection accuracy
- **Prediction Error Magnitude**: Size of prediction errors
- **Attention Focus Score**: Attention focus effectiveness
- **Modulation Effectiveness**: Overall neuromodulation effectiveness

### Cognition Metrics
- **Concept Formation Accuracy**: Accuracy of concept formation
- **Planning Accuracy**: Accuracy of planning
- **Attention Effectiveness**: Attention effectiveness
- **Working Memory Load**: Working memory load
- **Cognitive Flexibility**: Flexibility in cognitive processing
- **Concept Count**: Number of formed concepts
- **Plan Executions**: Number of plan executions
- **Cognitive Reserve**: Cognitive capacity

### Development Metrics
- **Developmental Stage Progress**: Progress through developmental stages
- **Plasticity Rate**: Neural plasticity rate
- **Maturation Progress**: Neural maturation progress
- **Stage Transition Smoothness**: Smoothness of stage transitions
- **Stage Transition Count**: Number of stage transitions
- **Critical Period Effectiveness**: Effectiveness of critical periods
- **Experience Dependent Plasticity**: Plasticity dependent on experience

## Statistical Analysis

### Descriptive Statistics
```cpp
ResearchMetrics::StatisticalSummary stats = metrics.calculateStatistics("firing_rate");
std::cout << "Mean: " << stats.mean << std::endl;
std::cout << "Standard Deviation: " << stats.standardDeviation << std::endl;
std::cout << "Median: " << stats.median << std::endl;
std::cout << "Min: " << stats.min << std::endl;
std::cout << "Max: " << stats.max << std::endl;
```

### Correlation Analysis
```cpp
double correlation = metrics.calculateCorrelation("firing_rate", "learning_progress");
std::cout << "Correlation: " << correlation << std::endl;
```

### Trend Analysis
```cpp
double trend = metrics.calculateTrend("firing_rate", 10);
std::cout << "Trend: " << trend << std::endl;
```

### Correlated Metrics
```cpp
std::vector<std::string> correlated = metrics.findCorrelatedMetrics("firing_rate", 0.8);
for (const std::string& name : correlated) {
    std::cout << "Correlated with firing_rate: " << name << std::endl;
}
```

## Data Export

### CSV Export
```cpp
bool success = metrics.exportToCSV("./results/metrics.csv");
if (success) {
    std::cout << "Metrics exported to CSV" << std::endl;
}
```

### JSON Export
```cpp
bool success = metrics.exportToJSON("./results/metrics.json");
if (success) {
    std::cout << "Metrics exported to JSON" << std::endl;
}
```

### MATLAB Export
```cpp
bool success = metrics.exportToMat("./results/metrics.mat");
if (success) {
    std::cout << "Metrics exported to MATLAB" << std::endl;
}
```

## Integration with Research Tools

### Experimental Conditions
```cpp
#include "ResearchTools.hpp"

int main() {
    ResearchTools tools;
    
    // Create experimental conditions
    ResearchTools::ExperimentalCondition condition1;
    condition1.name = "baseline";
    condition1.description = "Normal operation";
    condition1.parameters = {{/* parameters */}};
    tools.createExperimentalCondition(condition1);
    
    // Create ablation condition
    ResearchTools::ExperimentalCondition condition2;
    condition2.name = "ablated_memory";
    condition2.description = "Working memory disabled";
    condition2.disabledSystems = {"working_memory"};
    tools.createExperimentalCondition(condition2);
    
    // Set baseline
    tools.setBaselineCondition("baseline");
    
    // Run experiments
    tools.runExperimentalCondition("baseline");
    tools.runExperimentalCondition("ablated_memory");
    
    // Compare conditions
    auto comparisons = tools.compareAllConditions();
    
    return 0;
}
```

### Parameter Tuning
```cpp
ResearchTools tools;

// Add parameter tuning
ResearchTools::ParameterTune tune;
tune.parameterName = "learning_rate";
tune.initialValue = 0.01;
tune.minValue = 0.001;
tune.maxValue = 0.1;
tune.stepSize = 0.001;
tune.optimizeForMax = true;
tools.addParameterTune(tune);

// Run parameter tuning
bool success = tools.runParameterTuning("learning_rate");

// Get results
auto results = tools.getParameterTuningResults();
```

### Statistical Testing
```cpp
ResearchTools tools;

// Add statistical test
ResearchTools::StatisticalTestResult test;
test.testName = "baseline_vs_ablated";
test.testType = "independent_t_test";
test.statistic = 2.5;
test.pValue = 0.02;
test.isSignificant = true;
tools.addStatisticalTest(test);

// Run statistical tests
auto statistics = tools.runStatisticalTests();
```

## Data Export/Import

### Export Results
```cpp
bool success = tools.exportResults("./research_results.json");
```

### Import Results
```cpp
bool success = tools.importResults("./research_results.json");
```

### Export Conditions
```cpp
bool success = tools.exportConditions("./experimental_conditions.txt");
```

### Import Conditions
```cpp
bool success = tools.importConditions("./experimental_conditions.txt");
```

## Performance Optimization

### Efficient Data Handling
- **Batch Processing**: Process large datasets in batches
- **Memory Management**: Configure memory limits
- **Streaming**: Stream large datasets to disk
- **Compression**: Compress data for storage

### Real-time Processing
- **Non-blocking Operations**: Use asynchronous processing
- **Sampling**: Reduce sampling rate for long simulations
- **Buffering**: Buffer data for efficient processing
- **Parallel Processing**: Process multiple metrics in parallel

### File I/O Optimization
- **Buffered I/O**: Use buffered file operations
- **Binary Formats**: Use binary formats for numerical data
- **Chunked Processing**: Process data in chunks
- **Caching**: Cache frequently accessed data

## Error Handling

### Common Errors
- **File Not Found**: Check file paths and permissions
- **Invalid Parameters**: Validate parameter values
- **Memory Errors**: Check available memory
- **I/O Errors**: Handle disk full and permission errors

### Error Recovery
- **Graceful Degradation**: Continue with reduced functionality
- **Backup Files**: Keep backup copies of data
- **Retry Logic**: Implement retry for failed operations
- **Logging**: Log all errors for debugging

## Integration with Phase 6

### Complete Integration Example
```cpp
#include "Phase6IntegratedExperiment.hpp"

int main() {
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.neuronCount = 1000;
    config.maxSteps = 10000;
    config.enableVisualization = true;
    
    // Enable research features
    experiment.enableResearchVisualization();
    experiment.enableAdvancedMetrics();
    
    // Get research tools
    ResearchTools* tools = experiment.getResearchTools();
    if (tools) {
        tools->setResearchMode(true);
        tools->enableDebugMode(true);
        
        // Add experimental conditions
        ResearchTools::ExperimentalCondition cond1;
        cond1.name = "control";
        cond1.isBaseline = true;
        tools->createExperimentalCondition(cond1);
        
        ResearchTools::ExperimentalCondition cond2;
        cond2.name = "ablated";
        cond2.disabledSystems = {"working_memory"};
        tools->createExperimentalCondition(cond2);
        
        // Set baseline
        tools->setBaselineCondition("control");
        
        // Run experiments
        tools->runExperimentalCondition("control");
        tools->runExperimentalCondition("ablated");
    }
    
    // Get metrics
    ResearchMetrics* metrics = experiment.getResearchMetrics();
    if (metrics) {
        // Use metrics for analysis
        metrics->record("experiment_completed", 1.0);
        
        // Export metrics
        metrics->exportToCSV("./phase6_metrics.csv");
    }
    
    // Run Phase 6 experiment
    auto result = experiment.run(config);
    
    // Export research data
    experiment.exportResearchData("./phase6_research");
    
    return 0;
}
```

## Performance Monitoring

### System Monitoring
- **Resource Usage**: Monitor CPU, memory, and disk usage
- **Performance Metrics**: Track simulation performance
- **Data Size**: Monitor data storage requirements
- **Processing Time**: Track execution time

### Metrics Monitoring
- **Firing Rate**: Monitor average firing rate
- **Memory Usage**: Monitor memory consumption
- **Learning Progress**: Monitor learning progress
- **System Health**: Monitor overall system health

## Troubleshooting

### Common Issues

#### Metrics Collection Issues
1. **Data Not Recorded**: Check if metrics were properly initialized
2. **Missing Metrics**: Verify all metrics are being recorded
3. **Performance Issues**: Check system resources
4. **Memory Issues**: Check available memory

#### Export Issues
1. **File Permissions**: Check file write permissions
2. **Disk Space**: Check available disk space
3. **File Format**: Verify file format support
4. **Data Size**: Check data size for large datasets

#### Statistical Analysis Issues
1. **Insufficient Data**: Check if enough data was collected
2. **Invalid Parameters**: Validate analysis parameters
3. **Numerical Errors**: Check for division by zero or other numerical errors
4. **Memory Errors**: Check for memory allocation failures

### Error Codes
- **1**: Metrics initialization failed
- **2**: Data recording failed
- **3**: Export failed
- **4**: Statistical analysis failed
- **5**: Integration failed

## Customization

### Extend Metrics System
Create custom metrics by:
1. Implementing custom metric calculation functions
2. Adding custom metric categories
3. Implementing custom export formats

### Modify Configuration
- **Parameters**: Configure experimental parameters
- **Conditions**: Create custom experimental conditions
- **Analysis**: Modify statistical analysis settings
- **Output**: Configure output formats and destinations

### Add Custom Metrics
```cpp
// Add custom metric
ResearchMetrics::CustomMetric custom;
custom.name = "custom_metric";
custom.description = "A custom metric";
custom.category = ResearchMetrics::Category::Performance;
custom.calculator = []() { return 1.0; }; // Custom calculation
metrics.addCustomMetric(custom);
```

## Examples

### Example 1: Learning Experiment
```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Track learning progress
    for (int epoch = 0; epoch < 100; ++epoch) {
        double progress = static_cast<double>(epoch) / 100.0;
        double learningRate = 0.01 / (1.0 + epoch * 0.01);
        
        metrics.recordLearningProgress("training", progress, learningRate);
        
        // Sleep for demonstration
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Generate report
    std::string report = metrics.generateReport();
    std::cout << report << std::endl;
    
    return 0;
}
```

### Example 2: Memory Experiment
```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Track memory metrics
    for (int step = 0; step < 1000; ++step) {
        ResearchMetrics::MemoryMetrics mem;
        mem.workingMemoryCapacity = 100;
        mem.workingMemoryUtilization = 50 + (step % 50);
        mem.episodicMemoryCapacity = 1000;
        mem.episodicMemoryStored = (step / 10) % 100;
        mem.consolidationEffectiveness = 0.5 + (step % 100) * 0.005;
        
        metrics.recordMemoryMetrics(mem);
        
        // Sleep for demonstration
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return 0;
}
```

### Example 3: Neuromodulation Experiment
```cpp
#include "ResearchMetrics.hpp"

int main() {
    ResearchMetrics metrics;
    
    // Track neuromodulation metrics
    for (int step = 0; step < 1000; ++step) {
        ResearchMetrics::NeuromodulationMetrics nm;
        nm.dopamineSignalStrength = 0.5 + std::sin(step * 0.01) * 0.3;
        nm.curiositySignalStrength = 0.3 + std::cos(step * 0.02) * 0.2;
        nm.noveltyDetectionAccuracy = 0.7 + (step % 10) * 0.03;
        nm.predictionErrorMagnitude = 0.2 + (step % 5) * 0.02;
        
        metrics.recordNeuromodulationMetrics(nm);
        
        // Sleep for demonstration
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return 0;
}
```

## Support

### Documentation
- **Technical Documentation**: `src/experiments/RESEARCH_METRICS.md`
- **API Reference**: Header files in `src/experiments/`
- **Integration Guide**: `Phase6IntegratedExperiment.hpp`

### Getting Help
- **Issue Tracker**: Report bugs and issues
- **Community Forum**: Discuss implementation and usage
- **Documentation**: Comprehensive API documentation in header files

## License

MIT License

NLM Research Metrics System - Phase 6 Implementation
