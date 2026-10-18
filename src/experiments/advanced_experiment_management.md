# Advanced Experiment Management Documentation

## Overview

This document provides comprehensive guidance for using advanced experiment management features in the enhanced ExperimentRunner system. It covers batch experiments, checkpointing, cloning strategies, and performance optimization.

## Table of Contents

1. [Batch Experiment Management](#batch-experiment-management)
2. [Checkpointing Strategy](#checkpointing-strategy)
3. [Cloning Techniques](#cloning-techniques)
4. [Performance Optimization](#performance-optimization)
5. [Integration with Existing Systems](#integration-with-existing-systems)
6. [Error Handling and Recovery](#error-handling-and-recovery)
7. [Configuration Management](#configuration-management)
8. [Monitoring and Logging](#monitoring-and-logging)
9. [Security Considerations](#security-considerations)
10. [Best Practices](#best-practices)
11. [Troubleshooting](#troubleshooting)
12. [API Reference](#api-reference)

## Batch Experiment Management

### Overview

Batch experiment management enables researchers to run multiple experiments simultaneously with proper isolation and state management. The system supports various execution modes optimized for different workloads.

### Execution Modes

#### Sequential Mode
- **Best for**: Small batches, debugging, sequential dependency
- **Characteristics**: Simple, predictable, lower resource usage
- **Use Case**: Development and testing phases

```cpp
BatchExperimentManager::BatchConfig config;
config.executionMode = BatchExperimentManager::ExecutionMode::SEQUENTIAL;
config.maxConcurrentExperiments = 1;
```

#### Parallel Mode
- **Best for**: Independent experiments, maximum throughput
- **Characteristics**: High performance, complex synchronization
- **Use Case**: Production runs with independent experiments

```cpp
BatchExperimentManager::BatchConfig config;
config.executionMode = BatchExperimentManager::ExecutionMode::PARALLEL;
config.maxConcurrentExperiments = std::thread::hardware_concurrency();
```

#### Pipelined Mode
- **Best for**: Mixed I/O and CPU workloads
- **Characteristics**: Balanced throughput, continuous pipeline
- **Use Case**: Experiments with external dependencies

```cpp
BatchExperimentManager::BatchConfig config;
config.executionMode = BatchExperimentManager::ExecutionMode::PIPELINED;
config.maxConcurrentExperiments = 4;  // Pipeline stages
```

#### Interleaved Mode
- **Best for**: Balanced workloads with moderate parallelism
- **Characteristics**: Good compromise, moderate complexity
- **Use Case**: General purpose batch processing

```cpp
BatchExperimentManager::BatchConfig config;
config.executionMode = BatchExperimentManager::ExecutionMode::INTERLEAVED;
config.maxConcurrentExperiments = 2;
```

### Batch Configuration Parameters

```cpp
struct BatchConfig {
    size_t maxConcurrentExperiments;     // Number of experiments running simultaneously
    SimulationStep maxStepsPerExperiment; // Maximum steps per experiment
    double timeoutSeconds;              // Timeout per experiment
    bool checkpointEnabled;              // Enable checkpoint saving
    std::string checkpointPath;         // Directory for checkpoints
    bool pauseOnError;                   // Pause execution on error
    bool useBrainCloning;                // Use brain cloning for experiments
    bool useEnvironmentCloning;          // Use environment cloning for experiments
    BrainClone::CloneMode brainCloneMode; // Brain cloning mode
    EnvironmentClone::CloneMode envCloneMode; // Environment cloning mode
    bool threadSafeCloning;              // Enable thread-safe cloning
    bool preserveInternalState;          // Preserve internal state during cloning
    std::vector<std::string> excludedExperiments; // Skip these experiments
    std::map<std::string, std::string> experimentParams; // Per-experiment parameters
};
```

### Batch Execution Examples

#### Simple Batch Execution

```cpp
#include "BatchExperimentManager.hpp"

BatchExperimentManager manager;
BatchExperimentManager::BatchConfig config;
config.maxConcurrentExperiments = 4;
config.maxStepsPerExperiment = 1000;
config.timeoutSeconds = 300.0;

manager.configure(config);

std::vector<std::shared_ptr<Experiment>> experiments;
for (int i = 0; i < 10; ++i) {
    auto exp = std::make_shared<Experiment>("exp_" + std::to_string(i));
    exp->recordMetric("base_score", 50.0 + i * 5);
    experiments.push_back(exp);
}

std::vector<ExperimentResult> results = manager.executeBatch(
    experiments,
    brainTemplate,
    environmentTemplate
);
```

#### Batch Execution with Progress Tracking

```cpp
void progressCallback(size_t completed, size_t total, const std::string& currentExp) {
    double progress = static_cast<double>(completed) / total * 100.0;
    std::cout << "Progress: " << progress << "% - Current: " << currentExp << std::endl;
}

manager.setProgressCallback(progressCallback);

auto results = manager.executeBatch(
    experiments,
    brainTemplate,
    environmentTemplate,
    progressCallback
);
```

#### Batch Execution with Error Handling

```cpp
try {
    auto results = manager.executeBatch(experiments, brainTemplate, environmentTemplate);
    
    // Check for failures
    auto summary = manager.getResultSummary();
    if (summary.failedExperiments > 0) {
        std::cout << "Some experiments failed: " << summary.failedExperiments << std::endl;
        // Handle failures, maybe retry or analyze results
    }
    
} catch (const std::exception& e) {
    std::cout << "Batch execution failed: " << e.what() << std::endl;
    if (config.pauseOnError) {
        manager.pause();
        // Investigate and resume
    }
}
```

## Checkpointing Strategy

### Overview

Checkpointing enables recovery from failures and persistence of experiment state. This section covers checkpointing strategies, file formats, and best practices.

### Checkpoint Types

#### Experiment Checkpoints
- **Purpose**: Save experiment state (metrics, step data, configuration)
- **Use Case**: Quick recovery, experiment reproducibility
- **Frequency**: End of experiment or periodic saves

```cpp
// Save experiment checkpoint
bool saved = runner.saveExperimentCheckpoint(
    experiment,
    "/checkpoints/experiment_123.checkpoint"
);

// Load experiment checkpoint
auto loaded = ExperimentRunner::loadExperimentCheckpoint(
    "/checkpoints/experiment_123.checkpoint"
);
```

#### Brain Checkpoints
- **Purpose**: Save brain neural network state
- **Use Case**: Reuse brain configurations across experiments
- **Frequency**: Start of new experiment runs

```cpp
// Save brain checkpoint
bool saved = runner.saveBrainCheckpoint(
    brainTemplate,
    "/checkpoints/brain_template.checkpoint"
);

// Load brain checkpoint
auto loadedBrain = ExperimentRunner::loadBrainCheckpoint(
    "/checkpoints/brain_template.checkpoint"
);
```

#### Environment Checkpoints
- **Purpose**: Save environment configuration and state
- **Use Case**: Reproduce environment conditions
- **Frequency**: Before batch execution

```cpp
// Save environment checkpoint
bool saved = runner.saveEnvironmentCheckpoint(
    environmentTemplate,
    "/checkpoints/environment_template.checkpoint"
);

// Load environment checkpoint
auto loadedEnv = ExperimentRunner::loadEnvironmentCheckpoint(
    "/checkpoints/environment_template.checkpoint"
);
```

### Checkpoint Format

Checkpoint files use a simple text-based format with sections:

```
[SECTION: Metadata]
name: experiment_123
seed: 42
start_step: 0
end_step: 1000

[SECTION: Metrics]
total_reward: 150.5
accuracy: 0.85
epoch_count: 50

[SECTION: StepData]
Step 0: neurons=100, synapses=500, firing=10, firingRate=0.1
Step 1: neurons=102, synapses=520, firing=12, firingRate=0.12
...

[SECTION: Config]
mode: reinforcement_learning
learning_rate: 0.001
batch_size: 32
```

### Checkpoint Management

#### Automatic Checkpointing

```cpp
BatchExperimentManager::BatchConfig config;
config.checkpointEnabled = true;
config.checkpointPath = "/data/experiment_checkpoints/";
config.checkpointInterval = 100; // Save every 100 steps

manager.configure(config);
```

#### Manual Checkpointing

```cpp
// During batch execution
if (manager.shouldCheckpoint()) {
    manager.saveCheckpoint();
}

// Resume from checkpoint
if (manager.checkpointExists()) {
    manager.loadCheckpoint();
}
```

### Checkpoint Recovery

#### Recovery Process

1. **Check for Available Checkpoints**
2. **Load Latest Checkpoint**
3. **Resume Execution**
4. **Continue with Remaining Experiments**

```cpp
// Recovery example
BatchExperimentManager manager;
BatchExperimentManager::BatchConfig config;
config.checkpointEnabled = true;

manager.configure(config);

if (manager.checkpointExists()) {
    std::cout << "Found checkpoint, loading..." << std::endl;
    bool loaded = manager.loadCheckpoint();
    if (loaded) {
        std::cout << "Successfully loaded checkpoint" << std::endl;
    }
}

// Continue execution
auto results = manager.executeBatch(experiments, brainTemplate, environmentTemplate);
```

## Cloning Techniques

### Overview

Cloning enables independent experiment execution with preserved state. This section covers cloning strategies, performance considerations, and best practices.

### Brain Cloning Strategies

#### Deep Cloning
- **Characteristics**: Complete state copy, maximum isolation
- **Memory Usage**: High
- **Performance**: Slower
- **Use Case**: Experiments requiring complete independence

```cpp
auto clonedBrain = BrainClone::deepClone(
    originalBrain,
    true,  // copyMemorySystems
    true,  // copyPlasticity
    true   // copyDevelopmentState
);
```

#### Shallow Cloning
- **Characteristics**: Partial state copy, shared structures
- **Memory Usage**: Low
- **Performance**: Faster
- **Use Case**: Quick experimentation, development

```cpp
auto clonedBrain = BrainClone::shallowClone(originalBrain);
```

#### Thread-Safe Cloning
- **Characteristics**: Thread-safe deep cloning with locking
- **Memory Usage**: High
- **Performance**: Medium
- **Use Case**: Multi-threaded batch execution

```cpp
std::mutex lock;
auto clonedBrain = BrainClone::cloneThreadSafe(
    originalBrain,
    templateConfig,
    &lock
);
```

### Environment Cloning Strategies

#### State Preservation Cloning
- **Characteristics**: Preserve environment state, recreate state
- **Memory Usage**: Medium
- **Performance**: Fast
- **Use Case**: Experiments with complex initial states

```cpp
auto clonedEnv = EnvironmentClone::clonePreserveState(
    originalEnv,
    preservedState
);
```

#### Parameter-Based Cloning
- **Characteristics**: Clone with specific parameter overrides
- **Memory Usage**: Low
- **Performance**: Very fast
- **Use Case**: Quick environment modifications

```cpp
auto clonedEnv = EnvironmentClone::cloneWithParams(
    originalEnv,
    "width", "20",
    "height", "30"
);
```

### Cloning Configuration Templates

#### Template-Based Cloning

```cpp
// Configure cloning templates
ExperimentRunner runner;

// Brain template
auto brainTemplate = std::make_shared<Brain>(std::make_shared<Config>());
brainTemplate->initialize();
runner.setBrainTemplate("default", brainTemplate);

// Environment template
auto envTemplate = std::make_shared<GridWorldEnvironment>();
runner.setEnvironmentTemplate("default", envTemplate);

// Clone using templates
auto clonedBrain = runner.cloneBrain(
    brainTemplate,
    "default",
    BrainClone::CloneMode::DEEP
);
```

#### Configuration Templates for Different Experiment Types

```cpp
// Reinforcement Learning
struct RLTrainConfig {
    float learningRate;
    size_t bufferSize;
    bool useTargetNetwork;
};

// Neurogenesis
struct NeurogenesisConfig {
    size_t newNeuronsPerStep;
    float plasticityRate;
    bool allowPruning;
};

// Cognitive Architecture
struct CognitiveConfig {
    size_t workingMemorySize;
    size_t episodicMemorySize;
    float attentionThreshold;
};
```

### Clone Performance Optimization

#### Performance Metrics

```cpp
// Get cloning performance metrics
BrainClone::CloneStats brainStats = BrainClone::getClonePerformanceMetrics();
EnvironmentClone::CloneStats envStats = EnvironmentClone::getClonePerformanceMetrics();

std::cout << "Brain cloning: " << brainStats.neuronsCloned << " neurons, "
          << brainStats.cloningTimeMs << "ms" << std::endl;

std::cout << "Environment cloning: " << envStats.stateSize << " bytes, "
          << envStats.cloningTimeMs << "ms" << std::endl;
```

#### Batch Cloning Performance

```cpp
// Batch clone with thread count optimization
std::vector<std::shared_ptr<Brain>> brains;
for (int i = 0; i < 100; ++i) {
    brains.push_back(std::make_shared<Brain>(config));
}

// Optimize thread count based on workload
size_t optimalThreads = std::thread::hardware_concurrency();
if (optimalThreads > brains.size()) {
    optimalThreads = brains.size();
}

std::vector<std::shared_ptr<Brain>> clones = BrainClone::batchClone(
    brains,
    config,
    BrainClone::CloneMode::DEEP,
    optimalThreads
);
```

## Integration with Existing Systems

### Integration with Legacy Code

#### Migration Path

1. **Keep Existing Code**: Current `ExperimentRunner` API unchanged
2. **Gradual Adoption**: Add new functionality incrementally
3. **Configuration**: Support both old and new configuration formats
4. **Testing**: Maintain backward compatibility tests

```cpp
// Legacy code continues to work
ExperimentRunner legacyRunner;
auto exp = legacyRunner.createExperiment("legacy_exp");
bool success = legacyRunner.runExperiment(exp, brain, env, 1000);

// New code can use enhanced features
ExperimentRunner enhancedRunner;
// ... use enhanced features
```

#### Compatibility Layer

```cpp
class CompatibilityLayer {
public:
    // Legacy API wrapper
    static bool runLegacyExperiment(ExperimentRunner& runner,
                                     std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
        return runner.runExperiment(experiment, brain, environment, maxSteps);
    }
    
    // Enhanced API with compatibility
    static bool runEnhancedExperiment(ExperimentRunner& runner,
                                       std::shared_ptr<Experiment> experiment,
                                       std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Environment> environment,
                                       SimulationStep maxSteps,
                                       bool useCloning = false) {
        if (useCloning) {
            auto clonedBrain = runner.cloneBrain(brain, "default", BrainClone::CloneMode::DEEP);
            auto clonedEnv = runner.cloneEnvironment(environment, EnvironmentClone::CloneMode::DEEP);
            return runner.runExperiment(experiment, clonedBrain, clonedEnv, maxSteps);
        }
        return runner.runExperiment(experiment, brain, environment, maxSteps);
    }
};
```

### Integration with External Systems

#### Configuration Integration

```cpp
// Load configuration from external sources
class ConfigLoader {
public:
    static BatchExperimentManager::BatchConfig loadFromFile(const std::string& filepath) {
        // Parse YAML or JSON configuration
        // Return configured BatchConfig
    }
    
    static ExperimentRunner::ExperimentRunConfig loadRunConfig(const std::string& name) {
        // Load complete experiment run configuration
    }
};
```

#### Result Integration

```cpp
// Export results to external systems
class ResultExporter {
public:
    static bool exportToJSON(const std::vector<ExperimentResult>& results,
                             const std::string& filepath) {
        // Convert results to JSON format
    }
    
    static bool exportToDatabase(const std::vector<ExperimentResult>& results,
                                 const std::string& connectionString) {
        // Export to database
    }
    
    static bool exportToFile(const std::vector<ExperimentResult>& results,
                             const std::string& filepath) {
        // Export to text or binary format
    }
};
```

### Integration with Development Tools

#### Debugging Support

```cpp
// Add debugging support to ExperimentRunner
class DebuggingExperimentRunner : public ExperimentRunner {
public:
    void setDebugMode(bool enabled) {
        debugMode_ = enabled;
    }
    
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps) override {
        if (debugMode_) {
            std::cout << "Debug: Running experiment " << experiment->getName() << std::endl;
            std::cout << "Debug: Brain has " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        }
        return ExperimentRunner::runExperiment(experiment, brain, environment, maxSteps);
    }
    
private:
    bool debugMode_ = false;
};
```

## Error Handling and Recovery

### Error Classification

#### Error Types

1. **Configuration Errors**: Invalid parameters, missing dependencies
2. **Runtime Errors**: Experiment failures, cloning failures
3. **Resource Errors**: Memory limits, file system issues
4. **Synchronization Errors**: Threading issues, race conditions

#### Error Handling Strategies

```cpp
class ErrorHandler {
public:
    enum class ErrorType {
        CONFIGURATION,
        RUNTIME,
        RESOURCE,
        SYNCHRONIZATION
    };
    
    struct ErrorInfo {
        ErrorType type;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::string experimentName;
        bool recoverable;
    };
    
    static void handleError(const ErrorInfo& error) {
        // Log error
        // Take appropriate action based on error type
        switch (error.type) {
            case ErrorType::CONFIGURATION:
                handleConfigurationError(error);
                break;
            case ErrorType::RUNTIME:
                handleRuntimeError(error);
                break;
            case ErrorType::RESOURCE:
                handleResourceError(error);
                break;
            case ErrorType::SYNCHRONIZATION:
                handleSynchronizationError(error);
                break;
        }
    }
    
private:
    static void handleConfigurationError(const ErrorInfo& error) {
        // Validate configuration, provide suggestions
    }
    
    static void handleRuntimeError(const ErrorInfo& error) {
        // Try alternative execution, collect error details
    }
    
    static void handleResourceError(const ErrorInfo& error) {
        // Free resources, reduce load, wait for resources
    }
    
    static void handleSynchronizationError(const ErrorInfo& error) {
        // Retry with exponential backoff, reduce concurrency
    }
};
```

### Error Recovery Mechanisms

#### Checkpoint Recovery

```cpp
// Automatic error recovery with checkpointing
class AutoRecovery {
public:
    bool recoverFromError(const std::string& errorMessage) {
        // Analyze error
        // Determine if recovery is possible
        // Load last successful checkpoint
        // Resume execution
        
        if (shouldCheckpoint()) {
            saveCheckpoint();
        }
        
        return resumeExecution();
    }
    
private:
    bool shouldCheckpoint() {
        // Check if enough time has passed
        // Check if significant progress has been made
        return true;
    }
    
    bool saveCheckpoint() {
        // Save current state
        return checkpointManager_.saveCheckpoint();
    }
    
    bool resumeExecution() {
        // Continue with remaining experiments
        return executeRemainingExperiments();
    }
};
```

#### Fallback Strategies

```cpp
class FallbackManager {
public:
    enum class FallbackStrategy {
        REDUCE_CONCURRENCY,
        USE_SEQUENTIAL,
        SKIP_EXPERIMENT,
        RETRY_WITH_DELAY,
        USE_ALTERNATIVE_ALGORITHM
    };
    
    std::vector<ExperimentResult> executeWithFallback(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        const BatchExperimentManager::BatchConfig& config,
        const std::string& errorMessage) {
        
        FallbackStrategy strategy = determineFallbackStrategy(errorMessage);
        
        switch (strategy) {
            case FallbackStrategy::REDUCE_CONCURRENCY:
                return executeWithReducedConcurrency(experiments, config);
            case FallbackStrategy::USE_SEQUENTIAL:
                return executeSequentially(experiments, config);
            case FallbackStrategy::SKIP_EXPERIMENT:
                return executeSkippingFailed(experiments, config);
            case FallbackStrategy::RETRY_WITH_DELAY:
                return executeWithRetry(experiments, config);
            case FallbackStrategy::USE_ALTERNATIVE_ALGORITHM:
                return executeWithAlternative(experiments, config);
        }
        
        return {};
    }
    
private:
    FallbackStrategy determineFallbackStrategy(const std::string& error) {
        // Analyze error and determine best fallback strategy
        if (error.find("memory") != std::string::npos) {
            return FallbackStrategy::REDUCE_CONCURRENCY;
        } else if (error.find("timeout") != std::string::npos) {
            return FallbackStrategy::USE_SEQUENTIAL;
        } else {
            return FallbackStrategy::RETRY_WITH_DELAY;
        }
    }
};
```

## Configuration Management

### Configuration Management Overview

Configuration management enables consistent experiment configuration across multiple runs. This section covers configuration management best practices and APIs.

### Configuration Classes

#### ExperimentRunConfig

```cpp
struct ExperimentRunConfig {
    std::string runName;
    CloningConfig cloningConfig;
    BatchConfig batchConfig;
    std::vector<std::string> experimentNames;
    std::map<std::string, std::string> experimentParameters;
    
    ExperimentRunConfig() : runName("default_run") {}
};
```

#### CloningConfig

```cpp
struct CloningConfig {
    bool enableBrainCloning;
    bool enableEnvironmentCloning;
    BrainClone::CloneMode brainCloneMode;
    EnvironmentClone::CloneMode environmentCloneMode;
    bool preserveInternalState;
    bool threadSafeCloning;
    
    CloningConfig() : enableBrainCloning(true),
                     enableEnvironmentCloning(true),
                     brainCloneMode(BrainClone::CloneMode::DEEP),
                     environmentCloneMode(EnvironmentClone::CloneMode::DEEP),
                     preserveInternalState(false),
                     threadSafeCloning(true) {}
};
```

#### BatchConfig

```cpp
struct BatchConfig {
    size_t maxConcurrentExperiments;
    SimulationStep maxStepsPerExperiment;
    double timeoutSeconds;
    bool checkpointEnabled;
    std::string checkpointPath;
    bool pauseOnError;
    
    BatchConfig() : maxConcurrentExperiments(4),
                   maxStepsPerExperiment(1000),
                   timeoutSeconds(300.0),
                   checkpointEnabled(false),
                   pauseOnError(false) {}
};
```

### Configuration Management APIs

#### Configuration Loading

```cpp
class ConfigurationManager {
public:
    // Load configuration from file
    static ExperimentRunConfig loadFromFile(const std::string& filepath);
    
    // Load configuration from JSON
    static ExperimentRunConfig loadFromJSON(const std::string& jsonStr);
    
    // Load configuration from environment
    static ExperimentRunConfig loadFromEnvironment(const std::string& prefix);
    
    // Merge configurations
    static ExperimentRunConfig mergeConfigs(const ExperimentRunConfig& base,
                                           const ExperimentRunConfig& override);
    
    // Validate configuration
    static bool validateConfig(const ExperimentRunConfig& config);
    
    // Get default configuration
    static ExperimentRunConfig getDefaultConfig(const std::string& experimentType);
};
```

#### Configuration Validation

```cpp
class ConfigurationValidator {
public:
    static bool validateCloningConfig(const CloningConfig& config) {
        if (config.enableBrainCloning && config.brainCloneMode == BrainClone::CloneMode::DEEP) {
            if (config.preserveInternalState && !config.threadSafeCloning) {
                return false; // Thread safety required for internal state preservation
            }
        }
        return true;
    }
    
    static bool validateBatchConfig(const BatchConfig& config) {
        if (config.maxConcurrentExperiments == 0) return false;
        if (config.maxStepsPerExperiment == 0) return false;
        if (config.timeoutSeconds <= 0.0) return false;
        return true;
    }
    
    static bool validateExperimentRunConfig(const ExperimentRunConfig& config) {
        if (config.experimentNames.empty()) return false;
        if (!validateBatchConfig(config.batchConfig)) return false;
        if (!validateCloningConfig(config.cloningConfig)) return false;
        return true;
    }
};
```

### Configuration Examples

#### Configuration File Example

```yaml
# experiment_config.yaml
run_name: "large_scale_experiment"

cloning_config:
  enable_brain_cloning: true
  enable_environment_cloning: true
  brain_clone_mode: "deep"
  environment_clone_mode: "deep"
  preserve_internal_state: true
  thread_safe_cloning: true

batch_config:
  max_concurrent_experiments: 8
  max_steps_per_experiment: 5000
  timeout_seconds: 600.0
  checkpoint_enabled: true
  checkpoint_path: "/data/experiment_checkpoints/"
  pause_on_error: true

experiment_names:
  - "exp_reinforcement_learning"
  - "exp_neurogenesis"
  - "exp_cognitive_architecture"
  - "exp_deep_learning"
  - "exp_evolutionary_algorithms"

experiment_parameters:
  exp_reinforcement_learning:
    algorithm: "ddpg"
    environment: "mujoco"
  exp_neurogenesis:
    growth_rate: "high"
    pruning_enabled: true
```

#### Programmatic Configuration

```cpp
ExperimentRunConfig config;
config.runName = "my_experiment_run";

config.cloningConfig.enableBrainCloning = true;
config.cloningConfig.brainCloneMode = BrainClone::CloneMode::DEEP;
config.cloningConfig.enableEnvironmentCloning = true;
config.cloningConfig.environmentCloneMode = EnvironmentClone::CloneMode::DEEP;

config.batchConfig.maxConcurrentExperiments = 4;
config.batchConfig.maxStepsPerExperiment = 1000;
config.batchConfig.timeoutSeconds = 300.0;
config.batchConfig.checkpointEnabled = true;
config.batchConfig.checkpointPath = "/tmp/checkpoints/";

config.experimentNames = {"exp1", "exp2", "exp3"};
config.experimentParameters["exp1"] = "param1=value1";
config.experimentParameters["exp2"] = "param1=value2";
```

## Monitoring and Logging

### Monitoring Overview

Monitoring provides visibility into experiment execution, performance metrics, and system health. This section covers monitoring APIs and best practices.

### Monitoring APIs

#### Basic Monitoring

```cpp
class Monitor {
public:
    // Record experiment start
    static void recordExperimentStart(const std::string& experimentName);
    
    // Record experiment completion
    static void recordExperimentComplete(const std::string& experimentName,
                                         const ExperimentResult& result);
    
    // Record experiment failure
    static void recordExperimentFailure(const std::string& experimentName,
                                        const std::string& errorMessage);
    
    // Record performance metrics
    static void recordPerformanceMetrics(const std::string& metricName,
                                         double value,
                                         const std::string& experimentName = "");
    
    // Get current statistics
    static std::map<std::string, double> getStatistics();
    
    // Export statistics
    static bool exportStatistics(const std::string& filepath);
    
    // Clear statistics
    static void clearStatistics();
};
```

#### Performance Monitoring

```cpp
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double throughputExperimentsPerSecond;
        double avgExperimentTimeMs;
        double cpuUtilization;
        double memoryUsageMB;
        std::vector<double> experimentTimes;
        std::vector<std::string> failedExperiments;
        std::vector<std::string> completedExperiments;
    };
    
    static PerformanceMetrics getPerformanceMetrics() {
        PerformanceMonitor monitor;
        return monitor.calculateMetrics();
    }
    
    static double calculateExperimentThroughput(size_t completed, 
                                                std::chrono::system_clock::time_point startTime) {
        auto now = std::chrono::system_clock::now();
        double elapsedSeconds = std::chrono::duration<double>(now - startTime).count();
        if (elapsedSeconds > 0.0) {
            return completed / elapsedSeconds;
        }
        return 0.0;
    }
    
private:
    PerformanceMetrics calculateMetrics();
};
```

#### Logging APIs

```cpp
class Logger {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
        std::string message;
        std::string experimentName;
        std::string threadId;
    };
    
    // Log message
    static void log(LogLevel level, const std::string& message);
    static void log(LogLevel level, const std::string& message,
                   const std::string& experimentName);
    
    // Get log entries
    static std::vector<LogEntry> getLogEntries(LogLevel minLevel = LogLevel::DEBUG,
                                              const std::string& experimentName = "",
                                              const std::chrono::system_clock::time_point& since = {});
    
    // Export logs
    static bool exportLogs(const std::string& filepath);
    
    // Clear logs
    static void clearLogs();
    
    // Set log level
    static void setLogLevel(LogLevel level);
    
    // Enable/disable logging
    static void setLoggingEnabled(bool enabled);
    
    static bool isLoggingEnabled();
};
```

### Monitoring Examples

#### Basic Monitoring Usage

```cpp
#include "Monitor.hpp"

// Record experiment start
Monitor::recordExperimentStart("exp_reinforcement_learning");

// Record experiment completion
ExperimentResult result;
result.setExperimentName("exp_reinforcement_learning");
result.setMetric("total_reward", 150.5);
Monitor::recordExperimentComplete("exp_reinforcement_learning", result);

// Record performance metrics
Monitor::recordPerformanceMetrics("experiment_duration", 1000.0, "exp_reinforcement_learning");
Monitor::recordPerformanceMetrics("memory_usage", 512.0);
```

#### Performance Monitoring Usage

```cpp
PerformanceMonitor::PerformanceMetrics metrics = PerformanceMonitor::getPerformanceMetrics();

std::cout << "Performance Metrics:" << std::endl;
std::cout << "  Throughput: " << metrics.throughputExperimentsPerSecond << " exp/s" << std::endl;
std::cout << "  Average Duration: " << metrics.avgExperimentTimeMs << " ms" << std::endl;
std::cout << "  CPU Utilization: " << metrics.cpuUtilization << "%" << std::endl;
std::cout << "  Memory Usage: " << metrics.memoryUsageMB << " MB" << std::endl;
std::cout << "  Completed Experiments: " << metrics.completedExperiments.size() << std::endl;
std::cout << "  Failed Experiments: " << metrics.failedExperiments.size() << std::endl;
```

#### Logging Usage

```cpp
Logger::setLogLevel(Logger::LogLevel::INFO);

// Log experiment start
Logger::log(Logger::LogLevel::INFO, "Starting experiment", "exp_reinforcement_learning");

// Log experiment completion
Logger::log(Logger::LogLevel::INFO, "Experiment completed successfully", "exp_reinforcement_learning");

// Log experiment failure
Logger::log(Logger::LogLevel::ERROR, "Experiment failed: Out of memory", "exp_neurogenesis");

// Get recent logs
auto recentLogs = Logger::getLogEntries(Logger::LogLevel::WARNING, "exp_reinforcement_learning");
for (const auto& entry : recentLogs) {
    std::cout << "[" << entry.timestamp << "] " << entry.message << std::endl;
}
```

### Monitoring Best Practices

#### Logging Best Practices

1. **Log Level Management**: Use appropriate log levels for different message types
2. **Log Consistency**: Maintain consistent log format
3. **Log Filtering**: Filter logs based on experiment names or time ranges
4. **Log Rotation**: Implement log rotation for long-running sessions
5. **Performance Logging**: Log performance metrics at regular intervals

#### Performance Monitoring Best Practices

1. **Regular Metrics Collection**: Collect performance metrics at regular intervals
2. **Metric Aggregation**: Aggregate metrics over time periods
3. **Trend Analysis**: Analyze performance trends over multiple runs
4. **Benchmarking**: Establish benchmarks for performance comparison
5. **Alerting**: Set up alerts for performance degradation

## Security Considerations

### Security Overview

Security considerations ensure the safe operation of the enhanced ExperimentRunner system. This section covers security best practices, threat models, and mitigation strategies.

### Security Threats

#### Authentication and Authorization

**Threat**: Unauthorized access to experiment configurations and results

**Mitigation**:
1. Use authentication mechanisms for accessing experiment runners
2. Implement role-based access control for different experiment types
3. Encrypt sensitive experiment configurations and results
4. Use secure file permissions for checkpoint files

```cpp
class SecurityManager {
public:
    // Authenticate user
    static bool authenticate(const std::string& username, const std::string& password);
    
    // Check user permissions
    static bool hasPermission(const std::string& username, 
                              const std::string& resource, 
                              const std::string& action);
    
    // Encrypt sensitive data
    static std::string encryptSensitiveData(const std::string& data);
    
    // Decrypt sensitive data
    static std::string decryptSensitiveData(const std::string& encryptedData);
};
```

#### Data Integrity

**Threat**: Unauthorized modification of experiment data or checkpoints

**Mitigation**:
1. Use checksums and hash verification for critical files
2. Implement access controls for experiment files
3. Use digital signatures for experiment configurations
4. Regular integrity checks of checkpoint files

```cpp
class DataIntegrityManager {
public:
    // Calculate file checksum
    static std::string calculateChecksum(const std::string& filepath);
    
    // Verify file integrity
    static bool verifyFileIntegrity(const std::string& filepath,
                                    const std::string& expectedChecksum);
    
    // Sign experiment configuration
    static std::string signExperimentConfig(const ExperimentRunConfig& config);
    
    // Verify experiment configuration signature
    static bool verifyExperimentConfigSignature(const ExperimentRunConfig& config,
                                               const std::string& signature);
};
```

#### Network Security

**Threat**: Network-based attacks on experiment runners

**Mitigation**:
1. Use secure network protocols (TLS/SSL)
2. Implement firewall rules for experiment runner access
3. Use VPN for remote access
4. Monitor network traffic for suspicious activity

```cpp
class NetworkSecurityManager {
public:
    // Start secure server
    static bool startSecureServer(const std::string& host, int port);
    
    // Stop secure server
    static void stopSecureServer();
    
    // Check network security
    static std::map<std::string, bool> checkNetworkSecurity();
    
    // Monitor network traffic
    static std::vector<std::string> monitorNetworkTraffic();
};
```

### Security Best Practices

#### Authentication Best Practices

1. **Multi-Factor Authentication**: Use multi-factor authentication for critical experiments
2. **Session Management**: Implement secure session management
3. **Password Policy**: Enforce strong password policies
4. **Account Lockout**: Implement account lockout for failed login attempts

#### Authorization Best Practices

1. **Least Privilege**: Grant minimum required permissions
2. **Role-Based Access Control**: Implement RBAC for different experiment types
3. **Resource Limits**: Implement resource usage limits
4. **Audit Logging**: Log all access attempts

```cpp
// Role-based access control example
class AccessControl {
public:
    enum class Role {
        RESEARCHER,
        ANALYST,
        ADMIN,
        SUPER_ADMIN
    };
    
    static bool checkAccess(Role role, const std::string& resource, 
                           const std::string& action) {
        switch (role) {
            case Role::RESEARCHER:
                return canResearcherAccess(resource, action);
            case Role::ANALYST:
                return canAnalystAccess(resource, action);
            case Role::ADMIN:
                return canAdminAccess(resource, action);
            case Role::SUPER_ADMIN:
                return true; // Super admin has full access
        }
        return false;
    }
    
private:
    static bool canResearcherAccess(const std::string& resource, const std::string& action) {
        return resource == "experiment" && action == "run";
    }
    
    static bool canAnalystAccess(const std::string& resource, const std::string& action) {
        return (resource == "experiment" && action == "read") ||
               (resource == "result" && action == "export");
    }
    
    static bool canAdminAccess(const std::string& resource, const std::string& action) {
        return canAnalystAccess(resource, action) ||
               (resource == "config" && (action == "read" || action == "write"));
    }
};
```

#### Data Protection Best Practices

1. **Encryption**: Encrypt sensitive experiment data at rest and in transit
2. **Backup Security**: Secure backup of experiment checkpoints
3. **Access Logging**: Log all access to sensitive experiment data
4. **Data Retention**: Implement data retention policies

#### Network Security Best Practices

1. **Secure Configuration**: Use secure network configurations
2. **Firewall Rules**: Implement appropriate firewall rules
3. **Network Segmentation**: Segment network for different experiment types
4. **Traffic Monitoring**: Monitor network traffic for anomalies

## Best Practices

### Development Best Practices

#### Code Quality

1. **Modular Design**: Design modular, testable components
2. **Error Handling**: Implement comprehensive error handling
3. **Documentation**: Document all public APIs and classes
4. **Unit Testing**: Write comprehensive unit tests
5. **Integration Testing**: Test system integration thoroughly

#### Testing Best Practices

```cpp
// Unit test example
TEST(BrainCloneTest, CloneFromTemplate) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    auto clone = BrainClone::cloneFromTemplate(brain, *config, BrainClone::CloneMode::DEEP);
    ASSERT_NE(clone, nullptr);
    
    BrainClone::CloneStats stats;
    auto cloneWithStats = BrainClone::cloneFromTemplate(brain, *config, 
                                                        BrainClone::CloneMode::DEEP, 
                                                        true, &stats);
    ASSERT_NE(cloneWithStats, nullptr);
    ASSERT_TRUE(stats.success);
}

// Integration test example
TEST(ExperimentRunnerIntegrationTest, CloneFunctionality) {
    ExperimentRunner runner;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    auto clonedBrain = runner.cloneBrain(brain, "default", BrainClone::CloneMode::DEEP);
    ASSERT_NE(clonedBrain, nullptr);
    
    auto experiment = runner.createExperiment("TestExp");
    experiment->recordMetric("test", 42.0);
    
    auto clonedExperiment = runner.cloneExperiment(experiment);
    ASSERT_NE(clonedExperiment, nullptr);
    ASSERT_DOUBLE_EQ(clonedExperiment->getMetric("test"), 42.0);
}
```

#### Performance Testing

```cpp
// Performance test example
TEST(BatchExperimentManagerTest, PerformanceMetrics) {
    BatchExperimentManager manager;
    BatchExperimentManager::BatchConfig config;
    manager.configure(config);
    
    // Create test experiments
    std::vector<std::shared_ptr<Experiment>> experiments;
    for (int i = 0; i < 100; ++i) {
        auto exp = std::make_shared<Experiment>("exp_" + std::to_string(i));
        experiments.push_back(exp);
    }
    
    // Measure performance
    auto startTime = std::chrono::system_clock::now();
    auto results = manager.executeBatch(experiments, brainTemplate, envTemplate);
    auto endTime = std::chrono::system_clock::now();
    
    double elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();
    double throughput = experiments.size() / elapsedSeconds;
    
    EXPECT_GT(throughput, 10.0); // Should handle at least 10 experiments per second
}
```

### Operational Best Practices

#### System Administration

1. **Resource Management**: Monitor and manage system resources
2. **Backup Strategy**: Implement robust backup strategies
3. **Monitoring**: Set up comprehensive monitoring
4. **Logging**: Maintain comprehensive logs
5. **Security**: Implement security measures

#### Experiment Management

1. **Configuration Management**: Manage experiment configurations effectively
2. **Checkpoint Management**: Implement checkpoint strategies
3. **Error Handling**: Implement robust error handling
4. **Recovery**: Implement recovery mechanisms
5. **Performance Tuning**: Tune performance for specific workloads

#### Production Deployment

1. **Deployment Strategy**: Plan deployment strategy
2. **Testing**: Thoroughly test in production-like environments
3. **Monitoring**: Set up production monitoring
4. **Backup**: Implement production backup strategies
5. **Security**: Implement production security measures

### Usage Best Practices

#### For Researchers

1. **Use Appropriate Clone Modes**: Choose appropriate clone modes for experiments
2. **Leverage Batch Processing**: Use batch processing for large experiment sets
3. **Implement Checkpointing**: Use checkpointing for long-running experiments
4. **Monitor Performance**: Monitor experiment performance
5. **Document Experiments**: Document experiment configurations and results

#### For Developers

1. **Follow Code Conventions**: Follow code conventions and best practices
2. **Write Tests**: Write comprehensive tests
3. **Document APIs**: Document all public APIs
4. **Handle Errors**: Implement comprehensive error handling
5. **Optimize Performance**: Optimize performance for critical paths

#### For System Administrators

1. **Monitor System Health**: Monitor system health and performance
2. **Manage Resources**: Manage system resources effectively
3. **Implement Security**: Implement security measures
4. **Maintain Backups**: Maintain regular backups
5. **Troubleshoot**: Implement effective troubleshooting procedures

## Troubleshooting

### Common Issues

#### Issue 1: Brain Cloning Fails

**Symptoms**:
- Brain cloning returns null or throws exception
- Clone operation fails with error message
- Clone statistics show failure

**Causes**:
1. Invalid or missing configuration template
2. Brain initialization failure
3. Memory allocation failure
4. Thread synchronization issues

**Solutions**:
1. Verify configuration template is valid
2. Check if brain can be initialized
3. Monitor memory usage
4. Check for thread synchronization issues

```cpp
def troubleshootBrainClone():
    # Check configuration template
    if not config_template:
        print("Error: Configuration template is null")
        return False
    
    # Try with different clone mode
    try:
        clone = BrainClone.cloneFromTemplate(brain, config_template, 
                                             BrainClone.CloneMode.SHALLOW)
        if clone:
            print("Shallow cloning successful")
            return True
    except Exception as e:
        print(f"Shallow cloning failed: {e}")
    
    # Try deep cloning
    try:
        clone = BrainClone.cloneFromTemplate(brain, config_template, 
                                             BrainClone.CloneMode.DEEP)
        if clone:
            print("Deep cloning successful")
            return True
    except Exception as e:
        print(f"Deep cloning failed: {e}")
    
    return False
```

#### Issue 2: Environment Cloning Fails

**Symptoms**:
- Environment cloning returns null
- Clone operation fails
- Environment state not preserved

**Causes**:
1. Environment type not supported
2. State preservation issues
3. Parameter validation failures
4. Resource allocation issues

**Solutions**:
1. Verify environment type supports cloning
2. Check environment state preservation
3. Validate clone parameters
4. Check resource availability

```cpp
def troubleshootEnvironmentClone():
    # Verify environment type
    if not env:
        print("Error: Environment is null")
        return False
    
    env_name = env.getName()
    if env_name == "UnsupportedType":
        print("Error: Environment type does not support cloning")
        return False
    
    # Try simple cloning
    try:
        clone = EnvironmentClone.cloneFromTemplate(env, 
                                                  EnvironmentClone.CloneMode.SHALLOW)
        if clone:
            print("Shallow environment cloning successful")
            return True
    except Exception as e:
        print(f"Shallow environment cloning failed: {e}")
    
    # Try with state preservation
    try:
        state = env.getState()
        clone = EnvironmentClone.clonePreserveState(env, state)
        if clone:
            print("State-preserving environment cloning successful")
            return True
    except Exception as e:
        print(f"State-preserving environment cloning failed: {e}")
    
    return False
```

#### Issue 3: Batch Experiment Execution Fails

**Symptoms**:
- Batch execution returns empty results
- Batch execution times out
- Batch execution fails with errors

**Causes**:
1. Invalid batch configuration
2. Insufficient resources
3. Threading issues
4. Synchronization problems

**Solutions**:
1. Validate batch configuration
2. Check resource availability
3. Check threading configuration
4. Check synchronization

```cpp
def troubleshootBatchExecution():
    # Validate batch configuration
    if not batch_config:
        print("Error: Batch configuration is null")
        return False
    
    if batch_config.maxConcurrentExperiments == 0:
        print("Error: maxConcurrentExperiments is 0")
        return False
    
    if batch_config.maxStepsPerExperiment == 0:
        print("Error: maxStepsPerExperiment is 0")
        return False
    
    # Check resources
    if not checkResourceAvailability(batch_config):
        print("Error: Insufficient resources")
        return False
    
    # Try with reduced concurrency
    BatchExperimentManager::BatchConfig reducedConfig = batch_config
    reducedConfig.maxConcurrentExperiments = min(1, batch_config.maxConcurrentExperiments)
    
    BatchExperimentManager manager;
    manager.configure(reducedConfig)
    
    try:
        results = manager.executeBatch(experiments, brainTemplate, envTemplate)
        print(f"Batch execution completed with {len(results)} results")
        return True
    except Exception as e:
        print(f"Batch execution failed: {e}")
        return False
```

#### Issue 4: Checkpoint Operations Fail

**Symptoms**:
- Checkpoint save fails
- Checkpoint load fails
- Checkpoint file not found

**Causes**:
1. File system issues
2. Permission problems
3. Invalid checkpoint path
4. Checkpoint corruption

**Solutions**:
1. Check file system permissions
2. Verify checkpoint path exists
3. Check file system free space
4. Verify checkpoint file integrity

```cpp
def troubleshootCheckpoint():
    # Check file system permissions
    if not checkFileSystemPermissions():
        print("Error: File system permission issues")
        return False
    
    # Check checkpoint path
    if not checkPathExists(checkpoint_path):
        print(f"Error: Checkpoint path does not exist: {checkpoint_path}")
        return False
    
    # Check free space
    if not checkFreeSpace(checkpoint_path, 100 * 1024 * 1024):  # 100 MB required
        print("Error: Insufficient free space")
        return False
    
    # Try to save checkpoint
    try:
        if saveCheckpoint():
            print("Checkpoint save successful")
            return True
        else:
            print("Error: Checkpoint save failed")
            return False
    except Exception as e:
        print(f"Error saving checkpoint: {e}")
        return False
```

### Advanced Troubleshooting

#### Debugging Batch Execution Issues

```cpp
def debugBatchExecution():
    # Enable debug logging
    Logger.setLogLevel(Logger.LogLevel.DEBUG)
    
    # Check batch configuration
    BatchExperimentManager::BatchConfig config = manager.getConfig()
    print(f"Batch configuration: maxConcurrentExperiments={config.maxConcurrentExperiments}, "
          f"maxStepsPerExperiment={config.maxStepsPerExperiment}")
    
    # Check experiment queue
    size_t queueSize = manager.getExperimentQueueSize()
    print(f"Experiment queue size: {queueSize}")
    
    # Check resource usage
    auto metrics = manager.getPerformanceMetrics()
    print(f"Performance metrics: throughput={metrics.throughputExperimentsPerSecond}, "
          f"avgDuration={metrics.avgExperimentTimeMs}ms")
    
    # Check for failed experiments
    std::vector<ExperimentResult> results = manager.getResults()
    size_t failedCount = 0
    for result in results:
        if result.getStatus() != ExperimentResult.Status.SUCCESS:
            failedCount += 1
            print(f"Failed experiment: {result.getExperimentName()}, "
                  f"status={static_cast<int>(result.getStatus())}")
    
    print(f"Total failed experiments: {failedCount}")
    
    return True
```

#### Performance Troubleshooting

```cpp
def troubleshootPerformance():
    # Check cloning performance
    BrainClone::CloneStats brainStats = BrainClone.getClonePerformanceMetrics()
    print(f"Brain cloning: {brainStats.neuronsCloned} neurons, "
          f"{brainStats.cloningTimeMs}ms")
    
    EnvironmentClone::CloneStats envStats = EnvironmentClone.getClonePerformanceMetrics()
    print(f"Environment cloning: {envStats.stateSize} bytes, "
          f"{envStats.cloningTimeMs}ms")
    
    # Check batch performance
    BatchExperimentManager::PerformanceMetrics batchMetrics = 
        manager.getPerformanceMetrics()
    print(f"Batch performance: {batchMetrics.throughputExperimentsPerSecond} exp/s, "
          f"{batchMetrics.avgExperimentTimeMs}ms avg")
    
    # Check memory usage
    MemoryMonitor::MemoryInfo memoryInfo = MemoryMonitor.getMemoryInfo()
    print(f"Memory usage: {memoryInfo.usedMemoryMB} MB, "
          f"{memoryInfo.availableMemoryMB} MB available")
    
    return True
```

#### Recovery Troubleshooting

```cpp
def troubleshootRecovery():
    # Check for checkpoints
    if not manager.checkpointExists():
        print("Error: No checkpoints found")
        return False
    
    # Try to load checkpoint
    if not manager.loadCheckpoint():
        print("Error: Failed to load checkpoint")
        return False
    
    # Check checkpoint integrity
    if not verifyCheckpointIntegrity():
        print("Error: Checkpoint integrity check failed")
        return False
    
    print("Checkpoint recovery successful")
    return True
```

## API Reference

### BrainClone API

#### Class: BrainClone

```cpp
class BrainClone {
public:
    enum class CloneMode {
        SHALLOW,
        DEEP,
        SHALLOW_RETAIN,
        DEEP_RETAIN
    };
    
    struct CloneStats {
        uint64_t neuronsCloned;
        uint64_t synapsesCloned;
        uint64_t memoryBytesCloned;
        double cloningTimeMs;
        bool success;
        std::string errorMessage;
    };
    
    static std::shared_ptr<Brain> cloneFromTemplate(
        std::shared_ptr<Brain> original,
        const Config& templateConfig,
        CloneMode mode = CloneMode::DEEP,
        bool preserveInternalState = false,
        CloneStats* stats = nullptr
    );
    
    static std::shared_ptr<Brain> cloneWithCheckpoint(
        std::shared_ptr<Brain> original,
        const std::string& checkpointPath,
        CloneMode mode = CloneMode::DEEP
    );
    
    static bool applyConfigTemplate(
        std::shared_ptr<Brain> brain,
        const Config& templateConfig
    );
    
    static CloneStats getClonePerformanceMetrics();
    
    static std::shared_ptr<Brain> cloneThreadSafe(
        std::shared_ptr<Brain> original,
        const Config& templateConfig,
        std::mutex* lock = nullptr
    );
    
    static std::vector<std::shared_ptr<Brain>> batchClone(
        const std::vector<std::shared_ptr<Brain>>& originals,
        const Config& templateConfig,
        CloneMode mode = CloneMode::DEEP,
        size_t threadCount = 0,
        std::vector<CloneStats>* outStats = nullptr
    );
    
    static std::shared_ptr<Brain> deepClone(
        std::shared_ptr<Brain> original,
        bool copyMemorySystems = true,
        bool copyPlasticity = true,
        bool copyDevelopmentState = true
    );
    
    static std::shared_ptr<Brain> shallowClone(
        std::shared_ptr<Brain> original
    );
    
    static void resetCloneStats();
    
    static std::string serializeCloneConfig(
        const Config& config,
        CloneMode mode = CloneMode::DEEP
    );
    
    static Config deserializeCloneConfig(const std::string& serialized);
    
    static bool validateCloneConfig(const Config& config);
    
    static std::string getVersion();
};
```

### EnvironmentClone API

#### Class: EnvironmentClone

```cpp
class EnvironmentClone {
public:
    enum class CloneMode {
        SHALLOW,
        DEEP,
        CHECKPOINT
    };
    
    struct CloneStats {
        uint64_t stateSize;
        double cloningTimeMs;
        bool success;
        std::string errorMessage;
    };
    
    static std::shared_ptr<Environment> cloneFromTemplate(
        std::shared_ptr<Environment> original,
        CloneMode mode = CloneMode::DEEP,
        bool preserveState = true,
        CloneStats* stats = nullptr
    );
    
    static std::shared_ptr<Environment> cloneWithCheckpoint(
        std::shared_ptr<Environment> original,
        const std::string& checkpointPath
    );
    
    static std::shared_ptr<Environment> cloneWithParams(
        std::shared_ptr<Environment> original,
        const std::string& paramName,
        const std::string& paramValue
    );
    
    static CloneStats getClonePerformanceMetrics();
    
    static std::shared_ptr<Environment> cloneThreadSafe(
        std::shared_ptr<Environment> original,
        CloneMode mode = CloneMode::DEEP,
        std::mutex* lock = nullptr
    );
    
    static std::vector<std::shared_ptr<Environment>> batchClone(
        const std::vector<std::shared_ptr<Environment>>& originals,
        CloneMode mode = CloneMode::DEEP,
        size_t threadCount = 0,
        std::vector<CloneStats>* outStats = nullptr
    );
    
    static std::shared_ptr<Environment> clonePreserveState(
        std::shared_ptr<Environment> original,
        const EnvironmentState& preservedState
    );
    
    static void resetCloneStats();
    
    static bool validateCloneParams(const std::string& paramName, 
                                    const std::string& paramValue);
    
    static std::string serializeCloneConfig(const std::string& mode = "DEEP");
    
    static std::string deserializeCloneConfig(const std::string& serialized);
    
    static std::string getVersion();
};
```

### ExperimentResult API

#### Class: ExperimentResult

```cpp
class ExperimentResult {
public:
    enum class Status {
        SUCCESS,
        FAILED,
        RUNNING,
        PAUSED,
        TIMEOUT,
        ERROR
    };
    
    enum class AggregationMode {
        MEAN,
        SUM,
        MAX,
        MIN,
        WEIGHTED_MEAN,
        MEDIAN
    };
    
    struct PerformanceMetrics {
        double avgFiringRate;
        double totalReward;
        double efficiencyScore;
        double convergenceRate;
    };
    
    ExperimentResult();
    ~ExperimentResult();
    
    static ExperimentResult fromExperiment(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment
    );
    
    void addExperiment(std::shared_ptr<Experiment> experiment);
    
    double calculateMetric(const std::string& name, 
                          AggregationMode mode = AggregationMode::MEAN) const;
    
    bool compareWith(const ExperimentResult& other) const;
    
    Status getStatus() const { return status_; }
    void setStatus(Status status) { status_ = status; }
    
    const std::string& getExperimentName() const { return experimentName_; }
    void setExperimentName(const std::string& name) { experimentName_ = name; }
    
    const std::unordered_map<std::string, double>& getMetrics() const { return metrics_; }
    double getMetric(const std::string& name) const;
    void setMetric(const std::string& name, double value);
    
    double getDuration() const { return durationMs_; }
    void setDuration(double ms) { durationMs_ = ms; }
    
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    void setTimestamp(const std::chrono::system_clock::time_point& tp) { timestamp_ = tp; }
    
    const std::vector<StepData>& getStepData() const { return stepData_; }
    void addStepData(const StepData& stepData) { stepData_.push_back(stepData); }
    
    const std::vector<std::string>& getNotes() const { return notes_; }
    void addNote(const std::string& note) { notes_.push_back(note); }
    
    bool isValid() const { return status_ == Status::SUCCESS && !experimentName_.empty(); }
    
    std::string serialize() const;
    static ExperimentResult deserialize(const std::string& serialized);
    
    std::string getSummary() const;
    
    PerformanceMetrics calculatePerformanceMetrics() const;
    
    bool approximatelyEqual(const ExperimentResult& other, double tolerance = 1e-6) const;
    
    size_t getHash() const;
    
    std::vector<ExperimentResult> filterByMetric(const std::string& metricName, 
                                                 double threshold, 
                                                 bool greaterThan = true) const;
    
    static ExperimentResult getBestResult(const std::vector<ExperimentResult>& results,
                                         const std::string& metricName = "total_reward");
    
    static std::string getVersion();
};
```

### BatchExperimentManager API

#### Class: BatchExperimentManager

```cpp
class BatchExperimentManager {
public:
    enum class ExecutionMode {
        SEQUENTIAL,
        PARALLEL,
        PIPELINED,
        INTERLEAVED
    };
    
    enum class BatchStatus {
        IDLE,
        RUNNING,
        PAUSED,
        COMPLETED,
        FAILED,
        CANCELLED
    };
    
    struct BatchConfig {
        size_t maxConcurrentExperiments;
        SimulationStep maxStepsPerExperiment;
        double timeoutSeconds;
        bool checkpointEnabled;
        std::string checkpointPath;
        bool pauseOnError;
        bool useBrainCloning;
        bool useEnvironmentCloning;
        BrainClone::CloneMode brainCloneMode;
        EnvironmentClone::CloneMode envCloneMode;
        bool threadSafeCloning;
        bool preserveInternalState;
        std::vector<std::string> excludedExperiments;
        std::map<std::string, std::string> experimentParams;
        
        BatchConfig() : maxConcurrentExperiments(4),
                       maxStepsPerExperiment(1000),
                       timeoutSeconds(300.0),
                       checkpointEnabled(false),
                       pauseOnError(false),
                       useBrainCloning(true),
                       useEnvironmentCloning(true),
                       threadSafeCloning(true),
                       preserveInternalState(false) {}
    };
    
    struct BatchResultSummary {
        size_t totalExperiments;
        size_t completedExperiments;
        size_t failedExperiments;
        size_t pausedExperiments;
        double totalDurationMs;
        double avgExperimentDurationMs;
        std::vector<std::string> successfulExperimentNames;
        std::vector<std::string> failedExperimentNames;
        std::vector<std::string> pausedExperimentNames;
        
        BatchResultSummary() : totalExperiments(0),
                              completedExperiments(0),
                              failedExperiments(0),
                              pausedExperiments(0),
                              totalDurationMs(0.0),
                              avgExperimentDurationMs(0.0) {}
    };
    
    BatchExperimentManager();
    ~BatchExperimentManager();
    
    void configure(const BatchConfig& config);
    const BatchConfig& getConfig() const { return config_; }
    
    BatchStatus getStatus() const { return status_; }
    bool isRunning() const { return status_ == BatchStatus::RUNNING; }
    bool isPaused() const { return status_ == BatchStatus::PAUSED; }
    
    void pause();
    void resume();
    void cancel();
    
    std::vector<ExperimentResult> executeBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    std::vector<ExperimentResult> executeBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        std::function<void(size_t, size_t, const std::string&)> progressCallback
    );
    
    const std::vector<ExperimentResult>& getResults() const { return results_; }
    BatchResultSummary getResultSummary() const;
    
    bool saveResults(const std::string& filepath) const;
    bool loadResults(const std::string& filepath);
    
    std::vector<ExperimentResult> getExperimentResults(const std::string& experimentName) const;
    std::vector<bool> compareWithReference(const std::vector<ExperimentResult>& referenceResults,
                                          double tolerance = 1e-6) const;
    
    ExperimentResult aggregateResults(const std::string& metricName,
                                      const std::string& aggregationType = "mean") const;
    
    struct PerformanceMetrics {
        double throughputExperimentsPerSecond;
        double avgExperimentTimeMs;
        double cpuUtilization;
        double memoryUsageMB;
        std::vector<double> experimentTimes;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    void setProgressCallback(std::function<void(size_t, size_t, const std::string&)> callback) {
        progressCallback_ = callback;
    }
    
    bool checkpointExists() const;
    bool saveCheckpoint() const;
    bool loadCheckpoint();
    
    static BatchConfig createConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath) const;
    
    static std::string getVersion();
    
    void reset();
    bool validateConfig() const;
    
    double estimateExecutionTime(const std::vector<std::shared_ptr<Experiment>>& experiments) const;
};
```

### ExperimentRunner API (Enhanced)

#### Class: ExperimentRunner

```cpp
class ExperimentRunner {
public:
    // Basic functionality (backward compatible)
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    void clearExperiments();
    void setGlobalSeed(uint64_t seed);
    bool saveResults(const std::string& filepath) const;
    
    // Cloning functionality
    static std::shared_ptr<Brain> cloneBrain(std::shared_ptr<Brain> original,
                                            const std::string& templateName = "",
                                            BrainClone::CloneMode mode = BrainClone::CloneMode::DEEP);
    
    static std::shared_ptr<Environment> cloneEnvironment(std::shared_ptr<Environment> original,
                                                         EnvironmentClone::CloneMode mode = EnvironmentClone::CloneMode::DEEP);
    
    static std::shared_ptr<Experiment> cloneExperiment(std::shared_ptr<Experiment> original);
    
    void setBrainTemplate(const std::string& name, std::shared_ptr<Brain> templateBrain);
    void setEnvironmentTemplate(const std::string& name, std::shared_ptr<Environment> templateEnvironment);
    
    std::shared_ptr<Brain> getBrainTemplate(const std::string& name) const;
    std::shared_ptr<Environment> getEnvironmentTemplate(const std::string& name) const;
    
    // Batch experiment management
    std::vector<ExperimentResult> executeBatchExperiments(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate = nullptr,
        std::shared_ptr<Environment> environmentTemplate = nullptr,
        BatchExperimentManager::BatchConfig config = BatchExperimentManager::BatchConfig()
    );
    
    void configureBatchManager(const BatchExperimentManager::BatchConfig& config);
    BatchExperimentManager& getBatchManager() { return batchManager_; }
    
    // Experiment result aggregation
    static ExperimentResult aggregateResults(const std::vector<ExperimentResult>& results,
                                            const std::string& metricName = "total_reward",
                                            const std::string& aggregationType = "mean");
    
    static std::vector<bool> compareExperimentResults(const std::vector<ExperimentResult>& results,
                                                      double tolerance = 1e-6);
    
    static bool saveExperimentResults(const std::vector<ExperimentResult>& results,
                                      const std::string& filepath);
    
    static std::vector<ExperimentResult> loadExperimentResults(const std::string& filepath);
    
    // Pause/resume functionality
    void pauseBatchExecution();
    void resumeBatchExecution();
    bool isBatchPaused() const;
    std::pair<size_t, size_t> getBatchProgress() const;
    std::string getBatchStatusMessage() const;
    
    // Checkpoint functionality
    bool saveExperimentCheckpoint(std::shared_ptr<Experiment> experiment,
                                  const std::string& checkpointPath) const;
    
    static std::shared_ptr<Experiment> loadExperimentCheckpoint(const std::string& checkpointPath);
    
    bool saveBrainCheckpoint(std::shared_ptr<Brain> brain,
                            const std::string& checkpointPath) const;
    
    static std::shared_ptr<Brain> loadBrainCheckpoint(const std::string& checkpointPath);
    
    bool saveEnvironmentCheckpoint(std::shared_ptr<Environment> environment,
                                   const std::string& checkpointPath) const;
    
    static std::shared_ptr<Environment> loadEnvironmentCheckpoint(const std::string& checkpointPath);
    
    // Advanced experiment management
    struct CloningConfig {
        bool enableBrainCloning;
        bool enableEnvironmentCloning;
        BrainClone::CloneMode brainCloneMode;
        EnvironmentClone::CloneMode environmentCloneMode;
        bool preserveInternalState;
        bool threadSafeCloning;
        
        CloningConfig() : enableBrainCloning(true),
                         enableEnvironmentCloning(true),
                         brainCloneMode(BrainClone::CloneMode::DEEP),
                         environmentCloneMode(EnvironmentClone::CloneMode::DEEP),
                         preserveInternalState(false),
                         threadSafeCloning(true) {}
    };
    
    struct BatchConfig {
        size_t maxConcurrentExperiments;
        SimulationStep maxStepsPerExperiment;
        double timeoutSeconds;
        bool checkpointEnabled;
        std::string checkpointPath;
        bool pauseOnError;
        
        BatchConfig() : maxConcurrentExperiments(4),
                       maxStepsPerExperiment(1000),
                       timeoutSeconds(300.0),
                       checkpointEnabled(false),
                       pauseOnError(false) {}
    };
    
    struct ExperimentRunConfig {
        std::string runName;
        CloningConfig cloningConfig;
        BatchConfig batchConfig;
        std::vector<std::string> experimentNames;
        std::map<std::string, std::string> experimentParameters;
        
        ExperimentRunConfig() : runName("default_run") {}
    };
    
    std::vector<ExperimentResult> executeExperimentRun(const ExperimentRunConfig& config);
    
    // Utility functionality
    static std::string getVersion();
    void reset();
    
    static BrainClone::CloneStats getBrainCloneStats();
    static EnvironmentClone::CloneStats getEnvironmentCloneStats();
    
    void setLoggingEnabled(bool enabled) { loggingEnabled_ = enabled; }
    bool isLoggingEnabled() const { return loggingEnabled_; }
    
    void setName(const std::string& name) { runnerName_ = name; }
    const std::string& getName() const { return runnerName_; }
    
    std::chrono::system_clock::time_point getCreationTime() const { return creationTime_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BatchExperimentManager batchManager_;
    bool loggingEnabled_;
    std::string runnerName_;
    std::chrono::system_clock::time_point creationTime_;
};
```

### Configuration Management API

#### Class: ConfigurationManager

```cpp
class ConfigurationManager {
public:
    static ExperimentRunConfig loadFromFile(const std::string& filepath);
    static ExperimentRunConfig loadFromJSON(const std::string& jsonStr);
    static ExperimentRunConfig loadFromEnvironment(const std::string& prefix);
    static ExperimentRunConfig mergeConfigs(const ExperimentRunConfig& base,
                                           const ExperimentRunConfig& override);
    static bool validateConfig(const ExperimentRunConfig& config);
    static ExperimentRunConfig getDefaultConfig(const std::string& experimentType);
};
```

### Monitoring and Logging API

#### Class: Monitor

```cpp
class Monitor {
public:
    static void recordExperimentStart(const std::string& experimentName);
    static void recordExperimentComplete(const std::string& experimentName,
                                         const ExperimentResult& result);
    static void recordExperimentFailure(const std::string& experimentName,
                                        const std::string& errorMessage);
    static void recordPerformanceMetrics(const std::string& metricName,
                                         double value,
                                         const std::string& experimentName = "");
    static std::map<std::string, double> getStatistics();
    static bool exportStatistics(const std::string& filepath);
    static void clearStatistics();
};
```

#### Class: PerformanceMonitor

```cpp
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double throughputExperimentsPerSecond;
        double avgExperimentTimeMs;
        double cpuUtilization;
        double memoryUsageMB;
        std::vector<double> experimentTimes;
        std::vector<std::string> failedExperiments;
        std::vector<std::string> completedExperiments;
    };
    
    static PerformanceMetrics getPerformanceMetrics();
    static double calculateExperimentThroughput(size_t completed, 
                                                std::chrono::system_clock::time_point startTime);
};
```

#### Class: Logger

```cpp
class Logger {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
        std::string message;
        std::string experimentName;
        std::string threadId;
    };
    
    static void log(LogLevel level, const std::string& message);
    static void log(LogLevel level, const std::string& message,
                   const std::string& experimentName);
    static std::vector<LogEntry> getLogEntries(LogLevel minLevel = LogLevel::DEBUG,
                                              const std::string& experimentName = "",
                                              const std::chrono::system_clock::time_point& since = {});
    static bool exportLogs(const std::string& filepath);
    static void clearLogs();
    static void setLogLevel(LogLevel level);
    static void setLoggingEnabled(bool enabled);
    static bool isLoggingEnabled();
};
```

### Security Manager API

#### Class: SecurityManager

```cpp
class SecurityManager {
public:
    static bool authenticate(const std::string& username, const std::string& password);
    static bool hasPermission(const std::string& username, 
                              const std::string& resource, 
                              const std::string& action);
    static std::string encryptSensitiveData(const std::string& data);
    static std::string decryptSensitiveData(const std::string& encryptedData);
};
```

### Data Integrity Manager API

#### Class: DataIntegrityManager

```cpp
class DataIntegrityManager {
public:
    static std::string calculateChecksum(const std::string& filepath);
    static bool verifyFileIntegrity(const std::string& filepath,
                                    const std::string& expectedChecksum);
    static std::string signExperimentConfig(const ExperimentRunConfig& config);
    static bool verifyExperimentConfigSignature(const ExperimentRunConfig& config,
                                               const std::string& signature);
};
```

## Build and Run Instructions

### Building the Project

```bash
# Build all tests
./build_tests.sh

# Or build individually
cd build
make
```

### Running Tests

```bash
# Run all tests
./run_all_tests.sh

# Run specific test suites
./run_brainclone_tests.sh
./run_environmentclone_tests.sh
./run_experimentresult_tests.sh
./run_batchmanager_tests.sh
./run_integration_tests.sh
```

### Running Examples

```bash
# Run example programs
./examples/example_basic_cloning
./examples/example_batch_execution
./examples/example_checkpointing
./examples/example_advanced_configuration
```

## License

This documentation and code are licensed under the terms of the project license.

## Copyright

© 2026 NLM Project. All rights reserved.

## Acknowledgments

Special thanks to the contributors who helped develop the enhanced ExperimentRunner cloning functionality.

---

*Document generated: 2026-10-17*
*Version: 1.0.0*
*Author: Enhanced ExperimentRunner Development Team*