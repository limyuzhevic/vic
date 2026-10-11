# NLM Command Line Interface

A comprehensive command-line interface for running and configuring NLM simulations.

## Overview

The NLM CLI provides a unified interface for:
- Running simulations with various configurations
- Managing checkpoints and saved states
- Analyzing performance and logging
- Running experiments with different parameters
- Creating custom configurations

## Installation

Ensure the NLM Python package is installed:

```bash
pip install pynlm
```

## Basic Usage

### Run a simple simulation

```bash
nlm run --steps 1000 --neurons 500
```

### Run with configuration file

```bash
nlm run --config my_config.json --steps 2000 --checkpoint-interval 100
```

### Run multiple experiments

```bash
nlm experiment --name "curiosity_test" --config base_config.json --repeats 5
```

## Commands

### `nlm run` - Run a simulation

Run a NLM simulation with specified parameters.

**Options:**

- `--steps, -s` - Number of simulation steps (default: 1000)
- `--neurons, -n` - Number of neurons (default: 1000)
- `--regions, -r` - Number of brain regions (default: 1)
- `--timestep, -t` - Simulation timestep (default: 0.001)
- `--config, -c` - Configuration file path
- `--checkpoint, -k` - Enable checkpointing (save state periodically)
- `--checkpoint-interval, -ki` - Checkpoint interval in steps (default: 100)
- `--checkpoint-dir` - Directory for checkpoint files (default: ./checkpoints)
- `--log-level, -l` - Logging level (DEBUG, INFO, WARNING, ERROR)
- `--log-file` - File for logging output
- `--world-width` - World width (default: 20.0)
- `--world-height` - World height (default: 20.0)
- `--vision-width` - Vision width (default: 16)
- `--vision-height` - Vision height (default: 16)
- `--enable-reward` - Enable reward modulation (default: true)
- `--enable-curiosity` - Enable curiosity (default: true)
- `--enable-development` - Enable development (default: true)
- `--enable-structural-plasticity` - Enable structural plasticity (default: true)

**Example:**

```bash
nlm run \
  --steps 5000 \
  --neurons 1500 \
  --regions 3 \
  --enable-reward \
  --enable-curiosity \
  --log-level INFO \
  --checkpoint \
  --checkpoint-interval 500
```

### `nlm experiment` - Run experiment batches

Run multiple simulations with different configurations or random seeds.

**Options:**

- `--name, -n` - Experiment name
- `--config, -c` - Base configuration file
- `--repeats, -r` - Number of repetitions (default: 1)
- `--param-file` - Parameter file for sweep (CSV format)
- `--output-dir` - Output directory (default: ./experiments)
- `--generate-report` - Generate analysis report

**Example (parameter sweep):**

```bash
nlm experiment \
  --name "curiosity_sweep" \
  --param-file params.csv \
  --repeats 10 \
  --output-dir ./results \
  --generate-report
```

### `nlm checkpoint` - Manage checkpoints

Save, load, or list checkpoints.

**Subcommands:**

#### `list` - List available checkpoints

```bash
nlm checkpoint list --checkpoint-dir ./checkpoints
```

#### `save` - Save current state

```bash
nlm checkpoint save --name my_state --checkpoint-dir ./checkpoints
```

#### `load` - Load a saved state

```bash
nlm checkpoint load --name my_state --checkpoint-dir ./checkpoints
```

#### `delete` - Delete a checkpoint

```bash
nlm checkpoint delete --name my_state --checkpoint-dir ./checkpoints
```

### `nlm analyze` - Analyze simulation results

Analyze and visualize simulation data.

**Options:**

- `--input, -i` - Input directory with results
- `--output, -o` - Output directory for analysis
- `--generate-plots` - Generate visualization plots
- `--plot-format` - Plot format (PNG, SVG, PDF)
- `--metrics` - Comma-separated list of metrics to analyze
- `--generate-report` - Generate comprehensive report

**Example:**

```bash
nlm analyze \
  --input ./experiments/my_exp \
  --output ./analysis \
  --generate-plots \
  --plot-format PNG \
  --metrics "total_reward,avg_firing_rate,learning_rate"
```

### `nlm config` - Configuration management

Create, edit, and manage configuration files.

**Subcommands:**

#### `create` - Create a new configuration

```bash
nlm config create --name my_config --output config.json
```

#### `edit` - Edit an existing configuration

```bash
nlm config edit config.json
```

#### `validate` - Validate configuration file

```bash
nlm config validate config.json
```

#### `show` - Show configuration values

```bash
nlm config show config.json
```

### `nlm log` - View logs

View and filter simulation logs.

**Options:**

- `--log-file` - Log file to read
- `--level` - Minimum log level (DEBUG, INFO, WARNING, ERROR)
- `--lines, -n` - Number of lines to show (default: 50)
- `--tail` - Show last N lines
- `--grep, -g` - Filter by text pattern
- `--follow` - Follow log file (like `tail -f`)

**Example:**

```bash
nlm log --log-file simulation.log --level INFO --grep "ERROR" --lines 100
```

## Configuration Files

NLM supports multiple configuration file formats:

### JSON Configuration

```json
{
  "simulation": {
    "steps": 1000,
    "neurons": 1000,
    "regions": 1,
    "timestep": 0.001
  },
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "connection_probability": 0.05
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    },
    "hebbian": {
      "enable": true
    },
    "structural": {
      "enable": true
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true
    },
    "novelty": {
      "enable": true
    }
  },
  "world": {
    "width": 20.0,
    "height": 20.0,
    "visionWidth": 16,
    "visionHeight": 16
  },
  "agent": {
    "enableRewardModulation": true,
    "enableCuriosity": true,
    "enableDevelopment": true,
    "enableStructuralPlasticity": true
  }
}
```

### YAML Configuration

```yaml
simulation:
  steps: 1000
  neurons: 1000
  regions: 1
  timestep: 0.001

brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
  structural:
    enable: true

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true

world:
  width: 20.0
  height: 20.0
  visionWidth: 16
  visionHeight: 16

agent:
  enableRewardModulation: true
  enableCuriosity: true
  enableDevelopment: true
  enableStructuralPlasticity: true
```

### Parameter Sweep File (CSV)

```csv
param,value
neurons,500
neurons,1000
neurons,2000
curiosity_threshold,0.1
curiosity_threshold,0.3
curiosity_threshold,0.5
reward_modulation,enable
reward_modulation,disable
```

## Advanced Features

### Profile-Based Configurations

Create configuration profiles for different use cases:

```bash
# Profile: Fast debugging
nlm run --config debug.json --steps 100 --log-level DEBUG

# Profile: Research with full features
nlm run --config research.json --steps 10000 --checkpoint

# Profile: Production simulation
nlm run --config production.json --steps 50000 --checkpoint-interval 1000
```

### Custom Parameter Ranges

Create custom parameter ranges for experiments:

```bash
nlm experiment --name "parameter_sweep" \
  --param-file ranges.csv \
  --repeats 20 \
  --generate-report
```

### Real-Time Monitoring

For long-running simulations:

```bash
# Start with monitoring
nlm run --steps 10000 --neurons 2000 --checkpoint \
  --log-file simulation.log &

# Monitor in another terminal
nlm log --log-file simulation.log --follow --level INFO

# Check checkpoint status
nlm checkpoint list
```

## Automation

### Script Templates

Create reusable simulation scripts:

```python
#!/usr/bin/env python3
"""
NLM Simulation Automation Script
"""
import subprocess
import json
import time
from pathlib import Path

def run_simulation(config_file, steps, output_dir):
    """Run a NLM simulation with given configuration."""
    
    # Ensure output directory exists
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    
    # Run simulation
    cmd = [
        "nlm", "run",
        "--config", config_file,
        "--steps", str(steps),
        "--log-file", f"{output_dir}/simulation.log",
        "--checkpoint",
        "--checkpoint-interval", "1000"
    ]
    
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd)
    
    if result.returncode == 0:
        print(f"Simulation completed successfully in {output_dir}")
        return True
    else:
        print(f"Simulation failed with code {result.returncode}")
        return False

if __name__ == "__main__":
    run_simulation("configs/research.json", 5000, "results/exp1")
```

### Experiment Runner

Create comprehensive experiment management:

```python
#!/usr/bin/env python3
"""
NLM Experiment Management Script
"""
import argparse
import json
import subprocess
from pathlib import Path

class ExperimentManager:
    def __init__(self, base_dir="experiments"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
    
    def run_experiment(self, name, config, **kwargs):
        """Run a single experiment."""
        
        experiment_dir = self.base_dir / name
        experiment_dir.mkdir(exist_ok=True)
        
        # Save config
        config_path = experiment_dir / "config.json"
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        # Run simulation
        cmd = ["nlm", "run", "--config", str(config_path)]
        
        for key, value in kwargs.items():
            cmd.extend([f"--{key.replace('_', '-')}", str(value)])
        
        cmd.extend([
            "--checkpoint",
            "--checkpoint-interval", "1000",
            "--log-file", str(experiment_dir / "simulation.log")
        ])
        
        print(f"Running experiment '{name}'...")
        result = subprocess.run(cmd)
        
        if result.returncode == 0:
            print(f"Experiment '{name}' completed successfully")
            return True
        else:
            print(f"Experiment '{name}' failed")
            return False
    
    def run_batch(self, experiments):
        """Run multiple experiments."""
        
        results = {}
        for name, config in experiments.items():
            success = self.run_experiment(name, config)
            results[name] = success
        
        # Generate report
        self.generate_report(results)
        
        return results
    
    def generate_report(self, results):
        """Generate experiment results report."""
        
        report = {
            "timestamp": time.time(),
            "results": results,
            "summary": {
                "total_experiments": len(results),
                "successful": sum(1 for r in results.values() if r),
                "failed": sum(1 for r in results.values() if not r)
            }
        }
        
        report_path = self.base_dir / "experiment_report.json"
        
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Experiment report saved to {report_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Experiment Manager")
    parser.add_argument("action", choices=["run", "batch", "report"])
    parser.add_argument("name")
    
    args = parser.parse_args()
    
    # Example experiments
    experiments = {
        "baseline": {
            "steps": 1000,
            "neurons": 500,
            "enable_curiosity": False
        },
        "curiosity_enabled": {
            "steps": 1000,
            "neurons": 500,
            "enable_curiosity": True
        },
        "development_enabled": {
            "steps": 1000,
            "neurons": 500,
            "enable_development": True
        }
    }
    
    manager = ExperimentManager()
    
    if args.action == "run":
        manager.run_experiment(args.name, experiments.get(args.name, {}))
    elif args.action == "batch":
        manager.run_batch(experiments)
    elif args.action == "report":
        # Load existing results and generate report
        pass
```

## Troubleshooting

### Common Issues

1. **Import errors**: Ensure `pynlm` is installed correctly
   ```bash
   pip install --force-reinstall pynlm
   ```

2. **Permission errors**: Check file permissions
   ```bash
   chmod +x nlm
   ```

3. **Configuration errors**: Validate JSON syntax
   ```bash
   python -m json.tool config.json
   ```

4. **Memory errors**: Reduce neuron count for debugging
   ```bash
   nlm run --neurons 100 --steps 100
   ```

5. **Log file errors**: Check disk space
   ```bash
   df -h
   ```

### Getting Help

- **Command help**: `nlm --help` or `nlm <command> --help`
- **Examples**: Check the `examples/` directory
- **Documentation**: Visit the NLM documentation website
- **Community**: Join the NLM discussion forum

## License

This NLM CLI is part of the Neural Learning Machine project and is licensed under the MIT License.