# Scripting and Automation Capabilities

This document describes the scripting and automation capabilities added to NLM to enable advanced users to automate experiments and workflows.

## 1. NLM Scripting Language (NL-Script)

### Overview
NLM includes a simple scripting language specifically designed for:
- Automating experiment sequences
- Managing configuration updates during runtime
- Creating complex experimental protocols
- Recording and analyzing data automatically

### Basic Syntax
```nlscript
// Script header
// Format: nlm script <script_file> [options]

// Variable assignment
variable_name = value

// Function calls
run_experiment(phase, parameters)
save_checkpoint(name)
load_checkpoint(name)

// Control structures
if (condition) {
    execute_action()
} else {
    alternative_action()
}

while (condition) {
    iterate_action()
}

// Loops
for (i = 1; i <= 10; i++) {
    set_neuron_count(i * 100)
    run_step()
}
```

### Script Examples

#### A. Basic Experiment Automation
```nlscript
// Automated experiment sequence
script_name = "research_protocol"
config = {neuron_count: 1000, region_count: 2}

// Run multiple experiments with varying parameters
for (repetition = 1; repetition <= 5; repetition++) {
    set_seed(repetition * 12345)
    set_neuron_count(500 + repetition * 100)
    
    // Run phase 2 demo
    result = run_experiment("phase2", config)
    
    // Record results
    record_result("repetition", repetition)
    record_result("neuron_count", config.neuron_count)
    record_result("total_spikes", result.total_spikes)
    
    // Save intermediate results
    save_checkpoint("experiment_" + repetition)
    
    // Brief pause between experiments
    wait(1.0)
}

// Analyze and report
analyze_results()
generate_report()
```

#### B. Complex Protocol with Conditionals
```nlscript
// Adaptive experiment protocol
protocol = {
    learning_rate: 0.01,
    max_steps: 1000,
    adaptive: true,
    stop_conditions: {reward: 100, steps: 500}
}

initialize_brain(protocol)

while (true) {
    step = take_step()
    
    // Record observation
    observation = get_sensory_input()
    reward = get_reward()
    
    // Check stop conditions
    if (reward >= protocol.stop_conditions.reward) {
        log("Target reward achieved: " + reward)
        break
    }
    
    if (step >= protocol.stop_conditions.steps) {
        log("Maximum steps reached")
        break
    }
    
    // Adapt parameters if enabled
    if (protocol.adaptive && step % 100 == 0) {
        adjust_learning_rate(protocol.learning_rate * 0.9)
        log("Learning rate reduced to " + protocol.learning_rate)
    }
    
    // Save periodically
    if (step % 100 == 0) {
        save_checkpoint("step_" + step)
    }
}

finalize_experiment()
```

#### C. Parameter Search Script
```nlscript
// Grid search for optimal parameters
search_space = [
    {learning_rate: 0.001, neuron_count: 500},
    {learning_rate: 0.005, neuron_count: 500},
    {learning_rate: 0.01, neuron_count: 500},
    {learning_rate: 0.02, neuron_count: 500},
    {learning_rate: 0.005, neuron_count: 1000}
]

best_result = null
best_score = -infinity

for (params in search_space) {
    // Configure brain with current parameters
    config = {
        neuron_count: params.neuron_count,
        stdp_ltp_weight: params.learning_rate,
        stdp_ltd_weight: params.learning_rate * 0.9,
        connection_probability: 0.1
    }
    
    // Run experiment
    experiment = run_experiment("phase2", config)
    
    // Calculate performance score
    score = calculate_score(experiment)
    
    // Track best result
    if (score > best_score) {
        best_score = score
        best_result = {
            parameters: params,
            result: experiment,
            score: score
        }
    }
    
    // Save result for analysis
    save_result(params, experiment, score)
}

// Report best parameters
log("Best parameters found:")
log("  Learning rate: " + best_result.parameters.learning_rate)
log("  Neuron count: " + best_result.parameters.neuron_count)
log("  Performance score: " + best_result.score)

export_results("best_parameters.json")
```

## 2. Python Integration and Automation

### 2.1 NL-Script Python Interpreter

NLM includes a Python interpreter for NL-Scripts with enhanced features:

```python
import nlm.scripting as nlm_script

# Load and run a script
script = nlm_script.load_script("experiment_protocol.nls")
script.run()

# Access script variables and results
script.set_variable("learning_rate", 0.01)
result = script.run_experiment("phase2")

# Export results
result.save_as_csv("experiment_results.csv")
result.plot_metrics(["spike_count", "firing_rate"])
```

### 2.2 Advanced Python Automation

#### A. Experiment Manager
```python
class NLMExperimentManager:
    def __init__(self, config_file=None):
        self.config = self.load_config(config_file)
        self.results = []
        self.checkpoints = []
    
    def run_parameter_sweep(self, parameter_ranges):
        """Run a grid search over parameter space"""
        for params in self.generate_parameter_combinations(parameter_ranges):
            experiment = self.run_single_experiment(params)
            self.results.append({
                'parameters': params,
                'result': experiment,
                'timestamp': datetime.now()
            })
            
            # Save intermediate results
            self.save_results(f"sweep_{len(self.results)}.json")
    
    def run_adaptive_experiment(self, base_config, adaptation_rules):
        """Run experiment with adaptive parameter adjustment"""
        current_config = base_config.copy()
        
        for step in range(max_steps):
            # Run step with current configuration
            experiment = self.run_single_experiment(current_config)
            
            # Apply adaptation rules
            current_config = self.apply_adaptation_rules(
                current_config, experiment, adaptation_rules
            )
            
            # Check stop conditions
            if self.check_stop_conditions(experiment, adaptation_rules):
                break
    
    def create_research_workflow(self, workflow_definition):
        """Create complex research workflows"""
        for phase in workflow_definition['phases']:
            if phase['type'] == 'experiment':
                self.run_experiment(phase['config'])
            elif phase['type'] == 'analysis':
                self.analyze_results(phase['criteria'])
            elif phase['type'] == 'comparison':
                self.compare_experiments(phase['experiments'])
    
    def export_results(self, format='json', filename=None):
        """Export results in various formats"""
        if format == 'json':
            import json
            with open(filename or "nlm_results.json", 'w') as f:
                json.dump(self.results, f, indent=2, default=str)
        
        elif format == 'csv':
            import pandas as pd
            df = pd.DataFrame(self.results)
            df.to_csv(filename or "nlm_results.csv", index=False)
        
        elif format == 'hdf5':
            import h5py
            with h5py.File(filename or "nlm_results.h5", 'w') as f:
                for i, result in enumerate(self.results):
                    f.create_dataset(f"experiment_{i}", data=result)
    
    def generate_research_report(self, report_type='summary'):
        """Generate research reports"""
        if report_type == 'summary':
            return self._generate_summary_report()
        elif report_type == 'detailed':
            return self._generate_detailed_report()
        elif report_type == 'statistical':
            return self._generate_statistical_report()
```

#### B. Configuration Automation
```python
# Configuration automation and validation
class NLMConfigManager:
    def __init__(self):
        self.config_schema = self._load_config_schema()
        self.validated_configs = []
    
    def validate_and_adjust_config(self, config):
        """Validate configuration and make adjustments if needed"""
        validation_errors = self.validate_config(config)
        
        if validation_errors:
            config = self.auto_adjust_config(config, validation_errors)
            validation_errors = self.validate_config(config)
        
        if validation_errors:
            raise ConfigurationError(f"Cannot fix configuration errors: {validation_errors}")
        
        self.validated_configs.append(config)
        return config
    
    def generate_recommended_configs(self, experiment_type):
        """Generate recommended configurations for different experiment types"""
        if experiment_type == 'basic':
            return self._get_basic_recommendations()
        elif experiment_type == 'research':
            return self._get_research_recommendations()
        elif experiment_type == 'production':
            return self._get_production_recommendations()
        elif experiment_type == 'benchmark':
            return self._get_benchmark_recommendations()
    
    def optimize_config_for_performance(self, config, target_performance):
        """Optimize configuration for target performance"""
        # Implement optimization algorithms
        optimized_config = self._run_optimization(config, target_performance)
        return optimized_config
```

### 2.3 Script Library

#### A. Common Experiment Scripts
```python
# Common experiment templates
experiment_templates = {
    'basic_connectivity': {
        'config': {
            'neuron_count': 500,
            'region_count': 1,
            'connection_probability': 0.15,
            'simulation_timestep': 0.001
        },
        'description': 'Basic connectivity test with random connectivity'
    },
    
    'plasticity_experiment': {
        'config': {
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.1,
            'simulation_timestep': 0.001,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.015
        },
        'description': 'STDP plasticity learning experiment'
    },
    
    'memory_experiment': {
        'config': {
            'neuron_count': 2000,
            'region_count': 3,
            'connection_probability': 0.08,
            'simulation_timestep': 0.001,
            'enable_development': true
        },
        'description': 'Memory formation and consolidation experiment'
    }
}

# Utility functions for script creation
def create_research_script(name, template='plasticity', variations=None):
    """Create a research script with parameter variations"""
    template_config = experiment_templates[template]['config']
    
    scripts = []
    for variation in variations or [{}]:
        # Merge template with variations
        config = {**template_config, **variation}
        
        script = NLMResearchScript(
            name=f"{name}_{hash(str(config))}",
            config=config,
            description=f"Research script: {template} with variations"
        )
        
        scripts.append(script)
    
    return scripts
```

## 3. Command-Line Scripting Interface

### 3.1 Script Execution
```bash
# Run a script file
nlm script experiment_protocol.nls

# Run with configuration
nlm script experiment_protocol.nls --config my_config.json

# Run with output redirection
nlm script experiment_protocol.nls --output results.json

# Run with logging
nlm script experiment_protocol.nls --log-level debug

# Run multiple scripts
nlm script script1.nls script2.nls script3.nls

# Run script with timeout
nlm script experiment_protocol.nls --timeout 300

# Run script in background
nlm script experiment_protocol.nls --background
```

### 3.2 Script Management
```bash
# List available scripts
nlm script list

# Show script details
nlm script show experiment_protocol.nls

# Create a new script
nlm script create my_script.nls

# Edit a script
nlm script edit my_script.nls

# Run script with parameters
nlm script run my_script.nls --param neuron_count 1000

# Export script results
nlm script export results.nls --format json

# Import script from other formats
nlm script import experiment.py --from python
nlm script import protocol.json --from json
```

## 4. Advanced Scripting Features

### 4.1 State Management
```nlscript
// Persistent state management
state = {
    current_experiment: "phase2",
    parameters: {
        neuron_count: 1000,
        learning_rate: 0.01
    },
    results: [],
    checkpoints: []
}

// Save state
save_state("experiment_state.json")

// Load state
load_state("experiment_state.json")

// Update state
state.parameters.neuron_count = state.parameters.neuron_count * 2
state.results.append(last_result)
```

### 4.2 Data Logging and Monitoring
```nlscript
// Real-time data logging
log_data({
    timestamp: get_timestamp(),
    experiment: current_experiment,
    parameters: current_parameters,
    metrics: current_metrics,
    performance: calculate_performance()
})

// Export data to various formats
export_data({
    format: "csv",
    filename: "experiment_data.csv",
    data: accumulated_data
})

export_data({
    format: "json",
    filename: "experiment_results.json",
    data: results
})

export_data({
    format: "parquet",
    filename: "experiment_data.parquet",
    data: large_data
})
```

### 4.3 Parallel Processing
```nlscript
// Parallel experiment execution
experiments = [
    {params: {neuron_count: 500, learning_rate: 0.01}, phase: "phase2"},
    {params: {neuron_count: 1000, learning_rate: 0.01}, phase: "phase2"},
    {params: {neuron_count: 1500, learning_rate: 0.01}, phase: "phase2"},
    {params: {neuron_count: 2000, learning_rate: 0.01}, phase: "phase2"}
]

// Run experiments in parallel
results = run_parallel(experiments)

// Wait for all experiments to complete
wait_for_all(results)

// Analyze parallel results
analyze_parallel_results(results)
```

## 5. Integration with Development Tools

### 5.1 Jupyter Notebook Integration
```python
# NLM integration with Jupyter notebooks
import nlm.scripting as nlm_script
import pandas as pd
import matplotlib.pyplot as plt

# Create interactive experiment notebook
nb = nlm_script.create_notebook("nlm_research")

# Add experiment cells
nb.add_code_cell("""
import nlm.scripting as nlm_script
config = {'neuron_count': 1000, 'region_count': 2}
experiment = nlm_script.run_experiment('phase2', config)
""")

nb.add_analysis_cell("""
# Analyze experiment results
import matplotlib.pyplot as plt
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(experiment.timesteps, experiment.spike_counts)
plt.title('Spike Count Over Time')
plt.xlabel('Step')
plt.ylabel('Spike Count')

plt.subplot(2, 2, 2)
plt.hist(experiment.firing_rates, bins=20)
plt.title('Firing Rate Distribution')
plt.xlabel('Firing Rate')
plt.ylabel('Count')

plt.tight_layout()
plt.show()
""")

nb.save("nlm_research_notebook.ipynb")
```

### 5.2 IDE Integration
```bash
# Integration with popular IDEs and editors
# VS Code extension for NLM
# PyCharm plugin for NLM experiments
# Eclipse plugin for NLM scripting

# Example VS Code commands
nlm vs-code init
nlm vs-code experiment <experiment_type>
nlm vs-code script <script_file>
```

## 6. Documentation and Examples

### 6.1 Script Documentation
```markdown
# NLM Script Language Reference

## Variables
- Primitive types: integer, float, string, boolean
- Collections: array, object
- Special types: config, result, experiment

## Functions
- `run_experiment(phase, config)`: Run an experiment
- `save_checkpoint(name, data=None)`: Save checkpoint
- `load_checkpoint(name)`: Load checkpoint
- `set_variable(name, value)`: Set variable
- `get_variable(name)`: Get variable
- `log(message)`: Log message
- `wait(seconds)`: Wait for specified time

## Error Handling
- Try-catch blocks
- Error codes and messages
- Exception types
```

### 6.2 Script Examples Directory
```
/examples/
├── basic/
│   ├── hello_world.nls
│   ├── simple_experiment.nls
│   └── basic_connectivity.nls
├── advanced/
│   ├── parameter_sweep.nls
│   ├── adaptive_protocol.nls
│   └── memory_experiment.nls
├── research/
│   ├── learning_curves.nls
│   ├── comparison_experiments.nls
│   └── statistical_analysis.nls
└── production/
    ├── large_scale_experiment.nls
    ├── checkpointing_protocol.nls
    └── monitoring_dashboard.nls
```

## 7. Performance and Scalability

### 7.1 Script Performance Optimization
- Lazy evaluation
- Caching of frequently used functions
- Parallel execution for independent experiments
- Memory management for large datasets

### 7.2 Scalability Considerations
- Distributed script execution
- Grid computing support
- Cloud deployment options
- Containerization for reproducible experiments

## 8. Future Enhancements

### 8.1 Machine Learning Integration
- AutoML for experiment parameter optimization
- Neural network-based experiment design
- Reinforcement learning for protocol optimization

### 8.2 Advanced Scripting Features
- Domain-specific languages (DSLs) for different experiment types
- Template engines for experiment configuration
- Visualization and monitoring frameworks

### 8.3 Collaboration and Version Control
- Script version control
- Collaborative experiment design
- Experiment reproducibility platforms

## Benefits of Scripting and Automation

1. **Reproducibility**: Scripts ensure experiments can be exactly reproduced
2. **Scalability**: Automated execution of large experiment suites
3. **Flexibility**: Complex experiment protocols are easily defined
4. **Monitoring**: Built-in logging and monitoring capabilities
5. **Analysis**: Automated data analysis and reporting
6. **Integration**: Seamless integration with development tools
7. **Collaboration**: Team-based experiment design and execution
8. **Optimization**: Automated parameter optimization and tuning

These scripting and automation capabilities make NLM suitable for:
- Research experiments with complex protocols
- Large-scale parameter studies
- Automated benchmarking and performance testing
- Educational demonstrations and teaching materials
- Production environment monitoring and control
- Continuous integration and deployment pipelines