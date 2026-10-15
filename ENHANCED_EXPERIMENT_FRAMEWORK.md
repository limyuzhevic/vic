# Enhanced Experiment Management Framework

This document describes the enhanced experiment management framework for the NLM project.

## Overview

The enhanced experiment management framework provides a comprehensive system for creating, managing, executing, and analyzing neural learning experiments. It builds upon the existing basic experiment system and provides advanced features including:

- Abstract experiment base class with concrete implementations
- Parameter sweep support and optimization
- Multi-objective optimization experiments
- Systematic hyperparameter tuning
- Experiment templates for common use cases
- Experiment queuing and scheduling system
- Parallel execution support
- Experiment checkpointing and resumption
- Parameter sweep automation
- Multi-experiment comparison tools
- Statistical analysis and visualization
- Experiment result comparison
- Performance metrics aggregation
- Experiment versioning system
- Full dependency tracking
- Configuration snapshotting
- Automatic experiment documentation

## Architecture

The framework consists of several key components:

### 1. Experiment Framework (src/experiments/experiments/)

#### Base Experiment
- `base_experiment.hpp` - Abstract `BaseExperiment` class extending the existing `Experiment` class
- `base_experiment.cpp` - Implementation of the base experiment class
- Provides experiment state management, parameter handling, progress tracking, and serialization

#### Enhanced Experiment Implementations
- `enhanced_experiments.hpp` - Header file declaring concrete experiment implementations
- `enhanced_experiments.cpp` - Implementations of plasticity experiments
- `plasticity_experiment.cpp` - Plasticity study experiments

### 2. Management System (src/experiments/management/)

#### Experiment Factory
- `experiment_factory.hpp` - Creates standardized experiment types
- `experiment_factory.cpp` - Implementation of the experiment factory

#### Parameter Sweep Manager
- `parameter_sweep.hpp` - Manages parameter variations and sweeps
- `parameter_sweep.cpp` - Implementation of parameter sweep functionality

#### Result Analyzer
- `result_analyzer.hpp` - Statistical analysis and visualization
- `result_analyzer.cpp` - Implementation of result analysis capabilities

#### Experiment Scheduler
- `experiment_scheduler.hpp` - Queues and executes experiments
- `experiment_scheduler.cpp` - Implementation of the experiment scheduler

#### Reproducibility Manager
- `reproducibility_manager.hpp` - Version control and tracking
- `reproducibility_manager.cpp` - Implementation of reproducibility features

### 3. Utilities (src/experiments/utils/)

#### Common Utilities
- `experiment_utils.hpp` - Common experiment utilities
- `experiment_utils.cpp` - Implementation of common utilities

#### Parameter Parsing
- `parameter_parser.hpp` - Parameter parsing from various sources
- `parameter_parser.cpp` - Implementation of parameter parsing

#### Result Export
- `result_exporter.hpp` - Result export in multiple formats
- `result_exporter.cpp` - Implementation of result export functionality

### 4. Legacy Compatibility (src/experiments/legacy/)

#### Legacy Experiments
- `legacy_experiment.hpp` - Legacy experiment compatibility
- `legacy_experiment.cpp` - Implementation of legacy experiment compatibility

#### Legacy Runner
- `legacy_runner.hpp` - Legacy runner compatibility
- `legacy_runner.cpp` - Implementation of legacy runner compatibility

### 5. Experiment Manager (src/experiments/experiments/)

- `experiment_manager.hpp` - Main experiment management interface
- `experiment_manager.cpp` - Implementation of the main experiment manager

### 6. CLI Integration (src/cli/)

#### Enhanced Experiment Runner
- `enhanced_experiment_runner.hpp` - Enhanced experiment runner for CLI
- `enhanced_experiment_runner.cpp` - Implementation of enhanced experiment runner

## Experiment Types

The framework provides standardized experiment templates for common use cases:

### 1. Plasticity Experiments
- Studies synaptic plasticity and learning mechanisms
- Supports Hebbian learning, STDP, and structural plasticity
- Measures synaptic weight changes over time

### 2. Learning Curve Experiments
- Measures learning performance across training curves
- Supports adaptive training schedules
- Evaluates convergence and stability of learning

### 3. Network Connectivity Experiments
- Tests information propagation through neural networks
- Measures signal transmission and connectivity strength
- Supports different input patterns and injection currents

### 4. Developmental Experiments
- Studies brain development and maturation processes
- Simulates synaptogenesis, pruning, and maturation
- Tracks neuron and synapse counts over time

### 5. Neuromodulation Experiments
- Studies reward, motivation, and neuromodulation systems
- Simulates reward prediction, curiosity, and novelty detection
- Measures neuromodulation effects on learning

## Key Features

### Experiment Lifecycle Management
- Experiment initialization, execution, checkpointing, and resumption
- Progress tracking and callbacks for real-time monitoring
- Thread-safe operations with proper locking mechanisms

### Parameter Management
- Flexible parameter system supporting various data types
- Parameter validation and type checking
- Support for parameter sweeps and optimization

### Results Analysis and Visualization
- Statistical analysis (mean, median, standard deviation, percentiles)
- Experiment comparison with statistical significance testing
- Multiple export formats (CSV, JSON, Markdown, HTML)
- Visualization data generation for plotting

### Parameter Sweeps and Optimization
- Automated parameter sweep execution
- Multi-objective optimization with Pareto front analysis
- Support for grid search, random search, and Bayesian optimization

### Reproducibility and Versioning
- Experiment snapshotting and restoration
- Version control for experiment history
- Git integration for tracking changes
- Dependency tracking for reproducibility

### Scheduling and Parallel Execution
- Experiment queuing and scheduling system
- Parallel execution with configurable worker threads
- Resource management and monitoring

### Integration with Existing System
- Maintains backward compatibility with existing experiments
- Integration with existing CLI commands
- Support for legacy experiment formats
- Extension of existing experiment types

## Usage Examples

### Creating and Running an Experiment

```cpp
#include "experiment_manager.hpp"

int main() {
    // Create experiment manager
    nlm::ExperimentManager manager;
    
    // Create a plasticity experiment
    auto experiment = manager.createExperiment("plasticity", "plasticity_study_1");
    
    // Set parameters
    experiment->setParameter("learning_rate", "0.01");
    experiment->setParameter("max_steps", "1000");
    experiment->setParameter("stdp_enabled", "true");
    
    // Run experiment
    if (experiment->initialize() && experiment->run()) {
        experiment->finalize();
        
        // Analyze results
        auto stats = manager.analyzeExperiment("plasticity_study_1");
        std::cout << "Mean performance: " << stats.mean << std::endl;
    }
    
    return 0;
}
```

### Running Parameter Sweeps

```cpp
// Run parameter sweep for plasticity experiments
std::map<std::string, std::vector<std::string>> parameterMatrix = {
    {"learning_rate", {"0.001", "0.01", "0.1"}},
    {"max_steps", {"500", "1000", "2000"}}
};

auto sweepResults = manager.runParameterSweep("plasticity", parameterMatrix);

// Find best parameters
auto bestResult = std::max_element(sweepResults.begin(), sweepResults.end(),
                                  [](const SweepResult& a, const SweepResult& b) {
                                      return a.metricValue < b.metricValue;
                                  });
```

### Experiment Templates

```cpp
// Create experiment from template
auto experiment = manager.createExperimentFromTemplate(
    "plasticity_default",
    {{ "learning_rate", "0.05" }, { "max_steps", "2000" } }
);
```

### CLI Integration

The enhanced framework integrates with the existing CLI:

```bash
# Run an experiment using the new framework
nlm experiment plasticity_study_1 1000

# Run a parameter sweep
nlm experiment sweep plasticity_default --learning_rate 0.001,0.01,0.1 --max_steps 500,1000,2000

# Analyze experiment results
nlm experiment analyze plasticity_study_1

# Compare experiments
nlm experiment compare plasticity_study_1 plasticity_study_2
```

## File Structure

```
src/experiments/
├── experiments/           # Enhanced experiment framework
│   ├── base_experiment.hpp    # Abstract experiment base
│   ├── enhanced_experiments.hpp  # Enhanced experiment implementations
│   ├── plasticity_experiment.hpp  # Plasticity studies
│   └── experiment_manager.hpp    # Main experiment manager
│   └── experiment_manager.cpp
├── management/            # Experiment management system
│   ├── experiment_factory.hpp  # Standardized experiment creation
│   ├── parameter_sweep.hpp   # Parameter sweep automation
│   ├── result_analyzer.hpp   # Results analysis
│   ├── experiment_scheduler.hpp  # Experiment scheduling
│   └── reproducibility_manager.hpp  # Version control
├── utils/                 # Supporting utilities
│   ├── experiment_utils.hpp  # Common experiment utilities
│   ├── parameter_parser.hpp # Parameter parsing
│   └── result_exporter.hpp  # Result export formats
└── legacy/               # Backward compatibility
    ├── legacy_experiment.hpp  # Existing experiment compatibility
    └── legacy_runner.hpp     # Existing runner compatibility
```

## Integration with Existing System

The enhanced experiment management framework integrates with the existing NLM system:

### 1. Backward Compatibility
- Existing experiments continue to work without modification
- Legacy experiment formats are supported
- Gradual migration path from old to new system

### 2. CLI Integration
- Extended `nlm experiment` command with new options
- Added commands for parameter sweeps, analysis, and comparison
- Support for both old and new experiment types

### 3. Python Bindings
- Python bindings for creating and managing experiments
- Python-based experiment scripting support
- Integration with experiment management from Python

## Building the Framework

The framework is built as part of the main NLM build system:

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

The enhanced experiment management library (`nlm_experiments`) is automatically linked with all executables.

## Testing

The framework includes comprehensive tests:

1. Unit tests for individual components
2. Integration tests for experiment management
3. Performance tests for parameter sweeps
4. Compatibility tests for legacy experiments

Run tests with:
```bash
./nlm_test
```

## Future Enhancements

The framework is designed to be extensible:

1. New experiment types can be added by extending `BaseExperiment`
2. New analysis methods can be added to `ResultAnalyzer`
3. New export formats can be added to `ResultExporter`
4. New scheduling algorithms can be implemented in `ExperimentScheduler`
5. New optimization methods can be added to `ParameterSweepManager`

## Conclusion

The enhanced experiment management framework provides a comprehensive solution for advanced experiment management in the NLM project. It maintains backward compatibility while providing powerful new features for experiment management, analysis, and optimization.
