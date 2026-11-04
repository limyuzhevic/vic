# Experiment Module

This module provides comprehensive experiment management and execution tools for the enhanced NLM Python API.

## Overview

The experiment module extends the basic NLM API with advanced experiment management capabilities, enabling systematic testing, parameter sweeps, multi-objective optimization, and comprehensive experiment tracking. It provides:

- **Experiment execution** with flexible parameter management
- **Parameter sweep** tools for systematic testing
- **Multi-objective optimization** for balancing multiple goals
- **Experiment tracking** with detailed results and analysis
- **Cross-validation** for robust testing
- **Bayesian optimization** for intelligent parameter search
- **Experiment comparison** and visualization
- **Automated reporting** for experiment results

## Key Features

### Experiment Execution

```python
from nlm_utils.experiment import AdvancedExperiment

# Create advanced experiment
experiment = AdvancedExperiment(
    name="learning_rate_experiment",
    description="Test brain learning with different learning rates",
    function=run_learning_rate_test,
    parameters={
        "learning_rate": {"type": "continuous", "range": [0.001, 0.1]},
        "neuron_count": {"type": "integer", "range": [100, 1000]},
        "plasticity_strength": {"type": "continuous", "range": [0.1, 2.0]}
    }
)

# Run experiment
results = experiment.run(max_iterations=50)
experiment.print_results(results)
experiment.export_results("learning_rate_results.json")
```

### Parameter Sweep

```python
from nlm_utils.experiment import ParameterSweeper

class LearningRateParameterSweeper(ParameterSweeper):
    """Parameter sweeper for learning rate experiments."""
    
    def __init__(self):
        self.parameter_grids = {
            "learning_rate_sweep": {
                "learning_rate": [0.001, 0.005, 0.01, 0.05, 0.1],
                "plasticity_enabled": [True, False]
            },
            "neuron_count_sweep": {
                "neuron_count": [100, 500, 1000, 2000],
                "synapse_density": [0.05, 0.1, 0.2]
            },
            "neuromodulation_sweep": {
                "dopamine_scale": [0.5, 1.0, 2.0],
                "curiosity_enable": [True, False],
                "novelty_enable": [True, False]
            }
        }
    
    def create_experiment_configs(self, sweep_name):
        """Create experiment configurations from parameter grid."""
        if sweep_name not in self.parameter_grids:
            raise ValueError(f"Unknown sweep: {sweep_name}")
        
        param_grid = self.parameter_grids[sweep_name]
        
        # Generate all combinations
        import itertools
        param_names = list(param_grid.keys())
        param_values = list(param_grid.values())
        
        combinations = list(itertools.product(*param_values))
        
        # Create experiment configurations
        experiments = []
        for combination in combinations:
            config = dict(zip(param_names, combination))
            
            experiments.append({
                "name": f"{sweep_name}_{len(experiments)}",
                "parameters": config,
                "description": f"Experiment with parameters: {config}"
            })
        
        return experiments
    
    def run_sweep(self, sweep_name, experiment_func, output_callback=None):
        """Run parameter sweep with progress reporting."""
        experiments = self.create_experiment_configs(sweep_name)
        total = len(experiments)
        
        print(f"Running {sweep_name} with {total} parameter combinations...")
        
        results = {}
        for i, experiment in enumerate(experiments):
            print(f"Running experiment {i+1}/{total}: {experiment['name']}")
            
            try:
                # Run experiment
                experiment_result = experiment_func(experiment["parameters"])
                results[experiment["name"]] = experiment_result
                
                # Call output callback if provided
                if output_callback:
                    output_callback(i, total, experiment["parameters"], experiment_result)
                    
            except Exception as e:
                print(f"Experiment {experiment['name']} failed: {e}")
                results[experiment["name"]] = {
                    "success": False,
                    "error": str(e),
                    "parameters": experiment["parameters"]
                }
        
        # Analyze sweep results
        analysis = self.analyze_sweep_results(results)
        
        return {
            "sweep_name": sweep_name,
            "results": results,
            "analysis": analysis,
            "total_experiments": total,
            "successful_experiments": sum(1 for r in results.values() if r.get("success", True))
        }
```

### Multi-Objective Experiment

```python
from nlm_utils.experiment import (
    MultiObjectiveExperiment,
    ParetoOptimizer,
    ObjectiveWeights
)

class MultiObjectiveLearningExperiment(MultiObjectiveExperiment):
    """Multi-objective experiment for brain learning optimization."""
    
    def __init__(self):
        objectives = [
            {"name": "learning_speed", "weight": 0.3, "higher_is_better": True},
            {"name": "memory_efficiency", "weight": 0.3, "higher_is_better": True},
            {"name": "stability", "weight": 0.2, "higher_is_better": True},
            {"name": "resource_efficiency", "weight": 0.2, "higher_is_better": True}
        ]
        
        super().__init__("multi_objective_learning", objectives)
    
    def run_trial(self, parameters):
        """Run a single trial with given parameters."""
        import pynlm
        
        # Create brain with parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", parameters["neuron_count"])
        config.set("plasticity.stdp.learning_rate", parameters["learning_rate"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Collect multi-objective metrics
        metrics = {
            "learning_speed": brain.getAverageFiringRate(),
            "memory_efficiency": brain.getMemoryActivity(),
            "stability": brain.getStabilityMetric(),
            "resource_efficiency": brain.getResourceEfficiency()
        }
        
        return metrics
    
    def evaluate_objectives(self, metrics, parameters):
        """Evaluate all objectives for given metrics."""
        return {
            "learning_speed": metrics["learning_speed"],
            "memory_efficiency": metrics["memory_efficiency"], 
            "stability": metrics["stability"],
            "resource_efficiency": metrics["resource_efficiency"]
        }

# Run multi-objective experiment
experiment = MultiObjectiveLearningExperiment()
results = experiment.run(max_iterations=50, population_size=20)

# Get Pareto front
pareto_front = experiment.get_pareto_front(results)
experiment.visualize_pareto_front(pareto_front)

# Export results
experiment.export_results("multi_objective_results.json")

# Print best solutions
experiment.print_best_solutions(results, top_k=5)
```

### Cross-Validation

```python
from nlm_utils.experiment import CrossValidationExperiment

class CrossValidatedLearningExperiment(CrossValidationExperiment):
    """Cross-validated experiment for robust testing."""
    
    def __init__(self, k_folds=5):
        super().__init__("cross_validated_learning", k_folds=k_folds)
    
    def run_trial(self, parameters, fold=None):
        """Run a single trial with given parameters."""
        import pynlm
        
        # Create brain with parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", parameters["neuron_count"])
        config.set("plasticity.stdp.learning_rate", parameters["learning_rate"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Collect metrics
        return {
            "performance": brain.getAverageFiringRate(),
            "stability": brain.getStabilityMetric(),
            "efficiency": brain.getResourceEfficiency()
        }

# Run cross-validated experiment
experiment = CrossValidatedLearningExperiment(k_folds=5)
results = experiment.run()

# Get cross-validation results
cv_results = experiment.get_cross_validation_results()
print(f"Cross-validation accuracy: {cv_results['accuracy']:.2f}")
print(f"Standard deviation: {cv_results['std_dev']:.4f}")
print(f"Performance confidence interval: {cv_results['confidence_interval']}")
```

### Bayesian Optimization

```python
from nlm_utils.experiment import BayesianOptimizationExperiment

class BayesianOptimizedLearningExperiment(BayesianOptimizationExperiment):
    """Bayesian-optimized experiment for intelligent parameter search."""
    
    def __init__(self):
        super().__init__("bayesian_optimized_learning", n_initial=10, n_optimization=50)
    
    def run_trial(self, parameters):
        """Run a single trial with given parameters."""
        import pynlm
        
        # Create brain with parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", parameters["neuron_count"])
        config.set("plasticity.stdp.learning_rate", parameters["learning_rate"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Collect performance metric
        return {
            "performance": brain.getAverageFiringRate(),
            "stability": brain.getStabilityMetric()
        }

# Run Bayesian-optimized experiment
experiment = BayesianOptimizedLearningExperiment()
results = experiment.run()

# Get optimal parameters
optimal_params = experiment.get_optimal_parameters()
print(f"Optimal parameters: {optimal_params}")
print(f"Expected performance: {experiment.get_expected_performance()}")
```

## Integration Examples

### Comprehensive Experiment Management

```python
import pynlm
from nlm_utils import (
    AdvancedExperiment,
    ExperimentSuiteAdvanced,
    ParameterSweeper,
    AdvancedBrainSession
)

def run_comprehensive_experiment_suite():
    """Run comprehensive experiment suite with multiple strategies."""
    
    # Create experiment suite
    experiment_suite = ExperimentSuiteAdvanced("comprehensive_experiment_suite")
    
    # Define experiments
    experiments = [
        {
            "name": "learning_rate_experiment",
            "parameters": {"learning_rate": [0.001, 0.01, 0.1]},
            "func": run_learning_rate_experiment
        },
        {
            "name": "neuron_count_experiment",
            "parameters": {"neuron_count": [500, 1000, 2000]},
            "func": run_neuron_count_experiment
        },
        {
            "name": "plasticity_experiment",
            "parameters": {"plasticity_strength": [0.1, 0.5, 1.0]},
            "func": run_plasticity_experiment
        }
    ]
    
    # Add experiments to suite
    for exp in experiments:
        experiment_suite.add_experiment(
            exp["name"], 
            exp["parameters"], 
            exp["func"]
        )
    
    # Run experiment suite
    results = experiment_suite.run()
    
    # Export results
    experiment_suite.export_results("comprehensive_experiment_results.json")
    
    # Generate summary report
    summary = experiment_suite.generate_summary_report()
    print(f"Experiment Suite Summary: {summary}")
    
    return results
```

### Automated Experiment Reporting

```python
from nlm_utils.experiment import (
    AdvancedExperiment,
    ExperimentReporter
)

def run_automated_experiment_reporting():
    """Run experiment with automated reporting."""
    
    # Create experiment
    experiment = AdvancedExperiment(
        name="automated_reporting_experiment",
        description="Experiment with automated reporting",
        function=example_experiment_function,
        parameters={"learning_rate": [0.001, 0.01, 0.1]}
    )
    
    # Create reporter
    reporter = ExperimentReporter("experiment_reporter")
    
    # Run experiment
    results = experiment.run()
    
    # Generate reports
    html_report = reporter.generate_html_report(results)
    json_report = reporter.generate_json_report(results)
    latex_report = reporter.generate_latex_report(results)
    
    # Export reports
    reporter.save_report(html_report, "experiment_report.html")
    reporter.save_report(json_report, "experiment_report.json")
    reporter.save_report(latex_report, "experiment_report.tex")
    
    return {
        'results': results,
        'reports': {
            'html': html_report,
            'json': json_report,
            'latex': latex_report
        }
    }
```

## Best Practices

### 1. Design Experiments Carefully

Design experiments with clear objectives and appropriate parameter ranges:

```python
# Design experiment with clear objectives
experiment = AdvancedExperiment(
    name="learning_experiment",
    description="Test brain learning with different configurations",
    function=learning_experiment_function,
    parameters={
        "learning_rate": {"type": "continuous", "range": [0.001, 0.1]},
        "neuron_count": {"type": "integer", "range": [100, 1000]},
        "plasticity_enabled": {"type": "boolean", "default": True}
    },
    objectives=[
        {"name": "learning_speed", "weight": 0.4, "higher_is_better": True},
        {"name": "memory_efficiency", "weight": 0.3, "higher_is_better": True},
        {"name": "stability", "weight": 0.3, "higher_is_better": True}
    ]
)
```

### 2. Use Parameter Sweeps for Systematic Testing

Use parameter sweeps for systematic exploration:

```python
# Create parameter sweeper
parameter_sweeper = ParameterSweeper()

# Define parameter grids
parameter_grids = {
    "learning_rate_sweep": {
        "learning_rate": [0.001, 0.005, 0.01, 0.05, 0.1],
        "plasticity_enabled": [True, False]
    },
    "neuron_count_sweep": {
        "neuron_count": [100, 500, 1000, 2000],
        "synapse_density": [0.05, 0.1, 0.2]
    }
}

# Run parameter sweeps
for sweep_name in parameter_grids.keys():
    results = parameter_sweeper.run_sweep(sweep_name, experiment_function)
    analyze_sweep_results(results)
```

### 3. Implement Multi-Objective Optimization

Implement multi-objective optimization for balanced results:

```python
# Create multi-objective experiment
experiment = MultiObjectiveExperiment(
    name="multi_objective_optimization",
    objectives=[
        {"name": "learning_speed", "weight": 0.4, "higher_is_better": True},
        {"name": "memory_efficiency", "weight": 0.3, "higher_is_better": True},
        {"name": "stability", "weight": 0.3, "higher_is_better": True}
    ]
)

# Run multi-objective experiment
results = experiment.run(max_iterations=50)

# Analyze Pareto front
pareto_front = experiment.get_pareto_front(results)
print("Pareto front analysis:")
for solution in pareto_front:
    print(f"  Parameters: {solution['parameters']}")
    print(f"  Objectives: {solution['objectives']}")
```

### 4. Use Cross-Validation for Robust Results

Use cross-validation for robust and reliable results:

```python
# Create cross-validated experiment
experiment = CrossValidationExperiment(
    name="cross_validated_learning",
    k_folds=5,
    objective_function=learning_performance
)

# Run cross-validation
results = experiment.run()

# Analyze cross-validation results
cv_results = experiment.get_cross_validation_results()
print(f"Cross-validation accuracy: {cv_results['accuracy']:.2f}")
print(f"Performance standard deviation: {cv_results['std_dev']:.4f}")
```

### 5. Automate Experiment Reporting

Automate experiment reporting for better documentation:

```python
# Create experiment reporter
reporter = ExperimentReporter("experiment_reporter")

# Run experiment
experiment = AdvancedExperiment(
    name="reporting_experiment",
    description="Experiment with automated reporting",
    function=example_experiment,
    parameters={"learning_rate": [0.001, 0.01, 0.1]}
)

results = experiment.run()

# Generate comprehensive reports
reports = reporter.generate_all_reports(results)
reporter.export_all_reports(reports, "experiment_results")
```

## Performance Considerations

### Experiment Overhead

- Experiment execution has overhead from parameter processing
- Configure experiment complexity based on computational resources
- Use efficient experiment functions for long-running simulations

### Parameter Sweep Performance

- Parameter sweeps can be computationally expensive
- Configure sweep parameters based on available computational resources
- Use parallel execution for large parameter sweeps

### Multi-Objective Optimization

- Multi-objective optimization requires multiple function evaluations
- Configure optimization iterations based on available computational resources
- Use efficient optimization algorithms for large search spaces

## Migration Guide

### From Basic Experiment to Enhanced Experiment

```python
# Basic experiment
def basic_learning_experiment(params):
    # Create brain with parameters
    config = create_default_config()
    config.set("brain.neuron_count", params["neuron_count"])
    config.set("plasticity.stdp.learning_rate", params["learning_rate"])
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Return metrics
    return {
        "performance": brain.getAverageFiringRate(),
        "stability": brain.getStabilityMetric()
    }

# Enhanced experiment
experiment = AdvancedExperiment(
    name="basic_learning_experiment",
    description="Basic learning experiment with enhanced features",
    function=basic_learning_experiment,
    parameters={
        "neuron_count": [100, 500, 1000],
        "learning_rate": [0.001, 0.01, 0.1]
    }
)
```

### Adding Parameter Sweeps

```python
# Basic experiment
def learning_experiment(params):
    # Run experiment with given parameters
    brain = create_brain_with_params(params)
    return run_simulation_and_collect_metrics(brain)

# Enhanced experiment with parameter sweeps
from nlm_utils import ParameterSweeper

# Create parameter sweeper
parameter_sweeper = ParameterSweeper()

# Run parameter sweep
results = parameter_sweeper.run_sweep(
    "learning_experiment",
    learning_experiment,
    experiment_function=learning_experiment
)
```

### Adding Multi-Objective Optimization

```python
# Basic experiment
experiment = AdvancedExperiment(
    name="basic_experiment",
    description="Basic experiment",
    function=example_function,
    parameters={"learning_rate": [0.001, 0.01]}
)

# Enhanced experiment with multi-objective optimization
from nlm_utils import MultiObjectiveExperiment

experiment = MultiObjectiveExperiment(
    name="multi_objective_experiment",
    objectives=[
        {"name": "learning_speed", "weight": 0.5, "higher_is_better": True},
        {"name": "memory_efficiency", "weight": 0.5, "higher_is_better": True}
    ]
)
```

## Conclusion

The experiment module provides comprehensive tools for systematic testing and optimization in NLM applications. It enables:

- **Systematic parameter exploration** with parameter sweeps
- **Multi-objective optimization** for balanced results
- **Cross-validation** for robust testing
- **Bayesian optimization** for intelligent parameter search
- **Comprehensive experiment tracking** with detailed results
- **Automated reporting** for better documentation
- **Experiment comparison** and visualization

These enhancements make NLM applications more testable and optimizable while maintaining backward compatibility.

For more advanced experiment management features, see the other utility modules in the nlm_utils package.
