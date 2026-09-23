# NLM CLI Usage Guide

## Overview

The NLM Command-Line Interface (CLI) provides comprehensive control over the NLM neural simulation system. It enables users to run simulations, manage configurations, export/import brain states, and monitor performance directly from the command line.

**Current Status:** The NLM CLI implements Phase 2 real neural computation and provides excellent control over the working neural core components.

## Installation and Setup

### Building from Source
```bash
# Navigate to the project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Installing the CLI
```bash
# Install the CLI executable
make install

# Or install to custom location
cmake --install build --prefix /usr/local
```

### Verifying Installation
```bash
# Check if nlm_cli is available
which nlm_cli

# View help information
nlm_cli --help

# Run a simple test
nlm_cli run --neuron-count 10 --max-steps 5
```

## Basic CLI Commands

### 1. Running Basic Simulations

```bash
# Simple brain simulation with default settings
nlm_cli run --neuron-count 100 --max-steps 1000

# Run with verbose output for debugging
nlm_cli run --neuron-count 500 --max-steps 2000 --verbose

# Use custom timestep for specific accuracy requirements
nlm_cli run --neuron-count 200 --timestep 0.0001

# Limit memory usage by reducing neurons
nlm_cli run --neuron-count 50 --max-steps 500
```

### 2. Running Experiments

```bash
# Run predefined connectivity experiment
nlm_cli experiment connectivity

# Run plasticity learning experiment
nlm_cli experiment plasticity

# Run STDP verification experiment
nlm_cli experiment stdp

# Run complete Phase 2 demonstration
nlm_cli experiment phase2

# Run experiment with output file
nlm_cli experiment phase2 --output experiment_results.json

# Run experiment with custom configuration
nlm_cli experiment phase2 --config configs/experiment.cfg
```

### 3. Configuration Management

```bash
# List current configuration settings
nlm_cli config list

# Generate default configuration file
nlm_cli config generate --output default_config.cfg

# Generate configuration with custom settings
nlm_cli config generate --output custom_config.cfg

# Validate configuration file for errors
nlm_cli config validate my_config.cfg

# Validate and show configuration details
nlm_cli config validate --verbose important_config.cfg
```

### 4. Brain State Management

```bash
# Export brain state to JSON format (human-readable)
nlm_cli export json --output brain_state.json

# Export brain state to CSV format (for analysis)
nlm_cli export csv --output neural_data.csv

# Export brain state to XML format (structured data)
nlm_cli export xml --output brain_state.xml

# Export brain state to binary format (compact storage)
nlm_cli export binary --output brain_state.bin

# Export specific brain region only
nlm_cli export json --brain cortex --output cortex_state.json
```

### 5. Importing and Continuing Work

```bash
# Import saved brain state to continue simulation
nlm_cli import saved_brain.json

# Import with format specification
nlm_cli import neural_data.xml --format xml

# Import and run new simulation with loaded state
nlm_cli import previous_state.bin
nlm_cli run --max-steps 1000 --continue

# Import into specific brain region
nlm_cli import partial_state.json --region motor
```

### 6. Performance Monitoring

```bash
# Monitor brain performance in real-time
nlm_cli monitor --interval 0.5

# Monitor specific metrics (spikes, firing_rate, memory)
nlm_cli monitor --metrics spikes,firing_rate,memory,curiosity

# Log monitoring data to file
nlm_cli monitor --interval 1.0 --output monitoring.log

# High-frequency monitoring
nlm_cli monitor --interval 0.1 --metrics all
```

### 7. Performance Profiling

```bash
# Profile memory usage patterns
nlm_cli profile --output memory_profile.json

# Profile computational performance
nlm_cli profile --output performance_profile.json

# Profile with specific duration (5 minutes)
nlm_cli profile --duration 300 --output long_run_profile.json

# Profile specific brain regions
nlm_cli profile --output --brain cortex --brain motor
```

### 8. Running Tests

```bash
# Run all unit and integration tests
nlm_cli test

# Run only unit tests
nlm_cli test --unit

# Run only integration tests
nlm_cli test --integration

# Run tests with JSON output
nlm_cli test --format json --output test_results.json

# Run tests and save detailed output
nlm_cli test --unit --output unit_tests.json --format json
```

### 9. Benchmarking

```bash
# Run performance benchmarks
nlm_cli benchmark

# Benchmark with small scale (1 neuron per layer)
nlm_cli benchmark --scale 1

# Benchmark with medium scale (2 neurons per layer)
nlm_cli benchmark --scale 2

# Benchmark with large scale (3 neurons per layer)
nlm_cli benchmark --scale 3

# Run benchmark with more iterations for statistical significance
nlm_cli benchmark --scale 2 --iterations 20

# Run benchmark and save results
nlm_cli benchmark --output benchmark_results.json
```

### 10. Configuration Presets

```bash
# Run simulation with preset configurations

# Small: Fast testing, minimal resources
nlm_cli run --neuron-count 50 --timestep 0.001 --max-steps 100

# Medium: Balanced performance and accuracy
nlm_cli run --neuron-count 500 --timestep 0.001 --max-steps 1000

# Large: High accuracy, resource-intensive
nlm_cli run --neuron-count 2000 --timestep 0.0001 --max-steps 5000

# Research: Optimized for learning experiments
nlm_cli run --neuron-count 1000 --set plasticity.learning_rate=0.01
```

## Advanced CLI Usage

### 1. Command Combination and Chaining

```bash
# Chain commands for complex workflows
nlm_cli config generate --output temp.cfg
nlm_cli run --config temp.cfg --max-steps 500
nlm_cli export json --output results.json
nlm_cli profile --output profile.json

# Use output of one command in another
config_file=$(nlm_cli config generate --output dynamic_config.cfg)
echo "Generated config: $config_file"
```

### 2. Script Integration

```bash
#!/bin/bash
# NLM simulation workflow script

# Generate unique identifier for this run
RUN_ID=$(date +%Y%m%d_%H%M%S)

# Create output directory
mkdir -p results/$RUN_ID

# Run simulation with specific parameters
nlm_cli run --neuron-count 1000 --max-steps 2000 \
    --output results/$RUN_ID/simulation.log \
    --verbose

# Export brain state
nlm_cli export json --output results/$RUN_ID/brain_state.json

# Profile performance
nlm_cli profile --output results/$RUN_ID/profile.json

echo "Simulation completed. Results saved to results/$RUN_ID/"
```

```python
#!/usr/bin/env python3
import subprocess
import json
import os

def run_nlm_workflow(neuron_count=1000, max_steps=2000):
    """Run NLM workflow programmatically"""
    
    # Create run ID
    run_id = f"run_{int(time.time())}"
    output_dir = f"results/{run_id}"
    os.makedirs(output_dir, exist_ok=True)
    
    # Run NLM simulation
    cmd = [
        "nlm_cli", "run",
        "--neuron-count", str(neuron_count),
        "--max-steps", str(max_steps),
        "--output", f"{output_dir}/simulation.log",
        "--verbose"
    ]
    
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode == 0:
        print("Simulation completed successfully")
        
        # Export brain state
        export_cmd = [
            "nlm_cli", "export", "json",
            "--output", f"{output_dir}/brain_state.json"
        ]
        subprocess.run(export_cmd)
        
        # Profile performance
        profile_cmd = [
            "nlm_cli", "profile",
            "--output", f"{output_dir}/profile.json"
        ]
        subprocess.run(profile_cmd)
        
        return {
            "status": "success",
            "run_id": run_id,
            "output_dir": output_dir,
            "log": result.stdout
        }
    else:
        return {
            "status": "error",
            "message": result.stderr,
            "log": result.stdout
        }

# Usage
result = run_nlm_workflow(neuron_count=500, max_steps=1000)
print(json.dumps(result, indent=2))
```

### 3. Configuration File Usage

#### Example Configuration File
```bash
# configs/custom.cfg
custom_nlm_config()

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm_simulation.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0
```

#### Using Configuration Files
```bash
# Run simulation with custom configuration
nlm_cli run --config configs/custom.cfg

# Override configuration settings
nlm_cli run --config configs/custom.cfg --set neuron_count=2000
nlm_cli run --config configs/custom.cfg --set timestep=0.01

# Validate custom configuration
nlm_cli config validate configs/custom.cfg

# Generate configuration with specific settings
nlm_cli config generate --output specialized.cfg \
    --set neuron_count=1500 \
    --set timestep=0.001
```

## Configuration File Format

### JSON Configuration
```bash
# configs/nlm_config.json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity": {
        "learning_rate": 0.01,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_time_constant": 20.0
    },
    "development": {
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_simulation.log"
    },
    "simulation": {
        "max_steps": 10000,
        "time_limit": 0.0
    }
}
```

### YAML Configuration
```bash
# configs/nlm_config.yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1

plasticity:
  learning_rate: 0.01
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  stdp_time_constant: 20.0

development:
  synaptogenesis_rate: 0.001
  pruning_rate: 0.0001

logging:
  level: INFO
  to_file: true
  filename: nlm_simulation.log

simulation:
  max_steps: 10000
  time_limit: 0.0
```

## Common CLI Problems and Solutions

### 1. "Command not found: nlm_cli"
**Solution:** Install the NLM package or build from source:
```bash
make install
# or
cmake --install build
cd /path/to/nlm
```

### 2. "Failed to load configuration file"
**Solution:** Check that the configuration file exists and is readable:
```bash
ls -la configs/default.cfg
nlm_cli config validate configs/default.cfg
```

### 3. "Configuration validation failed"
**Solution:** Fix configuration errors and retry:
```bash
nlm_cli config validate invalid_config.cfg
```

### 4. "Permission denied"
**Solution:** Check file permissions:
```bash
chmod +x nlm_cli
chmod 755 nlm_cli
```

### 5. "Memory allocation failed"
**Solution:** Reduce resource requirements:
```bash
# Use smaller configuration
nlm_cli run --neuron-count 100 --max-steps 500

# Use custom timestep
nlm_cli run --neuron-count 500 --timestep 0.01
```

## CLI Output Formats

### JSON Output
```bash
# Export brain state as JSON
nlm_cli export json --output brain_state.json

# Export test results as JSON
nlm_cli test --format json --output test_results.json

# View JSON output
jq '.' brain_state.json
```

### XML Output
```bash
# Export brain state as XML
nlm_cli export xml --output brain_state.xml

# Export configuration as XML
nlm_cli config generate --output config.xml
```

### CSV Output
```bash
# Export neural data as CSV
nlm_cli export csv --output neurons.csv

# Monitor data as CSV
nlm_cli monitor --output monitoring.csv
```

### Text Output
```bash
# View human-readable output
nlm_cli run --neuron-count 100 --max-steps 10

# View help in text format
nlm_cli --help
nlm_cli run --help
nlm_cli experiment --help
```

## Scripting and Automation

### 1. Bash Scripting
```bash
#!/bin/bash
# Automated NLM simulation pipeline

# Function to run simulation with parameters
run_simulation() {
    local neuron_count=$1
    local max_steps=$2
    local output_file=$3
    
    echo "Running simulation: $neuron_count neurons, $max_steps steps"
    
    nlm_cli run \
        --neuron-count $neuron_count \
        --max-steps $max_steps \
        --output $output_file \
        --verbose
    
    return $?
}

# Function to process simulation results
process_results() {
    local output_file=$1
    local analysis_file=$2
    
    echo "Processing simulation results"
    
    # Export brain state
    nlm_cli export json --output $analysis_file/brain_state.json
    
    # Profile performance
    nlm_cli profile --output $analysis_file/profile.json
    
    # Generate analysis
    echo "Analysis complete. Results saved to $analysis_file"
}

# Main execution
run_simulation 1000 2000 results/simulation_1
process_results results/simulation_1 analysis_1

run_simulation 500 1000 results/simulation_2
process_results results/simulation_2 analysis_2

run_simulation 2000 5000 results/simulation_3
process_results results/simulation_3 analysis_3

echo "All simulations completed successfully"
```

### 2. Python Scripting
```python
#!/usr/bin/env python3
import subprocess
import json
import time
import os
import argparse

class NLMSimulationRunner:
    """Manage NLM simulations programmatically"""
    
    def __init__(self, nlm_path="nlm_cli"):
        self.nlm_path = nlm_path
        self.results = []
    
    def run_simulation(self, neuron_count=1000, max_steps=2000, 
                      config_file=None, output_file=None, verbose=False):
        """Run a single NLM simulation"""
        
        cmd = [self.nlm_path, "run", "--neuron-count", str(neuron_count),
               "--max-steps", str(max_steps)]
        
        if config_file:
            cmd.extend(["--config", config_file])
        
        if output_file:
            cmd.extend(["--output", output_file])
        
        if verbose:
            cmd.append("--verbose")
        
        print(f"Running: {' '.join(cmd)}")
        start_time = time.time()
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        end_time = time.time()
        
        simulation_result = {
            "command": " ".join(cmd),
            "start_time": start_time,
            "end_time": end_time,
            "duration": end_time - start_time,
            "return_code": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0
        }
        
        self.results.append(simulation_result)
        
        return simulation_result
    
    def export_brain_state(self, output_format="json", output_file=None,
                          brain_id=None):
        """Export brain state"""
        
        cmd = [self.nlm_path, "export", output_format]
        
        if output_file:
            cmd.extend(["--output", output_file])
        
        if brain_id:
            cmd.extend(["--brain", brain_id])
        
        print(f"Exporting: {' '.join(cmd)}")
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        return {
            "command": " ".join(cmd),
            "return_code": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0
        }
    
    def run_experiment(self, experiment_name, output_file=None):
        """Run predefined experiment"""
        
        cmd = [self.nlm_path, "experiment", experiment_name]
        
        if output_file:
            cmd.extend(["--output", output_file])
        
        print(f"Running experiment: {' '.join(cmd)}")
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        return {
            "command": " ".join(cmd),
            "return_code": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0
        }
    
    def profile_performance(self, duration=60, output_file=None):
        """Profile performance"""
        
        cmd = [self.nlm_path, "profile", "--duration", str(duration)]
        
        if output_file:
            cmd.extend(["--output", output_file])
        
        print(f"Profiling: {' '.join(cmd)}")
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        return {
            "command": " ".join(cmd),
            "return_code": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0
        }
    
    def get_summary(self):
        """Get summary of all simulation runs"""
        
        total_runs = len(self.results)
        successful_runs = sum(1 for r in self.results if r["success"])
        total_duration = sum(r["duration"] for r in self.results)
        
        return {
            "total_runs": total_runs,
            "successful_runs": successful_runs,
            "success_rate": successful_runs / total_runs if total_runs > 0 else 0,
            "total_duration": total_duration,
            "average_duration": total_duration / total_runs if total_runs > 0 else 0,
            "results": self.results
        }

# Usage example
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Simulation Runner")
    parser.add_argument("--neuron-count", type=int, default=1000,
                       help="Number of neurons")
    parser.add_argument("--max-steps", type=int, default=2000,
                       help="Maximum simulation steps")
    parser.add_argument("--experiments", action="store_true",
                       help="Run experiments instead of basic simulation")
    
    args = parser.parse_args()
    
    runner = NLMSimulationRunner()
    
    if args.experiments:
        # Run experiments
        experiments = ["connectivity", "plasticity", "stdp", "phase2"]
        for exp in experiments:
            result = runner.run_experiment(exp)
            print(f"Experiment '{exp}' completed: {result['success']}")
    else:
        # Run basic simulation
        result = runner.run_simulation(
            neuron_count=args.neuron_count,
            max_steps=args.max_steps
        )
        print(f"Simulation completed: {result['success']}")
        print(f"Duration: {result['duration']:.2f} seconds")
    
    # Get summary
    summary = runner.get_summary()
    print(f"\nSummary: {summary['success_rate']*100:.1f}% success rate")
    print(f"Total time: {summary['total_duration']:.2f} seconds")
```

## Conclusion

The NLM CLI provides a powerful command-line interface for controlling neural simulations. It offers:

1. **Comprehensive Control**: From basic simulations to complex experiments
2. **Flexible Configuration**: Multiple configuration file formats
3. **Extensive Output Options**: JSON, XML, CSV, and text formats
4. **Integration Capabilities**: Scripting and automation support
5. **Performance Monitoring**: Built-in profiling and monitoring tools

The CLI is designed to be both beginner-friendly and powerful enough for advanced use cases, making it suitable for research, education, and production environments.

## References

- [HOW_TO_USE.md](HOW_TO_USE.md) - Python API documentation
- [easy_usage.md](easy_usage.md) - Quick start guide
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API reference
- [PERFORMANCE_OPTIMIZATION.md](PERFORMANCE_OPTIMIZATION.md) - Performance guide
- [MACHINE_LEARNING_INTEGRATION.md](MACHINE_LEARNING_INTEGRATION.md) - ML integration guide
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current integration status

---

*Note: This CLI implements Phase 2 real neural computation capabilities. Phase 6 integration (Final Integration) is in progress but not yet complete.*