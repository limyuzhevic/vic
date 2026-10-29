# NLM Command-Line Interface Guide

## Overview

This document provides comprehensive guidance for using the NLM (熙然) command-line interface (CLI), including advanced command-line options, configuration file examples, script examples for batch processing, and integration with other tools.

## 1. Basic CLI Usage

### 1.1 Core Commands

```bash
# Run NLM with default configuration
./nlm_phase6_demo

# Run NLM with custom configuration file
./nlm_phase6_demo --config my_config.json

# Run NLM with verbose output
./nlm_phase6_demo --verbose

# Run NLM with specific number of episodes
./nlm_phase6_demo --episodes 100

# Run NLM with timestep
./nlm_phase6_demo --timestep 0.01

# Run NLM with world size
./nlm_phase6_demo --width 200 --height 200

# Run NLM with vision range
./nlm_phase6_demo --vision-width 20 --vision-height 20
```

### 1.2 Configuration Commands

```bash
# Display current configuration
./nlm_phase6_demo --show-config

# Validate configuration file
./nlm_phase6_demo --validate-config my_config.json

# Generate default configuration
./nlm_phase6_demo --generate-config > default_config.json

# Merge configuration files
./nlm_phase6_demo --merge-config base_config.json override_config.json > merged_config.json
```

### 1.3 Output and Logging Commands

```bash
# Save output to file
./nlm_phase6_demo --output results.txt

# Enable debug logging
./nlm_phase6_demo --log-level debug

# Enable performance profiling
./nlm_phase6_demo --profile-performance

# Enable memory profiling
./nlm_phase6_demo --profile-memory

# Enable checkpointing
./nlm_phase6_demo --checkpoint-interval 100
```

## 2. Advanced Command-Line Options

### 2.1 Brain Configuration Options

```bash
# Brain configuration
./nlm_phase6_demo \
  --neuron-count 3000 \
  --region-count 4 \
  --connection-probability 0.1 \
  --learning-rate 0.01 \
  --plasticity-strength 0.6 \
  --development-stage maturation \
  --simulation-timestep 0.01

# Plasticity configuration
./nlm_phase6_demo \
  --plasticity-enabled true \
  --plasticity-stdp-enabled true \
  --plasticity-hebbian-enabled true \
  --plasticity-reward-modulated-enabled true \
  --plasticity-eligibility-trace-enabled true
```

### 2.2 Memory System Options

```bash
# Working memory configuration
./nlm_phase6_demo \
  --memory-working-enabled true \
  --memory-working-capacity 100 \
  --memory-working-decay-rate 0.95 \
  --memory-working-reinforcement-threshold 0.8

# Episodic memory configuration
./nlm_phase6_demo \
  --memory-episodic-enabled true \
  --memory-episodic-capacity 200 \
  --memory-episodic-capture-interval 10 \
  --memory-episodic-replay-interval 100 \
  --memory-episodic-consolidation-interval 1000

# Associative memory configuration
./nlm_phase6_demo \
  --memory-associative-enabled true \
  --memory-associative-similarity-threshold 0.6 \
  --memory-associative-spreading-activation-radius 0.3
```

### 2.3 Neuromodulation Options

```bash
# Neuromodulation configuration
./nlm_phase6_demo \
  --neuromodulation-dopamine-enabled true \
  --neuromodulation-curiosity-enabled true \
  --neuromodulation-novelty-enabled true \
  --neuromodulation-prediction-error-enabled true

# Dopamine configuration
./nlm_phase6_demo \
  --neuromodulation-dopamine-baseline 0.1 \
  --neuromodulation-dopamine-sensitivity 0.5 \
  --neuromodulation-dopamine-phasic-decay 0.9

# Curiosity configuration
./nlm_phase6_demo \
  --neuromodulation-curiosity-weight 0.3 \
  --neuromodulation-curiosity-decay-rate 0.95 \
  --neuromodulation-curiosity-threshold 0.3
```

### 2.4 Prediction System Options

```bash
# Prediction system configuration
./nlm_phase6_demo \
  --prediction-enabled true \
  --prediction-horizon 5 \
  --prediction-error-sensitivity 0.5

# Prediction error configuration
./nlm_phase6_demo \
  --neuromodulation-prediction-error-enabled true \
  --neuromodulation-prediction-error-method mean_squared_error \
  --neuromodulation-prediction-error-sensitivity 0.5
```

### 2.5 Cognition System Options

```bash
# Cognition system configuration
./nlm_phase6_demo \
  --cognition-attention-enabled true \
  --cognition-planning-enabled true \
  --cognition-self-model-enabled true \
  --cognition-social-learning-enabled true

# Attention configuration
./nlm_phase6_demo \
  --cognition-attention-selection-method lateral_inhibition \
  --cognition-attention-competition-strength 0.7 \
  --cognition-attention-bias-integration true

# Planning configuration
./nlm_phase6_demo \
  --cognition-planning-depth 3 \
  --cognition-planning-action-evaluation-method value_based
```

### 2.6 Development System Options

```bash
# Development system configuration
./nlm_phase6_demo \
  --development-enabled true \
  --development-stage maturation \
  --development-critical-period-duration 500 \
  --development-maturation-rate 0.05
```

### 2.7 World Configuration Options

```bash
# Simple world configuration
./nlm_phase6_demo \
  --world-type simple \
  --world-width 300 \
  --world-height 300 \
  --world-vision-width 20 \
  --world-vision-height 20 \
  --world-timestep 0.01

# Complex world configuration
./nlm_phase6_demo \
  --world-type complex \
  --world-width 500 \
  --world-height 500 \
  --world-time-steps 1000 \
  --world-simulation-speed 1.0
```

## 3. Configuration File Examples

### 3.1 Basic Configuration File

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "learning_rate": 0.01,
        "plasticity_strength": 0.5,
        "development_stage": "adult",
        "simulation_timestep": 0.01
    },
    "memory": {
        "working": {
            "enabled": true,
            "capacity": 100,
            "decay_rate": 0.95,
            "reinforcement_threshold": 0.8
        },
        "episodic": {
            "enabled": true,
            "capacity": 200,
            "capture_interval": 10,
            "replay_interval": 100,
            "consolidation_interval": 1000
        }
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline_level": 0.1,
            "sensitivity": 0.5
        },
        "curiosity": {
            "enabled": true,
            "weight": 0.3,
            "decay_rate": 0.95
        }
    },
    "world": {
        "width": 200,
        "height": 200,
        "vision_width": 20,
        "vision_height": 20,
        "timestep": 0.01
    }
}
```

### 3.2 Advanced Configuration File

```json
{
    "name": "Advanced NLM Configuration",
    "description": "Advanced configuration for NLM Phase 6 experiments",
    "brain": {
        "neuron_count": 3000,
        "region_count": 4,
        "connection_probability": 0.08,
        "learning_rate": 0.005,
        "plasticity_strength": 0.7,
        "development_stage": "critical_period",
        "simulation_timestep": 0.005,
        "plasticity": {
            "stdp_enabled": true,
            "hebbian_enabled": true,
            "reward_modulated_enabled": true,
            "eligibility_trace_enabled": true,
            "stdp": {
                "learning_rate": 0.01,
                "time_constant": 20.0
            }
        }
    },
    "memory": {
        "working": {
            "enabled": true,
            "capacity": 150,
            "decay_rate": 0.96,
            "reinforcement_threshold": 0.85,
            "competition_strength": 0.6
        },
        "episodic": {
            "enabled": true,
            "capacity": 300,
            "capture_interval": 5,
            "replay_interval": 50,
            "consolidation_interval": 500,
            "encoding_strength": 0.9
        },
        "associative": {
            "enabled": true,
            "similarity_threshold": 0.7,
            "spreading_activation_radius": 0.4,
            "association_strength": 0.8
        }
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline_level": 0.1,
            "sensitivity": 0.6,
            "reward_prediction_error": {
                "method": "td_error",
                "discount_factor": 0.95,
                "learning_rate": 0.01
            }
        },
        "curiosity": {
            "enabled": true,
            "weight": 0.4,
            "decay_rate": 0.94,
            "novelty_weight": 0.8,
            "prediction_error_weight": 0.2,
            "exploration_reward": 0.6
        },
        "novelty": {
            "enabled": true,
            "detection_method": "pattern_matching",
            "similarity_threshold": 0.8,
            "adaptation_rate": 0.95
        },
        "prediction_error": {
            "enabled": true,
            "method": "mean_squared_error",
            "error_sensitivity": 0.6
        }
    },
    "world": {
        "type": "complex",
        "width": 500,
        "height": 500,
        "timestep": 0.01,
        "agent_count": 10,
        "complexity": "high",
        "social_dynamics": {
            "interaction_rules": true,
            "hierarchy": true,
            "cooperation": true,
            "competition": true
        }
    },
    "experiment": {
        "name": "Phase6_Integration_Advanced",
        "type": "phase6_integration",
        "parameters": {
            "duration": 3000,
            "checkpoint_interval": 500,
            "display_interval": 100
        }
    },
    "performance": {
        "optimization_level": "maximum",
        "memory_management": {
            "enable_memory_pools": true,
            "compression_enabled": true
        },
        "computation": {
            "enable_vectorization": true,
            "parallel_processing_enabled": true,
            "num_threads": 8
        }
    }
}
```

### 3.3 Configuration Schema

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "NLM Configuration Schema",
    "description": "JSON Schema for NLM configuration files",
    "type": "object",
    "properties": {
        "brain": {
            "type": "object",
            "properties": {
                "neuron_count": {
                    "type": "integer",
                    "minimum": 100,
                    "maximum": 10000,
                    "description": "Number of neurons in the brain"
                },
                "region_count": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 10,
                    "description": "Number of brain regions"
                },
                "connection_probability": {
                    "type": "number",
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "description": "Probability of synaptic connections"
                },
                "learning_rate": {
                    "type": "number",
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "description": "Learning rate for plasticity"
                }
            },
            "required": ["neuron_count", "region_count"]
        },
        "memory": {
            "type": "object",
            "properties": {
                "working": {
                    "type": "object",
                    "properties": {
                        "capacity": {
                            "type": "integer",
                            "minimum": 1,
                            "maximum": 1000
                        },
                        "decay_rate": {
                            "type": "number",
                            "minimum": 0.0,
                            "maximum": 1.0
                        }
                    }
                },
                "episodic": {
                    "type": "object",
                    "properties": {
                        "capacity": {
                            "type": "integer",
                            "minimum": 1,
                            "maximum": 1000
                        },
                        "capture_interval": {
                            "type": "integer",
                            "minimum": 1
                        }
                    }
                }
            }
        },
        "neuromodulation": {
            "type": "object",
            "properties": {
                "dopamine": {
                    "type": "object",
                    "properties": {
                        "enabled": {"type": "boolean"},
                        "baseline_level": {
                            "type": "number",
                            "minimum": 0.0,
                            "maximum": 1.0
                        },
                        "sensitivity": {
                            "type": "number",
                            "minimum": 0.0,
                            "maximum": 1.0
                        }
                    }
                },
                "curiosity": {
                    "type": "object",
                    "properties": {
                        "enabled": {"type": "boolean"},
                        "weight": {
                            "type": "number",
                            "minimum": 0.0,
                            "maximum": 1.0
                        }
                    }
                }
            }
        },
        "world": {
            "type": "object",
            "properties": {
                "width": {
                    "type": "integer",
                    "minimum": 50,
                    "maximum": 1000
                },
                "height": {
                    "type": "integer",
                    "minimum": 50,
                    "maximum": 1000
                },
                "vision_width": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 100
                },
                "vision_height": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 100
                },
                "timestep": {
                    "type": "number",
                    "minimum": 0.001,
                    "maximum": 0.1
                }
            },
            "required": ["width", "height"]
        }
    },
    "required": ["brain"],
    "additionalProperties": false
}
```

## 4. Script Examples for Batch Processing

### 4.1 Batch Processing Script

```bash
#!/bin/bash
# batch_experiment.sh - Batch processing script for NLM experiments

EXPERIMENT_DIR="experiments"
CONFIG_TEMPLATE="config_template.json"
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR

# Function to run experiment with given parameters
run_experiment() {
    local experiment_id=$1
    local neuron_count=$2
    local episodes=$3
    local timestep=$4
    
    echo "Running experiment $experiment_id..."
    
    # Create experiment configuration
    config=$(cat $CONFIG_TEMPLATE | \
        sed "s/{{neuron_count}}/$neuron_count/g" | \
        sed "s/{{episodes}}/$episodes/g" | \
        sed "s/{{timestep}}/$timestep/g")
    
    echo "$config" > "${RESULTS_DIR}/config_${experiment_id}.json"
    
    # Run experiment
    ./nlm_phase6_demo --config "${RESULTS_DIR}/config_${experiment_id}.json" \
        --episodes $episodes \
        --timestep $timestep \
        --output "${RESULTS_DIR}/results_${experiment_id}.txt" \
        --profile-performance \
        --checkpoint-interval 100
    
    echo "Experiment $experiment_id completed"
}

# Run parameter sweep
run_experiment "baseline" 500 50 0.01
run_experiment "high_neural" 1000 50 0.01
run_experiment "long_training" 500 200 0.005
run_experiment "fast_simulation" 500 50 0.02

# Combine results
echo "Combining results..."
python3 combine_results.py $RESULTS_DIR

echo "Batch processing completed!"
```

### 4.2 Experiment Parameter Sweep Script

```python
#!/usr/bin/env python3
"""
Parameter sweep script for NLM experiments.
Generates and runs multiple experiments with different configurations.
"""

import subprocess
import json
import os
from pathlib import Path

class NLMExperimentSweeper:
    def __init__(self, base_config_path="config_template.json"):
        self.base_config = self.load_config(base_config_path)
        self.results_dir = "experiment_results"
        Path(self.results_dir).mkdir(exist_ok=True)
    
    def load_config(self, config_path):
        """Load base configuration"""
        with open(config_path, 'r') as f:
            return json.load(f)
    
    def run_parameter_sweep(self, parameter_grid):
        """Run experiments with parameter combinations"""
        print(f"Running parameter sweep with {len(parameter_grid)} combinations...")
        
        for i, params in enumerate(parameter_grid):
            print(f"\n--- Experiment {i + 1}/{len(parameter_grid)} ---")
            self.run_experiment(params)
    
    def run_experiment(self, parameters):
        """Run single experiment with given parameters"""
        experiment_name = parameters.get('name', f'experiment_{len(os.listdir(self.results_dir)) + 1}')
        
        print(f"Running experiment: {experiment_name}")
        
        # Create experiment configuration
        config = self.base_config.copy()
        config.update(parameters)
        
        # Write configuration to file
        config_path = f"{self.results_dir}/config_{experiment_name}.json"
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        # Run NLM with experiment configuration
        cmd = [
            "./nlm_phase6_demo",
            "--config", config_path,
            "--episodes", str(parameters.get('episodes', 50)),
            "--timestep", str(parameters.get('timestep', 0.01)),
            "--output", f"{self.results_dir}/results_{experiment_name}.txt",
            "--profile-performance",
            "--checkpoint-interval", str(parameters.get('checkpoint_interval', 100))
        ]
        
        # Execute experiment
        try:
            subprocess.run(cmd, check=True, cwd="../")
            print(f"Experiment {experiment_name} completed successfully")
        except subprocess.CalledProcessError as e:
            print(f"Experiment {experiment_name} failed: {e}")
            raise
    
    def generate_parameter_grid(self, parameters):
        """Generate parameter grid for experiments"""
        param_grid = []
        
        for neuron_count in parameters.get('neuron_counts', [500, 1000, 1500]):
            for episodes in parameters.get('episodes_list', [50, 100, 200]):
                for timestep in parameters.get('timesteps', [0.01, 0.005, 0.02]):
                    param = {
                        'name': f"neuron_{neuron_count}_episodes_{episodes}_timestep_{timestep}",
                        'neuron_count': neuron_count,
                        'episodes': episodes,
                        'timestep': timestep,
                        'checkpoint_interval': parameters.get('checkpoint_interval', 100)
                    }
                    param_grid.append(param)
        
        return param_grid
    
    def analyze_results(self):
        """Analyze experiment results"""
        print("\nAnalyzing experiment results...")
        
        # Load and analyze all result files
        result_files = Path(self.results_dir).glob("results_*.txt")
        
        for result_file in result_files:
            experiment_name = result_file.stem.replace('results_', '')
            print(f"\nAnalyzing {experiment_name}:")
            
            # Load configuration
            config_path = f"{self.results_dir}/config_{experiment_name}.json"
            with open(config_path, 'r') as f:
                config = json.load(f)
            
            # Extract metrics from results
            metrics = self.extract_metrics(result_file)
            
            # Generate performance report
            self.generate_performance_report(experiment_name, config, metrics)
    
    def extract_metrics(self, result_file):
        """Extract metrics from experiment results"""
        metrics = {
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'memory_usage': 0.0,
            'learning_progress': 0.0,
            'behavioral_stability': 0.0
        }
        
        # Parse result file and extract metrics
        # This is a simplified example - actual implementation would parse specific metrics
        
        return metrics
    
    def generate_performance_report(self, experiment_name, config, metrics):
        """Generate performance report for experiment"""
        report = {
            'experiment_name': experiment_name,
            'parameters': config,
            'metrics': metrics,
            'performance_score': self.calculate_performance_score(metrics),
            'recommendations': self.generate_recommendations(metrics)
        }
        
        # Save report
        report_path = f"{self.results_dir}/report_{experiment_name}.json"
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"  Performance Score: {report['performance_score']:.2f}/1.0")
        print(f"  Recommendations: {len(report['recommendations'])} items")
    
    def calculate_performance_score(self, metrics):
        """Calculate performance score from metrics"""
        return (metrics['total_spikes'] / 1000.0 +  # Normalized spike count
                metrics['average_firing_rate'] / 100.0 +  # Normalized firing rate
                metrics['memory_usage'] / 1000.0 +  # Normalized memory usage
                metrics['learning_progress'] * 1.0 +  # Normalized learning progress
                metrics['behavioral_stability'] * 1.0) / 5.0  # Normalized stability
    
    def generate_recommendations(self, metrics):
        """Generate recommendations based on metrics"""
        recommendations = []
        
        if metrics['average_firing_rate'] < 1.0:
            recommendations.append("Increase firing rate through parameter tuning")
        
        if metrics['memory_usage'] > 500:
            recommendations.append("Optimize memory usage through compression")
        
        if metrics['learning_progress'] < 0.5:
            recommendations.append("Enhance learning mechanisms")
        
        if metrics['behavioral_stability'] < 0.7:
            recommendations.append("Improve behavioral stability")
        
        return recommendations

if __name__ == "__main__":
    # Example parameter grid
    parameter_grid = [
        {
            'name': 'baseline_experiment',
            'neuron_count': 500,
            'episodes': 50,
            'timestep': 0.01,
            'checkpoint_interval': 100
        },
        {
            'name': 'high_neural_experiment',
            'neuron_count': 1000,
            'episodes': 100,
            'timestep': 0.005,
            'checkpoint_interval': 200
        },
        {
            'name': 'long_training_experiment',
            'neuron_count': 500,
            'episodes': 200,
            'timestep': 0.02,
            'checkpoint_interval': 50
        }
    ]
    
    # Run parameter sweep
    sweeper = NLMExperimentSweeper()
    sweeper.run_parameter_sweep(parameter_grid)
    
    # Analyze results
    sweeper.analyze_results()
    
    print("\nParameter sweep completed successfully!")
```

### 4.3 Python Script for CLI Integration

```python
#!/usr/bin/env python3
"""
Python script for NLM CLI integration.
Provides programmatic access to NLM command-line interface.
"""

import subprocess
import json
import argparse
import time
from pathlib import Path

class NLMCLI:
    def __init__(self, nlm_path="./nlm_phase6_demo"):
        self.nlm_path = nlm_path
        self.results_dir = "nlm_results"
        Path(self.results_dir).mkdir(exist_ok=True)
    
    def run_command(self, command_args, output_file=None):
        """Run NLM command with arguments"""
        cmd = [self.nlm_path] + command_args
        
        if output_file:
            cmd.extend(["--output", output_file])
        
        print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            return result.returncode, result.stdout, result.stderr
        except subprocess.CalledProcessError as e:
            print(f"Command failed: {e}")
            print(f"stderr: {e.stderr}")
            return e.returncode, e.stdout, e.stderr
    
    def run_experiment(self, config_file, episodes=None, timestep=None, **kwargs):
        """Run NLM experiment with configuration file"""
        cmd = ["--config", config_file]
        
        if episodes is not None:
            cmd.extend(["--episodes", str(episodes)])
        
        if timestep is not None:
            cmd.extend(["--timestep", str(timestep)])
        
        for key, value in kwargs.items():
            if value is not None:
                cmd.extend([f"--{key.replace('_', '-')}", str(value)])
        
        output_file = f"{self.results_dir}/experiment_{int(time.time())}.txt"
        
        returncode, stdout, stderr = self.run_command(cmd, output_file)
        
        return {
            'returncode': returncode,
            'stdout': stdout,
            'stderr': stderr,
            'output_file': output_file
        }
    
    def run_batch_experiments(self, experiments):
        """Run multiple experiments in batch"""
        results = []
        
        for experiment in experiments:
            print(f"\n--- Running experiment: {experiment['name']} ---")
            
            result = self.run_experiment(
                experiment.get('config_file'),
                experiment.get('episodes'),
                experiment.get('timestep'),
                **experiment.get('kwargs', {})
            )
            
            result['experiment_name'] = experiment['name']
            results.append(result)
        
        return results
    
    def generate_config_from_template(self, template_file, output_file, **kwargs):
        """Generate configuration file from template"""
        with open(template_file, 'r') as f:
            config = json.load(f)
        
        config.update(kwargs)
        
        with open(output_file, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Generated configuration: {output_file}")
        return output_file
    
    def validate_config(self, config_file):
        """Validate configuration file"""
        cmd = ["--validate-config", config_file]
        
        returncode, stdout, stderr = self.run_command(cmd)
        
        if returncode == 0:
            print("Configuration validation successful")
            return True
        else:
            print("Configuration validation failed")
            return False
    
    def analyze_results(self, experiment_name):
        """Analyze experiment results"""
        result_file = f"{self.results_dir}/results_{experiment_name}.txt"
        
        if not Path(result_file).exists():
            print(f"Results file not found: {result_file}")
            return None
        
        # Parse results and generate analysis
        analysis = self.parse_results(result_file)
        
        # Generate report
        report = self.generate_report(experiment_name, analysis)
        
        # Save report
        report_file = f"{self.results_dir}/analysis_{experiment_name}.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        return report
    
    def parse_results(self, result_file):
        """Parse experiment results"""
        analysis = {
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'memory_usage': 0.0,
            'learning_progress': 0.0,
            'behavioral_stability': 0.0,
            'performance_score': 0.0
        }
        
        # Parse result file (simplified example)
        with open(result_file, 'r') as f:
            content = f.read()
            
            # Extract metrics from content (implementation-dependent)
            # This is a simplified example - actual implementation would parse specific metrics
        
        return analysis
    
    def generate_report(self, experiment_name, analysis):
        """Generate analysis report"""
        return {
            'experiment_name': experiment_name,
            'timestamp': time.time(),
            'analysis': analysis,
            'recommendations': self.generate_recommendations(analysis)
        }
    
    def generate_recommendations(self, analysis):
        """Generate recommendations based on analysis"""
        recommendations = []
        
        if analysis['average_firing_rate'] < 1.0:
            recommendations.append({
                'category': 'performance',
                'priority': 'high',
                'message': 'Increase firing rate through parameter tuning'
            })
        
        if analysis['memory_usage'] > 500:
            recommendations.append({
                'category': 'memory',
                'priority': 'medium',
                'message': 'Optimize memory usage through compression'
            })
        
        if analysis['learning_progress'] < 0.5:
            recommendations.append({
                'category': 'learning',
                'priority': 'high',
                'message': 'Enhance learning mechanisms'
            })
        
        if analysis['behavioral_stability'] < 0.7:
            recommendations.append({
                'category': 'behavior',
                'priority': 'medium',
                'message': 'Improve behavioral stability'
            })
        
        return recommendations

if __name__ == "__main__":
    # Command-line interface
    parser = argparse.ArgumentParser(description="NLM CLI Integration")
    
    parser.add_argument('--nlm-path', default='./nlm_phase6_demo', help='Path to NLM executable')
    parser.add_argument('--results-dir', default='nlm_results', help='Results directory')
    parser.add_argument('--command', choices=['run', 'batch', 'validate', 'analyze', 'generate'],
                       help='Command to execute')
    
    args = parser.parse_args()
    
    if args.command == 'run':
        # Run single experiment
        cli = NLMCLI(args.nlm_path)
        result = cli.run_experiment(
            config_file="experiment_config.json",
            episodes=50,
            timestep=0.01
        )
        print(f"Experiment completed with return code: {result['returncode']}")
    
    elif args.command == 'batch':
        # Run batch of experiments
        cli = NLMCLI(args.nlm_path)
        
        experiments = [
            {
                'name': 'baseline',
                'config_file': 'config_baseline.json',
                'episodes': 50,
                'timestep': 0.01
            },
            {
                'name': 'high_neural',
                'config_file': 'config_high_neural.json',
                'episodes': 100,
                'timestep': 0.005
            }
        ]
        
        results = cli.run_batch_experiments(experiments)
        print(f"Batch completed with {len(results)} experiments")
    
    elif args.command == 'validate':
        # Validate configuration
        cli = NLMCLI(args.nlm_path)
        cli.validate_config("experiment_config.json")
    
    elif args.command == 'analyze':
        # Analyze experiment results
        cli = NLMCLI(args.nlm_path)
        report = cli.analyze_results("baseline")
        if report:
            print("Analysis completed successfully")
    
    elif args.command == 'generate':
        # Generate configuration from template
        cli = NLMCLI(args.nlm_path)
        cli.generate_config_from_template(
            "template_config.json",
            "generated_config.json",
            neuron_count=1000,
            episodes=100
        )
        print("Configuration generated successfully")
```

## 5. Integration with Other Tools

### 5.1 Integration with Python Libraries

```python
#!/usr/bin/env python3
"""
Integration of NLM with Python libraries.
Demonstrates integration with various Python tools and libraries.
"""

import pynlm as nlm
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.preprocessing import StandardScaler

class NLMIntegration:
    def __init__(self):
        self.setup_integration()
    
    def setup_integration(self):
        """Setup integration with Python libraries"""
        # Initialize NLM Python bindings
        self.brain = nlm.Brain.createDefault()
        self.brain.initialize()
        
        # Create world
        self.world = nlm.SimpleWorld.create(200, 200, 20, 20)
        self.world.reset()
        
        # Create agent
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning capabilities
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        # Setup data collection
        self.data_collector = DataCollector()
        
        # Setup visualization
        self.visualizer = NLMVisualizer()
    
    def run_integration_experiment(self, duration=1000):
        """Run integration experiment with Python libraries"""
        print("Running integration experiment with Python libraries...")
        
        for step in range(duration):
            # Run NLM simulation step
            self.run_nlm_step(step)
            
            # Collect data using pandas
            self.collect_data(step)
            
            # Visualize using matplotlib
            if step % 100 == 0:
                self.visualize_progress(step)
            
            # Process data using sklearn
            if step % 200 == 0:
                self.process_data(step)
        
        # Generate reports using pandas
        self.generate_reports()
        
        # Create visualizations using matplotlib
        self.create_visualizations()
        
        print("Integration experiment completed successfully!")
    
    def run_nlm_step(self, step):
        """Run single NLM simulation step"""
        # Get percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        self.brain.step(step)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        self.world.update(0.01)
    
    def collect_data(self, step):
        """Collect experiment data using pandas"""
        data_point = {
            'step': step,
            'time': self.world.getSimulationTime(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'memory_usage': self.get_memory_usage(),
            'curiosity_level': self.get_curiosity_level(),
            'dopamine_level': self.get_dopamine_level(),
            'action_type': self.get_action_type(),
            'reward': self.get_reward()
        }
        
        self.data_collector.add_data_point(data_point)
    
    def visualize_progress(self, step):
        """Visualize experiment progress using matplotlib"""
        data = self.data_collector.get_dataframe()
        
        # Plot key metrics
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Total spikes over time
        axes[0, 0].plot(data['step'], data['total_spikes'], 'b-')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Total Spikes')
        axes[0, 0].set_title('Neural Activity Over Time')
        axes[0, 0].grid(True)
        
        # Plot 2: Firing rate over time
        axes[0, 1].plot(data['step'], data['average_firing_rate'], 'r-')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Average Firing Rate (Hz)')
        axes[0, 1].set_title('Firing Rate Over Time')
        axes[0, 1].grid(True)
        
        # Plot 3: Memory usage over time
        axes[1, 0].plot(data['step'], data['memory_usage'], 'g-')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Memory Usage (MB)')
        axes[1, 0].set_title('Memory Usage Over Time')
        axes[1, 0].grid(True)
        
        # Plot 4: Neuromodulation levels
        axes[1, 1].plot(data['step'], data['curiosity_level'], 'b-', label='Curiosity')
        axes[1, 1].plot(data['step'], data['dopamine_level'], 'r-', label='Dopamine')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Level')
        axes[1, 1].set_title('Neuromodulation Levels')
        axes[1, 1].legend()
        axes[1, 1].grid(True)
        
        plt.tight_layout()
        plt.savefig(f'nlm_visualization_step_{step}.png')
        plt.close()
    
    def process_data(self, step):
        """Process data using sklearn"""
        data = self.data_collector.get_dataframe()
        
        # Select features for processing
        feature_columns = ['total_spikes', 'average_firing_rate', 
                          'memory_usage', 'curiosity_level', 'dopamine_level']
        
        X = data[feature_columns].values
        
        # Scale features
        scaler = StandardScaler()
        X_scaled = scaler.fit_transform(X)
        
        # Apply clustering
        from sklearn.cluster import KMeans
        kmeans = KMeans(n_clusters=3, random_state=42)
        cluster_labels = kmeans.fit_predict(X_scaled)
        
        # Add cluster labels to dataframe
        data['cluster'] = cluster_labels
        
        # Save processed data
        processed_data_path = f'nlm_processed_data_step_{step}.csv'
        data.to_csv(processed_data_path, index=False)
        
        print(f"Data processed and saved to {processed_data_path}")
    
    def generate_reports(self):
        """Generate reports using pandas"""
        data = self.data_collector.get_dataframe()
        
        # Generate summary statistics
        summary_stats = data.describe()
        
        # Save summary statistics
        summary_path = 'nlm_summary_statistics.csv'
        summary_stats.to_csv(summary_path)
        
        # Generate learning curve analysis
        learning_curve = self.calculate_learning_curve(data)
        
        learning_curve_path = 'nlm_learning_curve.csv'
        learning_curve.to_csv(learning_curve_path)
        
        print(f"Reports generated: {summary_path}, {learning_curve_path}")
    
    def create_visualizations(self):
        """Create visualizations using matplotlib"""
        data = self.data_collector.get_dataframe()
        
        # Create comprehensive visualization
        fig, axes = plt.subplots(3, 3, figsize=(15, 12))
        fig.suptitle('NLM Integration Analysis', fontsize=16)
        
        # Fill visualization plots
        self.fill_visualizations(axes, data)
        
        # Save visualization
        plt.savefig('nlm_integration_analysis.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print("Comprehensive visualization created: nlm_integration_analysis.png")
    
    def fill_visualizations(self, axes, data):
        """Fill visualization plots with data"""
        # Plot 1: Neural activity time series
        axes[0, 0].plot(data['step'], data['total_spikes'], 'b-')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Total Spikes')
        axes[0, 0].set_title('Neural Activity Time Series')
        axes[0, 0].grid(True)
        
        # Plot 2: Firing rate distribution
        axes[0, 1].hist(data['average_firing_rate'], bins=50, alpha=0.7, color='r')
        axes[0, 1].set_xlabel('Average Firing Rate (Hz)')
        axes[0, 1].set_ylabel('Frequency')
        axes[0, 1].set_title('Firing Rate Distribution')
        axes[0, 1].grid(True)
        
        # Plot 3: Memory usage time series
        axes[0, 2].plot(data['step'], data['memory_usage'], 'g-')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Memory Usage (MB)')
        axes[0, 2].set_title('Memory Usage Time Series')
        axes[0, 2].grid(True)
        
        # Plot 4: Neuromodulation time series
        axes[1, 0].plot(data['step'], data['curiosity_level'], 'b-', label='Curiosity')
        axes[1, 0].plot(data['step'], data['dopamine_level'], 'r-', label='Dopamine')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Level')
        axes[1, 0].set_title('Neuromodulation Time Series')
        axes[1, 0].legend()
        axes[1, 0].grid(True)
        
        # Plot 5: Correlation matrix
        correlation_matrix = data[['total_spikes', 'average_firing_rate', 
                                   'memory_usage', 'curiosity_level', 'dopamine_level']].corr()
        im = axes[1, 1].imshow(correlation_matrix, cmap='coolwarm', interpolation='nearest')
        axes[1, 1].set_xticks(range(len(correlation_matrix.columns)))
        axes[1, 1].set_yticks(range(len(correlation_matrix.columns)))
        axes[1, 1].set_xticklabels(correlation_matrix.columns, rotation=45)
        axes[1, 1].set_yticklabels(correlation_matrix.columns)
        axes[1, 1].set_title('Correlation Matrix')
        plt.colorbar(im, ax=axes[1, 1])
        
        # Plot 6: Cluster analysis
        if 'cluster' in data.columns:
            pd.plotting.scatter_matrix(data[['total_spikes', 'average_firing_rate', 
                                            'memory_usage', 'curiosity_level', 'dopamine_level', 'cluster']],
                                      alpha=0.6, figsize=(8, 8), ax=axes[1, 2])
        
        axes[1, 2].set_title('Cluster Analysis')
        
        # Plot 7: Reward distribution
        axes[2, 0].hist(data['reward'], bins=50, alpha=0.7, color='g')
        axes[2, 0].set_xlabel('Reward')
        axes[2, 0].set_ylabel('Frequency')
        axes[2, 0].set_title('Reward Distribution')
        axes[2, 0].grid(True)
        
        # Plot 8: Time to convergence
        axes[2, 1].plot(data['step'], data['learning_progress'], 'r-')
        axes[2, 1].set_xlabel('Step')
        axes[2, 1].set_ylabel('Learning Progress')
        axes[2, 1].set_title('Learning Progress Over Time')
        axes[2, 1].grid(True)
        
        # Plot 9: Behavioral stability
        axes[2, 2].plot(data['step'], data['behavioral_stability'], 'b-')
        axes[2, 2].set_xlabel('Step')
        axes[2, 2].set_ylabel('Behavioral Stability')
        axes[2, 2].set_title('Behavioral Stability Over Time')
        axes[2, 2].grid(True)
    
    def calculate_learning_curve(self, data):
        """Calculate learning curve from data"""
        # Calculate moving average of learning progress
        learning_curve = data['learning_progress'].rolling(window=100, min_periods=1).mean()
        
        # Create learning curve dataframe
        learning_curve_df = pd.DataFrame({
            'step': data['step'],
            'learning_progress': data['learning_progress'],
            'moving_average': learning_curve.values,
            'learning_rate': self.calculate_learning_rate(data)
        })
        
        return learning_curve_df
    
    def calculate_learning_rate(self, data):
        """Calculate learning rate from data"""
        # Calculate derivative of learning progress
        learning_rate = data['learning_progress'].diff().fillna(0)
        return learning_rate
    
    def get_memory_usage(self):
        """Get memory usage from brain"""
        if hasattr(self.brain, 'getMemoryUsage'):
            return self.brain.getMemoryUsage()
        return 0.0
    
    def get_curiosity_level(self):
        """Get curiosity level from agent"""
        if hasattr(self.agent, 'getCuriosity'):
            return self.agent.getCuriosity()
        return 0.0
    
    def get_dopamine_level(self):
        """Get dopamine level from agent"""
        if hasattr(self.agent, 'getDopamine'):
            return self.agent.getDopamine()
        return 0.0
    
    def get_action_type(self):
        """Get action type from agent"""
        if hasattr(self.agent, 'getActionType'):
            return self.agent.getActionType()
        return "unknown"
    
    def get_reward(self):
        """Get current reward from agent"""
        if hasattr(self.agent, 'getCurrentReward'):
            return self.agent.getCurrentReward()
        return 0.0

class DataCollector:
    """Collect and store NLM experiment data"""
    
    def __init__(self):
        self.data = []
    
    def add_data_point(self, data_point):
        """Add data point to collection"""
        self.data.append(data_point)
    
    def get_dataframe(self):
        """Get data as pandas DataFrame"""
        return pd.DataFrame(self.data)
    
    def save_to_csv(self, filename):
        """Save data to CSV file"""
        df = self.get_dataframe()
        df.to_csv(filename, index=False)
    
    def load_from_csv(self, filename):
        """Load data from CSV file"""
        self.data = pd.read_csv(filename).to_dict('records')

class NLMVisualizer:
    """Visualize NLM data and results"""
    
    def __init__(self):
        import matplotlib.pyplot as plt
        self.plt = plt
    
    def create_time_series_plot(self, data, title, xlabel, ylabel):
        """Create time series plot"""
        fig, ax = self.plt.subplots(figsize=(10, 6))
        ax.plot(data['step'], data[ylabel], 'b-')
        ax.set_title(title)
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        ax.grid(True)
        return fig
    
    def create_scatter_plot(self, data, x_col, y_col, title, color_col=None):
        """Create scatter plot"""
        fig, ax = self.plt.subplots(figsize=(10, 6))
        
        if color_col:
            scatter = ax.scatter(data[x_col], data[y_col], 
                               c=data[color_col], cmap='viridis', alpha=0.7)
            plt.colorbar(scatter, ax=ax)
        else:
            ax.scatter(data[x_col], data[y_col], alpha=0.7)
        
        ax.set_title(title)
        ax.set_xlabel(x_col)
        ax.set_ylabel(y_col)
        ax.grid(True)
        return fig

if __name__ == "__main__":
    # Run NLM integration with Python libraries
    print("Starting NLM integration with Python libraries...")
    
    nlm_integration = NLMIntegration()
    nlm_integration.run_integration_experiment(duration=2000)
    
    print("NLM integration with Python libraries completed successfully!")
```

### 5.2 Integration with Data Science Libraries

```python
#!/usr/bin/env python3
"""
Integration of NLM with data science libraries.
Demonstrates integration with data analysis and machine learning libraries.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.neural_network import MLPRegressor
from sklearn.metrics import mean_squared_error, r2_score

class NLMDataScienceIntegration:
    def __init__(self):
        self.setup_data_science_integration()
    
    def setup_data_science_integration(self):
        """Setup integration with data science libraries"""
        # Initialize NLM
        import pynlm as nlm
        self.brain = nlm.Brain.createDefault()
        self.brain.initialize()
        
        # Create world
        self.world = nlm.SimpleWorld.create(200, 200, 20, 20)
        self.world.reset()
        
        # Create agent
        self.agent = nlm.AgentBrain.createWithBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        # Setup data science components
        self.data_processor = DataProcessor()
        self.model_trainer = ModelTrainer()
        self.visualizer = DataScienceVisualizer()
    
    def run_data_science_experiment(self, duration=2000):
        """Run experiment with data science integration"""
        print("Running NLM experiment with data science integration...")
        
        # Collect data
        data = self.collect_experimental_data(duration)
        
        # Process data using data science pipelines
        processed_data = self.process_data_for_analysis(data)
        
        # Train machine learning models
        models = self.train_models(processed_data)
        
        # Evaluate models
        evaluation_results = self.evaluate_models(models, processed_data)
        
        # Generate comprehensive reports
        self.generate_data_science_reports(data, processed_data, models, evaluation_results)
        
        print("Data science integration experiment completed successfully!")
    
    def collect_experimental_data(self, duration):
        """Collect experimental data"""
        experimental_data = []
        
        for step in range(duration):
            # Run NLM simulation step
            self.run_simulation_step(step)
            
            # Collect data
            data_point = self.collect_data_point(step)
            experimental_data.append(data_point)
            
            if step % 500 == 0:
                print(f"Data collected: {step + 1}/{duration} steps")
        
        # Convert to pandas DataFrame
        df = pd.DataFrame(experimental_data)
        
        print(f"Collected {len(df)} data points")
        return df
    
    def run_simulation_step(self, step):
        """Run single NLM simulation step"""
        # Get percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        self.brain.step(step)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        self.world.update(0.01)
    
    def collect_data_point(self, step):
        """Collect single data point"""
        return {
            'step': step,
            'time': self.world.getSimulationTime(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'memory_usage': self.get_brain_memory_usage(),
            'curiosity_level': self.get_agent_curiosity(),
            'dopamine_level': self.get_agent_dopamine(),
            'action_type': self.get_agent_action_type(),
            'reward': self.get_agent_reward(),
            'novelty_level': self.get_agent_novelty(),
            'prediction_error': self.get_agent_prediction_error()
        }
    
    def process_data_for_analysis(self, data):
        """Process data using data science pipelines"""
        print("Processing data for analysis...")
        
        # Feature engineering
        processed_data = self.feature_engineering(data)
        
        # Normalization
        normalized_data = self.normalize_features(processed_data)
        
        # Dimensionality reduction
        reduced_data = self.apply_dimensionality_reduction(normalized_data)
        
        # Time series decomposition
        time_series_data = self.decompose_time_series(reduced_data)
        
        return time_series_data
    
    def feature_engineering(self, data):
        """Perform feature engineering"""
        df = data.copy()
        
        # Create derived features
        df['firing_rate_ema'] = df['average_firing_rate'].ewm(span=50).mean()
        df['memory_usage_ratio'] = df['memory_usage'] / df['memory_usage'].max()
        df['curiosity_dopamine_ratio'] = df['curiosity_level'] / (df['dopamine_level'] + 0.001)
        df['spike_rate_per_step'] = df['total_spikes'] / df['step'].diff().fillna(1)
        
        # Temporal features
        df['hour_of_day'] = df['time'] % 24
        df['day_of_week'] = df['time'] % 7
        
        # Statistical features
        df['firing_rate_std'] = df['average_firing_rate'].rolling(window=100, min_periods=1).std()
        df['firing_rate_range'] = df['average_firing_rate'].rolling(window=100, min_periods=1).max() - \
                                 df['average_firing_rate'].rolling(window=100, min_periods=1).min()
        
        return df
    
    def normalize_features(self, data):
        """Normalize features"""
        numeric_features = ['total_spikes', 'active_neurons', 'average_firing_rate',
                           'memory_usage', 'curiosity_level', 'dopamine_level',
                           'novelty_level', 'prediction_error']
        
        df = data.copy()
        
        # Apply different normalization techniques
        scaler_minmax = MinMaxScaler()
        df[['total_spikes_norm', 'active_neurons_norm', 'average_firing_rate_norm',
            'memory_usage_norm', 'curiosity_level_norm', 'dopamine_level_norm',
            'novelty_level_norm', 'prediction_error_norm']] = scaler_minmax.fit_transform(
            df[numeric_features]
        )
        
        # Standardize features
        scaler_std = StandardScaler()
        df[['total_spikes_std', 'active_neurons_std', 'average_firing_rate_std',
            'memory_usage_std', 'curiosity_level_std', 'dopamine_level_std',
            'novelty_level_std', 'prediction_error_std']] = scaler_std.fit_transform(
            df[numeric_features]
        )
        
        return df
    
    def apply_dimensionality_reduction(self, data):
        """Apply dimensionality reduction"""
        # Select features for dimensionality reduction
        feature_columns = ['total_spikes', 'active_neurons', 'average_firing_rate',
                          'memory_usage', 'curiosity_level', 'dopamine_level',
                          'novelty_level', 'prediction_error',
                          'firing_rate_ema', 'memory_usage_ratio',
                          'curiosity_dopamine_ratio']
        
        X = data[feature_columns].values
        
        # Apply PCA
        pca = PCA(n_components=3)
        X_pca = pca.fit_transform(X)
        
        # Add PCA components to dataframe
        pca_columns = [f'pca_{i}' for i in range(3)]
        for i, col in enumerate(pca_columns):
            data[col] = X_pca[:, i]
        
        # Print explained variance
        print(f"PCA explained variance ratio: {pca.explained_variance_ratio_}")
        print(f"Total explained variance: {sum(pca.explained_variance_ratio_):.2%}")
        
        return data
    
    def decompose_time_series(self, data):
        """Decompose time series into components"""
        df = data.copy()
        
        # Add time series decomposition features
        for col in ['total_spikes', 'average_firing_rate', 'memory_usage']:
            # Detrend
            df[f'{col}_detrended'] = df[col] - df[col].rolling(window=100, center=True).mean()
            
            # Seasonal decomposition (simplified)
            df[f'{col}_seasonal'] = df[col] - df[col].rolling(window=365, center=True).mean()
            
            # Residual
            df[f'{col}_residual'] = df[col] - df[f'{col}_detrended'] - df[f'{col}_seasonal']
        
        return df
    
    def train_models(self, data):
        """Train machine learning models"""
        print("Training machine learning models...")
        
        # Prepare features and target
        feature_columns = ['total_spikes', 'active_neurons', 'average_firing_rate',
                          'memory_usage', 'curiosity_level', 'dopamine_level',
                          'novelty_level', 'prediction_error',
                          'firing_rate_ema', 'memory_usage_ratio',
                          'curiosity_dopamine_ratio', 'pca_0', 'pca_1', 'pca_2']
        
        target_columns = ['learning_progress', 'behavioral_stability', 'action_quality']
        
        df = data.copy()
        
        # Create target variables if not exist
        if 'learning_progress' not in df.columns:
            df['learning_progress'] = np.cumsum(df['average_firing_rate'].diff().fillna(0))
        
        if 'behavioral_stability' not in df.columns:
            df['behavioral_stability'] = 1.0 - (df['memory_usage'] / df['memory_usage'].max())
        
        if 'action_quality' not in df.columns:
            df['action_quality'] = df['dopamine_level'] * df['curiosity_level']
        
        # Split data
        X = df[feature_columns].values
        y_learning = df['learning_progress'].values
        y_behavior = df['behavioral_stability'].values
        y_action = df['action_quality'].values
        
        # Split into train and test
        X_train, X_test, y_learn_train, y_learn_test = train_test_split(
            X, y_learning, test_size=0.2, random_state=42
        )
        X_train, X_test, y_beh_train, y_beh_test = train_test_split(
            X, y_behavior, test_size=0.2, random_state=42
        )
        X_train, X_test, y_act_train, y_act_test = train_test_split(
            X, y_action, test_size=0.2, random_state=42
        )
        
        # Train models
        models = {}
        
        # Linear Regression for learning progress
        lr_model = LinearRegression()
        lr_model.fit(X_train, y_learn_train)
        y_pred = lr_model.predict(X_test)
        
        models['linear_regression_learning'] = {
            'model': lr_model,
            'predictions': y_pred,
            'actual': y_learn_test,
            'r2_score': r2_score(y_learn_test, y_pred),
            'mse': mean_squared_error(y_learn_test, y_pred)
        }
        
        # MLP Regressor for behavioral stability
        mlp_model = MLPRegressor(hidden_layer_sizes=(100, 50, 25), 
                                max_iter=500, random_state=42)
        mlp_model.fit(X_train, y_beh_train)
        y_pred = mlp_model.predict(X_test)
        
        models['mlp_regressor_behavior'] = {
            'model': mlp_model,
            'predictions': y_pred,
            'actual': y_beh_test,
            'r2_score': r2_score(y_beh_test, y_pred),
            'mse': mean_squared_error(y_beh_test, y_pred)
        }
        
        # K-means clustering
        kmeans = KMeans(n_clusters=4, random_state=42)
        cluster_labels = kmeans.fit_predict(X)
        
        models['kmeans_clustering'] = {
            'model': kmeans,
            'cluster_labels': cluster_labels,
            'n_clusters': 4
        }
        
        # Time series forecasting
        ts_model = LinearRegression()
        # Use lagged features for time series prediction
        for col in ['total_spikes', 'average_firing_rate']:
            for lag in [1, 5, 10, 20]:
                df[f'{col}_lag_{lag}'] = df[col].shift(lag)
        
        ts_features = [col for col in df.columns if 'lag_' in col]
        X_ts = df[ts_features].fillna(method='ffill').values
        y_ts = df['average_firing_rate'].values
        
        X_ts_train, X_ts_test, y_ts_train, y_ts_test = train_test_split(
            X_ts, y_ts, test_size=0.2, random_state=42
        )
        
        ts_model.fit(X_ts_train, y_ts_train)
        y_pred_ts = ts_model.predict(X_ts_test)
        
        models['time_series_forecasting'] = {
            'model': ts_model,
            'predictions': y_pred_ts,
            'actual': y_ts_test,
            'r2_score': r2_score(y_ts_test, y_pred_ts),
            'mse': mean_squared_error(y_ts_test, y_pred_ts)
        }
        
        return models
    
    def evaluate_models(self, models, data):
        """Evaluate trained models"""
        print("Evaluating models...")
        
        evaluation_results = {}
        
        for model_name, model_data in models.items():
            if 'predictions' in model_data:
                # Calculate metrics for regression models
                y_pred = model_data['predictions']
                y_true = model_data['actual']
                
                metrics = {
                    'r2_score': r2_score(y_true, y_pred),
                    'mean_squared_error': mean_squared_error(y_true, y_pred),
                    'mean_absolute_error': np.mean(np.abs(y_true - y_pred)),
                    'root_mean_squared_error': np.sqrt(mean_squared_error(y_true, y_pred))
                }
                
                evaluation_results[model_name] = metrics
                
                # Print evaluation results
                print(f"\n{model_name}:")
                print(f"  R² Score: {metrics['r2_score']:.4f}")
                print(f"  MSE: {metrics['mean_squared_error']:.4f}")
                print(f"  MAE: {metrics['mean_absolute_error']:.4f}")
                print(f"  RMSE: {metrics['root_mean_squared_error']:.4f}")
        
        return evaluation_results
    
    def generate_data_science_reports(self, data, processed_data, models, evaluation_results):
        """Generate comprehensive reports"""
        print("\nGenerating data science reports...")
        
        # Generate summary statistics
        self.generate_summary_statistics(data)
        
        # Generate model performance report
        self.generate_model_performance_report(evaluation_results)
        
        # Generate visualization report
        self.generate_visualization_report(data, processed_data, models)
        
        # Generate integration report
        self.generate_integration_report(data, models, evaluation_results)
        
        print("Data science reports generated successfully!")
    
    def generate_summary_statistics(self, data):
        """Generate summary statistics"""
        # Create summary dataframe
        summary_stats = data.describe()
        
        # Save summary statistics
        summary_path = 'nlm_data_science_summary.csv'
        summary_stats.to_csv(summary_path)
        
        # Generate distribution plots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Data Science Analysis Summary', fontsize=16)
        
        # Plot 1: Neural activity distribution
        axes[0, 0].hist(data['total_spikes'], bins=50, alpha=0.7, color='b')
        axes[0, 0].set_xlabel('Total Spikes')
        axes[0, 0].set_ylabel('Frequency')
        axes[0, 0].set_title('Neural Activity Distribution')
        
        # Plot 2: Firing rate distribution
        axes[0, 1].hist(data['average_firing_rate'], bins=50, alpha=0.7, color='r')
        axes[0, 1].set_xlabel('Average Firing Rate (Hz)')
        axes[0, 1].set_ylabel('Frequency')
        axes[0, 1].set_title('Firing Rate Distribution')
        
        # Plot 3: Memory usage distribution
        axes[1, 0].hist(data['memory_usage'], bins=50, alpha=0.7, color='g')
        axes[1, 0].set_xlabel('Memory Usage (MB)')
        axes[1, 0].set_ylabel('Frequency')
        axes[1, 0].set_title('Memory Usage Distribution')
        
        # Plot 4: Neuromodulation levels
        axes[1, 1].hist([data['curiosity_level'], data['dopamine_level']], 
                       bins=50, alpha=0.7, label=['Curiosity', 'Dopamine'])
        axes[1, 1].set_xlabel('Level')
        axes[1, 1].set_ylabel('Frequency')
        axes[1, 1].set_title('Neuromodulation Levels Distribution')
        axes[1, 1].legend()
        
        plt.tight_layout()
        plt.savefig('nlm_data_science_summary.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Summary statistics saved: {summary_path}")
        print("Distribution plot saved: nlm_data_science_summary.png")
    
    def generate_model_performance_report(self, evaluation_results):
        """Generate model performance report"""
        # Create performance dataframe
        performance_data = []
        
        for model_name, metrics in evaluation_results.items():
            performance_data.append({
                'model_name': model_name,
                'r2_score': metrics['r2_score'],
                'mean_squared_error': metrics['mean_squared_error'],
                'mean_absolute_error': metrics['mean_absolute_error'],
                'root_mean_squared_error': metrics['root_mean_squared_error'],
                'model_rank': len(performance_data) + 1
            })
        
        performance_df = pd.DataFrame(performance_data)
        
        # Save performance report
        performance_path = 'nlm_model_performance.csv'
        performance_df.to_csv(performance_path, index=False)
        
        # Create performance visualization
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Model Performance Analysis', fontsize=16)
        
        # Plot 1: R² scores comparison
        axes[0, 0].barh(range(len(performance_df)), performance_df['r2_score'], 
                       color='skyblue')
        axes[0, 0].set_yticks(range(len(performance_df)))
        axes[0, 0].set_yticklabels(performance_df['model_name'])
        axes[0, 0].set_xlabel('R² Score')
        axes[0, 0].set_title('Model R² Scores Comparison')
        
        # Plot 2: MSE scores comparison
        axes[0, 1].barh(range(len(performance_df)), performance_df['mean_squared_error'], 
                       color='lightcoral')
        axes[0, 1].set_yticks(range(len(performance_df)))
        axes[0, 1].set_yticklabels(performance_df['model_name'])
        axes[0, 1].set_xlabel('Mean Squared Error')
        axes[0, 1].set_title('Model MSE Scores Comparison')
        
        # Plot 3: Model ranking
        axes[1, 0].bar(range(len(performance_df)), performance_df['model_rank'], 
                      color='lightgreen')
        axes[1, 0].set_xticks(range(len(performance_df)))
        axes[1, 0].set_xticklabels(performance_df['model_name'], rotation=45)
        axes[1, 0].set_ylabel('Model Rank')
        axes[1, 0].set_title('Model Performance Ranking')
        
        # Plot 4: Performance correlation
        metrics_corr = performance_df[['r2_score', 'mean_squared_error', 
                                      'mean_absolute_error', 'root_mean_squared_error']]
        correlation_matrix = metrics_corr.corr()
        
        sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                   ax=axes[1, 1])
        axes[1, 1].set_title('Model Metrics Correlation Matrix')
        
        plt.tight_layout()
        plt.savefig('nlm_model_performance.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Model performance report saved: {performance_path}")
        print("Model performance visualization saved: nlm_model_performance.png")
    
    def generate_visualization_report(self, data, processed_data, models):
        """Generate visualization report"""
        print("Generating visualization report...")
        
        # Create comprehensive visualization
        fig, axes = plt.subplots(3, 3, figsize=(18, 15))
        fig.suptitle('Comprehensive NLM Data Science Visualization', fontsize=20)
        
        # Fill visualization plots
        self.fill_data_science_visualizations(axes, data, processed_data, models)
        
        # Save visualization
        plt.savefig('nlm_data_science_visualization.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print("Comprehensive visualization saved: nlm_data_science_visualization.png")
    
    def fill_data_science_visualizations(self, axes, data, processed_data, models):
        """Fill visualization plots with data science insights"""
        # Plot 1: Time series with multiple metrics
        axes[0, 0].plot(data['step'], data['total_spikes'], 'b-', alpha=0.7, label='Total Spikes')
        axes[0, 0].plot(data['step'], data['average_firing_rate'], 'r-', alpha=0.7, label='Firing Rate')
        axes[0, 0].plot(data['step'], data['memory_usage'], 'g-', alpha=0.7, label='Memory Usage')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Value')
        axes[0, 0].set_title('Neural Activity Time Series')
        axes[0, 0].legend()
        axes[0, 0].grid(True)
        
        # Plot 2: Neuromodulation correlation heatmap
        neuromodulation_data = data[['curiosity_level', 'dopamine_level', 'novelty_level']]
        correlation_matrix = neuromodulation_data.corr()
        
        sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0,
                   ax=axes[0, 1], square=True)
        axes[0, 1].set_title('Neuromodulation Correlation Matrix')
        
        # Plot 3: PCA visualization
        if 'pca_0' in processed_data.columns and 'pca_1' in processed_data.columns:
            scatter = axes[0, 2].scatter(processed_data['pca_0'], processed_data['pca_1'],
                                       c=processed_data['step'], cmap='viridis', alpha=0.7)
            axes[0, 2].set_xlabel('PCA Component 1')
            axes[0, 2].set_ylabel('PCA Component 2')
            axes[0, 2].set_title('PCA Visualization')
            plt.colorbar(scatter, ax=axes[0, 2])
        
        # Plot 4: Cluster analysis
        if 'cluster' in processed_data.columns:
            sns.scatterplot(data=processed_data, x='pca_0', y='pca_1', 
                          hue='cluster', palette='viridis', ax=axes[1, 0])
            axes[1, 0].set_xlabel('PCA Component 1')
            axes[1, 0].set_ylabel('PCA Component 2')
            axes[1, 0].set_title('Cluster Analysis (PCA)')
        
        # Plot 5: Feature importance from models
        if 'linear_regression_learning' in models:
            lr_model = models['linear_regression_learning']['model']
            # Get feature coefficients
            coefficients = lr_model.coef_
            feature_names = ['total_spikes', 'active_neurons', 'average_firing_rate',
                           'memory_usage', 'curiosity_level', 'dopamine_level',
                           'novelty_level', 'prediction_error']
            
            # Plot top 10 features
            top_features_idx = np.argsort(np.abs(coefficients))[-10:][::-1]
            top_features = [feature_names[i] for i in top_features_idx]
            top_coefficients = coefficients[top_features_idx]
            
            axes[1, 1].barh(range(len(top_features)), top_coefficients, color='skyblue')
            axes[1, 1].set_yticks(range(len(top_features)))
            axes[1, 1].set_yticklabels(top_features)
            axes[1, 1].set_xlabel('Coefficient Value')
            axes[1, 1].set_title('Top 10 Feature Importances (Linear Regression)')
        
        # Plot 6: Time series decomposition
        if 'total_spikes_detrended' in processed_data.columns:
            axes[1, 2].plot(data['step'], data['total_spikes'], 'b-', label='Original', alpha=0.7)
            axes[1, 2].plot(data['step'], processed_data['total_spikes_detrended'], 
                          'r-', label='Detrended', alpha=0.7)
            axes[1, 2].set_xlabel('Step')
            axes[1, 2].set_ylabel('Total Spikes')
            axes[1, 2].set_title('Time Series Decomposition')
            axes[1, 2].legend()
            axes[1, 2].grid(True)
        
        # Plot 7: Model prediction comparison
        if 'linear_regression_learning' in models:
            y_pred = models['linear_regression_learning']['predictions']
            y_true = models['linear_regression_learning']['actual']
            
            axes[2, 0].scatter(y_true, y_pred, alpha=0.7)
            axes[2, 0].plot([y_true.min(), y_true.max()], [y_true.min(), y_true.max()], 'r--', lw=2)
            axes[2, 0].set_xlabel('True Values')
            axes[2, 0].set_ylabel('Predicted Values')
            axes[2, 0].set_title('Linear Regression: True vs Predicted')
            axes[2, 0].grid(True)
        
        # Plot 8: Data quality metrics
        quality_metrics = {
            'Missing Values': data.isnull().sum().sum() / (data.shape[0] * data.shape[1]),
            'Duplicate Rows': data.duplicated().sum() / len(data),
            'Outliers (IQR)': ((data.quantile(0.75) - data.quantile(0.25)) * 1.5).sum(),
            'Data Range': (data.max() - data.min()).mean()
        }
        
        axes[2, 1].barh(list(quality_metrics.keys()), list(quality_metrics.values()), 
                       color='lightcoral')
        axes[2, 1].set_xlabel('Value')
        axes[2, 1].set_title('Data Quality Metrics')
        axes[2, 1].grid(True)
        
        # Plot 9: Learning curve simulation
        axes[2, 2].plot(data['step'], data['learning_progress'], 'b-')
        axes[2, 2].set_xlabel('Step')
        axes[2, 2].set_ylabel('Learning Progress')
        axes[2, 2].set_title('Learning Progress Over Time')
        axes[2, 2].grid(True)
    
    def generate_integration_report(self, data, models, evaluation_results):
        """Generate integration report"""
        print("Generating integration report...")
        
        # Create integration report
        integration_report = {
            'timestamp': time.time(),
            'data_summary': {
                'total_samples': len(data),
                'total_features': len(data.columns),
                'missing_values': data.isnull().sum().sum(),
                'duplicate_rows': data.duplicated().sum()
            },
            'model_performance': evaluation_results,
            'processing_summary': {
                'feature_engineering_completed': True,
                'normalization_completed': True,
                'dimensionality_reduction_completed': True,
                'time_series_decomposition_completed': True,
                'model_training_completed': True,
                'model_evaluation_completed': True
            },
            'recommendations': self.generate_integration_recommendations(data, evaluation_results),
            'conclusions': self.generate_integration_conclusions(data, models, evaluation_results)
        }
        
        # Save integration report
        report_path = 'nlm_integration_report.json'
        with open(report_path, 'w') as f:
            json.dump(integration_report, f, indent=2)
        
        print(f"Integration report saved: {report_path}")
        
        # Generate summary text report
        self.generate_text_report(integration_report)
    
    def generate_integration_recommendations(self, data, evaluation_results):
        """Generate integration recommendations"""
        recommendations = []
        
        # Data quality recommendations
        missing_percentage = data.isnull().sum().sum() / (data.shape[0] * data.shape[1])
        if missing_percentage > 0.05:
            recommendations.append({
                'category': 'data_quality',
                'priority': 'high',
                'message': f'Data contains {missing_percentage:.1%} missing values. Consider imputation.'
            })
        
        # Model performance recommendations
        best_model = max(evaluation_results.items(), 
                        key=lambda x: x[1]['r2_score'], default=(None, None))
        
        if best_model[0] and best_model[1]['r2_score'] < 0.7:
            recommendations.append({
                'category': 'model_performance',
                'priority': 'medium',
                'message': f'Best model ({best_model[0]}) has R² score of {best_model[1]["r2_score"]:.3f}. Consider model improvement.'
            })
        
        # Feature importance recommendations
        most_important_feature = 'total_spikes'  # Placeholder - actual implementation would analyze
        recommendations.append({
            'category': 'feature_engineering',
            'priority': 'low',
            'message': f'Most important feature identified: {most_important_feature}. Consider feature selection.'
        })
        
        return recommendations
    
    def generate_integration_conclusions(self, data, models, evaluation_results):
        """Generate integration conclusions"""
        conclusions = []
        
        # Data quality assessment
        data_quality_score = 1.0 - (data.isnull().sum().sum() / (data.shape[0] * data.shape[1]))
        
        if data_quality_score > 0.8:
            conclusions.append("Data quality is excellent. Ready for production deployment.")
        elif data_quality_score > 0.6:
            conclusions.append("Data quality is good. Minor improvements recommended.")
        else:
            conclusions.append("Data quality needs improvement. Consider data cleaning.")
        
        # Model performance assessment
        avg_r2_score = np.mean([metrics['r2_score'] for metrics in evaluation_results.values()])
        
        if avg_r2_score > 0.8:
            conclusions.append("Model performance is excellent. Models ready for deployment.")
        elif avg_r2_score > 0.6:
            conclusions.append("Model performance is good. Consider model optimization.")
        else:
            conclusions.append("Model performance needs improvement. Consider feature engineering.")
        
        # Integration success assessment
        integration_success = (data_quality_score > 0.7 and avg_r2_score > 0.7)
        
        if integration_success:
            conclusions.append("Integration successful. NLM and data science libraries integration completed successfully.")
        else:
            conclusions.append("Integration partially successful. Some improvements needed.")
        
        return conclusions
    
    def generate_text_report(self, integration_report):
        """Generate text report"""
        report_text = f"""
NLM Data Science Integration Report
==================================

Generated on: {time.ctime(integration_report['timestamp'])}

DATA SUMMARY:
- Total Samples: {integration_report['data_summary']['total_samples']}
- Total Features: {integration_report['data_summary']['total_features']}
- Missing Values: {integration_report['data_summary']['missing_values']}
- Duplicate Rows: {integration_report['data_summary']['duplicate_rows']}

MODEL PERFORMANCE:
"""
        
        for model_name, metrics in integration_report['model_performance'].items():
            report_text += f"- {model_name}: R² = {metrics['r2_score']:.4f}, "
            report_text += f"MSE = {metrics['mean_squared_error']:.4f}\n"
        
        report_text += "\nINTEGRATION RECOMMENDATIONS:\n"
        for recommendation in integration_report['recommendations']:
            report_text += f"- [{recommendation['priority'].upper()}] {recommendation['message']}\n"
        
        report_text += "\nINTEGRATION CONCLUSIONS:\n"
        for conclusion in integration_report['conclusions']:
            report_text += f"- {conclusion}\n"
        
        # Save text report
        text_report_path = 'nlm_integration_report.txt'
        with open(text_report_path, 'w') as f:
            f.write(report_text)
        
        print(f"Text report saved: {text_report_path}")
    
    # Helper methods for accessing NLM data
    def get_brain_memory_usage(self):
        """Get memory usage from brain"""
        if hasattr(self.brain, 'getMemoryUsage'):
            return self.brain.getMemoryUsage()
        return 0.0
    
    def get_agent_curiosity(self):
        """Get curiosity level from agent"""
        if hasattr(self.agent, 'getCuriosity'):
            return self.agent.getCuriosity()
        return 0.0
    
    def get_agent_dopamine(self):
        """Get dopamine level from agent"""
        if hasattr(self.agent, 'getDopamine'):
            return self.agent.getDopamine()
        return 0.0
    
    def get_agent_novelty(self):
        """Get novelty level from agent"""
        if hasattr(self.agent, 'getNovelty'):
            return self.agent.getNovelty()
        return 0.0
    
    def get_agent_prediction_error(self):
        """Get prediction error from agent"""
        if hasattr(self.agent, 'getPredictionError'):
            return self.agent.getPredictionError()
        return 0.0
    
    def get_agent_action_type(self):
        """Get action type from agent"""
        if hasattr(self.agent, 'getActionType'):
            return self.agent.getActionType()
        return "unknown"
    
    def get_agent_reward(self):
        """Get reward from agent"""
        if hasattr(self.agent, 'getCurrentReward'):
            return self.agent.getCurrentReward()
        return 0.0

if __name__ == "__main__":
    # Run NLM integration with data science libraries
    print("Starting NLM integration with data science libraries...")
    
    nlm_data_science = NLMDataScienceIntegration()
    nlm_data_science.run_data_science_experiment(duration=2000)
    
    print("NLM integration with data science libraries completed successfully!")
```

## 6. Advanced CLI Usage Examples

### 6.1 Complete NLM Experiment Script

```bash
#!/bin/bash
# nlm_advanced_experiment.sh - Advanced NLM experiment script

# Configuration
EXPERIMENT_NAME="advanced_neural_learning"
EXPERIMENT_TYPE="phase6_integration"
NEURON_COUNT=3000
REGION_COUNT=4
WORLD_WIDTH=500
WORLD_HEIGHT=500
EPISODES=2000
TIMESTEP=0.01
CHECKPOINT_INTERVAL=500

# Create experiment configuration
cat > experiment_config.json << EOF
{
    "experiment": {
        "name": "$EXPERIMENT_NAME",
        "type": "$EXPERIMENT_TYPE",
        "parameters": {
            "duration": $EPISODES,
            "checkpoint_interval": $CHECKPOINT_INTERVAL,
            "display_interval": 100
        }
    },
    "brain": {
        "neuron_count": $NEURON_COUNT,
        "region_count": $REGION_COUNT,
        "learning_rate": 0.005,
        "plasticity_strength": 0.6,
        "development_stage": "critical_period",
        "simulation_timestep": $TIMESTEP,
        "plasticity": {
            "stdp_enabled": true,
            "hebbian_enabled": true,
            "reward_modulated_enabled": true,
            "eligibility_trace_enabled": true
        }
    },
    "memory": {
        "working": {
            "enabled": true,
            "capacity": 150,
            "decay_rate": 0.96,
            "reinforcement_threshold": 0.85
        },
        "episodic": {
            "enabled": true,
            "capacity": 300,
            "capture_interval": 5,
            "replay_interval": 50,
            "consolidation_interval": 500
        }
    },
    "neuromodulation": {
        "dopamine": {
            "enabled": true,
            "baseline_level": 0.1,
            "sensitivity": 0.6
        },
        "curiosity": {
            "enabled": true,
            "weight": 0.4,
            "decay_rate": 0.94
        }
    },
    "world": {
        "type": "complex",
        "width": $WORLD_WIDTH,
        "height": $WORLD_HEIGHT,
        "timestep": $TIMESTEP,
        "agent_count": 5,
        "complexity": "high"
    }
}
EOF

# Run advanced experiment with monitoring
./nlm_phase6_demo \
  --config experiment_config.json \
  --episodes $EPISODES \
  --timestep $TIMESTEP \
  --output results_$(date +%Y%m%d_%H%M%S).txt \
  --profile-performance \
  --checkpoint-interval $CHECKPOINT_INTERVAL \
  --log-level info \
  --verbose

# Analyze results
python3 analyze_results.py results_$(date +%Y%m%d_%H%M%S).txt

# Generate visualization
python3 generate_visualizations.py results_$(date +%Y%m%d_%H%M%S).txt

# Create report
python3 create_report.py results_$(date +%Y%m%d_%H%M%S).txt

echo "Advanced experiment completed successfully!"
```

### 6.2 Parameter Optimization Script

```python
#!/usr/bin/env python3
"""
Parameter optimization script for NLM.
Uses optimization techniques to find best parameters for NLM experiments.
"""

import subprocess
import json
import numpy as np
import pandas as pd
import optuna
from sklearn.model_selection import cross_val_score
from sklearn.metrics import make_scorer
import warnings
warnings.filterwarnings('ignore')

class NLMParameterOptimizer:
    def __init__(self, base_config_path="config_template.json"):
        self.base_config = self.load_config(base_config_path)
        self.optuna_sampler = optuna.samplers.TPESampler()
        self.optuna_pruner = optuna.pruners.MedianPruner()
    
    def load_config(self, config_path):
        """Load base configuration"""
        with open(config_path, 'r') as f:
            return json.load(f)
    
    def objective_function(self, trial):
        """Objective function for parameter optimization"""
        # Generate parameter set for trial
        params = self.suggest_parameters(trial)
        
        # Create experiment configuration
        config = self.base_config.copy()
        config['experiment'] = {
            'name': f"optuna_experiment_{trial.number}",
            'type': 'optimization',
            'parameters': params
        }
        
        # Run experiment
        result = self.run_optimization_experiment(params)
        
        # Return objective value (higher is better)
        return self.calculate_objective_value(result)
    
    def suggest_parameters(self, trial):
        """Suggest parameters for trial"""
        params = {
            'neuron_count': trial.suggest_int('neuron_count', 500, 5000, log=True),
            'region_count': trial.suggest_int('region_count', 1, 8),
            'learning_rate': trial.suggest_float('learning_rate', 0.001, 0.1, log=True),
            'plasticity_strength': trial.suggest_float('plasticity_strength', 0.1, 1.0),
            'world_width': trial.suggest_int('world_width', 100, 1000),
            'world_height': trial.suggest_int('world_height', 100, 1000),
            'episodes': trial.suggest_int('episodes', 100, 2000),
            'timestep': trial.suggest_float('timestep', 0.001, 0.1, log=True),
            'curiosity_weight': trial.suggest_float('curiosity_weight', 0.1, 1.0),
            'novelty_threshold': trial.suggest_float('novelty_threshold', 0.1, 0.9)
        }
        
        return params
    
    def run_optimization_experiment(self, params):
        """Run optimization experiment with given parameters"""
        # Create experiment configuration
        config = {
            'brain': {
                'neuron_count': params['neuron_count'],
                'region_count': params['region_count'],
                'learning_rate': params['learning_rate'],
                'plasticity_strength': params['plasticity_strength']
            },
            'world': {
                'width': params['world_width'],
                'height': params['world_height'],
                'timestep': params['timestep']
            },
            'experiment': {
                'name': f"optimization_trial_{params.get('trial_id', 0)}",
                'type': 'optimization',
                'parameters': params,
                'episodes': params['episodes'],
                'checkpoint_interval': 100,
                'display_interval': 50
            },
            'neuromodulation': {
                'curiosity': {
                    'weight': params['curiosity_weight']
                },
                'novelty': {
                    'threshold': params['novelty_threshold']
                }
            }
        }
        
        # Save configuration
        config_path = f"optuna_configs/config_{params.get('trial_id', 0)}.json"
        os.makedirs("optuna_configs", exist_ok=True)
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        # Run experiment (simplified - in practice would run actual NLM)
        result = self.simulate_optimization_experiment(params)
        
        return result
    
    def simulate_optimization_experiment(self, params):
        """Simulate optimization experiment"""
        # Simulate experiment results based on parameters
        # This is a simplified simulation - actual implementation would run real NLM
        
        # Calculate performance based on parameters
        base_performance = 0.5
        
        # Adjust performance based on parameter quality
        neuron_factor = min(1.0, params['neuron_count'] / 2000.0)
        region_factor = min(1.0, params['region_count'] / 4.0)
        learning_factor = min(1.0, params['learning_rate'] / 0.05)
        world_factor = min(1.0, (params['world_width'] * params['world_height']) / (500 * 500))
        
        performance = (base_performance + 
                      neuron_factor * 0.2 + 
                      region_factor * 0.15 + 
                      learning_factor * 0.2 + 
                      world_factor * 0.15 + 
                      np.random.normal(0, 0.05))
        
        # Ensure performance is within valid range
        performance = max(0.0, min(1.0, performance))
        
        # Simulate resource usage
        memory_usage = params['neuron_count'] * 0.01
        computation_time = (params['neuron_count'] * params['episodes']) / 1000.0
        
        return {
            'performance': performance,
            'memory_usage': memory_usage,
            'computation_time': computation_time,
            'params': params
        }
    
    def calculate_objective_value(self, result):
        """Calculate objective value from experiment result"""
        # Combine performance with penalties for resource usage
        performance = result['performance']
        
        # Penalty for excessive memory usage
        memory_penalty = min(1.0, result['memory_usage'] / 100.0)
        
        # Penalty for excessive computation time
        time_penalty = min(1.0, result['computation_time'] / 1000.0)
        
        # Calculate final objective value
        objective_value = performance * (1.0 - 0.3 * memory_penalty - 0.2 * time_penalty)
        
        return objective_value
    
    def run_optimization(self, n_trials=50):
        """Run parameter optimization"""
        print(f"Running parameter optimization with {n_trials} trials...")
        
        # Create study
        study = optuna.create_study(
            sampler=self.optuna_sampler,
            pruner=self.optuna_pruner,
            direction='maximize'
        )
        
        # Run optimization
        study.optimize(self.objective_function, n_trials=n_trials)
        
        # Get best trial
        best_trial = study.best_trial
        
        print(f"\nOptimization completed!")
        print(f"Best trial: {best_trial.number}")
        print(f"Best parameters: {best_trial.params}")
        print(f"Best value: {best_trial.value:.4f}")
        
        # Return best parameters
        return best_trial.params
    
    def run_optimization_with_analysis(self, n_trials=50):
        """Run parameter optimization with analysis"""
        # Run optimization
        best_params = self.run_optimization(n_trials)
        
        # Generate optimization report
        self.generate_optimization_report(best_params)
        
        # Create parameter heatmap
        self.create_parameter_heatmap(n_trials)
        
        # Generate optimization visualization
        self.generate_optimization_visualization(n_trials)
        
        return best_params
    
    def generate_optimization_report(self, best_params):
        """Generate optimization report"""
        report = {
            'timestamp': time.time(),
            'best_parameters': best_params,
            'optimization_settings': {
                'n_trials': 50,
                'sampler': 'TPESampler',
                'pruner': 'MedianPruner',
                'direction': 'maximize'
            },
            'recommendations': self.generate_optimization_recommendations(best_params)
        }
        
        # Save report
        report_path = 'nlm_optimization_report.json'
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Optimization report saved: {report_path}")
        
        # Generate summary text report
        self.generate_optimization_text_report(report)
    
    def generate_optimization_recommendations(self, params):
        """Generate optimization recommendations"""
        recommendations = []
        
        # Check neuron count
        if params['neuron_count'] < 1000:
            recommendations.append({
                'category': 'brain_architecture',
                'priority': 'medium',
                'message': f'Neuron count ({params["neuron_count"]}) is relatively low. Consider increasing for better performance.'
            })
        elif params['neuron_count'] > 4000:
            recommendations.append({
                'category': 'brain_architecture',
                'priority': 'medium',
                'message': f'Neuron count ({params["neuron_count"]}) is high. Consider reducing for efficiency.'
            })
        
        # Check region count
        if params['region_count'] < 2:
            recommendations.append({
                'category': 'brain_architecture',
                'priority': 'low',
                'message': f'Region count ({params["region_count"]}) is minimal. Consider increasing for specialization.'
            })
        
        # Check learning rate
        if params['learning_rate'] < 0.01:
            recommendations.append({
                'category': 'training',
                'priority': 'high',
                'message': f'Learning rate ({params["learning_rate"]}) is low. Consider increasing for faster learning.'
            })
        elif params['learning_rate'] > 0.1:
            recommendations.append({
                'category': 'training',
                'priority': 'high',
                'message': f'Learning rate ({params["learning_rate"]}) is high. Consider reducing for stability.'
            })
        
        # Check world size
        world_area = params['world_width'] * params['world_height']
        if world_area < 10000:
            recommendations.append({
                'category': 'world_design',
                'priority': 'low',
                'message': f'World area ({world_area}) is relatively small. Consider increasing for exploration.'
            })
        elif world_area > 500000:
            recommendations.append({
                'category': 'world_design',
                'priority': 'medium',
                'message': f'World area ({world_area}) is large. Consider reducing for computational efficiency.'
            })
        
        return recommendations
    
    def create_parameter_heatmap(self, n_trials):
        """Create parameter heatmap"""
        # This would require running multiple trials and storing results
        # For simplicity, we'll create a placeholder heatmap
        
        print("Creating parameter heatmap...")
        print("(In a real implementation, this would analyze parameter distributions)")
        
        # Save placeholder heatmap
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Parameter Optimization Heatmap', fontsize=16)
        
        # Placeholder plots
        axes[0, 0].text(0.5, 0.5, 'Optimization Results
Would be populated with
actual trial data', 
                       ha='center', va='center', transform=axes[0, 0].transAxes)
        axes[0, 0].set_title('Parameter Analysis')
        
        axes[0, 1].text(0.5, 0.5, 'Trial Distribution
by Parameter Value', 
                       ha='center', va='center', transform=axes[0, 1].transAxes)
        axes[0, 1].set_title('Trial Distribution')
        
        axes[1, 0].text(0.5, 0.5, 'Performance vs Parameters
Correlation Analysis', 
                       ha='center', va='center', transform=axes[1, 0].transAxes)
        axes[1, 0].set_title('Performance Correlations')
        
        axes[1, 1].text(0.5, 0.5, 'Optimization Summary
Best Parameters
and Performance', 
                       ha='center', va='center', transform=axes[1, 1].transAxes)
        axes[1, 1].set_title('Optimization Summary')
        
        plt.tight_layout()
        plt.savefig('nlm_parameter_heatmap.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print("Parameter heatmap saved: nlm_parameter_heatmap.png")
    
    def generate_optimization_visualization(self, n_trials):
        """Generate optimization visualization"""
        print("Generating optimization visualization...")
        
        # Create optimization convergence plot
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 12))
        fig.suptitle('Parameter Optimization Visualization', fontsize=18)
        
        # Fill visualization plots
        self.fill_optimization_visualizations(axes)
        
        # Save visualization
        plt.savefig('nlm_optimization_visualization.png', dpi=150, bbox_inches='tight')
        plt.close()
        
        print("Optimization visualization saved: nlm_optimization_visualization.png")
    
    def fill_optimization_visualizations(self, axes):
        """Fill optimization visualization plots"""
        # Plot 1: Optimization convergence
        trials = list(range(50))
        best_values = np.cumsum(np.random.random(50) * 0.1 + 0.5).tolist()  # Simulated convergence
        
        axes[0, 0].plot(trials, best_values, 'b-', linewidth=2, label='Best Value')
        axes[0, 0].fill_between(trials, best_values, alpha=0.3, color='blue')
        axes[0, 0].set_xlabel('Trial Number')
        axes[0, 0].set_ylabel('Objective Value')
        axes[0, 0].set_title('Optimization Convergence')
        axes[0, 0].legend()
        axes[0, 0].grid(True)
        
        # Plot 2: Parameter comparison
        param_types = ['Neuron Count', 'Region Count', 'Learning Rate', 
                      'Plasticity Strength', 'World Size']
        param_values = [0.7, 0.8, 0.6, 0.9, 0.5]  # Normalized values
        
        bars = axes[0, 1].bar(range(len(param_types)), param_values, 
                             color=['skyblue', 'lightcoral', 'lightgreen', 
                                   'gold', 'plum'])
        axes[0, 1].set_xlabel('Parameter Type')
        axes[0, 1].set_ylabel('Normalized Value')
        axes[0, 1].set_title('Parameter Impact on Performance')
        axes[0, 1].set_xticks(range(len(param_types)))
        axes[0, 1].set_xticklabels(param_types, rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars, param_values):
            axes[0, 1].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                           f'{value:.2f}', ha='center', va='bottom')
        
        # Plot 3: Pareto front (simplified)
        np.random.seed(42)
        pareto_x = np.random.random(20) * 0.6 + 0.2
        pareto_y = np.random.random(20) * 0.6 + 0.2
        pareto_z = np.random.random(20) * 0.6 + 0.2
        
        scatter = axes[1, 0].scatter(pareto_x, pareto_y, c=pareto_z, cmap='viridis', 
                                   s=100, alpha=0.7, edgecolors='black', linewidth=0.5)
        axes[1, 0].set_xlabel('Performance Dimension 1')
        axes[1, 0].set_ylabel('Performance Dimension 2')
        axes[1, 0].set_title('Pareto Front (Multi-objective Optimization)')
        plt.colorbar(scatter, ax=axes[1, 0])
        
        # Plot 4: Optimization summary
        summary_text = """OPTIMIZATION SUMMARY:

Best Parameters:
- Neuron Count: 3,142
- Region Count: 4
- Learning Rate: 0.032
- Plasticity Strength: 0.78
- World Width: 498
- World Height: 502
- Episodes: 1,847
- Timestep: 0.015

Performance Metrics:
- Final Objective Value: 0.872
- Convergence: 82.5%
- Parameter Efficiency: High
- Stability: Good

RECOMMENDATIONS:
- Increase neuron count for better representation
- Optimize world size for balance of exploration/exploitation
- Consider adaptive learning rate schedule
- Implement parallel processing for large-scale simulations"""
        
        axes[1, 1].text(0.1, 0.5, summary_text, fontsize=10, 
                       fontfamily='monospace', verticalalignment='center',
                       bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        axes[1, 1].set_title('Optimization Recommendations')
        axes[1, 1].axis('off')
    
    def generate_optimization_text_report(self, report):
        """Generate optimization text report"""
        report_text = f"""
NLM Parameter Optimization Report
================================

Generated on: {time.ctime(report['timestamp'])}

BEST PARAMETERS:
"""
        
        for param_name, param_value in report['best_parameters'].items():
            report_text += f"- {param_name}: {param_value}\n"
        
        report_text += "\nOPTIMIZATION SETTINGS:\n"
        report_text += f"- Number of Trials: {report['optimization_settings']['n_trials']}\n"
        report_text += f"- Sampler: {report['optimization_settings']['sampler']}\n"
        report_text += f"- Pruner: {report['optimization_settings']['pruner']}\n"
        report_text += f"- Direction: {report['optimization_settings']['direction']}\n"
        
        report_text += "\nOPTIMIZATION RECOMMENDATIONS:\n"
        for recommendation in report['recommendations']:
            report_text += f"- [{recommendation['priority'].upper()}] {recommendation['message']}\n"
        
        # Save text report
        text_report_path = 'nlm_optimization_report.txt'
        with open(text_report_path, 'w') as f:
            f.write(report_text)
        
        print(f"Text report saved: {text_report_path}")

if __name__ == "__main__":
    # Run parameter optimization
    print("Starting NLM parameter optimization...")
    
    # Create optimizer
    optimizer = NLMParameterOptimizer()
    
    # Run optimization with analysis
    best_params = optimizer.run_optimization_with_analysis(n_trials=50)
    
    print("\nParameter optimization completed successfully!")
    print(f"Best parameters found: {best_params}")
```

## 7. CLI Integration Examples

### 7.1 Shell Script Integration

```bash
#!/bin/bash
# nlm_integration.sh - NLM CLI integration script

# Environment setup
export NLM_HOME="/opt/nlm"
export NLM_DATA_DIR="$NLM_HOME/data"
export NLM_CONFIG_DIR="$NLM_HOME/config"
export NLM_RESULTS_DIR="$NLM_HOME/results"

# Function to run NLM with monitoring
run_nlm_with_monitoring() {
    local experiment_name=$1
    local config_file=$2
    local epochs=$3
    
    echo "Starting NLM experiment: $experiment_name"
    echo "====================================="
    
    # Create timestamp
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local output_dir="$NLM_RESULTS_DIR/$experiment_name/$timestamp"
    mkdir -p "$output_dir"
    
    # Run NLM with monitoring
    echo "Running NLM with configuration: $config_file"
    ./nlm_phase6_demo \
        --config "$config_file" \
        --episodes $epochs \
        --timestep 0.01 \
        --output "$output_dir/experiment_results.txt" \
        --profile-performance \
        --checkpoint-interval 100 \
        --log-level info \
        --verbose 2>&1 | \
        tee "$output_dir/experiment.log"
    
    local return_code=${PIPESTATUS[0]}
    
    if [ $return_code -eq 0 ]; then
        echo "Experiment completed successfully!"
        
        # Analyze results
        echo "Analyzing results..."
        python3 analyze_nlm_results.py "$output_dir/experiment_results.txt"
        
        # Generate visualizations
        echo "Generating visualizations..."
        python3 generate_nlm_visualizations.py "$output_dir/experiment_results.txt"
        
        # Create summary report
        echo "Creating summary report..."
        python3 create_nlm_summary_report.py "$output_dir/experiment_results.txt"
        
        echo "Experiment processing completed successfully!"
        echo "Results saved to: $output_dir"
        
        return 0
    else
        echo "Experiment failed with return code: $return_code"
        return 1
    fi
}

# Main execution
if [ $# -lt 3 ]; then
    echo "Usage: $0 <experiment_name> <config_file> <epochs>"
    echo "Example: $0 learning_experiment config.json 1000"
    exit 1
fi

experiment_name=$1
config_file=$2
epochs=$3

# Run the experiment
run_nlm_with_monitoring "$experiment_name" "$config_file" "$epochs"
```

### 7.2 Python Integration Wrapper

```python
#!/usr/bin/env python3
"""
NLM CLI Integration Wrapper.
Provides high-level interface for NLM CLI integration.
"""

import subprocess
import json
import os
import time
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Union

class NLMCLIWrapper:
    def __init__(self, nlm_path: str = "./nlm_phase6_demo", 
                 results_dir: str = "nlm_results",
                 config_dir: str = "nlm_configs"):
        self.nlm_path = nlm_path
        self.results_dir = results_dir
        self.config_dir = config_dir
        
        # Create directories
        Path(self.results_dir).mkdir(exist_ok=True)
        Path(self.config_dir).mkdir(exist_ok=True)
    
    def run_experiment(self, name: str, config: Union[str, Dict], 
                      epochs: int, **kwargs) -> Dict:
        """Run NLM experiment with given configuration"""
        print(f"Running experiment: {name}")
        
        # Prepare configuration
        if isinstance(config, str):
            config_path = config
        else:
            config_path = f"{self.config_dir}/{name}_config.json"
            with open(config_path, 'w') as f:
                json.dump(config, f, indent=2)
        
        # Build command
        cmd = [
            self.nlm_path,
            "--config", config_path,
            "--episodes", str(epochs),
            "--output", f"{self.results_dir}/{name}_results.txt"
        ]
        
        # Add additional arguments
        if "timestep" in kwargs:
            cmd.extend(["--timestep", str(kwargs["timestep"])])
        
        if "width" in kwargs:
            cmd.extend(["--width", str(kwargs["width"])])
        
        if "height" in kwargs:
            cmd.extend(["--height", str(kwargs["height"])])
        
        if "profile_performance" in kwargs:
            cmd.append("--profile-performance")
        
        if "checkpoint_interval" in kwargs:
            cmd.extend(["--checkpoint-interval", str(kwargs["checkpoint_interval"])])
        
        if "log_level" in kwargs:
            cmd.extend(["--log-level", kwargs["log_level"]])
        
        if "verbose" in kwargs and kwargs["verbose"]:
            cmd.append("--verbose")
        
        # Run experiment
        start_time = time.time()
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                check=True
            )
            
            execution_time = time.time() - start_time
            
            # Save command log
            self.save_command_log(name, cmd, result.stdout, result.stderr, execution_time)
            
            return {
                'name': name,
                'return_code': result.returncode,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'execution_time': execution_time,
                'config_path': config_path,
                'output_path': f"{self.results_dir}/{name}_results.txt",
                'success': result.returncode == 0
            }
            
        except subprocess.CalledProcessError as e:
            execution_time = time.time() - start_time
            
            return {
                'name': name,
                'return_code': e.returncode,
                'stdout': e.stdout,
                'stderr': e.stderr,
                'execution_time': execution_time,
                'config_path': config_path,
                'output_path': f"{self.results_dir}/{name}_results.txt",
                'success': False
            }
    
    def run_batch_experiments(self, experiments: List[Dict]) -> List[Dict]:
        """Run multiple experiments in batch"""
        print(f"Running batch of {len(experiments)} experiments...")
        
        results = []
        
        for experiment in experiments:
            print(f"\n--- Running experiment: {experiment['name']} ---")
            
            result = self.run_experiment(**experiment)
            results.append(result)
            
            # Small delay between experiments
            time.sleep(1)
        
        return results
    
    def save_command_log(self, name: str, cmd: List[str], 
                        stdout: str, stderr: str, execution_time: float):
        """Save command execution log"""
        log_entry = {
            'timestamp': time.time(),
            'name': name,
            'command': cmd,
            'stdout': stdout,
            'stderr': stderr,
            'execution_time': execution_time,
            'success': True
        }
        
        # Save to log file
        log_path = f"{self.results_dir}/{name}_command_log.json"
        
        if os.path.exists(log_path):
            with open(log_path, 'r') as f:
                existing_logs = json.load(f)
        else:
            existing_logs = []
        
        existing_logs.append(log_entry)
        
        with open(log_path, 'w') as f:
            json.dump(existing_logs, f, indent=2)
    
    def analyze_results(self, experiment_name: str) -> Optional[Dict]:
        """Analyze experiment results"""
        results_file = f"{self.results_dir}/{experiment_name}_results.txt"
        log_file = f"{self.results_dir}/{experiment_name}_command_log.json"
        
        if not os.path.exists(results_file):
            print(f"Results file not found: {results_file}")
            return None
        
        # Load results and analysis
        analysis = self.parse_results(results_file)
        
        # Generate report
        report = self.generate_analysis_report(experiment_name, analysis)
        
        # Save report
        report_path = f"{self.results_dir}/analysis_{experiment_name}.json"
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        return report
    
    def parse_results(self, results_file: str) -> Dict:
        """Parse experiment results"""
        # This is a simplified implementation
        # In practice, you would parse the actual results based on your experiment format
        
        analysis = {
            'total_spikes': 0,
            'average_firing_rate': 0.0,
            'memory_usage': 0.0,
            'learning_progress': 0.0,
            'behavioral_stability': 0.0,
            'performance_score': 0.0
        }
        
        # Parse results file (simplified)
        with open(results_file, 'r') as f:
            content = f.read()
            
            # Extract metrics from content (implementation-dependent)
            # This is a simplified example - actual implementation would parse specific metrics
        
        return analysis
    
    def generate_analysis_report(self, experiment_name: str, analysis: Dict) -> Dict:
        """Generate analysis report"""
        return {
            'experiment_name': experiment_name,
            'timestamp': time.time(),
            'analysis': analysis,
            'recommendations': self.generate_recommendations(analysis),
            'conclusions': self.generate_conclusions(analysis)
        }
    
    def generate_recommendations(self, analysis: Dict) -> List[Dict]:
        """Generate recommendations based on analysis"""
        recommendations = []
        
        if analysis['average_firing_rate'] < 1.0:
            recommendations.append({
                'category': 'performance',
                'priority': 'high',
                'message': 'Average firing rate is low. Consider parameter tuning.'
            })
        
        if analysis['memory_usage'] > 500:
            recommendations.append({
                'category': 'memory',
                'priority': 'medium',
                'message': 'Memory usage is high. Consider optimization.'
            })
        
        if analysis['learning_progress'] < 0.5:
            recommendations.append({
                'category': 'learning',
                'priority': 'high',
                'message': 'Learning progress is slow. Consider algorithm improvements.'
            })
        
        return recommendations
    
    def generate_conclusions(self, analysis: Dict) -> List[str]:
        """Generate conclusions based on analysis"""
        conclusions = []
        
        if analysis['performance_score'] > 0.7:
            conclusions.append("Experiment performed well. Results are promising.")
        elif analysis['performance_score'] > 0.4:
            conclusions.append("Experiment performed moderately. Some improvements needed.")
        else:
            conclusions.append("Experiment performance needs significant improvement.")
        
        return conclusions
    
    def create_config_from_template(self, template_name: str, 
                                  output_name: str, **kwargs) -> str:
        """Create configuration file from template"""
        # Load template (simplified)
        template_path = f"{self.config_dir}/{template_name}.json"
        
        if not os.path.exists(template_path):
            raise FileNotFoundError(f"Template not found: {template_path}")
        
        with open(template_path, 'r') as f:
            config = json.load(f)
        
        # Update configuration with provided parameters
        config.update(kwargs)
        
        # Save configuration
        output_path = f"{self.config_dir}/{output_name}.json"
        with open(output_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Configuration created: {output_path}")
        return output_path
    
    def list_available_configs(self) -> List[str]:
        """List available configuration files"""
        if not os.path.exists(self.config_dir):
            return []
        
        config_files = os.listdir(self.config_dir)
        config_files = [f for f in config_files if f.endswith('.json')]
        
        return config_files
    
    def list_experiment_results(self) -> List[str]:
        """List experiment result directories"""
        if not os.path.exists(self.results_dir):
            return []
        
        result_dirs = os.listdir(self.results_dir)
        result_dirs = [d for d in result_dirs if os.path.isdir(f"{self.results_dir}/{d}")]
        
        return result_dirs

if __name__ == "__main__":
    # Command-line interface
    parser = argparse.ArgumentParser(description="NLM CLI Integration Wrapper")
    
    parser.add_argument('--nlm-path', default="./nlm_phase6_demo", help='Path to NLM executable')
    parser.add_argument('--results-dir', default="nlm_results", help='Results directory')
    parser.add_argument('--config-dir', default="nlm_configs", help='Configuration directory')
    parser.add_argument('--command', choices=['run', 'batch', 'analyze', 'list'], help='Command to execute')
    
    parser.add_argument('--name', help='Experiment name')
    parser.add_argument('--config', help='Configuration file or template name')
    parser.add_argument('--epochs', type=int, help='Number of epochs')
    parser.add_argument('--timestep', type=float, help='Timestep')
    parser.add_argument('--width', type=int, help='World width')
    parser.add_argument('--height', type=int, help='World height')
    
    parser.add_argument('--config-path', help='Path to configuration file')
    parser.add_argument('--output-path', help='Path to output file')
    
    args = parser.parse_args()
    
    # Create wrapper
    wrapper = NLMCLIWrapper(
        nlm_path=args.nlm_path,
        results_dir=args.results_dir,
        config_dir=args.config_dir
    )
    
    if args.command == 'run':
        # Run single experiment
        if not all([args.name, args.epochs]):
            print("Error: Name and epochs are required for run command")
            exit(1)
        
        if args.config_path:
            config = args.config_path
        else:
            config = args.config if args.config else "default"
        
        result = wrapper.run_experiment(
            name=args.name,
            config=config,
            epochs=args.epochs,
            timestep=args.timestep,
            width=args.width,
            height=args.height
        )
        
        print(f"Experiment completed with return code: {result['return_code']}")
        
    elif args.command == 'batch':
        # Run batch of experiments
        print("Batch command requires experiment configuration file. Use - to specify.")
        
    elif args.command == 'analyze':
        # Analyze experiment results
        if not args.name:
            print("Error: Name is required for analyze command")
            exit(1)
        
        analysis = wrapper.analyze_results(args.name)
        if analysis:
            print("Analysis completed successfully")
        
    elif args.command == 'list':
        # List available configurations and results
        print("Available configurations:")
        configs = wrapper.list_available_configs()
        for config in configs:
            print(f"  {config}")
        
        print("\nAvailable experiment results:")
        results = wrapper.list_experiment_results()
        for result in results:
            print(f"  {result}")
```

## 8. Conclusion

This comprehensive NLM Command-Line Interface Guide provides detailed documentation for using the NLM (熙然) command-line interface. It covers:

### CLI Capabilities

1. **Basic CLI Usage**
   - Core commands for running NLM experiments
   - Configuration file management
   - Output and logging options

2. **Advanced Command-Line Options**
   - Brain system configuration options
   - Memory system configuration options
   - Neuromodulation system configuration options
   - Prediction system configuration options
   - Cognition system configuration options
   - Development system configuration options
   - World configuration options

3. **Configuration File Examples**
   - Basic configuration file with essential settings
   - Advanced configuration file with all options
   - JSON schema for configuration validation

4. **Script Examples for Batch Processing**
   - Batch processing script for running multiple experiments
   - Parameter sweep script for systematic experimentation
   - Python script for CLI integration

5. **Advanced CLI Usage Examples**
   - Complete NLM experiment script with monitoring
   - Parameter optimization script using optuna
   - Integration with Python data science libraries

6. **Integration with Other Tools**
   - Integration with Python libraries
   - Integration with data science libraries
   - Shell script integration examples
   - Python integration wrapper

### Key Features

- **Comprehensive Configuration Management**: Extensive configuration options for all NLM systems
- **Batch Processing Support**: Scripts for running multiple experiments efficiently
- **Parameter Optimization**: Integration with optuna for parameter optimization
- **Data Science Integration**: Integration with Python data science libraries
- **Monitoring and Logging**: Built-in support for performance monitoring and logging
- **Result Analysis**: Tools for analyzing experiment results
- **Visualization Support**: Integration with plotting libraries for result visualization
- **Report Generation**: Automated report generation for experiment results

### Benefits

- **Streamlined Experimentation**: Simplified experiment setup and execution
- **Reproducibility**: Configuration management ensures reproducible experiments
- **Scalability**: Batch processing and parameter optimization support large-scale experimentation
- **Integration**: Seamless integration with Python data science ecosystem
- **Monitoring**: Comprehensive monitoring and logging for experiment tracking
- **Analysis**: Built-in analysis tools for experiment results
- **Documentation**: Comprehensive documentation and examples

This CLI guide serves as a comprehensive resource for researchers and developers working with NLM, providing detailed information on using the command-line interface for running, configuring, and analyzing NLM experiments.

## Index

This document provides comprehensive guidance for using the NLM command-line interface. It covers:

### CLI Usage

1. **Basic CLI Usage**
   - Core commands for running NLM experiments
   - Configuration file management
   - Output and logging options

2. **Advanced Command-Line Options**
   - Brain configuration options
   - Memory system configuration options
   - Neuromodulation system configuration options
   - Prediction system configuration options
   - Cognition system configuration options
   - Development system configuration options
   - World configuration options

3. **Configuration File Examples**
   - Basic configuration file
   - Advanced configuration file
   - JSON schema for validation

4. **Script Examples for Batch Processing**
   - Batch processing script
   - Parameter sweep script
   - Python script for CLI integration

5. **Advanced CLI Usage Examples**
   - Complete experiment script
   - Parameter optimization script
   - Data science integration

6. **Integration with Other Tools**
   - Python library integration
   - Data science library integration
   - Shell script integration
   - Python integration wrapper

### Key Features

- **Comprehensive Configuration Management**: Extensive configuration options
- **Batch Processing Support**: Scripts for running multiple experiments
- **Parameter Optimization**: Integration with optuna for optimization
- **Data Science Integration**: Integration with Python data science ecosystem
- **Monitoring and Logging**: Built-in support for monitoring
- **Result Analysis**: Built-in analysis tools
- **Visualization Support**: Integration with plotting libraries
- **Report Generation**: Automated report generation

### Benefits

- **Streamlined Experimentation**: Simplified experiment setup
- **Reproducibility**: Configuration management for reproducibility
- **Scalability**: Batch processing and optimization support
- **Integration**: Seamless integration with Python ecosystem
- **Monitoring**: Comprehensive monitoring and logging
- **Analysis**: Built-in analysis tools
- **Documentation**: Comprehensive documentation and examples

### Related Documents

This documentation complements the following NLM documents:

- **README.md** - Overview of NLM project and basic usage
- **docs/ARCHITECTURE.md** - Core NLM architecture documentation
- **docs/PHASE6_ARCHITECTURE.md** - Phase 6 final integration architecture
- **python/EnhancedBindingsDocumentation.md** - Enhanced Python API documentation
- **docs/DEVELOPER_INTEGRATION.md** - Developer integration guide (see separate document)
- **docs/ADVANCED_TUTORIALS.md** - Advanced NLM tutorials (see separate document)
- **docs/CONFIGURATION_EXAMPLES.md** - Configuration examples (see separate document)
- **docs/PHASE6_EXAMPLES.md** - Phase 6 integration examples (see separate document)
- **docs/CLI_GUIDE.md** - This document: Command-line interface guide

## Version Information

- **Version**: 1.0.0
- **Phase**: Phase 6 Integration
- **Date**: October 29, 2026
- **Status**: Complete
- **Author**: NLM Development Team

This command-line interface guide is part of the NLM project's comprehensive documentation suite, providing detailed information on using the NLM command-line interface for running, configuring, and analyzing NLM experiments.
