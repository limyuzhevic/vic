# NLM Experiment Guide

## Overview

This document provides comprehensive documentation for NLM experiment management and design. Experiments are a fundamental aspect of NLM that allow researchers to study neural behavior, learning, and development in controlled environments. This guide covers experiment design, management, analysis, and troubleshooting.

## Experiment Framework

NLM provides a comprehensive experiment framework with the following components:

- **Experiment Runner**: Executes experiments and collects data
- **Experiment Framework**: Defines experiment structure and behavior
- **Metrics**: Collects and analyzes experiment metrics
- **Phase 4 Experiments**: Specific experiments for testing cognitive capabilities
- **Ablation System**: Tests the effects of removing specific mechanisms
- **Scaling Benchmarks**: Tests NLM performance at different scales

## Experiment Design

### Experiment Components

An NLM experiment consists of the following components:

#### 1. Experiment Configuration

The experiment configuration defines:

- **Experiment name**: Unique identifier for the experiment
- **Experiment type**: Type of experiment (e.g., cognitive, learning, development)
- **Parameters**: Configuration parameters for the experiment
- **Duration**: Number of simulation steps
- **Output**: Output format and destination

```json
{
    "name": "temporal_prediction",
    "type": "cognitive",
    "parameters": {
        "neuron_count": 5000,
        "plasticity": "stdp",
        "learning_rate": 0.001
    },
    "duration": 2000,
    "output": {
        "format": "json",
        "destination": "./results"
    }
}
```

#### 2. Experiment Procedure

The experiment procedure defines:

- **World setup**: World configuration and initial state
- **Brain initialization**: Brain configuration and initialization
- **Agent setup**: Agent configuration and initialization
- **Simulation loop**: Main simulation procedure
- **Data collection**: Data collection during simulation
- **Result analysis**: Analysis of collected data

```python
def run_experiment(config, world_config, experiment_type):
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems based on experiment type
    enable_subsystems_for_experiment(agent, experiment_type)
    
    # Run simulation
    for step in range(config['duration']):
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_reward(percept)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        collect_metrics(step, brain, agent, world)
    
    # Analyze results
    analyze_results(brain, agent, world, experiment_type)
```

#### 3. Metrics Collection

Experiment metrics include:

- **Neural activity**: Firing rates, spike patterns, neuron utilization
- **Learning progress**: Prediction error, reward modulation, plasticity
- **Cognitive performance**: Attention, planning, memory retention
- **Behavior**: Action selection, world interaction, task performance

```python
def collect_metrics(step, brain, agent, world):
    metrics = {
        'step': step,
        'firing_rate': brain.getAverageFiringRate(),
        'neuron_utilization': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100,
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'simulation_time': world.getSimulationTime()
    }
    
    # Save metrics to database or file
    save_metrics(metrics)
```

### Experiment Types

#### Cognitive Experiments

Cognitive experiments test NLM's cognitive capabilities:

- **Temporal Prediction**: Test if NLM can learn temporal sequences
- **Working Memory**: Test if information can be maintained after stimulus disappearance
- **Episodic Recall**: Test if past experiences influence future behavior
- **Concept Formation**: Test if recurring patterns form stable representations
- **Attention Selection**: Test if competing signals produce selective processing
- **Multi-step Planning**: Test if NLM can plan multi-step sequences
- **Self-action Prediction**: Test if NLM learns that its actions cause predictable sensory changes
- **Object Permanence**: Test if hidden objects remain relevant to behavior
- **Social Learning**: Test if NLM can learn from observing other agents
- **Continual Learning**: Test if NLM can learn new tasks without forgetting old ones
- **Generalization**: Test if learned knowledge transfers to new situations

#### Learning Experiments

Learning experiments test NLM's learning capabilities:

- **Reinforcement Learning**: Test reward-based learning
- **Curiosity-driven Learning**: Test exploration based on novelty and prediction error
- **Developmental Learning**: Test learning across developmental stages
- **Memory Consolidation**: Test memory consolidation during rest
- **Social Learning**: Test learning from observation

#### Performance Experiments

Performance experiments test NLM's performance characteristics:

- **Scaling Experiments**: Test NLM performance at different scales
- **Memory Usage Experiments**: Test memory efficiency
- **Speed Experiments**: Test simulation speed
- **Resource Experiments**: Test resource utilization

### Experiment Templates

#### Experiment Template File

```json
{
    "template": {
        "name": "cognitive_template",
        "description": "Template for cognitive experiments",
        "category": "cognitive",
        "parameters": {
            "brain.neuron_count": "range:1000-10000:default=5000",
            "brain.synapse_density": "range:0.01-0.5:default=0.1",
            "plasticity.stdp.enable": "bool:default=true",
            "plasticity.hebbian.enable": "bool:default=true",
            "neuromod.dopamine.scale": "range:0.5-2.0:default=1.0",
            "performance.optimization_level": "range:1-3:default=2"
        },
        "experiment_procedures": {
            "temporal_prediction": "path/to/temporal_prediction.py",
            "working_memory": "path/to/working_memory.py",
            "episodic_recall": "path/to/episodic_recall.py"
        },
        "metrics": {
            "firing_rate": "continuous",
            "prediction_error": "continuous",
            "curiosity_level": "continuous",
            "novelty_level": "continuous"
        }
    }
}
```

## Experiment Management

### Experiment Runner

The experiment runner executes and manages experiments:

```python
import pynlm
from typing import Dict, List, Any
import json
import time
from datetime import datetime

class ExperimentRunner:
    def __init__(self, config_file):
        self.config_file = config_file
        self.experiments = self.load_experiments()
        self.results = {}
    
    def load_experiments(self):
        """Load experiments from configuration file."""
        with open(self.config_file, 'r') as f:
            return json.load(f)['experiments']
    
    def run_experiment(self, experiment_name, config_overrides=None):
        """Run a specific experiment."""
        if experiment_name not in self.experiments:
            raise ValueError(f"Experiment not found: {experiment_name}")
        
        experiment = self.experiments[experiment_name]
        
        # Create configuration
        config = self.create_configuration(experiment, config_overrides)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world_config = experiment.get('world_config', {})
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        self.enable_subsystems_for_experiment(agent, experiment.get('experiment_type', ''))
        
        # Run experiment
        experiment_result = self.run_experiment_procedure(
            brain, world, agent, experiment, config
        )
        
        # Save results
        self.save_experiment_result(experiment_name, experiment_result)
        
        return experiment_result
    
    def create_configuration(self, experiment, config_overrides):
        """Create configuration for experiment."""
        # Start with default configuration
        config = pynlm.createDefaultConfig()
        
        # Apply experiment configuration
        if 'config' in experiment:
            for key, value in experiment['config'].items():
                config.set(key, value)
        
        # Apply overrides
        if config_overrides:
            for key, value in config_overrides.items():
                config.set(key, value)
        
        return config
    
    def enable_subsystems_for_experiment(self, agent, experiment_type):
        """Enable subsystems based on experiment type."""
        # Enable all subsystems by default
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Experiment-specific subsystem enablement
        if experiment_type == 'temporal_prediction':
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        
        elif experiment_type == 'working_memory':
            agent.enableRewardModulation(True)
            agent.enableDevelopment(True)
        
        elif experiment_type == 'episodic_recall':
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        # Additional experiment-specific configurations
    
    def run_experiment_procedure(self, brain, world, agent, experiment, config):
        """Run experiment procedure."""
        experiment_name = experiment['name']
        duration = experiment.get('duration', 1000)
        
        # Initialize metrics collection
        metrics = self.initialize_metrics_collection(experiment)
        
        # Run simulation
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.calculate_reward(percept, experiment)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect metrics
            self.collect_metrics(metrics, step, brain, agent, world, experiment)
        
        # Analyze results
        results = self.analyze_experiment_results(
            brain, agent, world, agent, metrics, experiment
        )
        
        return results
    
    def initialize_metrics_collection(self, experiment):
        """Initialize metrics collection."""
        metrics_config = experiment.get('metrics', {})
        
        return {
            'neural': {
                'firing_rate': [],
                'neuron_utilization': [],
                'total_spikes': 0,
                'max_firing_rate': 0
            },
            'cognitive': {
                'curiosity_level': [],
                'novelty_level': [],
                'prediction_error': [],
                'developmental_stage': []
            },
            'behavior': {
                'actions_taken': [],
                'reward_received': [],
                'simulation_time': []
            },
            'custom': metrics_config.get('custom', [])
        }
    
    def collect_metrics(self, metrics, step, brain, agent, world, experiment):
        """Collect metrics for current step."""
        metrics['neural']['firing_rate'].append(brain.getAverageFiringRate())
        metrics['neural']['neuron_utilization'].append(
            (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100
        )
        metrics['neural']['total_spikes'] = brain.getTotalSpikeCount()
        metrics['neural']['max_firing_rate'] = max(
            metrics['neural']['max_firing_rate'],
            brain.getMaxFiringRate()
        )
        
        metrics['cognitive']['curiosity_level'].append(agent.getCuriosityLevel())
        metrics['cognitive']['novelty_level'].append(agent.getNoveltyLevel())
        metrics['cognitive']['prediction_error'].append(agent.getPredictionError())
        metrics['cognitive']['developmental_stage'].append(brain.getDevelopmentalStage())
        
        metrics['behavior']['actions_taken'].append(1)  # Simple action counter
        metrics['behavior']['reward_received'].append(self.get_reward(percept))
        metrics['behavior']['simulation_time'].append(world.getSimulationTime())
    
    def calculate_reward(self, percept, experiment):
        """Calculate reward based on percept and experiment configuration."""
        # Custom reward function - can be modified based on experiment goals
        internal = percept.getInternal() if percept.getInternal() else []
        return internal[0] if internal else 0.0
    
    def get_reward(self, percept):
        """Get reward from percept."""
        internal = percept.getInternal() if percept.getInternal() else []
        return internal[0] if internal else 0.0
    
    def analyze_experiment_results(self, brain, agent, world, metrics, experiment):
        """Analyze experiment results."""
        analysis = {
            'experiment_name': experiment['name'],
            'duration': experiment.get('duration', 0),
            'neural_analysis': self.analyze_neural_metrics(brain, metrics['neural']),
            'cognitive_analysis': self.analyze_cognitive_metrics(agent, metrics['cognitive']),
            'behavioral_analysis': self.analyze_behavioral_metrics(world, metrics['behavior']),
            'custom_analysis': self.analyze_custom_metrics(metrics['custom'])
        }
        
        return analysis
    
    def analyze_neural_metrics(self, brain, neural_metrics):
        """Analyze neural metrics."""
        return {
            'avg_firing_rate': np.mean(neural_metrics['firing_rate']) if neural_metrics['firing_rate'] else 0,
            'std_firing_rate': np.std(neural_metrics['firing_rate']) if neural_metrics['firing_rate'] else 0,
            'max_firing_rate': neural_metrics['max_firing_rate'],
            'total_spikes': neural_metrics['total_spikes'],
            'neuron_utilization_trend': self.calculate_trend(neural_metrics['neuron_utilization'])
        }
    
    def analyze_cognitive_metrics(self, agent, cognitive_metrics):
        """Analyze cognitive metrics."""
        return {
            'avg_curiosity': np.mean(cognitive_metrics['curiosity_level']) if cognitive_metrics['curiosity_level'] else 0,
            'std_curiosity': np.std(cognitive_metrics['curiosity_level']) if cognitive_metrics['curiosity_level'] else 0,
            'avg_novelty': np.mean(cognitive_metrics['novelty_level']) if cognitive_metrics['novelty_level'] else 0,
            'avg_prediction_error': np.mean(cognitive_metrics['prediction_error']) if cognitive_metrics['prediction_error'] else 0,
            'developmental_stage_progression': self.calculate_progression(cognitive_metrics['developmental_stage'])
        }
    
    def analyze_behavioral_metrics(self, world, behavioral_metrics):
        """Analyze behavioral metrics."""
        return {
            'total_actions': sum(behavioral_metrics['actions_taken']),
            'avg_reward': np.mean(behavioral_metrics['reward_received']) if behavioral_metrics['reward_received'] else 0,
            'simulation_time': behavioral_metrics['simulation_time'][-1] if behavioral_metrics['simulation_time'] else 0,
            'action_frequency': self.calculate_frequency(behavioral_metrics['actions_taken'])
        }
    
    def analyze_custom_metrics(self, custom_metrics):
        """Analyze custom metrics."""
        # Implement custom metrics analysis
        return {}
    
    def calculate_trend(self, values):
        """Calculate trend of values."""
        if len(values) < 2:
            return 'stable'
        
        if values[-1] > values[0] * 1.1:
            return 'increasing'
        elif values[-1] < values[0] * 0.9:
            return 'decreasing'
        else:
            return 'stable'
    
    def calculate_progression(self, values):
        """Calculate progression of developmental stages."""
        if not values:
            return []
        
        # Track developmental stage progression
        return values
    
    def calculate_frequency(self, values):
        """Calculate frequency of actions."""
        if not values:
            return 0
        
        # Calculate action frequency
        return sum(values) / len(values)
    
    def save_experiment_result(self, experiment_name, result):
        """Save experiment result."""
        filename = f"{experiment_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(filename, 'w') as f:
            json.dump(result, f, indent=2, default=str)
        
        print(f"Experiment result saved to: {filename}")

# Example usage
runner = ExperimentRunner("experiments.json")

# Run specific experiment
result = runner.run_experiment("temporal_prediction")

print(f"Experiment completed: {result['experiment_name']}")
print(f"Duration: {result['duration']} steps")
print(f"Neural performance: {result['neural_analysis']['avg_firing_rate']:.2f} Hz")
print(f"Cognitive performance: {result['cognitive_analysis']['avg_curiosity']:.3f} curiosity")
```

### Batch Experiment Running

#### Python Batch Experiment Running

```python
import pynlm
from typing import Dict, List, Any
import json
import time
from datetime import datetime

class BatchExperimentRunner:
    def __init__(self, experiment_config_file, result_directory):
        self.experiment_config_file = experiment_config_file
        self.result_directory = result_directory
        self.load_experiment_config()
        os.makedirs(result_directory, exist_ok=True)
    
    def load_experiment_config(self):
        """Load experiment configuration."""
        with open(self.experiment_config_file, 'r') as f:
            self.experiment_config = json.load(f)
    
    def create_parameter_grid(self, base_config, parameter_ranges):
        """Create parameter grid for experiments."""
        parameter_grid = []
        
        # Generate parameter combinations
        for param_name, param_range in parameter_ranges.items():
            if isinstance(param_range, list):
                # Discrete parameter values
                for value in param_range:
                    config = base_config.copy()
                    config[param_name] = value
                    parameter_grid.append(config)
            elif isinstance(param_range, dict):
                # Range parameter values
                start = param_range.get('start', 0)
                end = param_range.get('end', 100)
                step = param_range.get('step', 10)
                
                for value in range(start, end + step, step):
                    config = base_config.copy()
                    config[param_name] = value
                    parameter_grid.append(config)
        
        return parameter_grid
    
    def run_batch_experiment(self, parameter_grid, experiment_name, num_repeats=1):
        """Run batch experiment with parameter grid."""
        print(f"Running batch experiment: {experiment_name}")
        print(f"Parameter combinations: {len(parameter_grid)} x {num_repeats} repeats")
        
        all_results = []
        total_runs = len(parameter_grid) * num_repeats
        completed_runs = 0
        
        for i, config in enumerate(parameter_grid):
            for repeat in range(num_repeats):
                # Create unique experiment name
                exp_name = f"{experiment_name}_{i+1}_{repeat+1}"
                
                print(f"Running experiment {i+1}/{len(parameter_grid)} (repeat {repeat+1}/{num_repeats}): {exp_name}")
                
                # Run experiment
                result = self.run_single_experiment(exp_name, config)
                
                # Save result
                result_file = os.path.join(
                    self.result_directory,
                    f"{exp_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
                )
                
                with open(result_file, 'w') as f:
                    json.dump(result, f, indent=2, default=str)
                
                all_results.append(result)
                
                completed_runs += 1
                print(f"  Completed {completed_runs}/{total_runs} runs")
        
        return all_results
    
    def run_single_experiment(self, experiment_name, config):
        """Run single experiment."""
        # Initialize experiment runner
        runner = self.create_experiment_runner()
        
        # Run experiment
        result = runner.run_experiment(experiment_name, config)
        
        return result
    
    def create_experiment_runner(self):
        """Create experiment runner."""
        return ExperimentRunner(self.experiment_config_file)
    
    def analyze_batch_results(self, results):
        """Analyze batch experiment results."""
        if not results:
            return {}
        
        # Group results by parameter
        grouped_results = {}
        
        for result in results:
            exp_name = result['experiment_name']
            config = result.get('config', {})
            
            # Extract key parameters
            key_params = tuple(sorted(config.items()))
            
            if key_params not in grouped_results:
                grouped_results[key_params] = []
            
            grouped_results[key_params].append(result)
        
        # Analyze each parameter group
        analysis = {}
        
        for key_params, group_results in grouped_results.items():
            # Calculate statistics
            avg_performance = np.mean([
                r['neural_analysis']['avg_firing_rate'] for r in group_results
                if 'neural_analysis' in r and 'avg_firing_rate' in r['neural_analysis']
            ])
            
            avg_curiosity = np.mean([
                r['cognitive_analysis']['avg_curiosity'] for r in group_results
                if 'cognitive_analysis' in r and 'avg_curiosity' in r['cognitive_analysis']
            ])
            
            avg_reward = np.mean([
                r['behavioral_analysis']['avg_reward'] for r in group_results
                if 'behavioral_analysis' in r and 'avg_reward' in r['behavioral_analysis']
            ])
            
            analysis[key_params] = {
                'experiment_count': len(group_results),
                'avg_firing_rate': avg_performance,
                'avg_curiosity': avg_curiosity,
                'avg_reward': avg_reward
            }
        
        return analysis
    
    def generate_batch_report(self, results, analysis):
        """Generate batch experiment report."""
        report = {
            'benchmark_name': 'NLM_Batch_Experiment',
            'timestamp': datetime.now().isoformat(),
            'summary': {
                'total_experiments': len(results),
                'total_parameters': len(analysis),
                'best_configuration': self.get_best_configuration(analysis),
                'parameter_impact': self.analyze_parameter_impact(analysis)
            },
            'results': results,
            'analysis': analysis,
            'recommendations': self.generate_recommendations(analysis)
        }
        
        return report
    
    def get_best_configuration(self, analysis):
        """Get best configuration based on performance."""
        if not analysis:
            return None
        
        best_key_params = max(analysis.items(), key=lambda x: x[1]['avg_firing_rate'])
        
        return {
            'parameters': best_key_params[0],
            'performance': best_key_params[1]
        }
    
    def analyze_parameter_impact(self, analysis):
        """Analyze impact of different parameters."""
        parameter_impact = {}
        
        for key_params, data in analysis.items():
            for param in key_params:
                param_name, param_value = param
                
                if param_name not in parameter_impact:
                    parameter_impact[param_name] = []
                
                parameter_impact[param_name].append((param_value, data['avg_firing_rate']))
        
        return parameter_impact
    
    def generate_recommendations(self, analysis):
        """Generate experiment recommendations."""
        recommendations = []
        
        # Find best configuration
        best_config = self.get_best_configuration(analysis)
        
        if best_config:
            # Configuration-specific recommendations
            params = dict(best_config['parameters'])
            
            if params.get('brain.neuron_count', 0) > 5000:
                recommendations.append({
                    'type': 'neuron_count_optimization',
                    'description': f'Consider reducing neuron count to {params["brain.neuron_count"] - 1000} for better performance',
                    'expected_improvement': '10-15% performance gain',
                    'priority': 'Medium'
                })
            
            if params.get('brain.synapse_density', 0) < 0.05:
                recommendations.append({
                    'type': 'synaptic_connectivity',
                    'description': f'Consider increasing synapse density to {params["brain.synapse_density"] + 0.05} for better engagement',
                    'expected_improvement': '5-10% firing rate increase',
                    'priority': 'Low'
                })
        
        return recommendations

# Example usage
batch_runner = BatchExperimentRunner("experiments.json", "batch_results")

# Create parameter grid
base_config = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "performance.optimization_level": 2
}

parameter_ranges = {
    "brain.neuron_count": [1000, 2000, 5000],
    "brain.synapse_density": [0.05, 0.1, 0.2],
    "performance.optimization_level": [1, 2, 3]
}

parameter_grid = batch_runner.create_parameter_grid(base_config, parameter_ranges)

# Run batch experiment
results = batch_runner.run_batch_experiment(
    parameter_grid,
    "cognitive_experiments",
    num_repeats=2
)

# Analyze results
analysis = batch_runner.analyze_batch_results(results)

# Generate report
report = batch_runner.generate_batch_report(results, analysis)

print(f"Batch experiment completed: {len(results)} experiments")
print(f"Best configuration: {report['summary']['best_configuration']['parameters']}")

# Save report
with open("batch_experiment_report.json", 'w') as f:
    json.dump(report, f, indent=2, default=str)

print("Batch experiment report saved to: batch_experiment_report.json")
```

#### Shell Script Batch Experiment Running

```bash
#!/bin/bash

# Batch experiment running script for NLM

# Configuration
NLM_EXEC="./nlm"
EXPERIMENT_CONFIG="experiments.json"
RESULT_DIR="batch_results"
BATCH_SCRIPT="run_batch_experiments.py"

# Create batch experiment script
python3 << EOF
import pynlm
import json
import os
import numpy as np
from typing import Dict, List, Any
from datetime import datetime

class NLMBatchExperimentRunner:
    def __init__(self, experiment_config_file, result_directory):
        self.experiment_config_file = experiment_config_file
        self.result_directory = result_directory
        self.load_experiment_config()
        os.makedirs(result_directory, exist_ok=True)
    
    def load_experiment_config(self):
        """Load experiment configuration."""
        with open(self.experiment_config_file, 'r') as f:
            self.experiment_config = json.load(f)
    
    def create_parameter_grid(self, base_config, parameter_ranges):
        """Create parameter grid for experiments."""
        parameter_grid = []
        
        # Generate parameter combinations
        for param_name, param_range in parameter_ranges.items():
            if isinstance(param_range, list):
                # Discrete parameter values
                for value in param_range:
                    config = base_config.copy()
                    config[param_name] = value
                    parameter_grid.append(config)
            elif isinstance(param_range, dict):
                # Range parameter values
                start = param_range.get('start', 0)
                end = param_range.get('end', 100)
                step = param_range.get('step', 10)
                
                for value in range(start, end + step, step):
                    config = base_config.copy()
                    config[param_name] = value
                    parameter_grid.append(config)
        
        return parameter_grid
    
    def run_single_experiment(self, experiment_name, config):
        """Run single experiment."""
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        # Create agent
        agent = pynlm.createBrain(brain)
        agent.initialize(world)
        
        # Run experiment
        return self.run_experiment_procedure(brain, world, agent, experiment_name, config)
    
    def run_experiment_procedure(self, brain, world, agent, experiment_name, config):
        """Run experiment procedure."""
        duration = 500  # Default duration
        
        # Initialize metrics
        metrics = {
            'neural': {'firing_rate': [], 'neuron_utilization': [], 'total_spikes': 0},
            'cognitive': {'curiosity_level': [], 'prediction_error': []},
            'behavioral': {'actions_taken': [], 'reward_received': []}
        }
        
        # Run simulation
        for step in range(duration):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            metrics['neural']['firing_rate'].append(brain.getAverageFiringRate())
            metrics['neural']['neuron_utilization'].append(
                (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100
            )
            metrics['cognitive']['curiosity_level'].append(agent.getCuriosityLevel())
            metrics['cognitive']['prediction_error'].append(agent.getPredictionError())
            metrics['behavioral']['actions_taken'].append(1)
            metrics['behavioral']['reward_received'].append(0.0)  # Simplified reward
        
        # Analyze results
        results = {
            'experiment_name': experiment_name,
            'timestamp': datetime.now().isoformat(),
            'config': config,
            'performance': {
                'total_time': duration * 0.1,  # Simplified timing
                'avg_firing_rate': np.mean(metrics['neural']['firing_rate']) if metrics['neural']['firing_rate'] else 0,
                'avg_curiosity': np.mean(metrics['cognitive']['curiosity_level']) if metrics['cognitive']['curiosity_level'] else 0,
                'avg_prediction_error': np.mean(metrics['cognitive']['prediction_error']) if metrics['cognitive']['prediction_error'] else 0
            },
            'metrics': metrics
        }
        
        return results

# Define configurations to test
configs = {
    'small': {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "performance.optimization_level": 1
    },
    'medium': {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.05,
        "performance.optimization_level": 2
    },
    'large': {
        "brain.neuron_count": 10000,
        "brain.synapse_density": 0.02,
        "performance.optimization_level": 3
    }
}

# Create parameter grid
parameter_ranges = {
    "brain.neuron_count": [1000, 5000],
    "brain.synapse_density": [0.05, 0.1],
    "performance.optimization_level": [1, 2, 3]
}

parameter_grid = []
for config_name, base_config in configs.items():
    for param_name, param_range in parameter_ranges.items():
        for value in param_range:
            config = base_config.copy()
            config[param_name] = value
            parameter_grid.append(config)

# Run batch experiments
all_results = []
for i, config in enumerate(parameter_grid):
    exp_name = f"batch_{i+1}"
    print(f"Running experiment {i+1}/{len(parameter_grid)}: {exp_name}")
    
    result = NLMBatchExperimentRunner("").run_single_experiment(exp_name, config)
    
    # Save result
    result_file = os.path.join(
        "$RESULT_DIR",
        f"{exp_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
    )
    
    with open(result_file, 'w') as f:
        json.dump(result, f, indent=2, default=str)
    
    all_results.append(result)
    print(f"  Completed: {result['performance']['avg_firing_rate']:.2f} Hz firing rate")

print(f"\nBatch experiment completed: {len(all_results)} experiments")

# Generate batch report
report = {
    'benchmark_name': 'NLM_Batch_Experiment',
    'timestamp': datetime.now().isoformat(),
    'summary': {
        'total_experiments': len(all_results),
        'avg_firing_rate': np.mean([r['performance']['avg_firing_rate'] for r in all_results]),
        'best_config': max(all_results, key=lambda x: x['performance']['avg_firing_rate'])['config']
    },
    'results': all_results
}

# Save report
with open("batch_experiment_report.json", 'w') as f:
    json.dump(report, f, indent=2, default=str)

print("Batch experiment report saved to: batch_experiment_report.json")
EOF

echo "Batch experiment running completed!"
```

## Experiment Results Analysis

### Experiment Results Analysis

```python
import pynlm
import json
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any
import seaborn as sns

class ExperimentResultsAnalyzer:
    def __init__(self, experiment_results_file):
        self.experiment_results_file = experiment_results_file
        self.results = self.load_results()
    
    def load_results(self):
        """Load experiment results from file."""
        with open(self.experiment_results_file, 'r') as f:
            return json.load(f)['results']
    
    def analyze_experiments(self):
        """Analyze experiment results."""
        analysis = {}
        
        for result in self.results:
            experiment_name = result['experiment_name']
            analysis[experiment_name] = {
                'performance': self.analyze_performance(result['performance']),
                'neural_metrics': self.analyze_neural_metrics(result['metrics']['neural']),
                'cognitive_metrics': self.analyze_cognitive_metrics(result['metrics']['cognitive']),
                'behavioral_metrics': self.analyze_behavioral_metrics(result['metrics']['behavioral'])
            }
        
        return analysis
    
    def analyze_performance(self, performance):
        """Analyze performance metrics."""
        return {
            'avg_firing_rate': performance['avg_firing_rate'],
            'total_time': performance['total_time'],
            'steps_per_second': 500 / performance['total_time'] if performance['total_time'] > 0 else 0,
            'performance_score': self.calculate_performance_score(performance)
        }
    
    def analyze_neural_metrics(self, neural_metrics):
        """Analyze neural metrics."""
        return {
            'avg_firing_rate': np.mean(neural_metrics['firing_rate']) if neural_metrics['firing_rate'] else 0,
            'std_firing_rate': np.std(neural_metrics['firing_rate']) if neural_metrics['firing_rate'] else 0,
            'max_firing_rate': max(neural_metrics['firing_rate']) if neural_metrics['firing_rate'] else 0,
            'neuron_utilization_trend': self.calculate_trend(neural_metrics['neuron_utilization']),
            'total_spikes': neural_metrics['total_spikes']
        }
    
    def analyze_cognitive_metrics(self, cognitive_metrics):
        """Analyze cognitive metrics."""
        return {
            'avg_curiosity': np.mean(cognitive_metrics['curiosity_level']) if cognitive_metrics['curiosity_level'] else 0,
            'std_curiosity': np.std(cognitive_metrics['curiosity_level']) if cognitive_metrics['curiosity_level'] else 0,
            'avg_prediction_error': np.mean(cognitive_metrics['prediction_error']) if cognitive_metrics['prediction_error'] else 0,
            'curiosity_trend': self.calculate_trend(cognitive_metrics['curiosity_level']),
            'prediction_error_trend': self.calculate_trend(cognitive_metrics['prediction_error'])
        }
    
    def analyze_behavioral_metrics(self, behavioral_metrics):
        """Analyze behavioral metrics."""
        return {
            'total_actions': sum(behavioral_metrics['actions_taken']),
            'avg_reward': np.mean(behavioral_metrics['reward_received']) if behavioral_metrics['reward_received'] else 0,
            'action_frequency': sum(behavioral_metrics['actions_taken']) / len(behavioral_metrics['actions_taken']) if behavioral_metrics['actions_taken'] else 0,
            'reward_variance': np.var(behavioral_metrics['reward_received']) if len(behavioral_metrics['reward_received']) > 1 else 0
        }
    
    def calculate_performance_score(self, performance):
        """Calculate performance score."""
        # Weighted performance score
        score = (
            0.4 * min(performance['avg_firing_rate'] / 50, 1.0) +  # Firing rate (normalized)
            0.3 * min(performance['steps_per_second'] / 1000, 1.0) +  # Steps per second
            0.3 * min(1.0 / performance['total_time'], 1.0)  # Time efficiency
        )
        
        return score
    
    def calculate_trend(self, values):
        """Calculate trend of values."""
        if len(values) < 2:
            return 'stable'
        
        if values[-1] > values[0] * 1.1:
            return 'increasing'
        elif values[-1] < values[0] * 0.9:
            return 'decreasing'
        else:
            return 'stable'
    
    def plot_experiment_results(self):
        """Plot experiment results."""
        if not plt:
            return None
        
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle('NLM Experiment Results Analysis', fontsize=16)
        
        # Extract metrics
        experiment_names = list(self.results[0].keys()) if self.results else []
        if experiment_names and 'performance' in self.results[0][experiment_names[0]]:
            firing_rates = [r[experiment_names[0]]['performance']['avg_firing_rate'] for r in self.results]
            steps_per_second = [r[experiment_names[0]]['performance']['steps_per_second'] for r in self.results]
            curiosity_levels = [r[experiment_names[0]]['cognitive_metrics']['avg_curiosity'] for r in self.results]
        
        # Plot 1: Firing rate comparison
        if 'firing_rates' in locals() and firing_rates:
            axes[0, 0].bar(range(len(firing_rates)), firing_rates)
            axes[0, 0].set_title('Firing Rate Comparison')
            axes[0, 0].set_xlabel('Experiment')
            axes[0, 0].set_ylabel('Firing Rate (Hz)')
        
        # Plot 2: Steps per second comparison
        if 'steps_per_second' in locals() and steps_per_second:
            axes[0, 1].bar(range(len(steps_per_second)), steps_per_second)
            axes[0, 1].set_title('Steps per Second Comparison')
            axes[0, 1].set_xlabel('Experiment')
            axes[0, 1].set_ylabel('Steps per Second')
        
        # Plot 3: Curiosity level comparison
        if 'curiosity_levels' in locals() and curiosity_levels:
            axes[0, 2].bar(range(len(curiosity_levels)), curiosity_levels)
            axes[0, 2].set_title('Curiosity Level Comparison')
            axes[0, 2].set_xlabel('Experiment')
            axes[0, 2].set_ylabel('Curiosity Level')
        
        # Plot 4: Performance correlation
        if all(locals().get(var, False) for var in ['firing_rates', 'steps_per_second', 'curiosity_levels']):
            fig2, ax2 = plt.subplots(figsize=(10, 8))
            sns.heatmap(np.corrcoef([firing_rates, steps_per_second, curiosity_levels]),
                       annot=True, cmap='coolwarm', center=0,
                       xticklabels=['Firing Rate', 'Steps/s', 'Curiosity'],
                       yticklabels=['Firing Rate', 'Steps/s', 'Curiosity'],
                       ax=ax2)
            ax2.set_title('Performance Correlation Matrix')
            plt.tight_layout()
        
        plt.tight_layout()
        return fig
    
    def generate_experiment_report(self):
        """Generate experiment analysis report."""
        analysis = self.analyze_experiments()
        
        report = {
            'benchmark_name': 'NLM_Experiment_Analysis',
            'timestamp': datetime.now().isoformat(),
            'summary': {
                'total_experiments': len(self.results),
                'avg_firing_rate': np.mean([r['performance']['avg_firing_rate'] for r in self.results]),
                'avg_curiosity': np.mean([r['cognitive_metrics']['avg_curiosity'] for r in self.results]),
                'avg_steps_per_second': np.mean([r['performance']['steps_per_second'] for r in self.results]),
                'best_experiment': self.get_best_experiment(analysis)
            },
            'analysis': analysis,
            'recommendations': self.generate_recommendations(analysis)
        }
        
        return report
    
    def get_best_experiment(self, analysis):
        """Get best experiment based on performance."""
        if not analysis:
            return None
        
        best_experiment = max(analysis.items(), key=lambda x: x[1]['performance']['performance_score'])
        
        return {
            'experiment_name': best_experiment[0],
            'performance': best_experiment[1]['performance'],
            'neural_metrics': best_experiment[1]['neural_metrics'],
            'cognitive_metrics': best_experiment[1]['cognitive_metrics']
        }
    
    def generate_recommendations(self, analysis):
        """Generate experiment recommendations."""
        recommendations = []
        
        # Find best experiment
        best_experiment = self.get_best_experiment(analysis)
        
        if best_experiment:
            # Experiment-specific recommendations
            performance = best_experiment['performance']
            
            if performance['avg_firing_rate'] < 20:
                recommendations.append({
                    'type': 'increase_firing_rate',
                    'description': 'Increase neuronal excitability or plasticity settings to improve firing rate',
                    'expected_improvement': '5-15% firing rate increase',
                    'priority': 'Medium'
                })
            
            if performance['steps_per_second'] < 500:
                recommendations.append({
                    'type': 'optimize_performance',
                    'description': 'Enable performance optimizations (vectorization, parallel processing)',
                    'expected_improvement': '50-200% performance gain',
                    'priority': 'High'
                })
            
            if performance['avg_curiosity'] < 0.1:
                recommendations.append({
                    'type': 'increase_curiosity',
                    'description': 'Increase curiosity settings for better exploration',
                    'expected_improvement': '10-20% curiosity increase',
                    'priority': 'Low'
                })
        
        return recommendations

# Example usage
analyzer = ExperimentResultsAnalyzer("experiment_results.json")

# Generate experiment report
report = analyzer.generate_experiment_report()

print("Experiment Analysis Report:")
print(f"Total experiments: {report['summary']['total_experiments']}")
print(f"Average firing rate: {report['summary']['avg_firing_rate']:.2f} Hz")
print(f"Average curiosity: {report['summary']['avg_curiosity']:.3f}")
print(f"Average steps per second: {report['summary']['avg_steps_per_second']:.2f}")
print(f"Best experiment: {report['summary']['best_experiment']['experiment_name']}")

if report['recommendations']:
    print(f"\nRecommendations ({len(report['recommendations'])}):")
    for i, recommendation in enumerate(report['recommendations'], 1):
        print(f"{i}. {recommendation['type'].upper()}: {recommendation['description']}")
        print(f"   Expected: {recommendation['expected_improvement']} (Priority: {recommendation['priority']})")

# Generate experiment plots
fig = analyzer.plot_experiment_results()
if fig:
    fig.savefig("experiment_analysis.png")
    print(f"\nExperiment analysis plot saved to: experiment_analysis.png")

# Save report
with open("experiment_analysis_report.json", 'w') as f:
    json.dump(report, f, indent=2, default=str)

print("Experiment analysis report saved to: experiment_analysis_report.json")
```

## Experiment Documentation

### Experiment Documentation Template

```json
{
    "experiment_template": {
        "name": "experiment_template",
        "description": "Template for NLM experiments",
        "category": "cognitive",
        "required_parameters": [
            "brain.neuron_count",
            "brain.synapse_density",
            "performance.optimization_level"
        ],
        "optional_parameters": [
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable",
            "neuromod.dopamine.scale"
        ],
        "experiment_procedures": {
            "temporal_prediction": "path/to/temporal_prediction.py",
            "working_memory": "path/to/working_memory.py",
            "episodic_recall": "path/to/episodic_recall.py"
        },
        "metrics": {
            "firing_rate": "continuous",
            "prediction_error": "continuous",
            "curiosity_level": "continuous",
            "novelty_level": "continuous"
        },
        "output_format": "json",
        "analysis_techniques": [
            "statistical_analysis",
            "correlation_analysis",
            "trend_analysis",
            "comparison_analysis"
        ]
    }
}
```

## Experiment Quality Assurance

### Experiment Quality Assurance

```python
import pynlm
import json
from typing import Dict, List, Any
import numpy as np
from datetime import datetime

class ExperimentQualityAssurance:
    def __init__(self):
        self.quality_checks = {
            'data_completeness': self.check_data_completeness,
            'parameter_validation': self.check_parameter_validation,
            'performance_consistency': self.check_performance_consistency,
            'statistical_significance': self.check_statistical_significance
        }
    
    def check_data_completeness(self, experiment_results):
        """Check data completeness of experiment results."""
        checks = {}
        
        for experiment_name, results in experiment_results.items():
            # Check required fields
            required_fields = ['performance', 'neural_metrics', 'cognitive_metrics']
            
            completeness_score = 0
            total_fields = len(required_fields)
            
            for field in required_fields:
                if field in results and results[field]:
                    completeness_score += 1
            
            checks[experiment_name] = {
                'completeness_score': completeness_score / total_fields,
                'missing_fields': [
                    field for field in required_fields
                    if field not in results or not results[field]
                ]
            }
        
        return checks
    
    def check_parameter_validation(self, experiment_results):
        """Check parameter validation in experiment results."""
        checks = {}
        
        for experiment_name, results in experiment_results.items():
            config = results.get('config', {})
            
            # Check parameter ranges
            parameter_checks = {}
            
            # Check neuron count
            neuron_count = config.get('brain.neuron_count', 0)
            parameter_checks['neuron_count'] = 100 <= neuron_count <= 100000
            
            # Check synapse density
            synapse_density = config.get('brain.synapse_density', 0)
            parameter_checks['synapse_density'] = 0.01 <= synapse_density <= 0.5
            
            # Check optimization level
            optimization_level = config.get('performance.optimization_level', 0)
            parameter_checks['optimization_level'] = 1 <= optimization_level <= 3
            
            checks[experiment_name] = {
                'parameter_checks': parameter_checks,
                'overall_validity': all(parameter_checks.values())
            }
        
        return checks
    
    def check_performance_consistency(self, experiment_results):
        """Check performance consistency across experiments."""
        checks = {}
        
        # Group results by configuration
        config_groups = {}
        
        for experiment_name, results in experiment_results.items():
            config = tuple(sorted(results.get('config', {}).items()))
            
            if config not in config_groups:
                config_groups[config] = []
            
            config_groups[config].append(results)
        
        # Check consistency within each configuration group
        for config, group_results in config_groups.items():
            if len(group_results) < 2:
                continue
            
            # Check performance consistency
            firing_rates = []
            curiosity_levels = []
            steps_per_second = []
            
            for result in group_results:
                if 'performance' in result:
                    firing_rates.append(result['performance']['avg_firing_rate'])
                    curiosity_levels.append(result['cognitive_metrics']['avg_curiosity'])
                    steps_per_second.append(result['performance']['steps_per_second'])
            
            # Calculate standard deviations
            firing_rate_std = np.std(firing_rates) if len(firing_rates) > 1 else 0
            curiosity_std = np.std(curiosity_levels) if len(curiosity_levels) > 1 else 0
            performance_std = np.std(steps_per_second) if len(steps_per_second) > 1 else 0
            
            checks[f"config_{len(config_groups)}"] = {
                'firing_rate_std': firing_rate_std,
                'curiosity_std': curiosity_std,
                'performance_std': performance_std,
                'consistency_score': 1 - (firing_rate_std + curiosity_std + performance_std) / 3,
                'is_consistent': (firing_rate_std < 5 and curiosity_std < 0.1 and performance_std < 0.5)
            }
        
        return checks
    
    def check_statistical_significance(self, experiment_results):
        """Check statistical significance of experiment results."""
        checks = {}
        
        # Perform statistical analysis
        for experiment_name, results in experiment_results.items():
            # Collect metrics for statistical analysis
            metrics = []
            
            if 'performance' in results:
                metrics.append(results['performance']['avg_firing_rate'])
                metrics.append(results['performance']['steps_per_second'])
                metrics.append(results['cognitive_metrics']['avg_curiosity'])
            
            if metrics:
                # Calculate basic statistics
                mean = np.mean(metrics)
                std = np.std(metrics) if len(metrics) > 1 else 0
                
                # Check if results are statistically significant
                # (Simplified check - in practice, more sophisticated statistical tests would be used)
                is_significant = len(metrics) > 1 and std < mean * 0.5
                
                checks[experiment_name] = {
                    'mean': mean,
                    'std': std,
                    'is_significant': is_significant,
                    'sample_size': len(metrics)
                }
        
        return checks
    
    def perform_quality_assurance(self, experiment_results):
        """Perform quality assurance checks on experiment results."""
        qa_report = {
            'timestamp': datetime.now().isoformat(),
            'overall_quality_score': 0,
            'checks': {},
            'recommendations': []
        }
        
        # Perform each quality check
        for check_name, check_function in self.quality_checks.items():
            try:
                checks = check_function(experiment_results)
                qa_report['checks'][check_name] = checks
                
                # Calculate overall quality score
                for experiment_name, check_result in checks.items():
                    if isinstance(check_result, dict) and 'overall_validity' in check_result:
                        qa_report['overall_quality_score'] += check_result['overall_validity']
                    elif isinstance(check_result, dict) and 'is_consistent' in check_result:
                        qa_report['overall_quality_score'] += check_result['is_consistent']
                    elif isinstance(check_result, dict) and 'is_significant' in check_result:
                        qa_report['overall_quality_score'] += check_result['is_significant']
            except Exception as e:
                qa_report['recommendations'].append(
                    f"Fix {check_name} check: {str(e)}"
                )
        
        # Normalize quality score
        total_checks = sum(len(checks) for checks in qa_report['checks'].values())
        if total_checks > 0:
            qa_report['overall_quality_score'] = qa_report['overall_quality_score'] / total_checks
        
        return qa_report

# Example usage
qa_assurance = ExperimentQualityAssurance()

# Example experiment results
experiment_results = {
    'temporal_prediction': {
        'performance': {
            'avg_firing_rate': 25.0,
            'total_time': 100.0,
            'steps_per_second': 250.0
        },
        'neural_metrics': {'firing_rate': [20, 25, 30], 'neuron_utilization': [10, 15, 20]},
        'cognitive_metrics': {'curiosity_level': [0.2, 0.3, 0.4], 'prediction_error': [0.1, 0.05, 0.02]},
        'behavioral_metrics': {'actions_taken': [10, 15, 20], 'reward_received': [0.5, 0.7, 0.9]},
        'config': {
            'brain.neuron_count': 5000,
            'brain.synapse_density': 0.1,
            'performance.optimization_level': 2
        }
    },
    'working_memory': {
        'performance': {
            'avg_firing_rate': 15.0,
            'total_time': 100.0,
            'steps_per_second': 150.0
        },
        'neural_metrics': {'firing_rate': [10, 15, 20], 'neuron_utilization': [5, 10, 15]},
        'cognitive_metrics': {'curiosity_level': [0.1, 0.2, 0.3], 'prediction_error': [0.2, 0.15, 0.1]},
        'behavioral_metrics': {'actions_taken': [8, 12, 16], 'reward_received': [0.3, 0.5, 0.7]},
        'config': {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.2,
            'performance.optimization_level': 1
        }
    }
}

# Perform quality assurance
qa_report = qa_assurance.perform_quality_assurance(experiment_results)

print("Quality Assurance Report:")
print(f"Overall Quality Score: {qa_report['overall_quality_score']:.2%}")
print(f"Number of checks passed: {sum(1 for check in qa_report['checks'].values() if any(v for v in check.values() if isinstance(v, dict) and v.get('overall_validity', True))}")

if qa_report['recommendations']:
    print(f"\nRecommendations ({len(qa_report['recommendations'])}):")
    for i, recommendation in enumerate(qa_report['recommendations'], 1):
        print(f"{i}. {recommendation}")

# Save quality assurance report
with open("quality_assurance_report.json", 'w') as f:
    json.dump(qa_report, f, indent=2, default=str)

print("Quality assurance report saved to: quality_assurance_report.json")
```

## Experiment Troubleshooting

### Experiment Troubleshooting

```python
import pynlm
import json
from typing import Dict, List, Any

class ExperimentTroubleshooter:
    def __init__(self):
        self.common_issues = {
            'low_performance': {
                'symptoms': ['low_firing_rate', 'high_simulation_time', 'low_curiosity'],
                'solutions': [
                    'Increase neuron count',
                    'Enable performance optimizations',
                    'Increase plasticity settings',
                    'Optimize simulation parameters'
                ]
            },
            'memory_issues': {
                'symptoms': ['high_memory_usage', 'memory_allocation_failures', 'system_slowdown'],
                'solutions': [
                    'Enable memory pooling',
                    'Reduce simulation complexity',
                    'Optimize plasticity settings',
                    'Increase memory pool size'
                ]
            },
            'data_collection_issues': {
                'symptoms': ['missing_metrics', 'incomplete_data', 'data_corruption'],
                'solutions': [
                    'Check data collection code',
                    'Verify metrics recording',
                    'Validate data storage',
                    'Check file permissions'
                ]
            }
        }
    
    def diagnose_experiment_issue(self, experiment_results):
        """Diagnose experiment issue based on results."""
        issues = []
        
        for experiment_name, results in experiment_results.items():
            # Check for low performance
            if ('performance' in results and
                results['performance']['avg_firing_rate'] < 20):
                issues.append(('low_performance', experiment_name))
            
            # Check for memory issues
            if ('neural_metrics' in results and
                results['neural_metrics'].get('max_firing_rate', 0) < 10):
                issues.append(('low_performance', experiment_name))
            
            # Check for curiosity issues
            if ('cognitive_metrics' in results and
                results['cognitive_metrics'].get('avg_curiosity', 0) < 0.1):
                issues.append(('low_curiosity', experiment_name))
        
        return issues
    
    def get_troubleshooting_guide(self, issue_type, experiment_name):
        """Get troubleshooting guide for specific issue."""
        if issue_type in self.common_issues:
            return {
                'issue_type': issue_type,
                'experiment_name': experiment_name,
                'solutions': self.common_issues[issue_type]['solutions'],
                'description': f"Troubleshooting guide for {issue_type} in {experiment_name}"
            }
        
        return None
    
    def troubleshoot_experiments(self, experiment_results):
        """Troubleshoot experiment issues."""
        troubleshooting_report = {
            'timestamp': datetime.now().isoformat(),
            'issues_found': 0,
            'solutions_provided': 0,
            'issues': [],
            'recommendations': []
        }
        
        # Diagnose issues
        issues = self.diagnose_experiment_issue(experiment_results)
        troubleshooting_report['issues_found'] = len(issues)
        
        # Get troubleshooting guides
        for issue_type, experiment_name in issues:
            guide = self.get_troubleshooting_guide(issue_type, experiment_name)
            
            if guide:
                troubleshooting_report['issues'].append(guide)
                troubleshooting_report['solutions_provided'] += len(guide['solutions'])
        
        # Generate recommendations
        for issue in troubleshooting_report['issues']:
            troubleshooting_report['recommendations'].append(
                f"Fix {issue['issue_type']} for experiment {issue['experiment_name']}: {issue['description']}"
            )
        
        return troubleshooting_report

# Example usage
troubleshooter = ExperimentTroubleshooter()

# Example experiment results
experiment_results = {
    'temporal_prediction': {
        'performance': {
            'avg_firing_rate': 15.0,
            'total_time': 200.0,
            'steps_per_second': 100.0
        },
        'neural_metrics': {'firing_rate': [10, 15, 20], 'max_firing_rate': 20},
        'cognitive_metrics': {'curiosity_level': [0.1, 0.2, 0.15], 'avg_curiosity': 0.15},
        'config': {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.2,
            'performance.optimization_level': 1
        }
    }
}

# Troubleshoot experiments
troubleshooting_report = troubleshooter.troubleshoot_experiments(experiment_results)

print("Experiment Troubleshooting Report:")
print(f"Issues found: {troubleshooting_report['issues_found']}")
print(f"Solutions provided: {troubleshooting_report['solutions_provided']}")

if troubleshooting_report['issues']:
    print(f"\nIssues ({len(troubleshooting_report['issues'])}):")
    for i, issue in enumerate(troubleshooting_report['issues'], 1):
        print(f"{i}. {issue['issue_type'].upper()} for {issue['experiment_name']}:")
        for j, solution in enumerate(issue['solutions'], 1):
            print(f"   {j}. {solution}")

if troubleshooting_report['recommendations']:
    print(f"\nRecommendations ({len(troubleshooting_report['recommendations'])}):")
    for i, recommendation in enumerate(troubleshooting_report['recommendations'], 1):
        print(f"{i}. {recommendation}")

# Save troubleshooting report
troubleshooting_report_data = {
    'benchmark_name': 'NLM_Experiment_Troubleshooting',
    'timestamp': datetime.now().isoformat(),
    'troubleshooting_report': troubleshooting_report
}

with open("experiment_troubleshooting_report.json", 'w') as f:
    json.dump(troubleshooting_report_data, f, indent=2, default=str)

print("Experiment troubleshooting report saved to: experiment_troubleshooting_report.json")
```

## Experiment Best Practices

### Experiment Best Practices

1. **Use consistent configuration**: Use consistent configuration across experiments
2. **Document experiments**: Document all experiments with detailed descriptions
3. **Use standard metrics**: Use standard metrics for all experiments
4. **Control variables**: Control all variables except the one being tested
5. **Replicate experiments**: Replicate experiments to ensure consistency
6. **Validate results**: Validate results using statistical analysis
7. **Document methodology**: Document all methodology and procedures
8. **Share results**: Share results with other researchers

### Experiment Documentation Best Practices

1. **Document experiment design**: Document experiment design and rationale
2. **Document configuration**: Document all configuration parameters
3. **Document procedures**: Document all procedures and protocols
4. **Document analysis**: Document all analysis techniques
5. **Document results**: Document all results and interpretations
6. **Document limitations**: Document all limitations and assumptions
7. **Document future work**: Document future work and recommendations

## Experiment Quality Checklist

### Experiment Quality Checklist

#### Experiment Design Checklist
- [ ] Experiment has clear objectives
- [ ] Experiment has well-defined hypotheses
- [ ] Experiment has appropriate control groups
- [ ] Experiment uses valid measurement techniques
- [ ] Experiment has adequate sample size
- [ ] Experiment is reproducible
- [ ] Experiment has clear methodology
- [ ] Experiment has documentation

#### Experiment Execution Checklist
- [ ] All equipment is calibrated
- [ ] All procedures are documented
- [ ] All data are collected
- [ ] All data are validated
- [ ] All analyses are performed
- [ ] All results are interpreted
- [ ] All conclusions are drawn
- [ ] All recommendations are made

#### Experiment Documentation Checklist
- [ ] Experiment has clear title
- [ ] Experiment has abstract
- [ ] Experiment has introduction
- [ ] Experiment has methodology
- [ ] Experiment has results
- [ ] Experiment has discussion
- [ ] Experiment has conclusion
- [ ] Experiment has references

## Future Enhancements

### Planned Features

1. **Automated experiment design**: Automated experiment design and optimization
2. **Experiment sharing**: Share experiments with other researchers
3. **Experiment comparison**: Compare experiments across different conditions
4. **Experiment replication**: Replicate experiments across different platforms
5. **Experiment validation**: Validate experiments using statistical methods
6. **Experiment documentation**: Auto-generate experiment documentation
7. **Experiment monitoring**: Monitor experiment progress and performance
8. **Experiment optimization**: Optimize experiment parameters

### Known Limitations

1. **Experiment complexity**: Complex experiments are hard to design
2. **Experiment validation**: Experiment validation can be challenging
3. **Experiment replication**: Experiment replication can be difficult
4. **Experiment comparison**: Experiment comparison can be complex
5. **Experiment documentation**: Experiment documentation can be time-consuming

## Experiment Support and Resources

### Getting Help

- **Documentation**: `docs/EXPERIMENT_GUIDE.md`
- **API Reference**: `docs/API_REFERENCE.md`
- **Command Line Guide**: `docs/COMMAND_LINE_GUIDE.md`
- **Configuration Guide**: `docs/CONFIGURATION_GUIDE.md`
- **Performance Guide**: `docs/PERFORMANCE_GUIDE.md`

### Additional Resources

- **Experiment Templates**: `templates/` directory
- **Experiment Examples**: `examples/` directory
- **Experiment Testing**: `tests/` directory
- **Experiment Documentation**: `docs/` directory

## Conclusion

NLM experiment management is a critical aspect of NLM that allows researchers to study neural behavior, learning, and development in controlled environments. This guide provides comprehensive documentation for NLM experiment management, including experiment design, management, analysis, and troubleshooting.

Effective experiment management is essential for:

1. **Research and experimentation**: Design and run experiments to study neural behavior
2. **Learning and development**: Study learning and development processes
3. **Cognitive testing**: Test cognitive capabilities of NLM
4. **Performance evaluation**: Evaluate NLM performance across different conditions
5. **Validation and verification**: Validate and verify experiment results

This guide provides the knowledge and tools needed to effectively manage NLM experiments. For more advanced experiment tasks, refer to the API reference, command-line guide, and other documentation files.