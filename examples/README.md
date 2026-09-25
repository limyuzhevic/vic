# NLM Advanced Configuration Examples

This directory contains example configuration files and scripts for advanced NLM usage.

## Configuration Examples

### 1. Basic Phase 2 Configuration

**File:** `examples/basic_phase2.cfg`

```ini
# Basic Phase 2 Neural Computation Configuration
# Suitable for educational demonstrations and testing

[simulation]
  simulation_timestep = 0.001
  max_steps = 2000
  enable_progress_bar = false

[brain]
  neuron_count = 500
  region_count = 1
  region_0_type = "Cortical"

[plasticity]
  enable_stdp = true
  enable_hebbian = true
  stdp_ltp_weight = 0.02
  stdp_ltd_weight = 0.015
  stdp_tau = 20.0
  enable_structural_plasticity = true
  synaptogenesis_rate = 0.0001
  pruning_rate = 0.00001

[neuromodulation]
  enable_dopamine = true
  enable_novelty = true
  enable_curiosity = true
  enable_prediction_error = true
  dopamine_level = 0.5
  novelty_threshold = 0.1
  curiosity_strength = 0.3

[initialization]
  random_seed = 42
  enable_checkpointing = false
  enable_replay = false

[logging]
  level = "INFO"
  enable_file_logging = false
  log_file = "simulation.log"
```

### 2. Phase 4 Memory Configuration

**File:** `examples/phase4_memory.cfg`

```ini
# Phase 4 Configuration with Advanced Memory Systems
# Demonstrates working memory, episodic memory, and cognition

[simulation]
  simulation_timestep = 0.0005  # Higher precision for memory dynamics
  max_steps = 5000
  enable_progress_bar = true

[brain]
  neuron_count = 1000
  region_count = 2
  region_0_type = "Cortical"
  region_1_type = "Hippocampal"

[memory]
  enable_working_memory = true
  enable_episodic_memory = true
  enable_associative_memory = true
  wm_capacity = 20  # Number of items
  wm_decay_rate = 0.001
  em_encoding_strength = 0.8
  em_replay_frequency = 0.1

[cognition]
  enable_attention = true
  enable_planning = true
  enable_self_model = true
  enable_social_learning = true
  attention_strength = 0.7
  planning_horizon = 10

[prediction]
  enable_prediction = true
  prediction_horizon = 20
  prediction_error_weight = 0.5

[plasticity]
  enable_stdp = true
  enable_hebbian = true
  stdp_ltp_weight = 0.025
  stdp_ltd_weight = 0.01
  stdp_tau = 15.0
  enable_structural_plasticity = true
  synaptogenesis_rate = 0.00005
  pruning_rate = 0.00002

[environment]
  type = "ComplexWorld"
  complexity = "high"
  enable_rewards = true
  reward_range = "(-10, 10)"
```

### 3. Phase 6 Integration Configuration

**File:** `examples/phase6_integration.cfg`

```ini
# Phase 6 Final Integration Configuration
# All systems working together in a comprehensive brain model

[simulation]
  simulation_timestep = 0.0005
  max_steps = 10000
  enable_progress_bar = true
  enable_logging = true
  log_level = "DEBUG"

[brain]
  neuron_count = 2000
  region_count = 4
  region_0_type = "Sensory"
  region_1_type = "Cortical"
  region_2_type = "Hippocampal"
  region_3_type = "Motor"

[memory]
  enable_working_memory = true
  enable_episodic_memory = true
  enable_associative_memory = true
  enable_concept_formation = true
  enable_spatial_representation = true
  enable_temporal_relation = true
  wm_capacity = 50
  wm_decay_rate = 0.0005
  em_encoding_strength = 0.9
  em_replay_frequency = 0.2

[neuromodulation]
  enable_dopamine = true
  enable_novelty = true
  enable_curiosity = true
  enable_prediction_error = true
  dopamine_level = 0.7
  novelty_threshold = 0.05
  curiosity_strength = 0.5
  error_discount_factor = 0.9

[prediction]
  enable_prediction = true
  prediction_horizon = 50
  prediction_error_weight = 0.6
  confidence_threshold = 0.8

[cognition]
  enable_attention = true
  enable_planning = true
  enable_self_model = true
  enable_social_learning = true
  attention_strength = 0.8
  planning_horizon = 30
  self_model_update_rate = 0.01

[development]
  enable_development = true
  developmental_stages = 10
  plasticity_decay_rate = 0.05
  maturation_threshold = 0.9

[plasticity]
  enable_stdp = true
  enable_hebbian = true
  enable_reward_modulated = true
  stdp_ltp_weight = 0.03
  stdp_ltd_weight = 0.02
  stdp_tau = 10.0
  enable_structural_plasticity = true
  synaptogenesis_rate = 0.0001
  pruning_rate = 0.00005
  reward_modulation_strength = 0.8

[checkpointing]
  enable_checkpointing = true
  checkpoint_interval = 1000
  checkpoint_history = 10
  enable_replay = true
  replay_interval = 100

[environment]
  type = "ComplexWorld"
  complexity = "extreme"
  enable_rewards = true
  reward_range = "(-100, 100)"
  enable_social_agents = true
  social_agent_count = 5

[monitoring]
  enable_performance_monitoring = true
  enable_memory_monitoring = true
  enable_neural_activity_monitoring = true
  monitoring_interval = 100
```

### 4. Advanced Configuration Scripts

**File:** `examples/run_experiments.sh`

```bash
#!/bin/bash

# Script to run a series of NLM experiments with different configurations
# Usage: ./run_experiments.sh [base_dir]

BASE_DIR=${1:-"examples"}

# Create output directory
mkdir -p "$BASE_DIR/results"

# Experiment 1: Phase 2 Basic Test
echo "Running Phase 2 Basic Test..."
nlm run --config "$BASE_DIR/basic_phase2.cfg" --output "$BASE_DIR/results/phase2_output.txt"
nlm brain save "$BASE_DIR/results/phase2_brain.brain"

# Experiment 2: Phase 4 Memory Test
echo "Running Phase 4 Memory Test..."
nlm run --config "$BASE_DIR/phase4_memory.cfg" --output "$BASE_DIR/results/phase4_output.txt"
nlm brain save "$BASE_DIR/results/phase4_brain.brain"

# Experiment 3: Phase 6 Integration Test
echo "Running Phase 6 Integration Test..."
nlm run --config "$BASE_DIR/phase6_integration.cfg" --output "$BASE_DIR/results/phase6_output.txt"
nlm brain save "$BASE_DIR/results/phase6_brain.brain"

# Generate comparison report
echo "Generating comparison report..."
cat > "$BASE_DIR/results/comparison_report.txt" << EOF
=== NLM Experiment Comparison Report ===
Generated: $(date)

Experiment 1: Phase 2 Basic
- Configuration: basic_phase2.cfg
- Duration: $(wc -l < "$BASE_DIR/results/phase2_output.txt") log lines
- Output: $BASE_DIR/results/phase2_output.txt
- Brain saved: $BASE_DIR/results/phase2_brain.brain

Experiment 2: Phase 4 Memory
- Configuration: phase4_memory.cfg
- Duration: $(wc -l < "$BASE_DIR/results/phase4_output.txt") log lines
- Output: $BASE_DIR/results/phase4_output.txt
- Brain saved: $BASE_DIR/results/phase4_brain.brain

Experiment 3: Phase 6 Integration
- Configuration: phase6_integration.cfg
- Duration: $(wc -l < "$BASE_DIR/results/phase6_output.txt") log lines
- Output: $BASE_DIR/results/phase6_output.txt
- Brain saved: $BASE_DIR/results/phase6_brain.brain
EOF

echo "All experiments completed successfully!"
echo "Results available in: $BASE_DIR/results/"
```

### 5. Configuration Template Generator

**File:** `examples/generate_config.py`

```python
#!/usr/bin/env python3

"""
Configuration Generator for NLM
Generates custom configuration files based on user parameters
"""

import argparse
import json
import os
from pathlib import Path

def generate_config(args):
    """Generate NLM configuration based on arguments"""
    
    config = {
        "simulation": {
            "simulation_timestep": args.timestep or 0.001,
            "max_steps": args.steps or 1000,
            "enable_progress_bar": not args.quiet,
        },
        "brain": {
            "neuron_count": args.neurons or 500,
            "region_count": args.regions or 1,
        },
        "plasticity": {
            "enable_stdp": True,
            "enable_hebbian": True,
            "stdp_ltp_weight": args.stdp_ltp or 0.02,
            "stdp_ltd_weight": args.stdp_ltd or 0.015,
            "stdp_tau": args.stdp_tau or 20.0,
            "enable_structural_plasticity": True,
        },
        "neuromodulation": {
            "enable_dopamine": True,
            "enable_novelty": True,
            "enable_curiosity": True,
            "dopamine_level": 0.5,
        }
    }
    
    # Add section-specific configurations
    if args.memory:
        config["memory"] = {
            "enable_working_memory": True,
            "enable_episodic_memory": True,
            "wm_capacity": 20,
        }
    
    if args.cognition:
        config["cognition"] = {
            "enable_attention": True,
            "enable_planning": True,
            "attention_strength": 0.7,
        }
    
    # Write configuration file
    output_file = args.output or f"configs/generated_{args.name}.cfg"
    Path(output_file).parent.mkdir(parents=True, exist_ok=True)
    
    # Write in INI format
    with open(output_file, 'w') as f:
        f.write(f"# Generated NLM Configuration\n")
        f.write(f"# Name: {args.name}\n")
        f.write(f"# Generated: {__import__('datetime').datetime.now()}\n\n")
        
        for section, values in config.items():
            f.write(f"[{section}]\n")
            for key, value in values.items():
                if isinstance(value, float):
                    f.write(f"  {key} = {value}\n")
                elif isinstance(value, int):
                    f.write(f"  {key} = {value}\n")
                elif isinstance(value, bool):
                    f.write(f"  {key} = {'true' if value else 'false'}\n")
                else:
                    f.write(f"  {key} = {value}\n")
            f.write("\n")
    
    print(f"Configuration generated: {output_file}")

def main():
    parser = argparse.ArgumentParser(description="Generate NLM configuration files")
    
    parser.add_argument("-n", "--name", default="experiment", help="Configuration name")
    parser.add_argument("-o", "--output", help="Output file path")
    parser.add_argument("--neurons", type=int, help="Number of neurons")
    parser.add_argument("--regions", type=int, help="Number of brain regions")
    parser.add_argument("--steps", type=int, help="Maximum simulation steps")
    parser.add_argument("--timestep", type=float, help="Simulation timestep")
    parser.add_argument("--stdp-ltp", type=float, help="STDP LTP weight")
    parser.add_argument("--stdp-ltd", type=float, help="STDP LTD weight")
    parser.add_argument("--stdp-tau", type=float, help="STDP time constant")
    parser.add_argument("--memory", action="store_true", help="Enable memory systems")
    parser.add_argument("--cognition", action="store_true", help="Enable cognitive systems")
    parser.add_argument("--quiet", action="store_true", help="Suppress output")
    
    args = parser.parse_args()
    generate_config(args)

if __name__ == "__main__":
    main()
```

### 6. Batch Processing Script

**File:** `examples/batch_run.py`

```python
#!/usr/bin/env python3

"""
Batch processing for NLM simulations
Runs multiple configurations and collects results
"""

import subprocess
import os
import time
import json
from pathlib import Path

class NLMBatchProcessor:
    def __init__(self, config_dir="configs", output_dir="results"):
        self.config_dir = Path(config_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.results = []
    
    def run_configuration(self, config_file, experiment_name=None):
        """Run NLM with a specific configuration"""
        if experiment_name is None:
            experiment_name = config_file.stem
        
        print(f"\n=== Running Experiment: {experiment_name} ===")
        print(f"Configuration: {config_file}")
        
        start_time = time.time()
        
        # Run NLM simulation
        cmd = ["nlm", "run", "--config", str(config_file)]
        print(f"Command: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=3600  # 1 hour timeout
            )
            
            end_time = time.time()
            duration = end_time - start_time
            
            experiment_result = {
                "name": experiment_name,
                "config": str(config_file),
                "success": result.returncode == 0,
                "duration": duration,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "return_code": result.returncode,
                "timestamp": time.time()
            }
            
            self.results.append(experiment_result)
            
            # Save brain state
            brain_file = self.output_dir / f"{experiment_name}_brain.brain"
            subprocess.run(["nlm", "brain", "save", str(brain_file)], 
                         capture_output=True)
            
            # Save output
            output_file = self.output_dir / f"{experiment_name}_output.txt"
            with open(output_file, 'w') as f:
                f.write(result.stdout)
            
            print(f"Completed in {duration:.2f} seconds")
            print(f"Success: {'Yes' if result.returncode == 0 else 'No'}")
            
            return experiment_result
            
        except subprocess.TimeoutExpired:
            print(f"Experiment {experiment_name} timed out!")
            experiment_result = {
                "name": experiment_name,
                "config": str(config_file),
                "success": False,
                "duration": 3600,
                "error": "Timeout (1 hour)",
                "timestamp": time.time()
            }
            self.results.append(experiment_result)
            return experiment_result
        except Exception as e:
            print(f"Experiment {experiment_name} failed: {e}")
            experiment_result = {
                "name": experiment_name,
                "config": str(config_file),
                "success": False,
                "duration": time.time() - start_time,
                "error": str(e),
                "timestamp": time.time()
            }
            self.results.append(experiment_result)
            return experiment_result
    
    def run_all_configurations(self):
        """Run all configuration files in the config directory"""
        config_files = list(self.config_dir.glob("*.cfg"))
        
        if not config_files:
            print(f"No configuration files found in {self.config_dir}")
            return
        
        print(f"Found {len(config_files)} configuration files")
        
        for config_file in config_files:
            self.run_configuration(config_file)
    
    def generate_report(self):
        """Generate summary report of all experiments"""
        report_file = self.output_dir / "batch_report.json"
        
        # Calculate statistics
        successful = [r for r in self.results if r.get("success", False)]
        failed = [r for r in self.results if not r.get("success", False)]
        
        report = {
            "total_experiments": len(self.results),
            "successful_experiments": len(successful),
            "failed_experiments": len(failed),
            "total_duration": sum(r.get("duration", 0) for r in self.results),
            "results": self.results
        }
        
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\n=== Batch Processing Report ===")
        print(f"Total experiments: {len(self.results)}")
        print(f"Successful: {len(successful)}")
        print(f"Failed: {len(failed)}")
        print(f"Total duration: {report['total_duration']:.2f} seconds")
        print(f"Report saved to: {report_file}")
        
        return report

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="Batch process NLM simulations")
    parser.add_argument("--config-dir", default="configs", help="Directory containing configuration files")
    parser.add_argument("--output-dir", default="results", help="Directory for output files")
    parser.add_argument("--run-all", action="store_true", help="Run all configurations in directory")
    
    args = parser.parse_args()
    
    processor = NLMBatchProcessor(args.config_dir, args.output_dir)
    
    if args.run_all:
        processor.run_all_configurations()
    
    processor.generate_report()

if __name__ == "__main__":
    main()
```

This directory provides comprehensive configuration management tools for NLM users, from basic templates to advanced batch processing capabilities.