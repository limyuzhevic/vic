# NLM Research Toolkit - Advanced Features

## Overview

The NLM (熙然) Neural Learning Machine has been significantly enhanced with advanced research capabilities. This document describes the new features available for researchers and advanced users.

## New Advanced Features

### 1. Enhanced Experiment Manager

**File**: `src/experiments/AdvancedExperimentManager.hpp/cpp`

**Capabilities**:
- **Batch Experiment Execution**: Run multiple experiments with different configurations in parallel
- **Comparative Analysis**: Compare results across different experimental conditions
- **Parameter Optimization**: Find optimal parameters based on experimental results
- **Multi-format Export**: Export results as JSON, CSV, and Markdown
- **Statistical Analysis**: Calculate comprehensive statistics and perform hypothesis testing
- **Sensitivity Analysis**: Analyze how different parameters affect experimental outcomes

**Key Methods**:
- `runExperimentSet(configs, prefix)`: Run a set of experiments with different configurations
- `compareExperiments(results)`: Generate comparative analysis of experimental results
- `findOptimalParameters(configs, results)`: Identify best performing configurations
- `exportResults(results, directory, formats)`: Export results in multiple formats
- `performStatisticalAnalysis(results)`: Calculate advanced statistics and perform testing

### 2. Research Visualization System

**File**: `src/experiments/ResearchVisualization.hpp/cpp`

**Capabilities**:
- **Comparative Plots**: Generate scatter plots, box plots, and violin plots comparing experiments
- **Learning Curves**: Visualize learning progress over time for different experiments
- **Heat Maps**: Create parameter sensitivity heat maps
- **Network Visualization**: Generate network diagrams for the best performing experiments
- **Statistical Plots**: Create histograms, box plots, and other statistical visualizations
- **Timeline Visualization**: Show experiment execution timeline and results

**Key Methods**:
- `generateComparativePlots(outputPrefix, metrics)`: Create comparative visualization plots
- `generateLearningCurves(outputPrefix)`: Generate learning curve visualizations
- `generateParameterHeatmap(sensitivityData, outputFile)`: Create parameter sensitivity heat maps
- `createNetworkVisualization(resultFile, outputFile)`: Generate network diagrams
- `generateStatisticalPlots(outputPrefix)`: Create statistical distribution plots

### 3. Advanced Metrics Collection

**File**: `src/experiments/AdvancedMetrics.hpp/cpp`

**Capabilities**:
- **Comprehensive Metrics Collection**: Track detailed metrics during experiment execution
- **Time-Series Analysis**: Analyze temporal patterns in experimental data
- **Learning Progress Analysis**: Calculate learning curves and convergence metrics
- **Correlation Analysis**: Compute correlations between different metrics
- **Performance Scoring**: Generate performance scores based on target metrics
- **Best Experiment Identification**: Automatically identify top-performing experiments

**Key Methods**:
- `collectMetrics(experimentName, result)`: Collect metrics for a specific experiment
- `calculateAdvancedStats(experimentName)`: Calculate comprehensive statistics
- `analyzeTimeSeries(experimentName, metric)`: Analyze time-series data
- `calculateLearningProgress(experimentName)`: Calculate learning progress metrics
- `computeMetricCorrelations(experimentName)`: Compute correlations between metrics
- `generatePerformanceScore(experimentName, targetMetrics)`: Generate performance scores

### 4. Parameter Optimization

**File**: `src/experiments/ParameterOptimizer.hpp/cpp`

**Capabilities**:
- **Evolutionary Algorithm**: Implement genetic algorithm for parameter optimization
- **Grid Search**: Perform systematic parameter grid search
- **Random Search**: Use random sampling for optimization
- **Bayesian Optimization**: Implement Bayesian optimization for efficient search
- **Result Persistence**: Save and load optimization results
- **Optimization History**: Track optimization progress and results

**Key Methods**:
- `optimizeParameters(initialParams, fitnessFunction, populationSize, generations)`: Run evolutionary optimization
- `gridSearch(parameterRanges, fitnessFunction)`: Perform systematic grid search
- `randomSearch(parameterRanges, fitnessFunction, iterations)`: Use random sampling
- `bayesianOptimization(parameterRanges, fitnessFunction, iterations)`: Implement Bayesian optimization
- `saveOptimizationResults(filename)`: Save optimization results
- `loadOptimizationResults(filename)`: Load optimization results

### 5. Research Toolkit (Integration)

**File**: `src/experiments/ResearchToolkit.hpp/cpp`

**Capabilities**:
- **Integrated Research Pipeline**: Orchestrate complete research workflows
- **Automated Experiment Management**: Manage multiple experiments automatically
- **Comprehensive Reporting**: Generate detailed research reports with analysis
- **Data Export**: Export research data in multiple formats
- **Configuration Management**: Manage research configuration and settings

**Key Methods**:
- `runResearchPipeline(configs, outputDir)`: Run complete research pipeline
- `generateResearchReport(title)`: Generate comprehensive research report
- `exportAllData(outputDir)`: Export all research data
- `setResearchConfig(config)`: Set research configuration
- `getResearchConfig()`: Get current research configuration

## Getting Started

### Basic Usage

The simplest way to use the advanced research features is through the `ResearchToolkit` class:

```cpp
#include "experiments/ResearchToolkit.hpp"

int main() {
    nlm::ResearchToolkit toolkit;
    
    if (!toolkit.initialize()) {
        std::cerr << "Failed to initialize Research Toolkit" << std::endl;
        return 1;
    }
    
    // Create experiment configurations
    std::vector<Phase6Config> configs = createExperimentConfigurations();
    
    // Run comprehensive research pipeline
    toolkit.runResearchPipeline(configs, "my_research");
    
    return 0;
}
```

### Advanced Usage

For more advanced usage, you can use the individual components directly:

```cpp
#include "experiments/AdvancedExperimentManager.hpp"
#include "experiments/ResearchVisualization.hpp"
#include "experiments/AdvancedMetrics.hpp"
#include "experiments/ParameterOptimizer.hpp"

// 1. Create experiment configurations
std::vector<Phase6Config> configs = createExperimentConfigurations();

// 2. Run experiments
nlm::AdvancedExperimentManager expManager;
std::vector<Phase6IntegrationResult> results = expManager.runExperimentSet(configs, "experiment_1");

// 3. Analyze results
nlm::json comparison = expManager.compareExperiments(results);

// 4. Create visualizations
nlm::ResearchVisualization viz;
viz.initialize(results);
viz.generateComparativePlots("output_prefix");
viz.generateLearningCurves("output_prefix");

// 5. Advanced metrics
nlm::AdvancedMetrics metrics;
for (const auto& result : results) {
    metrics.collectMetrics("exp_1", result);
}

// 6. Parameter optimization
nlm::ParameterOptimizer optimizer;
Phase6Config optimalConfig = optimizer.optimizeParameters(configs, fitnessFunction);

// 7. Generate report
nlm::ResearchToolkit toolkit;
toolkit.initialize();
toolkit.runResearchPipeline(configs, "advanced_research");
```

## Configuration Examples

### Basic Configuration

```cpp
Phase6Config config;
config.neuronCount = 1000;
config.maxSteps = 5000;
config.regionCount = 1;
config.connectionProbability = 0.1f;
config.enableCheckpointing = true;
config.enableReplay = true;
config.enableDevelopment = true;
```

### Experiment Set Configuration

```cpp
std::vector<Phase6Config> createExperimentSet() {
    std::vector<Phase6Config> configs;
    
    // Small network
    Phase6Config smallConfig;
    smallConfig.neuronCount = 500;
    smallConfig.maxSteps = 2000;
    smallConfig.connectionProbability = 0.05f;
    configs.push_back(smallConfig);
    
    // Medium network
    Phase6Config mediumConfig;
    mediumConfig.neuronCount = 1000;
    mediumConfig.maxSteps = 5000;
    mediumConfig.connectionProbability = 0.1f;
    configs.push_back(mediumConfig);
    
    // Large network
    Phase6Config largeConfig;
    largeConfig.neuronCount = 2000;
    largeConfig.maxSteps = 10000;
    largeConfig.connectionProbability = 0.2f;
    configs.push_back(largeConfig);
    
    return configs;
}
```

### Advanced Configuration with Optimization

```cpp
// Create parameter ranges for optimization
std::map<std::string, std::vector<double>> parameterRanges;
parameterRanges["neuronCount"] = {500, 1000, 2000, 5000};
parameterRanges["connectionProbability"] = {0.01, 0.05, 0.1, 0.2};
parameterRanges["maxSteps"] = {1000, 5000, 10000};

// Create grid search
std::vector<Phase6Config> gridConfigs = createParameterGrid(parameterRanges);

// Run optimization
nlm::ParameterOptimizer optimizer;
Phase6Config optimalConfig = optimizer.gridSearch(parameterRanges, fitnessFunction);
```

## Output Formats

### JSON Format

The research toolkit exports results in JSON format for easy parsing and integration:

```json
{
  "experiment_count": 6,
  "timestamp": "20240101_120000",
  "experiments": [
    {
      "experiment_id": 0,
      "total_reward": 85.2,
      "avg_firing_rate": 12.3,
      "memory_episodes_stored": 45,
      "novelty_level": 0.8,
      "curiosity_level": 0.7,
      "dopamine_level": 0.6,
      "learning_success": true
    }
  ]
}
```

### CSV Format

Results are also available in CSV format for spreadsheet analysis:

```csv
experiment_id,total_reward,avg_firing_rate,novelty_level,curiosity_level,dopamine_level,learning_success,wall_clock_time
0,85.2,12.3,0.8,0.7,0.6,true,120.5
1,92.1,15.7,0.6,0.8,0.7,false,180.2
2,78.5,10.2,0.9,0.6,0.5,true,90.1
```

### Markdown Format

Comprehensive research reports are generated in Markdown format:

```markdown
# NLM Research Analysis

## Executive Summary
- Total experiments: 6
- Overall success rate: 66.7%

## Key Findings
- Average total reward: 85.27
- Average firing rate: 12.73 Hz
- Average wall clock time: 130.27 s

## Detailed Experiment Results
| Experiment ID | Total Reward | Firing Rate | Novelty | Curiosity | Dopamine | Success | Time (s) |
|----|--------------|-------------|---------|-----------|----------|---------|----------|
| 0 | 85.2 | 12.3 | 0.8 | 0.7 | 0.6 | ✓ | 120.5 |
| 1 | 92.1 | 15.7 | 0.6 | 0.8 | 0.7 | ✗ | 180.2 |
| 2 | 78.5 | 10.2 | 0.9 | 0.6 | 0.5 | ✓ | 90.1 |
```

## Files Generated

When running the research pipeline, the following files are generated in the output directory:

- `all_results.json`: Complete experimental results in JSON format
- `summary.csv`: Summary statistics in CSV format
- `research_report.md`: Comprehensive research report in Markdown format
- `visualization/`: Directory containing all visualization outputs
  - `comparative_plots.json`: Comparative plot data
  - `learning_curves.json`: Learning curve data
  - `network_best.json`: Network visualization data
- `advanced_stats/`: Directory containing advanced statistical analysis
  - `advanced_stats.json`: Advanced statistical analysis results

## Usage Examples

### Example 1: Basic Research Pipeline

```cpp
#include "examples/research_demo.cpp"

int main() {
    return main_research_demo();
}
```

### Example 2: Custom Experiment Configuration

```cpp
Phase6Config createCustomExperiment() {
    Phase6Config config;
    config.neuronCount = 1500;
    config.maxSteps = 8000;
    config.regionCount = 2;
    config.connectionProbability = 0.12f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    return config;
}

int main() {
    nlm::ResearchToolkit toolkit;
    toolkit.initialize();
    
    std::vector<Phase6Config> configs;
    configs.push_back(createCustomExperiment());
    
    toolkit.runResearchPipeline(configs, "custom_research");
    
    return 0;
}
```

### Example 3: Optimization-Based Research

```cpp
double fitnessFunction(const Phase6Config& config) {
    // Custom fitness function based on research goals
    double baseFitness = config.neuronCount / 1000.0;
    baseFitness *= (1.0 + config.connectionProbability / 0.1f);
    
    if (config.enableDevelopment) baseFitness *= 1.2;
    
    return baseFitness;
}

int main() {
    nlm::ParameterOptimizer optimizer;
    
    // Create parameter ranges
    std::map<std::string, std::vector<double>> ranges;
    ranges["neuronCount"] = {500, 1000, 2000};
    ranges["connectionProbability"] = {0.01, 0.1, 0.2};
    ranges["maxSteps"] = {1000, 5000, 10000};
    
    // Run grid search
    nlm::json results = optimizer.gridSearch(ranges, fitnessFunction);
    
    // Get best configuration
    Phase6Config bestConfig = results["best_config"].get<Phase6Config>();
    
    return 0;
}
```

## Installation and Setup

### Dependencies

The advanced research features require:

1. **C++17 compiler** with modern C++ support
2. **CMake 3.16 or higher**
3. **nlohmann/json library** (included)
4. **pthread library** (for parallel operations)

### Building

Build the research toolkit with:

```bash
mkdir build
cd build
cmake ..
make -j4
```

### Running Examples

Run the research demo:

```bash
./nlm examples/research_demo.cpp
```

Or build the research toolkit as a library and link it with your own applications.

## Advanced Features Summary

| Feature | Description | File | Complexity |
|---------|-------------|------|------------|
| **Advanced Experiment Manager** | Batch experiment execution and comparative analysis | `AdvancedExperimentManager.hpp/cpp` | Medium |
| **Research Visualization** | Comprehensive visualization tools | `ResearchVisualization.hpp/cpp` | High |
| **Advanced Metrics** | Detailed metrics collection and analysis | `AdvancedMetrics.hpp/cpp` | Medium |
| **Parameter Optimization** | Multiple optimization algorithms | `ParameterOptimizer.hpp/cpp` | High |
| **Research Toolkit** | Integrated research pipeline | `ResearchToolkit.hpp/cpp` | High |

## Conclusion

The NLM Research Toolkit provides comprehensive capabilities for conducting advanced neural network research. It enables researchers to:

1. **Run systematic experiments** with different configurations
2. **Analyze results** comprehensively with statistical tools
3. **Visualize findings** with interactive plots and diagrams
4. **Optimize parameters** using advanced algorithms
5. **Generate comprehensive reports** with detailed analysis
6. **Export data** in multiple formats for further analysis

These features make the NLM framework suitable for rigorous scientific research, parameter optimization, and comprehensive analysis of neural network behavior.
