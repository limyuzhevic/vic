# Enhanced ExperimentRunner Cloning Functionality

## Overview

The enhanced ExperimentRunner provides comprehensive cloning capabilities for running large-scale experiments with independent brain instances while maintaining reproducibility and state preservation.

## Key Features

### 1. Brain Cloning

#### Clone Modes
- **DEEP**: Complete deep copy of all brain state including neurons, synapses, memory systems, plasticity rules, and developmental state
- **SHALLOW**: Fast shallow copy that shares some internal structures
- **SHALLOW_RETAIN**: Shallow copy with retained parent references
- **DEEP_RETAIN**: Deep copy with retained parent references

#### Clone Classes
- `BrainClone`: Handles brain cloning from configuration templates
- `BrainClone::cloneFromTemplate()`: Clone brain using template configuration
- `BrainClone::cloneWithCheckpoint()`: Clone brain with saved checkpoint data
- `BrainClone::deepClone()`: Complete deep clone with all internal systems
- `BrainClone::shallowClone()`: Fast shallow clone

### 2. Environment Cloning

#### Clone Modes
- **DEEP**: Complete environment state preservation
- **SHALLOW**: Fast environment cloning
- **CHECKPOINT**: Clone with specific checkpoint data

#### Clone Classes
- `EnvironmentClone`: Handles environment state cloning
- `EnvironmentClone::cloneFromTemplate()`: Clone environment with state preservation
- `EnvironmentClone::cloneWithCheckpoint()`: Clone with saved checkpoint
- `EnvironmentClone::cloneWithParams()`: Clone with specific parameters
- `EnvironmentClone::batchClone()`: Thread-safe batch cloning

### 3. Experiment Result Management

#### Result Classes
- `ExperimentResult`: Stores and manages experiment results
- `ExperimentResult::Status`: Result status (SUCCESS, FAILED, RUNNING, PAUSED, TIMEOUT, ERROR)
- `ExperimentResult::AggregationMode`: Aggregation methods (MEAN, SUM, MAX, MIN, WEIGHTED_MEAN, MEDIAN)

#### Features
- Result serialization/deserialization
- Result comparison and filtering
- Performance metrics calculation
- Result aggregation

### 4. Batch Experiment Management

#### Manager Class
- `BatchExperimentManager`: Manages batch experiment execution
- `BatchExperimentManager::ExecutionMode`: Execution modes (SEQUENTIAL, PARALLEL, PIPELINED, INTERLEAVED)

#### Features
- Thread-safe batch execution
- Configurable batch size and concurrency
- Progress tracking with callbacks
- Checkpoint support
- Pause/resume functionality
- Result aggregation and comparison

## API Usage Examples

### Basic Brain Cloning

```cpp
#include "ExperimentRunner.hpp"

// Create brain template
auto config = std::make_shared<Config>();
auto brainTemplate = std::make_shared<Brain>(config);

// Create experiment runner
ExperimentRunner runner;

// Clone brain from template
auto clonedBrain = ExperimentRunner::cloneBrain(
    brainTemplate, 
    "default_template", 
    BrainClone::CloneMode::DEEP
);
```

### Environment Cloning

```cpp
#include "EnvironmentClone.hpp"

// Clone environment
auto env = std::make_shared<GridWorldEnvironment>();
auto clonedEnv = EnvironmentClone::cloneFromTemplate(
    env, 
    EnvironmentClone::CloneMode::DEEP,
    true
);
```

### Batch Experiment Execution

```cpp
#include "BatchExperimentManager.hpp"

// Configure batch execution
BatchExperimentManager::BatchConfig config;
config.maxConcurrentExperiments = 4;
config.maxStepsPerExperiment = 1000;
config.useBrainCloning = true;
config.useEnvironmentCloning = true;

// Create batch manager
BatchExperimentManager manager;
manager.configure(config);

// Execute batch of experiments
std::vector<std::shared_ptr<Experiment>> experiments;
auto results = manager.executeBatch(
    experiments,
    brainTemplate,
    environmentTemplate,
    [](size_t completed, size_t total, const std::string& name) {
        // Progress callback
        std::cout << "Completed: " << completed << "/" << total 
                  << " - Current: " << name << std::endl;
    }
);
```

### Experiment Result Management

```cpp
#include "ExperimentResult.hpp"

// Create experiment result
ExperimentResult result;
result.setExperimentName("TestExperiment");
result.setMetric("total_reward", 150.5);
result.addNote("Experiment completed successfully");

// Compare results
ExperimentResult otherResult;
otherResult.setExperimentName("TestExperiment");
otherResult.setMetric("total_reward", 150.5);

if (result.compareWith(otherResult)) {
    std::cout << "Results are identical!" << std::endl;
}

// Aggregate results
std::vector<ExperimentResult> allResults;
// ... add results to vector ...

auto aggregated = ExperimentResult::aggregateResults(
    allResults, 
    "total_reward", 
    "mean"
);
```

### Advanced Batch Configuration

```cpp
#include "ExperimentRunner.hpp"

// Configure complete experiment run
ExperimentRunner::ExperimentRunConfig runConfig;
runConfig.runName = "largeScaleExperiment";
runConfig.cloningConfig.enableBrainCloning = true;
runConfig.cloningConfig.enableEnvironmentCloning = true;
runConfig.cloningConfig.brainCloneMode = BrainClone::CloneMode::DEEP;
runConfig.cloningConfig.environmentCloneMode = EnvironmentClone::CloneMode::DEEP;

runConfig.batchConfig.maxConcurrentExperiments = 8;
runConfig.batchConfig.maxStepsPerExperiment = 5000;
runConfig.batchConfig.timeoutSeconds = 600.0;
runConfig.batchConfig.checkpointEnabled = true;
runConfig.batchConfig.checkpointPath = "/data/experiment_checkpoints/";

runConfig.experimentNames = {"exp1", "exp2", "exp3", "exp4", "exp5"};

// Execute with full configuration
ExperimentRunner runner;
auto results = runner.executeExperimentRun(runConfig);
```

### Checkpoint Operations

```cpp
#include "ExperimentRunner.hpp"

ExperimentRunner runner;

// Save experiment checkpoint
auto experiment = runner.getExperiment("myExperiment");
bool saved = runner.saveExperimentCheckpoint(
    experiment, 
    "/data/experiment_checkpoints/exp_123.checkpoint"
);

// Load experiment checkpoint
auto loadedExperiment = ExperimentRunner::loadExperimentCheckpoint(
    "/data/experiment_checkpoints/exp_123.checkpoint"
);

// Save brain checkpoint
bool brainSaved = runner.saveBrainCheckpoint(
    brainTemplate,
    "/data/brain_checkpoints/brain_template.checkpoint"
);

// Load brain checkpoint
auto loadedBrain = ExperimentRunner::loadBrainCheckpoint(
    "/data/brain_checkpoints/brain_template.checkpoint"
);
```

## Thread Safety

All cloning and batch operations are thread-safe:

- `BrainClone::cloneThreadSafe()`
- `EnvironmentClone::cloneThreadSafe()`
- `BatchExperimentManager::executeBatch()` (parallel mode)
- `BrainClone::batchClone()`
- `EnvironmentClone::batchClone()`

## Performance Considerations

### Clone Performance

1. **Deep vs Shallow**: Deep cloning is more memory-intensive but provides complete isolation
2. **Batch Operations**: Use `batchClone()` for multiple cloning operations
3. **Thread Count**: Optimize thread count based on workload and hardware
4. **Caching**: Reuse configuration templates for faster cloning

### Batch Performance

1. **Execution Modes**:
   - SEQUENTIAL: Simpler, lower overhead
   - PARALLEL: Best for independent experiments
   - PIPELINED: Good for I/O bound operations
   - INTERLEAVED: Balanced approach

2. **Memory Management**:
   - Monitor memory usage during large batch runs
   - Use checkpointing for long-running experiments
   - Configure concurrent experiment limits

## Error Handling

### Common Error Scenarios

1. **Null Templates**: Provide valid brain/environment templates
2. **Invalid Parameters**: Use validation functions to check parameters
3. **Checkpoint Issues**: Ensure file system permissions and disk space
4. **Thread Errors**: Handle synchronization exceptions

### Error Recovery

1. **Pause/Resume**: Use pause/resume functionality for long operations
2. **Checkpoint Recovery**: Load from last successful checkpoint
3. **Fallback Strategies**: Implement fallback to sequential execution

## Configuration Templates

### Brain Clone Configuration

```cpp
BrainClone::CloneStats stats;
auto cloned = BrainClone::cloneFromTemplate(
    originalBrain,
    configTemplate,
    BrainClone::CloneMode::DEEP,
    true,  // preserveInternalState
    &stats
);
```

### Environment Clone Configuration

```cpp
EnvironmentClone::CloneStats envStats;
auto clonedEnv = EnvironmentClone::cloneFromTemplate(
    originalEnv,
    EnvironmentClone::CloneMode::DEEP,
    true,  // preserveState
    &envStats
);
```

### Batch Configuration

```cpp
BatchExperimentManager::BatchConfig config;
config.maxConcurrentExperiments = std::thread::hardware_concurrency();
config.maxStepsPerExperiment = 1000;
config.timeoutSeconds = 300.0;
config.checkpointEnabled = true;
config.pauseOnError = true;
config.excludedExperiments = {"exp_debug"};
```

## Testing

Comprehensive test suites are included:

- `BrainCloneTest.hpp/cpp`: Tests brain cloning functionality
- `EnvironmentCloneTest.hpp/cpp`: Tests environment cloning functionality
- `ExperimentResultTest.hpp/cpp`: Tests result management
- `BatchExperimentManagerTest.hpp/cpp`: Tests batch management
- `ExperimentRunnerIntegrationTest.hpp/cpp`: Integration tests

Run tests with:

```bash
./run_tests.sh  # Script to compile and run all tests
make test     # If Makefile exists
```

## Backward Compatibility

The enhanced ExperimentRunner maintains full backward compatibility:

1. **Existing API**: All original `ExperimentRunner` methods remain unchanged
2. **Default Behavior**: Existing code continues to work without modification
3. **Optional Features**: All new functionality is optional and opt-in
4. **Migration Path**: Gradual adoption of new features is supported

## Best Practices

### For Large-Scale Experiments

1. **Use Batch Processing**: Leverage `BatchExperimentManager` for large experiment sets
2. **Configure Cloning**: Use appropriate clone modes based on isolation requirements
3. **Monitor Performance**: Use `BatchExperimentManager::getPerformanceMetrics()`
4. **Use Checkpoints**: Enable checkpointing for long-running experiments
5. **Error Handling**: Implement robust error handling and recovery

### For Development and Testing

1. **Use Shallow Cloning**: For faster development iterations
2. **Test All Modes**: Test both deep and shallow cloning
3. **Validate Configurations**: Use `validateCloneConfig()` and `validateCloneParams()`
4. **Performance Profiling**: Profile cloning operations with `getClonePerformanceMetrics()`
5. **Comprehensive Testing**: Use the provided test suites

## Future Enhancements

Potential future improvements include:

1. **GPU Acceleration**: GPU-accelerated cloning for large networks
2. **Distributed Cloning**: Multi-machine cloning for very large experiments
3. **Smart Cloning**: Intelligent cloning based on usage patterns
4. **Automatic Checkpointing**: Intelligent checkpoint placement
5. **Metrics Integration**: Enhanced metrics collection and analysis

## References

- BrainClone documentation
- EnvironmentClone documentation
- BatchExperimentManager documentation
- ExperimentResult documentation