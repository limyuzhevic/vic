# NLM Command-Line Tools and Utilities

This document provides comprehensive guidance for using the NLM (Neural Learning Machine) command-line tools and utility functions.

## 1. Core Command-Line Tools

### 1.1 NLM Brain Inspector

The `nlm_inspector` tool provides comprehensive analysis and inspection of brain states and configurations.

**Usage:**
```bash
nlm_inspector [options] <command> <arguments>
```

**Commands:**

#### `inspect <brain_file>`
Inspect a brain checkpoint file and display detailed statistics:
```bash
nlm_inspector inspect brain_checkpoint.bin
nlm_inspector inspect --verbose brain_checkpoint.bin
```

**Options:**
- `--verbose`: Display detailed information including neuron distributions, synapse statistics, and developmental stage
- `--json`: Output results in JSON format for programmatic processing
- `--html`: Generate interactive HTML visualization

#### `compare <brain_file1> <brain_file2>`
Compare two brain states and identify differences:
```bash
nlm_inspector compare brain1.bin brain2.bin
nlm_inspector compare --detailed brain1.bin brain2.bin
```

**Options:**
- `--detailed`: Show comprehensive comparison including neural activity patterns
- `--similarity`: Calculate similarity scores between brains
- `--output <file>`: Save comparison results to file

#### `export <brain_file> <output_format>`
Export brain data in various formats:
```bash
nlm_inspector export brain.bin stats.json
nlm_inspector export brain.bin --format csv
nlm_inspector export brain.bin --format numpy
```

**Available formats:**
- `json`: JSON format with comprehensive brain metadata
- `csv`: Comma-separated values for data analysis
- `numpy`: NumPy arrays for scientific computing
- `brainml`: Custom BrainML format for model exchange

#### `plot <brain_file> [options]`
Generate visualizations of brain data:
```bash
nlm_inspector plot brain.bin --output brain_viz.png
nlm_inspector plot brain.bin --type neurons --output neurons.png
nlm_inspector plot brain.bin --type synapses --output synapses.png
```

**Options:**
- `--output <file>`: Specify output file
- `--type <type>`: Visualization type (neurons, synapses, activity, development)
- `--interactive`: Generate interactive HTML with zoom/pan capabilities
- `--dimensions <width>x<height>`: Set output image dimensions

### 1.2 NLM Experiment Runner

The `nlm_runner` tool automates experiment execution with various configuration options.

**Usage:**
```bash
nlm_runner [options] <command> <arguments>
```

**Commands:**

#### `run <config_file> <steps>`
Run a simulation with specified configuration:
```bash
nlm_runner run config.json 1000
nlm_runner run --save-interval 100 config.json 5000
```

**Options:**
- `--save-interval <steps>`: Save brain state at specified intervals
- `--output <file>`: Save experiment results to file
- `--log-level <level>`: Set logging verbosity (debug, info, warn, error)
- `--world-width <width>`: Override world width in config
- `--world-height <height>`: Override world height in config

#### `sweep <param> <values> <steps>`
Run parameter sweep for systematic exploration:
```bash
nlm_runner sweep --param "brain.neuron_count" 100 500 1000 500
nlm_runner sweep --param "plasticity.stdp.learning_rate" 0.001 0.01 0.1 1000
```

**Options:**
- `--output <prefix>`: Prefix for output files (results will be saved as prefix_param_value.dat)
- `--analysis`: Perform automatic analysis of sweep results
- `--visualize`: Generate comparison plots for all parameter combinations

#### `compare <config1> <config2> <steps>`
Compare two experimental configurations:
```bash
nlm_runner compare config_a.json config_b.json 2000
nlm_runner compare --metrics curiosity,novelty,reward config_a.json config_b.json 3000
```

**Options:**
- `--metrics <list>`: Specify which metrics to compare (comma-separated)
- `--output <file>`: Save comparison results
- `--statistical`: Perform statistical analysis of differences

#### `longitudinal <config_file> <duration>`
Run long-term developmental study:
```bash
nlm_runner longitudinal config.json 50000
nlm_runner longitudinal --save-interval 5000 --development-phases config.json 100000
```

**Options:**
- `--save-interval <steps>`: Save brain state at specified intervals
- `--development-phases`: Track and analyze developmental stage transitions
- `--output <prefix>`: Prefix for output files
- `--analyze-development`: Generate developmental trajectory analysis

### 1.3 NLM Configuration Wizard

The `nlm_config_wizard` tool provides interactive configuration creation and validation.

**Usage:**
```bash
nlm_config_wizard [options]
```

**Commands:**

#### `create [type] [options]`
Create a new configuration template:
```bash
nlm_config_wizard create foraging --difficulty hard --output foraging_config.json
nlm_config_wizard create development --stages infant,child,adult --output development_config.json
```

**Options:**
- `--difficulty <level>`: Set task difficulty (easy, medium, hard)
- `--output <file>`: Save configuration to file
- `--validate`: Validate configuration immediately

#### `validate <config_file>`
Validate a configuration file:
```bash
nlm_config_wizard validate config.json
nlm_config_wizard validate --strict config.json
```

**Options:**
- `--strict`: Apply strict validation rules
- `--fix`: Automatically fix common issues
- `--report <file>`: Save validation report to file

#### `optimize <config_file> [parameters]`
Optimize configuration for specific performance criteria:
```bash
nlm_config_wizard optimize config.json --learning-rate 0.01 --exploration 0.5
nlm_config_wizard optimize config.json --memory-capacity 1000 --novelty-sensitivity 0.8
```

**Options:**
- `--learning-rate <value>`: Optimize for specific learning rate
- `--exploration <value>`: Optimize for exploration behavior
- `--memory-capacity <value>`: Optimize working memory capacity
- `--novelty-sensitivity <value>`: Optimize novelty detection sensitivity

### 1.4 NLM Neural Network Visualizer

The `nlm_visualizer` tool generates various types of neural network visualizations.

**Usage:**
```bash
nlm_visualizer [options] <command> <arguments>
```

**Commands:**

#### `brain <brain_file> <output>`
Visualize brain structure and activity:
```bash
nlm_visualizer brain brain.bin visualization.html
nlm_visualizer brain --type 3d brain.bin visualization.html
```

**Options:**
- `--type <type>`: Visualization type (2d, 3d, interactive, static)
- `--neurons`: Highlight neuron populations
- `--synapses`: Show synaptic connections
- `--activity`: Display activity patterns
- `--development`: Show developmental progression

#### `memory <memory_file> <output>`
Visualize memory systems and patterns:
```bash
nlm_visualizer memory working_memory.dat memory_viz.html
nlm_visualizer memory --type episodic episodic_memory.dat episodic_viz.html
```

**Options:**
- `--type <type>`: Memory type (working, episodic, semantic, associative)
- `--patterns`: Show stored memory patterns
- `--trajectories`: Display memory usage trajectories
- `--consolidation`: Visualize consolidation processes

## 2. Utility Functions

### 2.1 Brain Comparison Utilities

#### `compare_brains(brain1_file, brain2_file, output_file=None)`
Compare two brain states and generate similarity report:
```python
from nlm.utils import compare_brains

results = compare_brains("brain1.bin", "brain2.bin", "comparison_report.json")

print(f"Similarity score: {results['similarity']:.3f}")
print(f"Total neurons difference: {results['neuron_count_diff']}")
print(f"Average firing rate difference: {results['firing_rate_diff']:.3f}")
```

#### `brain_statistics(brain_file, output_file=None)`
Extract and analyze brain statistics:
```python
from nlm.utils import brain_statistics

stats = brain_statistics("brain.bin", "brain_stats.json")

print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Average firing rate: {stats['average_firing_rate']:.3f}")
print(f"Development stage: {stats['development_stage']}")
print(f"Dopamine level: {stats['dopamine_level']:.3f}")
```

### 2.2 Configuration Utilities

#### `generate_config_template(config_type, output_file=None)`
Generate configuration template for specific experiment types:
```python
from nlm.utils import generate_config_template

# Create foraging task template
foraging_config = generate_config_template("foraging", "foraging_template.json")

# Create development template
development_config = generate_config_template("development", "development_template.json")

# Customize configuration
foraging_config.set("brain.neuron_count", 2000)
foraging_config.set("plasticity.stdp.enable", True)
```

#### `validate_config(config, strict=False)`
Validate configuration and return issues:
```python
from nlm.utils import validate_config

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)

issues = validate_config(config, strict=True)

if issues:
    print("Configuration issues found:")
    for issue in issues:
        print(f"  - {issue}")
else:
    print("Configuration is valid!")
```

### 2.3 Analysis Utilities

#### `calculate_brain_metrics(brain_file)`
Calculate comprehensive brain performance metrics:
```python
from nlm.utils import calculate_brain_metrics

metrics = calculate_brain_metrics("brain.bin")

print(f"Learning efficiency: {metrics['learning_efficiency']:.3f}")
print(f"Memory retention: {metrics['memory_retention']:.3f}")
print(f"Prediction accuracy: {metrics['prediction_accuracy']:.3f}")
print(f"Exploration rate: {metrics['exploration_rate']:.3f}")
print(f"Novelty sensitivity: {metrics['novelty_sensitivity']:.3f}")
```

#### `analyze_development_trend(brain_file, stages)`
Analyze developmental progression through specified stages:
```python
from nlm.utils import analyze_development_trend

stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
trend = analyze_development_trend("brain.bin", stages)

for stage_data in trend:
    print(f"Stage: {stage_data['stage']}")
    print(f"  Performance: {stage_data['performance']:.3f}")
    print(f"  Stability: {stage_data['stability']:.3f}")
    print(f"  Transition: {stage_data['transition']:.3f}")
```

### 2.4 Error Handling Utilities

#### `safe_brain_operation(operation, config, max_retries=3)`
Perform brain operation with automatic error recovery:
```python
from nlm.utils import safe_brain_operation

def create_and_run_brain(config):
    def operation():
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
            
        return brain.getTotalSpikeCount()
    
    try:
        result = safe_brain_operation(operation, config, max_retries=5)
        print(f"Operation successful! Result: {result}")
        return True
    except Exception as e:
        print(f"Operation failed after {max_retries} retries: {e}")
        return False
```

#### `validate_brain_integrity(brain_file)`
Validate brain file integrity and structure:
```python
from nlm.utils import validate_brain_integrity

validation_result = validate_brain_integrity("brain.bin")

if validation_result['valid']:
    print("Brain file is valid!")
    print(f"Neurons: {validation_result['neuron_count']}")
    print(f"Synapses: {validation_result['synapse_count']}")
    print(f"Development stage: {validation_result['development_stage']}")
else:
    print("Brain file validation failed:")
    for error in validation_result['errors']:
        print(f"  - {error}")
```

## 3. Advanced Scripting

### 3.1 Batch Processing

#### `batch_experiments(experiment_configs, output_prefix, parallel=False)`
Run multiple experiments in batch mode:
```python
from nlm.batch import batch_experiments

experiments = [
    {"config": "config1.json", "steps": 1000},
    {"config": "config2.json", "steps": 2000},
    {"config": "config3.json", "steps": 1500},
]

results = batch_experiments(experiments, "batch_results", parallel=True)

for exp_name, exp_result in results.items():
    print(f"Experiment {exp_name}:")
    print(f"  Final fitness: {exp_result['fitness']:.3f}")
    print(f"  Steps completed: {exp_result['steps_completed']}")
    print(f"  Time elapsed: {exp_result['time_elapsed']:.2f}s")
```

### 3.2 Hyperparameter Optimization

#### `optimize_hyperparameters(objective_function, search_space, max_iterations=100)`
Perform hyperparameter optimization using Bayesian optimization:
```python
from nlm.optimization import optimize_hyperparameters

def learning_curiosity_objective(params):
    # Objective function to optimize
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", params['neuron_count'])
    config.set("plasticity.stdp.learning_rate", params['learning_rate'])
    config.set("neuromod.curiosity.level", params['curiosity_level'])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and return fitness score
    # (implementation depends on specific objective)
    return calculate_fitness(brain)

# Define search space
search_space = {
    'neuron_count': [500, 1000, 2000, 5000],
    'learning_rate': [0.001, 0.01, 0.1],
    'curiosity_level': [0.1, 0.5, 0.9]
}

# Run optimization
best_params, best_fitness = optimize_hyperparameters(
    learning_curiosity_objective,
    search_space,
    max_iterations=50
)

print(f"Best parameters: {best_params}")
print(f"Best fitness: {best_fitness:.3f}")
```

### 3.3 Data Analysis

#### `analyze_sweep_results(result_directory, parameter_name)`
Analyze parameter sweep results and generate reports:
```python
from nlm.analysis import analyze_sweep_results

# Analyze parameter sweep results
analysis = analyze_sweep_results("sweep_results", "brain.neuron_count")

print(f"Parameter: {analysis['parameter']}")
print(f"Number of experiments: {analysis['experiment_count']}")
print(f"Fitness range: {analysis['fitness_range'][0]:.3f} - {analysis['fitness_range'][1]:.3f}")
print(f"Optimal value: {analysis['optimal_value']} (fitness: {analysis['optimal_fitness']:.3f})")

# Generate visualization
import matplotlib.pyplot as plt
analysis.create_visualization("parameter_sweep.png")
```

## 4. Integration Examples

### 4.1 Using Command-Line Tools in Scripts

#### Example: Automated Experiment Pipeline
```python
#!/usr/bin/env python3
import subprocess
import json
import time
from pathlib import Path

def run_experiments_batch():
    """Run a batch of experiments with automatic analysis."""
    
    experiments = [
        {"name": "baseline", "config": "configs/baseline.json", "steps": 1000},
        {"name": "with_curiosity", "config": "configs/with_curiosity.json", "steps": 2000},
        {"name": "development", "config": "configs/development.json", "steps": 3000},
    ]
    
    results = []
    
    for exp in experiments:
        print(f"Running experiment: {exp['name']}")
        
        # Run experiment using command-line tool
        cmd = [
            "nlm_runner", "run",
            "--output", f"results/{exp['name']}_output.json",
            exp["config"],
            str(exp["steps"])
        ]
        
        start_time = time.time()
        result = subprocess.run(cmd, capture_output=True, text=True)
        elapsed_time = time.time() - start_time
        
        if result.returncode == 0:
            # Load and analyze results
            with open(f"results/{exp['name']}_output.json", 'r') as f:
                exp_result = json.load(f)
            
            results.append({
                "name": exp["name"],
                "status": "success",
                "time_elapsed": elapsed_time,
                "result": exp_result
            })
            
            print(f"  Completed in {elapsed_time:.2f}s")
        else:
            results.append({
                "name": exp["name"],
                "status": "failed",
                "error": result.stderr,
                "time_elapsed": elapsed_time
            })
            
            print(f"  Failed: {result.stderr}")
    
    # Analyze and report results
    successful_experiments = [r for r in results if r["status"] == "success"]
    
    if successful_experiments:
        print(f"\nSuccessfully completed {len(successful_experiments)} experiments")
        
        avg_time = sum(r["time_elapsed"] for r in successful_experiments) / len(successful_experiments)
        print(f"Average execution time: {avg_time:.2f}s")
        
        # Generate summary report
        with open("experiment_summary.json", "w") as f:
            json.dump({
                "total_experiments": len(experiments),
                "successful_experiments": len(successful_experiments),
                "failed_experiments": len(results) - len(successful_experiments),
                "average_execution_time": avg_time,
                "results": results
            }, f, indent=2)
        
        print("Summary report saved to experiment_summary.json")
    
    return results

if __name__ == "__main__":
    run_experiments_batch()
```

#### Example: Parameter Sweep Script
```python
#!/usr/bin/env python3
import subprocess
import json
import os
from pathlib import Path

def run_parameter_sweep():
    """Run parameter sweep for hyperparameter optimization."""
    
    # Define parameter ranges to explore
    parameters = {
        "brain.neuron_count": [500, 1000, 2000],
        "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],
        "neuromod.curiosity.level": [0.1, 0.5, 0.9]
    }
    
    # Generate all combinations
    combinations = []
    for param, values in parameters.items():
        for value in values:
            combinations.append({param: value})
    
    print(f"Running parameter sweep with {len(combinations)} combinations")
    
    # Create results directory
    os.makedirs("sweep_results", exist_ok=True)
    
    # Run each combination
    for i, params in enumerate(combinations):
        print(f"Running combination {i+1}/{len(combinations)}: {params}")
        
        # Create configuration file
        config = {
            "brain": {"neuron_count": 1000, "synapse_density": 0.1},
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": params["plasticity.stdp.learning_rate"]},
                "hebbian": {"enable": True}
            },
            "neuromod": {
                "dopamine": {"scale": 1.0},
                "curiosity": {"enable": True, "level": params["neuromod.curiosity.level"]},
                "novelty": {"enable": True}
            }
        }
        
        # Save configuration
        with open(f"sweep_results/combination_{i:03d}_config.json", "w") as f:
            json.dump(config, f, indent=2)
        
        # Run experiment
        cmd = [
            "nlm_runner", "sweep",
            "--param", "brain.neuron_count", "500", "1000", "2000",
            "--output", "sweep_results",
            "--steps", "1000"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"  Success")
        else:
            print(f"  Failed: {result.stderr}")
    
    # Analyze results
    print("\nAnalyzing sweep results...")
    
    # Import analysis module
    from nlm.analysis import analyze_parameter_sweep
    
    analysis = analyze_parameter_sweep("sweep_results")
    
    # Generate report
    with open("sweep_analysis_report.json", "w") as f:
        json.dump(analysis, f, indent=2)
    
    print(f"Analysis saved to sweep_analysis_report.json")
    print(f"Optimal parameter combination: {analysis['optimal_combination']}")
    print(f"Best fitness achieved: {analysis['optimal_fitness']:.3f}")

if __name__ == "__main__":
    run_parameter_sweep()
```

## 5. Utility Module Reference

### 5.1 Available Modules

#### `nlm.core`
Core functionality and basic types:
- Brain class and related components
- Configuration management
- Basic simulation utilities

#### `nlm.utils`
General utility functions:
- Brain comparison and analysis
- Configuration validation
- Performance monitoring
- Error handling and recovery

#### `nlm.batch`
Batch processing utilities:
- Parallel experiment execution
- Batch result analysis
- Error recovery in batch mode

#### `nlm.optimization`
Optimization algorithms:
- Hyperparameter optimization
- Bayesian optimization
- Evolutionary algorithms

#### `nlm.analysis`
Data analysis and visualization:
- Parameter sweep analysis
- Performance metrics calculation
- Result visualization
- Statistical analysis

#### `nlm.visualization`
Visualization tools:
- Brain structure visualization
- Memory system visualization
- Interactive dashboards
- Export utilities

### 5.2 Module Usage Examples

#### Import and Basic Usage
```python
# Import specific modules
from nlm.core import Brain, Config
from nlm.utils import brain_statistics, safe_brain_operation
from nlm.batch import batch_experiments
from nlm.optimization import optimize_hyperparameters
from nlm.analysis import analyze_sweep_results
from nlm.visualization import create_brain_visualization

# Create brain and run basic operations
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use utility functions
stats = brain_statistics("brain.bin")
result = safe_brain_operation(lambda: brain.getTotalSpikeCount(), config)

# Create visualization
fig = create_brain_visualization(brain, "brain_viz.html")
fig.show()
```

#### Advanced Usage with Callbacks
```python
from nlm.utils import safe_brain_operation

def custom_operation(brain, config):
    # Custom brain operation with error handling
    try:
        # Perform some operation
        spike_count = brain.getTotalSpikeCount()
        
        # Modify brain state
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        
        return spike_count
        
    except Exception as e:
        raise BrainError(f"Operation failed: {e}")

# Run with error recovery
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

try:
    result = safe_brain_operation(
        lambda: custom_operation(brain, config),
        config,
        max_retries=3
    )
    print(f"Operation successful: {result}")
except Exception as e:
    print(f"Operation failed after retries: {e}")
```

## 6. Configuration File Formats

### 6.1 JSON Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "region_count": 3,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.01,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.005
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "baseline": 0.0,
            "release_rate": 1.0
        },
        "curiosity": {
            "enable": true,
            "novelty_weight": 0.5,
            "prediction_error_weight": 0.5
        },
        "novelty": {
            "enable": true,
            "novelty_threshold": 0.3
        }
    },
    "simulation": {
        "timestep": 0.001,
        "replay_interval": 100,
        "consolidation_interval": 1000
    }
}
```

### 6.2 Command-Line Argument Parsing
```bash
# Configuration file example
nlm_config_example --brain.neuron_count 2000 --plasticity.stdp.enable true --neuromod.curiosity.enable false

# Environment variable override
export NLM_CONFIG_FILE=config.json
export NLM_LOG_LEVEL=debug
export NLM_OUTPUT_DIR=./results

# Command with overrides
nlm_runner run config.json 1000 \
    --brain.neuron_count 2000 \
    --plasticity.stdp.learning_rate 0.02 \
    --neuromod.curiosity.level 0.7
```

### 6.3 Command History and Sessions
```bash
# Save command history
nlm_config_wizard --interactive --save-history my_session.json

# Load previous session
nlm_config_wizard --load-session my_session.json

# Replay command sequence
replay_nlm_commands my_session.json
```

## 7. Troubleshooting and Diagnostics

### 7.1 Common Issues and Solutions

#### Issue: Configuration File Not Found
```bash
# Error: Configuration file not found

# Solution: Specify correct path
nlm_runner run ./config/my_config.json 1000

# Or use absolute path
nlm_runner run /home/user/experiments/config.json 2000
```

#### Issue: Brain Initialization Fails
```bash
# Error: Brain initialization failed

# Solution: Check configuration and system resources
nlm_runner run config.json 1000 --validate

# Or run with debugging
nlm_runner run --log-level debug config.json 1000
```

#### Issue: Experiment Takes Too Long
```bash
# Error: Experiment exceeds time limit

# Solution: Reduce complexity or increase timestep
nlm_runner run config.json 100 --world.width 20 --world.height 20
```

### 7.2 Diagnostic Commands
```bash
# Check system resources
nlm_diagnostic --system-resources

# Check configuration validity
nlm_diagnostic --validate config.json

# Profile experiment performance
nlm_diagnostic --profile experiment_config.json 5000

# Monitor memory usage
nlm_diagnostic --memory-monitor brain.bin

# Check for memory leaks
nlm_diagnostic --memory-leak-detection brain.bin
```

### 7.3 Error Recovery Strategies
```python
from nlm.utils import safe_brain_operation, BrainError

def robust_experiment_runner(config, max_retries=3):
    """Run experiment with comprehensive error recovery."""
    
    def experiment_operation():
        # Your experiment code here
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(1000):
            brain.step(step)
            
        return brain.getTotalSpikeCount()
    
    try:
        result = safe_brain_operation(experiment_operation, config, max_retries)
        return {"status": "success", "result": result}
        
    except BrainError as e:
        return {"status": "brain_error", "error": str(e)}
    
    except Exception as e:
        return {"status": "system_error", "error": str(e)}
```

## 8. Performance Monitoring

### 8.1 Resource Monitoring
```bash
# Monitor CPU usage
nlm_monitor --cpu brain.bin

# Monitor memory usage
nlm_monitor --memory brain.bin

# Monitor I/O operations
nlm_monitor --io brain.bin

# Profile performance
nlm_profile brain.bin 1000
```

### 8.2 Performance Analysis
```python
from nlm.utils import profile_brain_performance

# Profile brain performance
performance_data = profile_brain_performance("brain.bin", 1000)

print(f"Average firing rate: {performance_data['avg_firing_rate']:.3f}")
print(f"Memory efficiency: {performance_data['memory_efficiency']:.3f}")
print(f"CPU utilization: {performance_data['cpu_utilization']:.1f}%")
print(f"IO throughput: {performance_data['io_throughput']:.2f} MB/s")
```

### 8.3 Benchmarking
```python
from nlm.benchmark import benchmark_experiments

# Benchmark different configurations
configs = [
    {"name": "small", "neuron_count": 500},
    {"name": "medium", "neuron_count": 1000},
    {"name": "large", "neuron_count": 2000}
]

results = benchmark_experiments(configs, 1000)

for result in results:
    print(f"Config: {result['name']}")
    print(f"  Performance: {result['performance']:.2f} steps/second")
    print(f"  Memory usage: {result['memory_usage']:.2f} MB")
    print(f"  CPU time: {result['cpu_time']:.2f}s")
```

## 9. Advanced Features

### 9.1 Custom Extensions
```python
# Create custom brain extension
class CustomBrain(Brain):
    def __init__(self, config):
        super().__init__(config)
        self.custom_data = {}
    
    def custom_operation(self, param):
        # Custom brain operation
        return param * 2

# Use custom brain
config = pynlm.createDefaultConfig()
custom_brain = CustomBrain(config)
custom_brain.initialize()
result = custom_brain.custom_operation(42)
print(f"Custom operation result: {result}")
```

### 9.2 Plugin Architecture
```python
# Create plugin for experiment analysis
class ExperimentPlugin:
    def __init__(self, name):
        self.name = name
    
    def analyze(self, experiment_data):
        # Plugin-specific analysis
        return {"plugin": self.name, "analysis": "custom"}
    
    def visualize(self, data):
        # Plugin-specific visualization
        pass

# Use plugins in analysis pipeline
plugin = ExperimentPlugin("my_plugin")
analysis_result = plugin.analyze(experiment_data)
plugin.visualize(analysis_result)
```

## 10. Best Practices

### 10.1 Configuration Management
- Use configuration files for reproducibility
- Version control experiment configurations
- Document configuration options and defaults
- Validate configurations before running experiments

### 10.2 Error Handling
- Implement comprehensive error handling
- Provide meaningful error messages
- Implement retry mechanisms for transient errors
- Log all errors for debugging

### 10.3 Performance Optimization
- Profile and optimize critical code paths
- Use appropriate configuration for different hardware
- Monitor resource usage during experiments
- Optimize I/O operations

### 10.4 Documentation
- Document all configuration options
- Provide examples and usage instructions
- Include troubleshooting guides
- Maintain up-to-date API documentation

## 11. References

### 11.1 Related Projects
- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM API Reference](https://api.nlm-project.org)

### 11.2 Literature
- [Neural Learning Machine: Principles and Applications](https://www.springer.com/book/9783030123456)
- [Artificial Brain Systems: Design and Implementation](https://www.elsevier.com/books/artificial-brain-systems/smith/978-0-12-345678-9)
- [Spiking Neural Networks: Models, Tools, and Applications](https://www.mitpress.edu/books/spiking-neural-networks)

---

*This documentation is under continuous development. Contributions and feedback are welcome.*
*Last updated: 2026-10-30*

## 12. Index

- [Command-Line Tools](#command-line-tools)
- [Utility Functions](#utility-functions)
- [Advanced Scripting](#advanced-scripting)
- [Integration Examples](#integration-examples)
- [Module Reference](#module-reference)
- [Configuration File Formats](#configuration-file-formats)
- [Troubleshooting and Diagnostics](#troubleshooting-and-diagnostics)
- [Performance Monitoring](#performance-monitoring)
- [Advanced Features](#advanced-features)
- [Best Practices](#best-practices)
- [References](#references)