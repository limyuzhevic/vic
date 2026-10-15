# NLM Experiment Framework

## Overview
NLM's experiment framework provides a comprehensive system for designing, running, and analyzing neural network experiments. It supports reproducible research and systematic exploration of parameter spaces.

## Experiment Architecture

### Core Components

#### Experiment Class
The base `Experiment` class defines the interface for all experiments.

#### ExperimentRunner
Manages experiment execution, data collection, and result storage.

#### Metrics Calculator
Calculates performance metrics and statistical measures.

#### Data Logger
Logs experiment data to files for analysis.

### Experiment Types

#### 1. Basic Neural Network Experiments
- **Learning Curves**: Track performance over time
- **Connectivity Analysis**: Study network structure
- **Plasticity Studies**: Investigate learning dynamics

#### 2. Advanced Cognitive Experiments
- **Prediction Tasks**: Test temporal sequence learning
- **Memory Tasks**: Evaluate working and episodic memory
- **Attention Tasks**: Study selective processing
- **Planning Tasks**: Assess multi-step prediction

#### 3. Benchmarking Experiments
- **Performance Benchmarking**: Measure computational efficiency
- **Scalability Tests**: Study performance with increasing scale
- **Resource Usage**: Monitor memory and CPU usage

## Experiment Design Principles

### 1. Reproducibility
- Deterministic random seeds
- Fixed experimental conditions
- Complete data logging
- Version control of experimental parameters

### 2. Modularity
- Separate experiment design from execution
- Configurable experimental parameters
- Pluggable metrics and analysis
- Extendable for new experiment types

### 3. Comprehensiveness
- Capture all relevant experimental data
- Include both primary and secondary metrics
- Record environmental conditions
- Document experimental procedures

## Experiment API

### Core Experiment Interface

#### Experiment Class
```cpp
class Experiment {
public:
    virtual ~Experiment() = default;
    
    // Experiment initialization
    virtual bool initialize(const Config& config) = 0;
    
    // Experiment execution
    virtual void runStep(float dt) = 0;
    
    // Experiment data collection
    virtual ExperimentData collectData() const = 0;
    
    // Experiment analysis
    virtual AnalysisResults analyze(const std::vector<ExperimentData>& data) = 0;
    
    // Experiment reporting
    virtual std::string generateReport(const AnalysisResults& results) = 0;
    
    // Experiment configuration
    virtual void configure(const std::map<std::string, std::string>& parameters) = 0;
};
```

### ExperimentRunner Interface
```cpp
class ExperimentRunner {
public:
    virtual ~ExperimentRunner() = default;
    
    // Run experiment
    virtual bool run(Experiment& experiment, const Config& config) = 0;
    
    // Run experiment with data collection
    virtual bool runWithCollection(Experiment& experiment, 
                                  const Config& config,
                                  DataLogger& logger) = 0;
    
    // Get experiment results
    virtual const std::vector<ExperimentData>& getResults() const = 0;
    
    // Get experiment metrics
    virtual const std::vector<AnalysisResults>& getMetrics() const = 0;
};
```

### Metrics Calculator Interface
```cpp
class MetricsCalculator {
public:
    virtual ~MetricsCalculator() = default;
    
    // Calculate metrics
    virtual AnalysisResults calculate(const std::vector<ExperimentData>& data) = 0;
    
    // Validate metrics
    virtual bool validate(const AnalysisResults& results) = 0;
    
    // Get metric names
    virtual std::vector<std::string> getMetricNames() const = 0;
};
```

### Data Logger Interface
```cpp
class DataLogger {
public:
    virtual ~DataLogger() = default;
    
    // Log data
    virtual bool logData(const ExperimentData& data) = 0;
    
    // Log experiment metadata
    virtual bool logMetadata(const std::map<std::string, std::string>& metadata) = 0;
    
    // Get log file path
    virtual std::string getLogPath() const = 0;
    
    // Close logger
    virtual void close() = 0;
};
```

## Experiment Framework Implementation

### Experiment Implementation Example
```cpp
class NeuralNetworkExperiment : public Experiment {
public:
    bool initialize(const Config& config) override {
        // Initialize brain
        brain = std::make_shared<Brain>(config);
        
        // Initialize metrics calculator
        metricsCalculator = std::make_shared<MetricsCalculatorImpl>();
        
        // Set up data collection
        dataLogger = std::make_shared<DataLoggerImpl>("experiment_data.csv");
        
        return true;
    }
    
    void runStep(float dt) override {
        // Run one simulation step
        brain->step(dt);
        
        // Collect data
        ExperimentData data;
        data.timestamp = brain->getSimulationTime();
        data.neuronActivity = brain->getNeuronActivity();
        data.spikeCount = brain->getSpikeCount();
        data.memoryContent = brain->getMemoryContent();
        
        // Log data
        dataLogger->logData(data);
    }
    
    ExperimentData collectData() const override {
        // Return current experiment data
        ExperimentData data;
        data.timestamp = brain->getSimulationTime();
        data.neuronActivity = brain->getNeuronActivity();
        data.spikeCount = brain->getSpikeCount();
        data.memoryContent = brain->getMemoryContent();
        return data;
    }
    
    AnalysisResults analyze(const std::vector<ExperimentData>& data) override {
        // Analyze collected data
        return metricsCalculator->calculate(data);
    }
    
    std::string generateReport(const AnalysisResults& results) override {
        // Generate experiment report
        std::stringstream report;
        report << "Experiment Results:" << std::endl;
        report << "  Total Steps: " << results.totalSteps << std::endl;
        report << "  Average Firing Rate: " << results.averageFiringRate << std::endl;
        report << "  Total Spike Count: " << results.totalSpikeCount << std::endl;
        report << "  Final Memory Content: " << results.finalMemoryContent << std::endl;
        
        return report.str();
    }
    
    void configure(const std::map<std::string, std::string>& parameters) override {
        // Configure experiment
        for (const auto& param : parameters) {
            experimentParameters[param.first] = param.second;
        }
        
        // Update brain configuration
        brain->updateConfig(experimentParameters);
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<MetricsCalculator> metricsCalculator;
    std::shared_ptr<DataLogger> dataLogger;
    std::map<std::string, std::string> experimentParameters;
};
```

### ExperimentRunner Implementation Example
```cpp
class DefaultExperimentRunner : public ExperimentRunner {
public:
    bool run(Experiment& experiment, const Config& config) override {
        // Initialize experiment
        if (!experiment.initialize(config)) {
            return false;
        }
        
        // Run experiment
        for (int step = 0; step < config.max_steps; ++step) {
            experiment.runStep(config.dt);
        }
        
        // Collect results
        collectResults(experiment);
        
        return true;
    }
    
    bool runWithCollection(Experiment& experiment, const Config& config, DataLogger& logger) override {
        // Initialize experiment
        if (!experiment.initialize(config)) {
            return false;
        }
        
        // Run experiment with data collection
        for (int step = 0; step < config.max_steps; ++step) {
            experiment.runStep(config.dt);
            ExperimentData data = experiment.collectData();
            logger.logData(data);
        }
        
        // Collect results
        collectResults(experiment);
        
        return true;
    }
    
    const std::vector<ExperimentData>& getResults() const override {
        return results;
    }
    
    const std::vector<AnalysisResults>& getMetrics() const override {
        return metrics;
    }
    
private:
    void collectResults(Experiment& experiment) {
        // Run experiment to collect data
        for (int step = 0; step < collect_steps; ++step) {
            experiment.runStep(collect_dt);
            results.push_back(experiment.collectData());
        }
        
        // Analyze results
        metrics.push_back(experiment.analyze(results));
    }
    
    std::vector<ExperimentData> results;
    std::vector<AnalysisResults> metrics;
    const int collect_steps = 1000;
    const float collect_dt = 0.001;
};
```

## Experiment Types

### 1. Basic Neural Network Experiment
```cpp
class BasicNeuralNetworkExperiment : public Experiment {
public:
    std::string getType() const override { return "BasicNeuralNetwork"; }
    
    AnalysisResults run(const Config& config) override {
        // Run basic experiment
        BasicMetrics metrics;
        metrics.totalSteps = config.max_steps;
        metrics.averageFiringRate = calculateAverageFiringRate();
        metrics.totalSpikeCount = calculateTotalSpikeCount();
        
        return metrics;
    }
};
```

### 2. Plasticity Experiment
```cpp
class PlasticityExperiment : public Experiment {
public:
    std::string getType() const override { return "Plasticity"; }
    
    AnalysisResults run(const Config& config) override {
        // Run plasticity experiment
        PlasticityMetrics metrics;
        metrics.stdpLTPWeight = config.std_p_ltp_weight;
        metrics.stdpLTDWeight = config.std_p_ltd_weight;
        metrics.hebbianLearningRate = config.hebbian_learning_rate;
        
        // Run experiment
        runPlasticitySimulation(config);
        
        return metrics;
    }
    
private:
    void runPlasticitySimulation(const Config& config) {
        // Run plasticity simulation
        for (int step = 0; step < config.max_steps; ++step) {
            // Simulate plasticity
            simulatePlasticity(step, config);
        }
    }
};
```

### 3. Memory Experiment
```cpp
class MemoryExperiment : public Experiment {
public:
    std::string getType() const override { return "Memory"; }
    
    AnalysisResults run(const Config& config) override {
        // Run memory experiment
        MemoryMetrics metrics;
        metrics.workingMemoryCapacity = config.working_memory_capacity;
        metrics.episodicMemoryMaxEpisodes = config.episodic_memory_max_episodes;
        
        // Run experiment
        runMemorySimulation(config);
        
        return metrics;
    }
    
private:
    void runMemorySimulation(const Config& config) {
        // Run memory simulation
        for (int step = 0; step < config.max_steps; ++step) {
            // Simulate memory
            simulateMemory(step, config);
        }
    }
};
```

### 4. Prediction Experiment
```cpp
class PredictionExperiment : public Experiment {
public:
    std::string getType() const override { return "Prediction"; }
    
    AnalysisResults run(const Config& config) override {
        // Run prediction experiment
        PredictionMetrics metrics;
        metrics.predictionAccuracy = calculatePredictionAccuracy();
        metrics.predictionLatency = calculatePredictionLatency();
        
        // Run experiment
        runPredictionSimulation(config);
        
        return metrics;
    }
    
private:
    void runPredictionSimulation(const Config& config) {
        // Run prediction simulation
        for (int step = 0; step < config.max_steps; ++step) {
            // Simulate prediction
            simulatePrediction(step, config);
        }
    }
};
```

## Experiment Configuration

### Experiment Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| max_steps | int | 1000 | Maximum number of simulation steps |
| dt | float | 0.001 | Simulation timestep |
| collect_data | bool | true | Whether to collect data |
| log_data | bool | true | Whether to log data |
| output_file | string | experiment_data.csv | Output file for data |

### Experiment Configuration Example
```ini
[experiment]
max_steps = 10000
dt = 0.001
collect_data = true
log_data = true
output_file = experiment_results.csv

[metrics]
collect_neural_activity = true
collect_spike_count = true
collect_memory_content = true
collect_plasticity_weights = true

[logging]
log_level = INFO
log_to_file = true
log_file = experiment.log
```

## Experiment Analysis

### Analysis Metrics

#### Neural Activity Metrics
- **Average Firing Rate**: Average firing rate across all neurons
- **Total Spike Count**: Total number of spikes in the simulation
- **Neural Activity Variance**: Variance of neural activity
- **Neural Activity Skewness**: Skewness of neural activity distribution

#### Memory Metrics
- **Working Memory Utilization**: Percentage of working memory capacity used
- **Episodic Memory Storage**: Number of stored episodes
- **Memory Retrieval Accuracy**: Accuracy of memory retrieval
- **Memory Decay Rate**: Rate of memory decay

#### Plasticity Metrics
- **Synaptic Weight Change**: Total change in synaptic weights
- **STDP LTP/LTD Ratio**: Ratio of LTP to LTD effects
- **Hebbian Learning Rate**: Hebbian learning rate
- **Structural Plasticity**: Rate of structural plasticity

#### Prediction Metrics
- **Prediction Accuracy**: Accuracy of predictions
- **Prediction Latency**: Latency of predictions
- **Prediction Confidence**: Confidence of predictions
- **Prediction Error**: Error of predictions

### Analysis Results
```cpp
struct AnalysisResults {
    // Neural activity metrics
    float averageFiringRate;
    int totalSpikeCount;
    float neuralActivityVariance;
    float neuralActivitySkewness;
    
    // Memory metrics
    float workingMemoryUtilization;
    int episodicMemoryStorage;
    float memoryRetrievalAccuracy;
    float memoryDecayRate;
    
    // Plasticity metrics
    float synapticWeightChange;
    float stdpLTPToLDTRatio;
    float hebbianLearningRate;
    float structuralPlasticity;
    
    // Prediction metrics
    float predictionAccuracy;
    float predictionLatency;
    float predictionConfidence;
    float predictionError;
    
    // Other metrics
    float simulationTime;
    int totalSteps;
    std::string experimentType;
};
```

## Experiment Reporting

### Report Generation
```cpp
class ExperimentReportGenerator {
public:
    std::string generateReport(const Experiment& experiment,
                              const AnalysisResults& results,
                              const Config& config) {
        std::stringstream report;
        
        report << "NLM Experiment Report" << std::endl;
        report << "======================" << std::endl;
        report << std::endl;
        
        report << "Experiment Type: " << experiment.getType() << std::endl;
        report << "Experiment Parameters:" << std::endl;
        report << "  Max Steps: " << config.max_steps << std::endl;
        report << "  Time Step: " << config.dt << std::endl;
        report << "  Data Collection: " << (config.collect_data ? "Yes" : "No") << std::endl;
        report << std::endl;
        
        report << "Results Summary:" << std::endl;
        report << "  Average Firing Rate: " << results.averageFiringRate << std::endl;
        report << "  Total Spike Count: " << results.totalSpikeCount << std::endl;
        report << "  Neural Activity Variance: " << results.neuralActivityVariance << std::endl;
        report << std::endl;
        
        report << "Performance Analysis:" << std::endl;
        report << "  Working Memory Utilization: " << results.workingMemoryUtilization << "%" << std::endl;
        report << "  Episodic Memory Storage: " << results.episodicMemoryStorage << " episodes" << std::endl;
        report << "  Synaptic Weight Change: " << results.synapticWeightChange << std::endl;
        report << std::endl;
        
        report << "Prediction Performance:" << std::endl;
        report << "  Prediction Accuracy: " << results.predictionAccuracy << std::endl;
        report << "  Prediction Latency: " << results.predictionLatency << " ms" << std::endl;
        report << "  Prediction Error: " << results.predictionError << std::endl;
        
        return report.str();
    }
};
```

## Experiment Automation

### Batch Experiment Execution
```cpp
class BatchExperimentRunner {
public:
    void runExperiments(const std::vector<Config>& configs,
                       const std::string& outputDir) {
        for (const auto& config : configs) {
            runSingleExperiment(config, outputDir);
        }
    }
    
private:
    void runSingleExperiment(const Config& config, const std::string& outputDir) {
        // Create experiment
        auto experiment = createExperiment(config.experiment_type);
        
        // Create experiment runner
        auto runner = std::make_shared<DefaultExperimentRunner>();
        
        // Run experiment
        if (runner->run(*experiment, config)) {
            // Save results
            saveResults(*experiment, outputDir, config);
        }
    }
    
    std::unique_ptr<Experiment> createExperiment(const std::string& type) {
        if (type == "basic") {
            return std::make_unique<BasicNeuralNetworkExperiment>();
        } else if (type == "plasticity") {
            return std::make_unique<PlasticityExperiment>();
        } else if (type == "memory") {
            return std::make_unique<MemoryExperiment>();
        } else if (type == "prediction") {
            return std::make_unique<PredictionExperiment>();
        }
        
        throw std::runtime_error("Unknown experiment type: " + type);
    }
};
```

### Hyperparameter Optimization
```cpp
class HyperparameterOptimizer {
public:
    void optimize(const std::vector<Config>& baseConfigs,
                 const std::map<std::string, std::vector<float>>& parameterRanges,
                 const std::string& outputDir) {
        // Run experiments with different parameter combinations
        auto results = runParameterSearch(baseConfigs, parameterRanges);
        
        // Analyze results
        auto bestConfig = analyzeResults(results);
        
        // Save best configuration
        saveConfig(bestConfig, outputDir);
    }
    
private:
    std::vector<ExperimentResult> runParameterSearch(
        const std::vector<Config>& baseConfigs,
        const std::map<std::string, std::vector<float>>& parameterRanges) {
        std::vector<ExperimentResult> results;
        
        // Generate parameter combinations
        auto parameterCombinations = generateParameterCombinations(parameterRanges);
        
        // Run experiments
        for (const auto& combination : parameterCombinations) {
            for (const auto& baseConfig : baseConfigs) {
                auto result = runExperiment(combineConfigs(baseConfig, combination));
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    ExperimentResult runExperiment(const Config& config) {
        // Create and run experiment
        auto experiment = createExperiment(config.experiment_type);
        auto runner = std::make_shared<DefaultExperimentRunner>();
        
        bool success = runner->run(*experiment, config);
        
        ExperimentResult result;
        result.config = config;
        result.success = success;
        if (success) {
            result.data = experiment->collectData();
            result.analysis = experiment->analyze({result.data});
        }
        
        return result;
    }
};
```