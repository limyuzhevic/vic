# NLM Experimental Framework API Reference

## Overview

The Experimental Framework API provides comprehensive access to NLM's experimental capabilities. NLM implements multiple experimental frameworks that enable researchers to conduct sophisticated neuroscience experiments, test hypotheses, and study neural systems.

## Experimental Framework Architecture

### Core Experimental Components

1. **Experiment Classes** - Base classes for experimental tasks
2. **ExperimentRunner** - Manages experiment execution
3. **Metrics** - Data collection and analysis
4. **ConfigurationTemplates** - Predefined experiment configurations
5. **AdvancedExperiment** - Complex experimental scenarios
6. **ParallelManager** - Parallel processing for large experiments

### Experimental Framework Types

#### 1. Basic Experiment Classes
- **Experiment** - Base class for simple experiments
- **Phase4Experiment** - Phase 4 cognition experiments
- **Phase6IntegratedExperiment** - Phase 6 integration experiments
- **AdvancedExperiment** - Advanced experimental frameworks

#### 2. Specialized Experiment Types
- **Memory Experiments** - Working memory, episodic memory, associative memory
- **Prediction Experiments** - Temporal prediction, action consequence prediction
- **Neuromodulation Experiments** - Reward, curiosity, novelty experiments
- **Development Experiments** - Developmental stage studies
- **Cognition Experiments** - Attention, planning, concept formation

#### 3. Performance Optimization
- **Parallel Processing** - Multi-core experiment execution
- **Memory Pool Management** - Efficient memory usage
- **Event-Driven Processing** - Optimized experiment flow
- **Load Balancing** - Distributes experiment load

## Experiment Base Class

### Experiment Class

The `Experiment` class (in `src/experiments/Experiment.hpp` and `src/experiments/Experiment.cpp`) provides the base functionality for all experiments.

#### Experiment(std::shared_ptr<Brain> brain)

**Description**: Creates an Experiment instance with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### virtual bool initialize()

**Description**: Initializes experiment with default settings.

**Returns**: `true` if initialization succeeds, `false` otherwise

**Example**:
```cpp
auto experiment = std::make_shared<Experiment>(brain);
if (!experiment->initialize()) {
    std::cerr << "Failed to initialize experiment!" << std::endl;
    return 1;
}
```

#### virtual bool run(size_t num_trials, size_t steps_per_trial)

**Description**: Runs experiment with specified trials and steps.

**Parameters**:
- `num_trials`: Number of experimental trials
- `steps_per_trial`: Steps per trial

**Returns**: `true` if experiment completes successfully, `false` otherwise

**Example**:
```cpp
bool success = experiment->run(10, 50);
if (!success) {
    std::cerr << "Experiment failed!" << std::endl;
    return 1;
}
```

#### virtual void reset()

**Description**: Resets experiment to initial state.

**Example**:
```cpp
experiment->reset();
```

#### virtual ExperimentResults getResults() const

**Description**: Gets experiment results.

**Returns**: Experiment results

**Example**:
```cpp
ExperimentResults results = experiment->getResults();
```

#### virtual bool isCompleted() const

**Description**: Checks if experiment is completed.

**Returns**: Whether experiment is completed

**Example**:
```cpp
bool completed = experiment->isCompleted();
```

### ExperimentResults Structure

```cpp
struct ExperimentResults {
    size_t trialsCompleted;              // Number of completed trials
    float overallAccuracy;               // Overall experiment accuracy (0.0-1.0)
    float averagePerformance;            // Average performance across trials
    float totalReward;                    // Total reward accumulated
    float learningRate;                   // Observed learning rate
    float adaptationSpeed;                // Speed of adaptation
    float behavioralStability;           // Behavioral stability (0.0-1.0)
    std::vector<float> trialPerformance;  // Performance for each trial
    std::vector<float> trialRewards;      // Rewards for each trial
    std::map<std::string, float> metrics; // Additional metrics
    float executionTime;                  // Total execution time
    size_t stepsExecuted;                // Total steps executed
};
```

## Phase4Experiment Class

### Phase4Experiment

The `Phase4Experiment` class (in `src/experiments/Phase4Experiment.hpp` and `src/experiments/Phase4Experiment.cpp`) provides Phase 4 experimental capabilities for studying cognition.

#### Phase4Experiment(std::shared_ptr<Brain> brain)

**Description**: Creates Phase4Experiment with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### Phase4Results run(Brain* brain, size_t num_trials)

**Description**: Runs Phase 4 experiment with specified trials.

**Parameters**:
- `brain`: Brain instance
- `num_trials`: Number of experimental trials

**Returns**: Phase 4 experiment results

**Example**:
```cpp
Phase4Experiment experiment;
Phase4Results results = experiment.run(brain.get(), 10);
```

### Phase4Results Structure

```cpp
struct Phase4Results {
    float predictionAccuracy;            // Temporal prediction accuracy
    float workingMemoryCapacity;         // Working memory capacity
    float episodicRecallAccuracy;       // Episodic memory recall accuracy
    float conceptsFormed;                // Number of concepts formed
    float attentionSelectivity;         // Attention selectivity
    float planningAccuracy;              // Planning accuracy
    float selfPredictionAccuracy;        // Self-prediction accuracy
    float imitationAccuracy;            // Social learning imitation accuracy
    float continualLearningGain;        // Continual learning improvement
    float neuralAttention;               // Neural attention measure
    float attentionNetwork;              // Attention network strength
    float planningNetwork;               // Planning network strength
    float selfModelNetwork;              // Self-model network strength
    float socialLearningNetwork;         // Social learning network strength
    float continualLearningNetwork;     // Continual learning network strength
    float integrationScore;              // Overall integration score
};
```

## Phase6IntegratedExperiment Class

### Phase6IntegratedExperiment

The `Phase6IntegratedExperiment` class (in `src/experiments/Phase6IntegratedExperiment.hpp` and `src/experiments/Phase6IntegratedExperiment.cpp`) provides Phase 6 integration experiment capabilities.

#### Phase6IntegratedExperiment(std::shared_ptr<Brain> brain)

**Description**: Creates Phase6IntegratedExperiment with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### bool verifyIntegration()

**Description**: Verifies system integration.

**Returns**: Whether integration is successful

**Example**:
```cpp
Phase6IntegratedExperiment experiment;
bool integration_ok = experiment.verifyIntegration();
```

#### Phase6Results run(Phase6Config config)

**Description**: Runs Phase 6 integration experiment with configuration.

**Parameters**:
- `config`: Phase 6 experiment configuration

**Returns**: Phase 6 experiment results

**Example**:
```cpp
Phase6Config config;
config.neuronCount = 500;
config.maxSteps = 2000;
config.enableCheckpointing = true;
Phase6Results results = experiment.run(config);
```

### Phase6Config Structure

```cpp
struct Phase6Config {
    size_t neuronCount;                // Number of neurons
    size_t maxSteps;                   // Maximum simulation steps
    bool enableCheckpointing;          // Enable checkpointing
    bool enableReplay;                 // Enable memory replay
    bool enableDevelopment;            // Enable development
    bool enableNeuromodulation;        // Enable neuromodulation
    bool enablePrediction;             // Enable prediction system
    float explorationRate;             // Exploration rate
    float noveltyThreshold;            // Novelty detection threshold
    size_t memoryCapacity;             // Memory capacity
    size_t replayInterval;             // Memory replay interval
    float plasticityDecay;             // Plasticity decay rate
};
```

### Phase6Results Structure

```cpp
struct Phase6Results {
    float totalReward;                  // Total accumulated reward
    float avgFiringRate;                // Average firing rate
    size_t memoryEpisodesStored;       // Memory episodes stored
    float dopamineLevel;               // Dopamine level
    bool memoryWorkingMemoryIntegrated; // Working memory integration
    bool memoryEpisodicMemoryIntegrated; // Episodic memory integration
    bool neuromodulationIntegrated;      // Neuromodulation integration
    bool predictionIntegrated;          // Prediction integration
    bool developmentIntegrated;         // Development integration
    bool checkpointingWorks;             // Checkpointing functionality
    float totalWallClockTime;           // Total execution time
    size_t totalStepsExecuted;          // Total steps executed
};
```

## AdvancedExperiment Class

### AdvancedExperiment

The `AdvancedExperiment` class (in `src/experiments/AdvancedExperiment.hpp` and `src/experiments/AdvancedExperiment.cpp`) provides advanced experimental capabilities for complex scenarios.

#### AdvancedExperiment(std::shared_ptr<Brain> brain)

**Description**: Creates AdvancedExperiment with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### AdvancedExperimentResults run(AdvancedExperimentConfig config)

**Description**: Runs advanced experiment with configuration.

**Parameters**:
- `config`: Advanced experiment configuration

**Returns**: Advanced experiment results

**Example**:
```cpp
AdvancedExperimentConfig config;
config.experimentType = "cognition";
config.numTrials = 20;
config.maxSteps = 500;
AdvancedExperimentResults results = experiment.run(config);
```

### AdvancedExperimentConfig Structure

```cpp
struct AdvancedExperimentConfig {
    std::string experimentType;           // Type of experiment
    size_t numTrials;                    // Number of trials
    size_t maxSteps;                     // Maximum steps
    float complexity;                    // Experiment complexity
    std::map<std::string, float> parameters; // Additional parameters
    std::vector<std::string> conditions; // Experimental conditions
    std::string hypothesis;              // Experimental hypothesis
};
```

### AdvancedExperimentResults Structure

```cpp
struct AdvancedExperimentResults {
    std::string experimentType;           // Type of experiment
    size_t trialsCompleted;              // Number of completed trials
    float overallSuccess;                // Overall success rate
    std::vector<std::string> discoveries; // Scientific discoveries
    std::vector<std::string> hypothesesSupported; // Supported hypotheses
    std::vector<std::string> hypothesesFalsified; // Falsified hypotheses
    float scientificImpact;              // Scientific impact score
    std::string conclusion;              // Experimental conclusion
};
```

## ExperimentRunner Class

### ExperimentRunner

The `ExperimentRunner` class (in `src/experiments/ExperimentRunner.hpp` and `src/experiments/ExperimentRunner.cpp`) provides experiment execution and management capabilities.

#### ExperimentRunner()

**Description**: Default constructor.

#### void setMaxWorkers(size_t max_workers)

**Description**: Sets maximum number of worker threads.

**Parameters**:
- `max_workers`: Maximum number of workers

**Example**:
```cpp
runner.setMaxWorkers(4);
```

#### void setParallelExecution(bool enable)

**Description**: Enables or disables parallel execution.

**Parameters**:
- `enable`: Whether to enable parallel execution

**Example**:
```cpp
runner.setParallelExecution(true);
```

#### template<typename ExperimentType> ExperimentResults runExperiment(
    std::shared_ptr<ExperimentType> experiment,
    size_t num_trials,
    size_t steps_per_trial
)

**Description**: Runs experiment using template function.

**Parameters**:
- `experiment`: Experiment instance
- `num_trials`: Number of trials
- `steps_per_trial`: Steps per trial

**Returns**: Experiment results

**Example**:
```cpp
auto results = runner.runExperiment(experiment, 10, 50);
```

#### void setProgressCallback(std::function<void(float)> callback)

**Description**: Sets progress callback function.

**Parameters**:
- `callback`: Progress callback function

**Example**:
```cpp
runner.setProgressCallback([](float progress) {
    std::cout << "Progress: " << progress * 100 << "%" << std::endl;
});
```

## Metrics Collection

### Metrics Class

The `Metrics` class (in `src/experiments/Metrics.hpp` and `src/experiments/Metrics.cpp`) provides metrics collection and analysis.

#### Metrics()

**Description**: Default constructor.

#### void recordMetric(const std::string& name, float value, const std::string& category = "general")

**Description**: Records a metric value.

**Parameters**:
- `name`: Metric name
- `value`: Metric value
- `category`: Metric category (optional)

**Example**:
```cpp
metrics.recordMetric("accuracy", 0.85, "performance");
metrics.recordMetric("reward", 1.5, "outcome");
```

#### float getMetric(const std::string& name) const

**Description**: Gets metric value.

**Parameters**:
- `name`: Metric name

**Returns**: Metric value

**Example**:
```cpp
float accuracy = metrics.getMetric("accuracy");
```

#### std::vector<float> getMetrics(const std::string& category) const

**Description**: Gets metrics for specified category.

**Parameters**:
- `category`: Metric category

**Returns**: List of metric values

**Example**:
```cpp
std::vector<float> performance_metrics = metrics.getMetrics("performance");
```

#### MetricsResults getResults() const

**Description**: Gets metrics results.

**Returns**: Metrics results

**Example**:
```cpp
MetricsResults results = metrics.getResults();
```

### MetricsResults Structure

```cpp
struct MetricsResults {
    std::map<std::string, std::vector<float>> categoryData; // Data by category
    std::map<std::string, float> summaryStats;            // Summary statistics
    float averageValue;                                 // Average value
    float standardDeviation;                           // Standard deviation
    float minValue;                                     // Minimum value
    float maxValue;                                     // Maximum value
    size_t dataPoints;                                  // Number of data points
    std::string timestamp;                             // Timestamp
};
```

## ParallelManager

### ParallelManager

The `ParallelManager` class (in `src/performance/ParallelManager.hpp` and `src/performance/ParallelManager.cpp`) provides parallel processing capabilities for experiments.

#### ParallelManager()

**Description**: Default constructor.

#### void setThreadCount(size_t thread_count)

**Description**: Sets thread count.

**Parameters**:
- `thread_count`: Number of threads

**Example**:
```cpp
manager.setThreadCount(4);
```

#### void enableSIMD(bool enable)

**Description**: Enables or disables SIMD processing.

**Parameters**:
- `enable`: Whether to enable SIMD

**Example**:
```cpp
manager.enableSIMD(true);
```

#### void enableMemoryPool(bool enable)

**Description**: Enables or disables memory pool.

**Parameters**:
- `enable`: Whether to enable memory pool

**Example**:
```cpp
manager.enableMemoryPool(true);
```

#### template<typename Function> auto parallelFor(size_t start, size_t end, Function func)

**Description**: Executes function in parallel over range.

**Parameters**:
- `start`: Start index
- `end`: End index
- `func`: Function to execute

**Returns**: Future or result

**Example**:
```cpp
auto future = manager.parallelFor(0, 100, [](size_t i) {
    return some_computation(i);
});
```

## Configuration Templates

### ConfigurationTemplates Class

The `ConfigurationTemplates` class (in `src/experiments/ConfigurationTemplates.hpp` and `src/experiments/ConfigurationTemplates.cpp`) provides predefined configuration templates.

#### ConfigurationTemplates()

**Description**: Default constructor.

#### Config createDefaultConfig()

**Description**: Creates default configuration.

**Returns**: Default configuration

**Example**:
```cpp
Config config = templates.createDefaultConfig();
```

#### Config createResearchConfig()

**Description**: Creates research configuration.

**Returns**: Research configuration

**Example**:
```cpp
Config config = templates.createResearchConfig();
```

#### Config createPerformanceConfig()

**Description**: Creates performance configuration.

**Returns**: Performance configuration

**Example**:
```cpp
Config config = templates.createPerformanceConfig();
```

#### Config getTemplate(const std::string& name)

**Description**: Gets configuration template by name.

**Parameters**:
- `name`: Template name

**Returns**: Configuration template

**Example**:
```cpp
Config config = templates.getTemplate("research");
```

## Experiment Examples

### Example 1: Basic Experiment

```cpp
// basic_experiment.cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"

class SimpleExperiment : public Experiment {
public:
    SimpleExperiment(std::shared_ptr<Brain> brain) : Experiment(brain) {}
    
    bool initialize() override {
        std::cout << "Initializing experiment..." << std::endl;
        return true;
    }
    
    bool run(size_t num_trials, size_t steps_per_trial) override {
        std::cout << "Running " << num_trials << " trials with " 
                  << steps_per_trial << " steps each..." << std::endl;
        
        for (size_t trial = 0; trial < num_trials; ++trial) {
            for (size_t step = 0; step < steps_per_trial; ++step) {
                brain->step(step);
            }
        }
        
        return true;
    }
};

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return 1;
    }
    
    auto experiment = std::make_shared<SimpleExperiment>(brain);
    if (!experiment->initialize()) {
        return 1;
    }
    
    ExperimentResults results = experiment->run(10, 50);
    
    std::cout << "Experiment completed!" << std::endl;
    std::cout << "Accuracy: " << results.overallAccuracy << std::endl;
    std::cout << "Performance: " << results.averagePerformance << std::endl;
    
    return 0;
}
```

### Example 2: Phase4Experiment

```cpp
// phase4_experiment_example.cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "experiments/Phase4Experiment.hpp"

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    Phase4Experiment experiment(brain);
    
    std::cout << "Running Phase 4 experiment..." << std::endl;
    Phase4Results results = experiment.run(brain.get(), 10);
    
    std::cout << "Phase 4 Experiment Results:" << std::endl;
    std::cout << "  Temporal Prediction Accuracy: " << results.predictionAccuracy << std::endl;
    std::cout << "  Working Memory Capacity: " << results.workingMemoryCapacity << std::endl;
    std::cout << "  Episodic Recall Accuracy: " << results.episodicRecallAccuracy << std::endl;
    std::cout << "  Concepts Formed: " << results.conceptsFormed << std::endl;
    std::cout << "  Attention Selectivity: " << results.attentionSelectivity << std::endl;
    std::cout << "  Planning Accuracy: " << results.planningAccuracy << std::endl;
    
    return 0;
}
```

### Example 3: Parallel Experiment

```cpp
// parallel_experiment_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"

class ParallelExperiment : public Experiment {
public:
    ParallelExperiment(std::shared_ptr<Brain> brain) : Experiment(brain) {}
    
    bool initialize() override {
        return true;
    }
    
    bool run(size_t num_trials, size_t steps_per_trial) override {
        std::cout << "Running parallel experiment..." << std::endl;
        
        // Run trials in parallel
        std::vector<std::future<bool>> futures;
        
        for (size_t trial = 0; trial < num_trials; ++trial) {
            futures.push_back(std::async(std::launch::async, [this, trial, steps_per_trial]() {
                return runTrial(trial, steps_per_trial);
            }));
        }
        
        // Wait for all trials to complete
        bool all_success = true;
        for (auto& future : futures) {
            if (!future.get()) {
                all_success = false;
            }
        }
        
        return all_success;
    }
    
private:
    bool runTrial(size_t trial, size_t steps_per_trial) {
        std::cout << "Running trial " << trial << std::endl;
        
        for (size_t step = 0; step < steps_per_trial; ++step) {
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "  Trial " << trial << ", Step " << step << std::endl;
            }
        }
        
        return true;
    }
};

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 2000);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto experiment = std::make_shared<ParallelExperiment>(brain);
    ExperimentRunner runner;
    runner.setMaxWorkers(4);
    
    ExperimentResults results = runner.runExperiment(experiment, 20, 100);
    
    std::cout << "Parallel experiment completed!" << std::endl;
    std::cout << "Success rate: " << (results.overallSuccess ? "100%" : "0%") << std::endl;
    std::cout << "Performance: " << results.averagePerformance << std::endl;
    
    return 0;
}
```

### Example 4: Metrics Collection

```cpp
// metrics_example.cpp
#include <iostream>
#include <fstream>
#include "brain/Brain.hpp"
#include "experiments/Metrics.hpp"

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    Metrics metrics;
    
    std::cout << "Collecting metrics..." << std::endl;
    
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Record performance metrics
        float accuracy = brain->getAverageFiringRate() / 20.0f; // Normalized
        metrics.recordMetric("accuracy", accuracy, "performance");
        
        float reward = (step % 10 == 0) ? 1.0f : 0.0f;
        metrics.recordMetric("reward", reward, "outcome");
        
        float firing = brain->getFiringNeuronCount();
        metrics.recordMetric("firing", firing, "neural_activity");
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": Metrics recorded" << std::endl;
        }
    }
    
    MetricsResults results = metrics.getResults();
    
    std::cout << "\nMetrics Results:" << std::endl;
    std::cout << "  Average accuracy: " << results.averageValue << std::endl;
    std::cout << "  Standard deviation: " << results.standardDeviation << std::endl;
    std::cout << "  Min value: " << results.minValue << std::endl;
    std::cout << "  Max value: " << results.maxValue << std::endl;
    
    // Save metrics to file
    std::ofstream file("metrics.json");
    if (file.is_open()) {
        // Write metrics to file (implementation depends on JSON library)
        file.close();
    }
    
    return 0;
}
```

## Python Experimental API

### Experiment Base Class

```python
import pynlm

class Experiment:
    def __init__(self, brain):
        self.brain = brain
        
    def initialize(self):
        """Initialize experiment"""
        pass
        
    def run(self, num_trials, steps_per_trial):
        """Run experiment"""
        pass
        
    def reset(self):
        """Reset experiment"""
        pass
        
    def get_results(self):
        """Get experiment results"""
        pass
```

### Python Experiment Examples

```python
import pynlm

# Create default brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Create and run simple experiment
def run_simple_experiment():
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return {
        "accuracy": brain.getAverageFiringRate() / 20.0,
        "reward": 1.0 if 99 % 10 == 0 else 0.0,
        "firing": brain.getFiringNeuronCount()
    }

results = run_simple_experiment()
print(f"Experiment results: {results}")
```

## Experimental API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Experiment | `Experiment` | `initialize()`, `run()`, `reset()`, `getResults()`, `isCompleted()` |
| Phase4 | `Phase4Experiment` | `run()`, `getResults()` |
| Phase6 | `Phase6IntegratedExperiment` | `verifyIntegration()`, `run()`, `getResults()` |
| Advanced | `AdvancedExperiment` | `run()`, `getResults()` |
| ExperimentRunner | `ExperimentRunner` | `setMaxWorkers()`, `setParallelExecution()`, `runExperiment()`, `setProgressCallback()` |
| Metrics | `Metrics` | `recordMetric()`, `getMetric()`, `getMetrics()`, `getResults()` |
| ParallelManager | `ParallelManager` | `setThreadCount()`, `enableSIMD()`, `enableMemoryPool()`, `parallelFor()` |
| Configuration Templates | `ConfigurationTemplates` | `createDefaultConfig()`, `createResearchConfig()`, `createPerformanceConfig()`, `getTemplate()` |

## Python Experimental API Reference Summary

| Class | Key Methods | Description |
|-------|-------------|-------------|
| `Experiment` | `initialize()`, `run()`, `reset()`, `get_results()` | Base experiment class |
| `ExperimentRunner` | `set_max_workers()`, `set_parallel_execution()`, `run_experiment()` | Experiment runner |
| `Metrics` | `record_metric()`, `get_metric()`, `get_results()` | Metrics collection |
| `ParallelManager` | `set_thread_count()`, `enable_simd()`, `enable_memory_pool()` | Parallel processing |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/basic_experiment.cpp`
2. **Explore Documentation**: Read `configuration.md` for experiment configuration details
3. **Run Demos**: Execute experiment-specific demo programs
4. **Create Experiments**: Design experimental NLM experiments
5. **Extend Experiments**: Add new experiment features to NLM

The Experimental Framework API provides comprehensive access to NLM's experimental capabilities. Whether you're interested in basic experiments, Phase 4 cognition, Phase 6 integration, advanced experiments, parallel processing, metrics collection, or configuration templates, the API provides the tools you need to create sophisticated experimental scenarios.

---

*Experimental Framework API Reference*
- **Location**: `docs/api/experimental_api.md`
- **Source**: Various experiment headers in `src/experiments/`
- **Python Bindings**: Experimental API in Python bindings
- **Documentation**: Complete experimental API reference with examples

*Ready to start using the Experimental API?*

1. **Read the examples**: Check `docs/examples/basic_experiment.cpp`
2. **Try Python API**: Use `configuration.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom experimental NLM experiments
5. **Contribute**: Share your experimental API usage with the community

The Experimental Framework API is essential for NLM's research capabilities. It provides everything you need to conduct comprehensive experiments and advance NLM research!

---

*Experimental Framework API enables NLM to:
• Provide base classes for experiments
• Support specialized experiment types
• Enable parallel processing
• Collect and analyze metrics
• Provide configuration templates
• Facilitate experiment management
• Support research and testing

Your experimental framework API research contributions will help advance neural system experimental capabilities!

---