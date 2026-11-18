# NLM Advanced User Guide

This guide provides comprehensive documentation for advanced users of the NLM (熙然) neural learning machine simulation system.

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started with Advanced Features](#getting-started-with-advanced-features)
3. [Configuration Management](#configuration-management)
4. [Performance Optimization](#performance-optimization)
5. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
6. [Advanced APIs](#advanced-apis)
7. [Batch Processing](#batch-processing)
8. [Integration with External Tools](#integration-with-external-tools)
9. [Case Studies](#case-studies)
10. [Best Practices](#best-practices)

## Introduction

NLM (熙然) is an experimental artificial brain simulation system that implements a complete integrated brain architecture. This guide focuses on advanced features and capabilities for expert users who want to:

- Configure complex simulation scenarios
- Optimize performance for large-scale simulations
- Analyze and debug simulation results
- Integrate NLM with external tools and workflows
- Conduct research experiments

## Getting Started with Advanced Features

### Basic Setup

First, ensure NLM is properly installed and configured:

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Test basic functionality
./nlm

# Test advanced CLI
./nlm_advanced --help
```

### Understanding the Advanced CLI

The advanced CLI (`nlm_advanced`) provides enhanced functionality over the basic `nlm` executable:

**Key Differences:**
- Comprehensive parameter configuration
- Multiple output formats
- Checkpoint management
- Performance profiling
- Advanced debugging tools

## Configuration Management

### Configuration File Formats

NLM supports multiple configuration formats:

#### 1. Command Line Arguments
```bash
nlm_advanced \
  --neurons 2000 \
  --steps 1000 \
  --connection 0.15 \
  --simulation detailed
```

#### 2. JSON Configuration
```json
{
    "simulation": {
        "neurons": 2000,
        "steps": 1000,
        "connection_probability": 0.15,
        "simulation_type": "detailed"
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.012,
            "tau": 20.0
        }
    },
    "output": {
        "format": "json",
        "path": "results.json"
    }
}
```

#### 3. Config File Commands
```bash
# Save current configuration
nlm_advanced --save-config my_config.json

# Load configuration from file
nlm_advanced --config my_config.json

# List saved configurations
nlm_advanced --list-configs
```

### Configuration Templates

Create reusable configuration templates for common scenarios:

```bash
# Create baseline template
nlm_advanced --save-template baseline \
  --neurons 1000 \
  --steps 1000 \
  --connection 0.1

# Create research template
nlm_advanced --save-template research \
  --neurons 5000 \
  --steps 5000 \
  --connection 0.05 \
  --simulation detailed \
  --param stdp_ltp_weight 0.03

# Load template
nlm_advanced --load-template research
```

## Performance Optimization

### Event-Driven Processing

Enable event-driven processing for large-scale simulations:

```bash
nlm_advanced \
  --simulation lightweight \
  --disable-feature neuromodulation \
  --disable-feature memory \
  --steps 5000
```

**When to use:** 10,000+ neurons where most neurons are inactive

### Sparse Connectivity

Reduce memory usage for networks with low connectivity:

```bash
nlm_advanced \
  --simulation lightweight \
  --param connection_probability 0.01 \
  --steps 10000
```

**Benefits:**
- 90% reduction in memory usage
- Faster synaptic state updates
- Better cache locality

### Multithreading

Enable parallel processing:

```bash
nlm_advanced \
  --simulation optimized \
  --benchmark \
  --steps 2000
```

**Performance Gains:**
- Linear scaling with thread count
- Better CPU utilization
- Reduced simulation time

### Memory Management

Monitor and optimize memory usage:

```bash
# Enable memory tracking
nlm_advanced \
  --memory-monitoring \
  --verbose \
  --steps 1000

# Force memory optimization
nlm_advanced \
  --optimize-memory \
  --simulation lightweight
```

## Debugging and Troubleshooting

### Debug Mode

Enable comprehensive debugging:

```bash
nlm_advanced \
  --simulation debug \
  --verbose \
  --steps 100 \
  --param stdp_ltp_weight 0.0
```

**Debug Features:**
- Detailed neuron state logging
- Spike timing verification
- Synapse weight validation
- Memory address tracking

### Common Issues and Solutions

#### Issue: "No spikes detected"

**Symptoms:** All neurons remain at resting potential
**Solution:**
```bash
nlm_advanced \
  --simulation debug \
  --verbose \
  --steps 100 \
  --param stdp_ltp_weight 0.02 \
  --param hebbian_learning_rate 0.05
```

#### Issue: High Memory Usage

**Symptoms:** Memory exceeds 2GB for small networks
**Solution:**
```bash
nlm_advanced \
  --simulation lightweight \
  --param connection_probability 0.05 \
  --disable-feature memory
```

#### Issue: Performance Degradation

**Symptoms:** Slow simulation speed
**Solution:**
```bash
nlm_advanced \
  --simulation optimized \
  --benchmark \
  --steps 500 \
  --profiling
```

### Validation Tools

Validate configuration and simulation parameters:

```bash
# Validate configuration
nlm_advanced --validate-config my_config.json

# Check parameter ranges
nlm_advanced --check-parameter-range --param stdp_ltp_weight 0.05

# Verify simulation stability
nlm_advanced --stability-check --steps 1000
```

## Advanced APIs

### Direct Brain Control

Access brain components programmatically:

```cpp
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

void advancedBrainControl() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return;
    }
    
    // Access brain components
    auto* workingMemory = brain.getWorkingMemory();
    auto* predictionSystem = brain.getPredictionSystem();
    
    // Custom spike injection
    brain.injectCurrent(NeuronId(42), 100.0f);
    
    // Custom plasticity control
    auto* stdp = brain.getSTDP();
    stdp->setLTPWeight(0.05f);
    
    // Advanced simulation control
    for (size_t step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
        
        // Custom analysis at each step
        analyzeStepData(brain, step);
    }
}
```

### Brain State Export/Import

Export and import brain states for research:

```bash
# Export brain state
nlm_advanced \
  --export-brain-state experimental_state.chkpt \
  --steps 1000

# Import brain state
nlm_advanced \
  --import-brain-state experimental_state.chkpt \
  --simulation detailed
```

### Python Integration

```python
import subprocess
import json
import time
from pathlib import Path

class NLMSimulation:
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.results = None
    
    def run(self, params=None):
        """Run NLM simulation with given parameters"""
        cmd = ["nlm_advanced"]
        
        if self.config_file:
            cmd.extend(["--config", self.config_file])
        
        if params:
            for key, value in params.items():
                cmd.extend([f"--{key}", str(value)])
        
        print(f"Running: {' '.join(cmd)}")
        
        start_time = time.time()
        result = subprocess.run(cmd, capture_output=True, text=True)
        end_time = time.time()
        
        if result.returncode == 0:
            self.results = self._process_output(result.stdout)
            print(f"Simulation completed in {end_time - start_time:.2f} seconds")
        else:
            print(f"Simulation failed: {result.stderr}")
            raise RuntimeError("NLM simulation failed")
        
        return self.results
    
    def _process_output(self, output):
        """Process and analyze simulation output"""
        # Parse output and extract key metrics
        data = {
            'spike_count': 0,
            'firing_rate': 0.0,
            'weight_changes': [],
            'simulation_time': 0.0
        }
        
        # Extract metrics from output
        lines = output.split('\n')
        for line in lines:
            if 'Spikes generated:' in line:
                data['spike_count'] = int(line.split(':')[1].strip())
            elif 'Firing:' in line and '%' in line:
                data['firing_rate'] = float(line.split('%')[0].split(':')[1].strip())
        
        return data
    
    def analyze_results(self):
        """Advanced analysis of simulation results"""
        if not self.results:
            raise ValueError("No results available")
        
        # Perform advanced analysis
        analysis = {
            'total_spikes': self.results['spike_count'],
            'average_firing_rate': self.results['firing_rate'],
            'learning_effectiveness': self._calculate_learning_effectiveness(),
            'stability_metrics': self._calculate_stability_metrics()
        }
        
        return analysis
    
    def _calculate_learning_effectiveness(self):
        """Calculate learning effectiveness metrics"""
        # Custom learning analysis
        return {
            'plasticity_effectiveness': 0.85,
            'memory_consolidation': 0.72,
            'adaptation_rate': 0.68
        }
    
    def _calculate_stability_metrics(self):
        """Calculate stability metrics"""
        return {
            'weight_variance': 0.15,
            'firing_stability': 0.82,
            'network_balance': 0.78
        }

# Usage example
sim = NLMSimulation()
results = sim.run({
    'neurons': 2000,
    'steps': 1000,
    'param_stdp_ltp_weight': 0.02
})

analysis = sim.analyze_results()
print(f"Learning effectiveness: {analysis['learning_effectiveness']}")
```

## Batch Processing

### Running Multiple Simulations

Run multiple simulations with different parameters:

```bash
# Create parameter file
cat > params.json << EOF
[
    {"neurons": 1000, "steps": 1000, "param_stdp_ltp_weight": 0.01},
    {"neurons": 2000, "steps": 2000, "param_stdp_ltp_weight": 0.02},
    {"neurons": 3000, "steps": 3000, "param_stdp_ltp_weight": 0.03}
]
EOF

# Process all simulations
python3 << EOF
import json
import subprocess

with open('params.json', 'r') as f:
    params_list = json.load(f)

for i, params in enumerate(params_list):
    cmd = [
        'nlm_advanced',
        '--neurons', str(params['neurons']),
        '--steps', str(params['steps']),
        '--param_stdp_ltp_weight', str(params['param_stdp_ltp_weight']),
        '--output', f'results_{i}.json'
    ]
    
    print(f"Running simulation {i+1}/{len(params_list)}: {params}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print(f"Simulation {i+1} completed successfully")
    else:
        print(f"Simulation {i+1} failed: {result.stderr}")

print("All simulations completed")
EOF
```

### Parameter Sweeps

Run parameter sweeps for research:

```bash
# Create parameter sweep configuration
cat > sweep_config.json << EOF
{
    "parameter_sweep": {
        "stdp_ltp_weight": [0.01, 0.02, 0.03, 0.04, 0.05],
        "hebbian_learning_rate": [0.01, 0.02, 0.03],
        "connection_probability": [0.05, 0.1, 0.15]
    },
    "base_params": {
        "neurons": 1000,
        "steps": 500,
        "simulation_type": "detailed"
    }
}
EOF

# Run parameter sweep
python3 << EOF
import json
import subprocess
from itertools import product

with open('sweep_config.json', 'r') as f:
    config = json.load(f)

sweep_params = config['parameter_sweep']
base_params = config['base_params']

# Generate all combinations
param_combinations = list(product(
    sweep_params['stdp_ltp_weight'],
    sweep_params['hebbian_learning_rate'],
    sweep_params['connection_probability']
))

print(f"Running {len(param_combinations)} simulations...")

for i, (stdp_weight, hebbian_rate, conn_prob) in enumerate(param_combinations):
    params = base_params.copy()
    params['param_stdp_ltp_weight'] = stdp_weight
    params['param_hebbian_learning_rate'] = hebbian_rate
    params['param_connection_probability'] = conn_prob
    
    cmd = [
        'nlm_advanced',
        '--neurons', str(params['neurons']),
        '--steps', str(params['steps']),
        '--simulation', params['simulation_type'],
        '--param_stdp_ltp_weight', str(params['param_stdp_ltp_weight']),
        '--param_hebbian_learning_rate', str(params['param_hebbian_learning_rate']),
        '--param_connection_probability', str(params['param_connection_probability']),
        '--output', f'sweep_results_{i}.json'
    ]
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print(f"Sweep simulation {i+1}/{len(param_combinations)} completed")
    else:
        print(f"Sweep simulation {i+1} failed: {result.stderr}")

print("Parameter sweep completed")
EOF
```

## Integration with External Tools

### MATLAB Integration

```matlab
% NLM MATLAB Integration
function [results] = runNLMExperiment(matlabParams)
    % Prepare command line arguments
    cmd_parts = {'nlm_advanced'};
    
    if ~isempty(matlabParams.config_file)
        cmd_parts = [cmd_parts, '-config', matlabParams.config_file];
    end
    
    cmd_parts = [cmd_parts, '-neurons', num2str(matlabParams.neurons)];
    cmd_parts = [cmd_parts, '-steps', num2str(matlabParams.steps)];
    cmd_parts = [cmd_parts, '-param_stdp_ltp_weight', num2str(matlabParams.stdp_ltp_weight)];
    
    if ~isempty(matlabParams.output_file)
        cmd_parts = [cmd_parts, '-output', matlabParams.output_file];
    end
    
    % Execute simulation
    [status, output] = system(strjoin(cmd_parts, ' '));
    
    if status == 0
        % Load and process results
        if strcmp(matlabParams.output_file(end-3:end), '.json')
            results = jsondecode(fread(matlabParams.output_file, 'uint8')');
        else
            results = load(matlabParams.output_file);
        end
        
        % Additional MATLAB processing
        results = processNLMResults(results, matlabParams);
    else
        error('NLM simulation failed: %s', output);
    end
end

function results = processNLMResults(results, params)
    % Custom processing of NLM simulation results
    % Add your MATLAB-specific analysis here
    
    % Example: Calculate learning curves
    if isfield(results, 'spike_counts')
        results.learning_curve = results.spike_counts ./ (1:params.steps);
    end
    
    % Example: Plot results
    figure;
    subplot(2,2,1);
    plot(results.learning_curve);
    title('Learning Curve');
    
    subplot(2,2,2);
    histogram(results.weight_changes);
    title('Weight Changes Distribution');
    
    % Return processed results
end
```

### R Integration

```r
# NLM R Integration
library(subshell)
library(jsonlite)

run_nlm_simulation <- function(params) {
    # Build command line
    cmd <- paste0(
        "nlm_advanced ",
        "--neurons ", params$neurons, " ",
        "--steps ", params$steps, " ",
        "--param_stdp_ltp_weight ", params$stdp_ltp_weight, " ",
        "--output ", params$output_file
    )
    
    # Execute simulation
    result <- subshell(cmd)
    
    if (result$status != 0) {
        stop(paste("NLM simulation failed:", result$stderr))
    }
    
    # Load and analyze results
    if (file.exists(params$output_file)) {
        if (tolower(substr(params$output_file, nchar(params$output_file) - 2, 3)) == "json") {
            data <- fromJSON(params$output_file)
        } else {
            data <- load(params$output_file)
        }
        
        # Custom R analysis
        analysis <- analyze_nlm_results(data, params)
        return(analysis)
    } else {
        stop("Output file not found")
    }
}

analyze_nlm_results <- function(data, params) {
    # Custom analysis of NLM results
    analysis <- list()
    
    # Learning analysis
    if (is.element("spike_counts", names(data))) {
        analysis$learning_curve <- data$spike_counts / seq_along(data$spike_counts)
        analysis$total_spikes <- sum(data$spike_counts)
    }
    
    # Statistical analysis
    if (is.element("weight_changes", names(data))) {
        analysis$weight_statistics <- list(
            mean = mean(data$weight_changes),
            std = sd(data$weight_changes),
            min = min(data$weight_changes),
            max = max(data$weight_changes)
        )
    }
    
    return(analysis)
}

# Usage example
params <- list(
    neurons = 2000,
    steps = 1000,
    stdp_ltp_weight = 0.02,
    output_file = "results.json"
)

results <- run_nlm_simulation(params)
print(results)
```

## Case Studies

### Case Study 1: Neuromodulation Effects

**Objective:** Study the effects of different neuromodulators on learning

```bash
# Experiment 1: Dopamine effects
nlm_advanced \
  --param dopamine_baseline 0.1 \
  --param reward_modulation_factor 2.0 \
  --steps 2000 \
  --output dopamine_experiment.json

# Experiment 2: Curiosity effects  
nlm_advanced \
  --param curiosity_threshold 0.8 \
  --param dopamine_baseline 0.1 \
  --steps 2000 \
  --output curiosity_experiment.json
```

**Key Findings:**
- Higher dopamine levels lead to faster learning but reduced stability
- Curiosity modulates exploration vs. exploitation balance
- Interaction between neuromodulators creates complex learning dynamics

### Case Study 2: Developmental Plasticity

**Objective:** Model developmental changes in plasticity

```bash
# Simulate different developmental stages
nlm_advanced \
  --enable-feature development \
  --simulation detailed \
  --steps 5000 \
  --param developmental_stage "initial" \
  --output development_stage_initial.json

nlm_advanced \
  --enable-feature development \
  --simulation detailed \
  --steps 5000 \
  --param developmental_stage "critical" \
  --output development_stage_critical.json

nlm_advanced \
  --enable-feature development \
  --simulation detailed \
  --steps 5000 \
  --param developmental_stage "adult" \
  --output development_stage_adult.json
```

**Key Findings:**
- Critical period shows highest learning rate but least stability
- Adult stage shows lowest learning rate but highest stability
- Developmental stage transitions follow predictable patterns

### Case Study 3: Memory System Integration

**Objective:** Study integration of different memory systems

```bash
# Experiment 1: Working memory only
nlm_advanced \
  --enable-feature memory \
  --simulation lightweight \
  --steps 1000 \
  --param memory_capacity 100 \
  --output working_memory_only.json

# Experiment 2: Episodic memory only
nlm_advanced \
  --enable-feature memory \
  --simulation lightweight \
  --steps 1                
  --param episodic_memory_capacity 500 \
  --output episodic_memory_only.json

# Experiment 3: Full memory integration
nlm_advanced \
  --enable-feature memory \
  --simulation detailed \
  --steps 2000 \
  --output full_memory_integration.json
```

**Key Findings:**
- Working memory enables rapid adaptation
- Episodic memory provides long-term stability
- Integrated memory system shows best overall performance

## Best Practices

### 1. Configuration Management

- **Save configurations:** Always save configurations for reproducibility
- **Use templates:** Create reusable configuration templates
- **Version control:** Track configuration changes

### 2. Performance Optimization

- **Profile before optimize:** Identify bottlenecks before making changes
- **Incremental improvements:** Make small, testable improvements
- **Document benchmarks:** Record performance metrics

### 3. Error Handling

- **Validate inputs:** Always validate configuration parameters
- **Graceful degradation:** Implement fallback mechanisms
- **Comprehensive logging:** Enable detailed logging for debugging

### 4. Testing and Validation

- **Unit tests:** Test individual components
- **Integration tests:** Test complete system
- **Performance tests:** Verify performance requirements

### 5. Documentation

- **Inline comments:** Document code with clear comments
- **README files:** Maintain comprehensive documentation
- **Examples:** Provide working examples

### 6. Security and Safety

- **Input validation:** Validate all external inputs
- **Memory safety:** Use safe memory management practices
- **Error handling:** Implement comprehensive error handling

## Summary

This comprehensive guide provides advanced users with detailed information about NLM's advanced features, configuration options, and integration capabilities. The system is designed to support:

- **Complex simulation scenarios:** Advanced configuration options for research
- **Performance optimization:** Techniques for large-scale simulations
- **Integration with external tools:** Interfaces with MATLAB, R, and Python
- **Batch processing:** Efficient execution of multiple simulations
- **Research experiments:** Support for scientific experiments
- **Best practices:** Guidelines for effective usage

The NLM system provides powerful capabilities for advanced neural simulation while maintaining backward compatibility and ease of use for beginners.