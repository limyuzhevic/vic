# Example 4: Configuration Management and Experiment Tracking
# ================================================

## Overview

This example demonstrates how to manage configurations, run experiments, and track results systematically. It covers experimental design, parameter management, systematic execution, and comprehensive results analysis.

## Learning Objectives

- Understand experimental design and parameter management
- Learn how to create and run systematic experiments
- Experience experiment tracking and results analysis
- Learn how to organize experimental data and results
- Understand parameter sweeps and factorial experiments
- Master NLM experimental workflow and management

## Code Example

```python
#!/usr/bin/env python3
"""
Example 4: Configuration Management and Experiment Tracking
===========================================================

This example demonstrates how to manage configurations, run experiments,
and track results systematically.
"""

import pynlm
import json
import os
import time
import argparse
from pathlib import Path

def create_experiment_config(base_config, modifications, experiment_name):
    """Create a new configuration based on a base configuration with modifications."""
    # Load base configuration
    with open(base_config, 'r') as f:
        config = json.load(f)
    
    # Apply modifications
    for key, value in modifications.items():
        config[key] = value
    
    # Add experiment metadata
    config["experiment_name"] = experiment_name
    config["creation_time"] = time.time()
    config["description"] = f"Experiment {experiment_name}"
    
    return config

def run_experiment(config, experiment_name, output_dir):
    """Run an experiment with the given configuration."""
    print(f"Running experiment: {experiment_name}")
    
    # Create brain from config
    pynlm_config = pynlm.createDefaultConfig()
    
    # Apply config parameters to pynlm config
    if "neuron_count" in config:
        pynlm_config.set("brain.neuron_count", config["neuron_count"])
    if "plasticity_learning_rate" in config:
        pynlm_config.set("plasticity.stdp.learning_rate", config["plasticity_learning_rate"])
    
    brain = pynlm.createBrain(pynlm_config)
    brain.initialize()
    
    # Run simulation
    steps = config.get("max_simulation_steps", 500)
    for step in range(steps):
        brain.step(step)
        
        # Log progress
        if step % 100 == 0:
            print(f"  Step {step}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Save results
    results = {
        "experiment_name": experiment_name,
        "config": config,
        "final_neuron_count": brain.getTotalNeuronCount(),
        "final_synapse_count": brain.getTotalSynapseCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "simulation_steps": steps,
        "completion_time": time.time()
    }
    
    # Save results file
    results_file = Path(output_dir) / f"{experiment_name}_results.json"
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"  Experiment completed. Results saved to: {results_file}")
    return results

def main():
    """Main experiment management function."""
    print("=== Configuration Management and Experiment Tracking ===")
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Manage experiments and configurations")
    parser.add_argument('--base-config', default='configs/default.cfg',
                       help='Base configuration file')
    parser.add_argument('--name', required=True, help='Experiment name')
    parser.add_argument('--output-dir', default='./experiments',
                       help='Output directory for experiment data')
    
    args = parser.parse_args()
    
    # Create output directory
    Path(args.output_dir).mkdir(exist_ok=True)
    
    # Define experiment parameters
    experiment_params = {
        "plasticity_learning_rate": 0.001,
        "connection_probability": 0.1,
        "development_synaptogenesis_rate": 0.001
    }
    
    # Create experiment configuration
    config = create_experiment_config(
        args.base_config, 
        experiment_params, 
        args.name
    )
    
    # Save configuration
    config_file = Path(args.output_dir) / f"{args.name}_config.json"
    with open(config_file, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Configuration saved to: {config_file}")
    print(f"Experiment parameters: {experiment_params}")
    
    # Run experiment
    results = run_experiment(config, args.name, args.output_dir)
    
    print(f"\nExperiment Summary:")
    print(f"  Name: {results['experiment_name']}")
    print(f"  Final neurons: {results['final_neuron_count']}")
    print(f"  Final synapses: {results['final_synapse_count']}")
    print(f"  Final firing rate: {results['final_firing_rate']:.3f}")
    print(f"  Steps simulated: {results['simulation_steps']}")
    
    # List all experiment files
    print(f"\nExperiment files in {args.output_dir}:")
    for file in Path(args.output_dir).glob("*"):
        if file.is_file():
            print(f"  {file.name}")

if __name__ == "__main__":
    main()
```

## Key Concepts

### 1. Experiment Management

```python
def experiment_manager(base_config, experiment_configs):
    """Manage multiple experiments systematically."""
    
    experiments = []
    
    for exp_config in experiment_configs:
        print(f"Processing experiment: {exp_config['name']}")
        
        # Create experiment configuration
        config = create_experiment_config(
            base_config,
            exp_config['parameters'],
            exp_config['name']
        )
        
        # Create output directory
        output_dir = Path(exp_config.get('output_dir', './experiments'))
        output_dir.mkdir(exist_ok=True)
        
        # Run experiment
        results = run_experiment(config, exp_config['name'], str(output_dir))
        
        experiments.append({
            'name': exp_config['name'],
            'config': config,
            'results': results,
            'output_dir': str(output_dir)
        })
    
    return experiments
```

### 2. Parameter Sweeps

```python
def parameter_sweep(base_config, parameter_grid, output_dir):
    """Run parameter sweep experiment."""
    print(f"Running parameter sweep in {output_dir}")
    
    # Create output directory
    Path(output_dir).mkdir(exist_ok=True)
    
    # Generate all parameter combinations
    import itertools
    
    param_names = list(parameter_grid.keys())
    param_values = list(parameter_grid.values())
    
    # Generate all combinations
    param_combinations = list(itertools.product(*param_values))
    
    results = []
    
    for i, combination in enumerate(param_combinations):
        params = dict(zip(param_names, combination))
        
        print(f"  Testing parameter set {i+1}: {params}")
        
        # Create experiment configuration
        config = create_experiment_config(
            base_config,
            params,
            f"param_test_{i+1}"
        )
        
        # Run experiment
        exp_results = run_experiment(config, f"param_test_{i+1}", output_dir)
        
        results.append({
            'parameters': params,
            'results': exp_results
        })
        
        # Save individual result
        with open(f"{output_dir}/result_{i+1}.json", 'w') as f:
            json.dump(results[-1], f, indent=2)
    
    # Save summary
    summary = {
        'total_tests': len(param_combinations),
        'successful_tests': len([r for r in results if r['results']['success']]),
        'results': results
    }
    
    with open(f"{output_dir}/parameter_sweep_summary.json", 'w') as f:
        json.dump(summary, f, indent=2)
    
    return results
```

### 3. Experiment Tracking

```python
class ExperimentTracker:
    """Track and manage experimental results."""
    
    def- __init__(self, tracking_dir="experiment_tracker/"):
        self.tracking_dir = Path(tracking_dir)
        self.tracking_dir.mkdir(exist_ok=True)
        
        self.experiments_file = self.tracking_dir / "experiments.json"
        self.results_file = self.tracking_dir / "results.json"
        
        # Load existing data
        self.experiments = self.load_experiments()
        self.results = self.load_results()
    
    def add_experiment(self, exp_name, config, parameters):
        """Add a new experiment to the tracker."""
        experiment = {
            'name': exp_name,
            'config': config,
            'parameters': parameters,
            'creation_time': time.time(),
            'status': 'pending'
        }
        
        self.experiments[exp_name] = experiment
        self.save_experiments()
        
        return experiment
    
    def update_experiment_result(self, exp_name, results):
        """Update experiment with results."""
        if exp_name in self.experiments:
            self.experiments[exp_name]['results'] = results
            self.experiments[exp_name]['status'] = 'completed'
            self.experiments[exp_name]['completion_time'] = time.time()
            
            # Add to results index
            self.results[results['experiment_name']] = results
            self.save_results()
            self.save_experiments()
        
        return results
    
    def load_experiments(self):
        """Load experiment list from file."""
        if self.experiments_file.exists():
            with open(self.experiments_file, 'r') as f:
                return json.load(f)
        return {}
    
    def load_results(self):
        """Load results from file."""
        if self.results_file.exists():
            with open(self.results_file, 'r') as f:
                return json.load(f)
        return {}
    
    def save_experiments(self):
        """Save experiments to file."""
        with open(self.experiments_file, 'w') as f:
            json.dump(self.experiments, f, indent=2)
    
    def save_results(self):
        """Save results to file."""
        with open(self.results_file, 'w') as f:
            json.dump(self.results, f, indent=2)
    
    def get_experiment_summary(self, exp_name):
        """Get summary of a specific experiment."""
        if exp_name in self.experiments:
            return self.experiments[exp_name]
        return None
    
    def list_experiments(self):
        """List all experiments."""
        return list(self.experiments.keys())
    
    def get_completed_experiments(self):
        """Get list of completed experiments."""
        return [name for name, exp in self.experiments.items() if exp.get('status') == 'completed']
```

## Expected Output

### Sample Script Output

```
=== Configuration Management and Experiment Tracking ===
Configuration saved to: ./experiments/test_exp_config.json
Experiment parameters: {'plasticity_learning_rate': 0.001, 'connection_probability': 0.1, 'development_synaptogenesis_rate': 0.001}
Running experiment: test_exp
  Step 0/500: 125 firing neurons
  Step 100/500: 138 firing neurons
  Step 200/500: 142 firing neurons
  Step 300/500: 145 firing neurons
  Step 400/500: 148 firing neurons
  Step 500/500: 152 firing neurons
  Experiment completed. Results saved to: ./experiments/test_exp_results.json

Experiment Summary:
  Name: test_exp
  Final neurons: 1000
  Final synapses: 100000
  Final firing rate: 145.2
  Steps simulated: 500

Experiment files in ./experiments:
  test_exp_config.json
  test_exp_results.json
```

### Experiment Management Output

```
Experiment Management System
==========================

Available Experiments:
1. baseline_test
2. parameter_sweep
3. development_study
4. comparative_analysis

Experiment Status:
- baseline_test: Completed
- parameter_sweep: Completed  
- development_study: Completed
- comparative_analysis: Pending

Total Experiments: 4
Completed: 3
Pending: 1

Last Completed: comparative_analysis (2024-01-15 10:30:45)
```

## Practical Applications

### 1. Research Studies

```python
def nlm_research_workflow():
    """Complete research workflow using experiment management."""
    
    # Initialize experiment tracker
    tracker = ExperimentTracker()
    
    # Define research questions
    research_questions = [
        {
            'name': 'neuron_count_impact',
            'parameters': {'neuron_count': [500, 1000, 2000]},
            'description': 'Study impact of neuron count on performance'
        },
        {
            'name': 'plasticity_effects',
            'parameters': {'plasticity_learning_rate': [0.001, 0.01, 0.1]},
            'description': 'Study effects of plasticity on learning'
        },
        {
            'name': 'environment_impact',
            'parameters': {'environment_width': [10, 20, 30]},
            'description': 'Study environmental impact on development'
        }
    ]
    
    # Run research studies
    for question in research_questions:
        print(f"\nRunning research: {question['name']}")
        print(f"Question: {question['description']}")
        
        # Create parameter combinations
        param_combinations = generate_parameter_combinations(question['parameters'])
        
        # Add experiments to tracker
        for i, params in enumerate(param_combinations):
            exp_name = f"{question['name']}_param_{i+1}"
            
            config = create_experiment_config(
                'configs/default.cfg',
                params,
                exp_name
            )
            
            tracker.add_experiment(exp_name, config, params)
            
            # Run experiment
            results = run_experiment(config, exp_name, f"results/{question['name']}")
            
            tracker.update_experiment_result(exp_name, results)
        
        print(f"Completed {question['name']} research study")
    
    return tracker
```

### 2. Parameter Optimization

```python
def parameter_optimization_pipeline():
    """Complete parameter optimization pipeline."""
    
    print("=== Parameter Optimization Pipeline ===")
    
    # Define parameter space to explore
    parameter_space = {
        'neuron_count': [500, 1000, 1500, 2000],
        'plasticity_learning_rate': [0.001, 0.005, 0.01],
        'environment_width': [10, 15, 20],
        'development_initial_plasticity': [0.5, 1.0, 1.5]
    }
    
    # Initialize experiment tracker
    tracker = ExperimentTracker()
    
    # Create experiment for parameter sweep
    sweep_params = generate_all_combinations(parameter_space)
    
    for i, params in enumerate(sweep_params):
        exp_name = f"opt_test_{i+1}"
        
        config = create_experiment_config(
            'configs/default.cfg',
            params,
            exp_name
        )
        
        tracker.add_experiment(exp_name, config, params)
        
        # Run experiment
        results = run_experiment(config, exp_name, "optimization_results")
        
        tracker.update_experiment_result(exp_name, results)
        
        # Log progress
        if (i + 1) % 10 == 0:
            print(f"Completed {i + 1}/{len(sweep_params)} parameter tests")
    
    # Analyze results and identify optimal configuration
    optimal_config = analyze_optimization_results(tracker)
    
    print(f"\n=== Optimization Complete ===")
    print(f"Optimal configuration: {optimal_config}")
    print(f"Best performance: {optimal_config['performance']:.2f}")
    
    return tracker, optimal_config
```

### 3. Quality Assurance

```python
class ExperimentQualityAssurance:
    """Quality assurance for NLM experiments."""
    
    def __init__(self):
        self.quality_metrics = {}
    
    def validate_experiment_setup(self, config, experiment_name):
        """Validate experiment setup before execution."""
        issues = []
        warnings = []
        
        # Check configuration validity
        if not config:
            issues.append("Configuration is missing")
        
        # Check required parameters
        required_params = ['neuron_count', 'plasticity_learning_rate']
        for param in required_params:
            if param not in config:
                warnings.append(f"Parameter '{param}' not specified")
        
        # Validate parameter ranges
        if 'neuron_count' in config and config['neuron_count'] <= 0:
            issues.append("Neuron count must be positive")
        
        if 'plasticity_learning_rate' in config and config['plasticity_learning_rate'] < 0:
            issues.append("Plasticity learning rate must be non-negative")
        
        # Check simulation parameters
        if config.get('max_simulation_steps', 0) < 100:
            warnings.append("Simulation steps may be too low for meaningful results")
        
        return {
            'valid': len(issues) == 0,
            'issues': issues,
            'warnings': warnings
        }
    
    def validate_experiment_results(self, results):
        """Validate experiment results for quality."""
        quality_score = 0.0
        max_score = 10.0
        
        # Check result completeness
        required_fields = ['experiment_name', 'final_neuron_count', 'final_firing_rate']
        completeness = sum(1 for field in required_fields if field in results) / len(required_fields)
        quality_score += completeness * 3.0
        
        # Check result reasonableness
        firing_rate = results.get('final_firing_rate', 0)
        if 0 <= firing_rate <= 1000:
            quality_score += 2.0
        
        # Check simulation completion
        if results.get('simulation_steps', 0) > 0:
            quality_score += 2.0
        
        # Check neuron count
        neuron_count = results.get('final_neuron_count', 0)
        if 10 <= neuron_count <= 10000:
            quality_score += 1.5
        
        # Check synapse count
        synapse_count = results.get('final_synapse_count', 0)
        if synapse_count > 0:
            quality_score += 1.0
        
        return {
            'quality_score': quality_score,
            'max_score': max_score,
            'quality_grade': self.calculate_quality_grade(quality_score),
            'passed': quality_score >= 7.0
        }
    
    def calculate_quality_grade(self, score):
        """Calculate quality grade based on score."""
        if score >= 8.5:
            return 'A+'
        elif score >= 8.0:
            return 'A'
        elif score >= 7.0:
            return 'B'
        elif score >= 6.0:
            return 'C'
        else:
            return 'F'
```

## Comparison with Other Examples

### Example 4 vs Example 1

| Feature | Example 1 | Example 4 |
|---------|-----------|-----------|
| **Focus** | Basic functionality | Experiment management |
| **Complexity** | Low | Medium |
| **Key Concepts** | Brain creation, simulation | Experimental design, parameter management |
| **Use Case** | Learning NLM basics | Running systematic experiments |

### Example 4 vs Example 2

| Feature | Example 2 | Example 4 |
|---------|-----------|-----------|
| **Goal** | Demonstrate integration | Manage and track experiments |
| **Components** | Brain + Agent + World | Multiple experiment runs |
| **Focus** | Real-time behavior | Experimental workflow |
| **Management** | Single simulation | Multiple simulations |

### Example 4 vs Example 5

| Feature | Example 4 | Example 5 |
|---------|-----------|-----------|
| **Objective** | Experiment management | Performance optimization |
| **Metrics** | Experimental results | Performance metrics |
| **Approach** | Systematic testing | Systematic benchmarking |
| **Focus** | How to run experiments | How well system performs |

## Error Handling and Debugging

### Common Issues

1. **Configuration File Not Found**
   - **Problem**: Base configuration file is missing or invalid
   - **Solution**: Use valid configuration files or create default configuration
   - **Command**: Check config file exists with `ls configs/default.cfg`

2. **Experiment Directory Issues**
   - **Problem**: Output directory cannot be created
   - **Solution**: Check permissions and create directory manually
   - **Command**: `mkdir -p ./experiments` before running

3. **Parameter Validation Errors**
   - **Problem**: Invalid parameter values in experiment configuration
   - **Solution**: Validate parameters before running experiment
   - **Command**: Use validation tools or check parameter ranges

### Debugging Tools

```python
import pynlm
import json
import os
from pathlib import Path

def debug_experiment_manager():
    """Debug experiment manager with detailed logging."""
    print("=== Experiment Manager Debug ===")
    
    # Initialize experiment tracker
    from experiment_tracker import ExperimentTracker
    tracker = ExperimentTracker()
    
    # Test experiment creation
    test_config = {
        'neuron_count': 1000,
        'plasticity_learning_rate': 0.01,
        'environment_width': 10,
        'experiment_name': 'debug_test'
    }
    
    print(f"Creating experiment: debug_test")
    print(f"Configuration: {test_config}")
    
    # Validate configuration
    validation_result = validate_experiment_setup(test_config, 'debug_test')
    print(f"Validation result: {validation_result}")
    
    if validation_result['valid']:
        print("Configuration is valid")
        
        # Create experiment
        exp_name = 'debug_test'
        tracker.add_experiment(exp_name, test_config, test_config)
        
        # Run experiment
        results = run_experiment(test_config, exp_name, "debug_results")
        
        tracker.update_experiment_result(exp_name, results)
        
        # Debug results
        print(f"\nExperiment Results:")
        print(f"  Name: {results.get('experiment_name', 'N/A')}")
        print(f"  Neurons: {results.get('final_neuron_count', 'N/A')}")
        print(f"  Synapses: {results.get('final_synapse_count', 'N/A')}")
        print(f"  Firing Rate: {results.get('final_firing_rate', 'N/A'):.3f}")
        
        return True
    else:
        print("Configuration validation failed:")
        for issue in validation_result['issues']:
            print(f"  ERROR: {issue}")
        for warning in validation_result['warnings']:
            print(f"  WARNING: {warning}")
        
        return False
```

## Performance Considerations

### Experiment Efficiency

```python
def optimize_experiment_workflow(tracker):
    """Optimize experiment workflow for efficiency."""
    
    # Batch experiment execution
    def batch_execute_experiment_names(exp_names, base_dir):
        """Execute multiple experiments in batch."""
        results = {}
        
        for exp_name in exp_names:
            print(f"Processing experiment: {exp_name}")
            
            # Get experiment configuration
            experiment = tracker.get_experiment_summary(exp_name)
            if not experiment:
                print(f"  ERROR: Experiment {exp_name} not found")
                continue
            
            # Create output directory
            exp_output_dir = Path(base_dir) / exp_name
            exp_output_dir.mkdir(exist_ok=True)
            
            # Run experiment
            results[exp_name] = run_experiment(
                experiment['config'],
                exp_name,
                str(exp_output_dir)
            )
            
            # Update tracker
            tracker.update_experiment_result(exp_name, results[exp_name])
            
            print(f"  Completed: {exp_name}")
        
        return results
    
    # Execute batch
    experiment_list = tracker.list_experiments()
    batch_results = batch_execute_experiment_names(experiment_list, "batch_results")
    
    print(f"Batch execution complete: {len(batch_results)} experiments")
    return batch_results
```

### Resource Management

```python
def resource_monitoring_experiment_manager(tracker):
    """Monitor resources during experiment execution."""
    import psutil
    import time
    
    process = psutil.Process()
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    initial_cpu = psutil.cpu_percent(interval=None)
    
    start_time = time.time()
    
    # Execute experiments
    experiment_names = tracker.list_experiments()
    
    for exp_name in experiment_names:
        print(f"\nMonitoring experiment: {exp_name}")
        
        # Record resources before experiment
        before_memory = process.memory_info().rss / 1024 / 1024
        before_cpu = psutil.cpu_percent(interval=None)
        
        # Get experiment configuration
        experiment = tracker.get_experiment_summary(exp_name)
        if not experiment:
            continue
        
        # Run experiment
        results = run_experiment(
            experiment['config'],
            exp_name,
            f"monitored_results/{exp_name}"
        )
        
        # Record resources after experiment
        after_memory = process.memory_info().rss / 1024 / 1024
        after_cpu = psutil.cpu_percent(interval=None)
        
        # Calculate resource usage
        memory_change = after_memory - before_memory
        cpu_avg = (before_cpu + after_cpu) / 2
        
        print(f"  Memory usage: {before_memory:.1f} -> {after_memory:.1f} MB (Δ: {memory_change:+.1f} MB)")
        print(f"  CPU usage: {before_cpu:.1f}% -> {after_cpu:.1f}% (avg: {cpu_avg:.1f}%)")
        
        # Update tracker
        tracker.update_experiment_result(exp_name, results)
        
        # Sleep briefly between experiments
        time.sleep(0.1)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Calculate overall resource usage
    final_memory = process.memory_info().rss / 1024 / 1024
    final_cpu = psutil.cpu_percent(interval=None)
    
    print(f"\n=== Resource Usage Summary ===")
    print(f"Total execution time: {total_time:.2f} seconds")
    print(f"Memory change: {final_memory - initial_memory:+.1f} MB")
    print(f"Average CPU: {((initial_cpu + final_cpu) / 2):.1f}%")
    
    return tracker
```

## Extensions and Customization

### 1. Experiment Templates

```python
class ExperimentTemplateManager:
    """Manage experiment templates for reusable workflows."""
    
    def __init__(self, templates_dir="experiment_templates/"):
        self.templates_dir = Path(templates_dir)
        self.templates_dir.mkdir(exist_ok=True)
        
        self.templates_file = self.templates_dir / "templates.json"
        
        # Load existing templates
        self.templates = self.load_templates()
    
    def add_template(self, template_name, template_config, description):
        """Add a new experiment template."""
        template = {
            'name': template_name,
            'config': template_config,
            'description': description,
            'creation_time': time.time()
        }
        
        self.templates[template_name] = template
        self.save_templates()
        
        return template
    
    def load_templates(self):
        """Load experiment templates from file."""
        if self.templates_file.exists():
            with open(self.templates_file, 'r') as f:
                return json.load(f)
        return {}
    
    def save_templates(self):
        """Save templates to file."""
        with open(self.templates_file, 'w') as f:
            json.dump(self.templates, f, indent=2)
    
    def create_experiment_from_template(self, template_name, customizations):
        """Create experiment from template with customizations."""
        if template_name not in self.templates:
            raise ValueError(f"Template not found: {template_name}")
        
        template = self.templates[template_name]
        
        # Merge template config with customizations
        experiment_config = template['config'].copy()
        
        # Apply customizations
        for key, value in customizations.items():
            experiment_config[key] = value
        
        # Generate experiment name
        exp_name = f"{template_name}_{int(time.time())}"
        
        return exp_name, experiment_config
    
    def list_templates(self):
        """List all available templates."""
        return list(self.templates.keys())
    
    def get_template_description(self, template_name):
        """Get template description."""
        if template_name in self.templates:
            return self.templates[template_name]['description']
        return None
```

### 2. Automated Experiment Generation

```python
def generate_experiment_suite(base_config, experiment_definitions):
    """Generate automated experiment suite."""
    
    print("=== Generating Experiment Suite ===")
    
    # Initialize experiment tracker
    from experiment_tracker import ExperimentTracker
    tracker = ExperimentTracker()
    
    # Generate experiments
    generated_experiments = []
    
    for exp_def in experiment_definitions:
        exp_name = exp_def['name']
        param_space = exp_def['parameter_space']
        
        print(f"\nGenerating experiments for: {exp_name}")
        print(f"Parameter space: {param_space}")
        
        # Generate parameter combinations
        param_combinations = generate_parameter_combinations(param_space)
        
        # Create experiments
        for i, params in enumerate(param_combinations):
            # Create experiment name
            exp_instance_name = f"{exp_name}_param_{i+1}"
            
            # Create configuration
            config = create_experiment_config(
                base_config,
                params,
                exp_instance_name
            )
            
            # Add to tracker
            tracker.add_experiment(exp_instance_name, config, params)
            
            # Add to generated list
            generated_experiments.append({
                'name': exp_instance_name,
                'parameters': params,
                'definition': exp_def
            })
            
            print(f"  Generated: {exp_instance_name}")
    
    print(f"\nGenerated {len(generated_experiments)} experiments")
    print(f"Total parameter combinations: {sum(len(generate_parameter_combinations(ed['parameter_space'])) for ed in experiment_definitions)}")
    
    return tracker, generated_experiments
```

## Conclusion

Example 4 provides a comprehensive foundation for understanding experimental design and management in NLM. By working through this example, you can:

1. **Master Experimental Design**: Learn how to design and run systematic experiments
2. **Understand Parameter Management**: Experience parameter sweeps and factorial designs
3. **Develop Management Skills**: Learn how to track and manage experimental results
4. **Explore Extensions**: Create experiment templates and automated workflows
5. **Build Research Capability**: Develop skills for systematic NLM research

This example serves as a bridge between basic NLM usage and advanced experimental research, introducing key concepts and patterns used throughout scientific NLM applications.

---

*This example is part of the NLM (Neural Learning Machine) examples suite.
For additional examples, documentation, and support, visit the NLM project.*
