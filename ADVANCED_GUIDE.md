Advanced NLM Usage Guide

## Overview

This guide builds upon the [Easy Usage Guide](easy_usage.md) and provides advanced features for experienced NLM users, including:

- Custom agent and world implementations
- Advanced simulation management
- Performance optimization
- Experiment tracking and analysis
- Configuration management
- Plugin system usage
- Advanced debugging and monitoring

---

## Advanced Concepts

### 1. Custom Agent Implementations

#### Creating Custom Agents

NLM's extensibility framework allows you to create custom agents without modifying the core library. Here's how to create a custom agent:

```python
import pynlm
from nlm_extensibility import PluginHelpers

# Create a custom agent factory
def create_advanced_agent():
    """Example advanced agent with custom behavior."""
    # Create brain and world as usual
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create custom agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Add custom behavior
    def custom_behavior(percept, brain, agent):
        # Custom decision logic
        # This could implement Q-learning, custom reward shaping, etc.
        action = agent.decodeMotorCommand()
        
        # Custom neuromodulation
        reward = percept.getInternal()[0] * 10.0  # Use internal signals as reward
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        return action
    
    # Replace agent's processSensoryInput with custom version
    original_process = agent.processSensoryInput
    def enhanced_processSensoryInput(percept):
        original_process(percept)
        custom_behavior(percept, brain, agent)
    
    agent.processSensoryInput = enhanced_processSensoryInput
    
    return {
        'brain': brain,
        'world': world, 
        'agent': agent,
        'custom_behavior': custom_behavior
    }

# Register custom agent with extensibility system
advanced_agent = create_advanced_agent()
pynlm.register_custom_agent('advanced', advanced_agent)
```

#### Using the Plugin System

```python
from nlm_extensibility import NLMRegistry, initializeExtensibilitySystem

# Initialize the extensibility system
initializeExtensibilitySystem()

# Register custom agent factory
from nlm_extensibility import DefaultAgentFactory

def my_custom_agent_factory():
    """Factory function for creating custom agents."""
    # Implementation of agent creation
    pass

registry = NLMRegistry.getInstance()
factory = registry.getAgentFactory()
if factory:
    std::dynamic_pointer_cast<DefaultAgentFactory>(factory)->registerAgent(
        'my_custom_agent', my_custom_agent_factory
    )

# Create agent using the factory
agent = PluginHelpers.createAgentTyped<MyCustomAgentType>('my_custom_agent')
```

### 2. Advanced Simulation Management

#### Batch Processing with Configuration Variations

```python
import pynlm
import numpy as np
from datetime import datetime

def run_parameter_sweep(base_config, param_ranges, iterations=10):
    """Run a parameter sweep to find optimal configurations."""
    results = []
    
    for iteration in range(iterations):
        # Create configuration with parameter variations
        config = pynlm.createDefaultConfig()
        
        # Copy base config parameters
        for key, value in base_config.items():
            config.set(key, value)
        
        # Add random variations based on ranges
        for param, range_values in param_ranges.items():
            if isinstance(range_values, tuple):  # (min, max)
                value = np.random.uniform(range_values[0], range_values[1])
            elif isinstance(range_values, list):  # discrete values
                value = np.random.choice(range_values)
            else:
                value = range_values * (1 + np.random.normal(0, 0.1))  # 10% noise
            
            config.set(param, value)
        
        # Set unique random seed for reproducibility
        config.set("random_seed", 42 + iteration)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(1000):  # 1000 steps
            brain.step(step)
        end_time = time.time()
        
        # Record results
        result = {
            'iteration': iteration,
            'parameters': param_ranges,
            'final_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'simulation_time': end_time - start_time,
            'avg_firing_rate': brain.getAverageFiringRate(),
            'config': config
        }
        
        results.append(result)
        print(f"Iteration {iteration + 1}/{iterations} completed: {result['final_spikes']} spikes")
    
    return results

# Define parameter ranges for sweep
param_ranges = {
    'neuron_count': (100, 500),           # Continuous range
    'plasticity_learning_rate': [0.001, 0.01, 0.05, 0.1],  # Discrete values
    'connection_probability': (0.05, 0.2),
    'dopamine_baseline': (0.0, 0.5)
}

# Run parameter sweep
base_config = {
    'simulation_timestep': 0.001,
    'region_count': 1
}

results = run_parameter_sweep(base_config, param_ranges, iterations=20)

# Analyze results and find best configuration
best_result = max(results, key=lambda x: x['final_spikes'] / x['simulation_time'])
print(f"Best configuration: {best_result['parameters']}")
print(f"Score: {best_result['final_spikes'] / best_result['simulation_time']:.2f} spikes/sec")
```

### 3. Performance Optimization

#### Parallel Processing

```python
import multiprocessing
import pynlm
from concurrent.futures import ProcessPoolExecutor
import os

def simulate_worker(config_data):
    """Worker function for parallel simulation."""
    iteration, config_dict = config_data
    
    # Convert config dict to pynlm config
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    # Run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
    
    return {
        'iteration': iteration,
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'neurons': brain.getTotalNeuronCount()
    }

def parallel_simulations(base_config, num_processes=4, iterations_per_process=5):
    """Run simulations in parallel."""
    # Prepare work for parallel processing
    work = []
    for i in range(num_processes * iterations_per_process):
        config_dict = base_config.copy()
        config_dict['random_seed'] = 42 + i
        work.append((i, config_dict))
    
    # Process in parallel
    with ProcessPoolExecutor(max_workers=num_processes) as executor:
        results = list(executor.map(simulate_worker, work))
    
    # Aggregate results
    iterations = {}
    for result in results:
        if result['iteration'] not in iterations:
            iterations[result['iteration']] = []
        iterations[result['iteration']].append(result)
    
    return results, iterations

# Run parallel simulations
base_config = {
    'neuron_count': 500,
    'region_count': 2,
    'simulation_timestep': 0.001
}

results, iterations = parallel_simulations(base_config, num_processes=4, iterations_per_process=5)
print(f"Completed {len(results)} parallel simulations")

# Analyze parallel results
avg_spikes = sum(r['spikes'] for r in results) / len(results)
print(f"Average spikes across all simulations: {avg_spikes:.2f}")
```

#### Memory Optimization

```python
import pynlm
import gc
import psutil
import os

def memory_optimized_simulation(config, steps=5000):
    """Run simulation with memory optimization."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Pre-allocate arrays to reduce memory allocation overhead
    spike_counts = []
    firing_rates = []
    
    for step in range(steps):
        brain.step(step)
        
        # Record metrics periodically to avoid large arrays
        if step % 100 == 0:
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getFiringNeuronCount())
            
            # Force garbage collection periodically
            if step % 1000 == 0:
                gc.collect()
                process = psutil.Process(os.getpid())
                print(f"Step {step}: Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
    
    return {
        'final_spikes': spike_counts[-1] if spike_counts else 0,
        'final_firing_rate': firing_rates[-1] if firing_rates else 0,
        'spike_history': spike_counts,
        'firing_rate_history': firing_rates
    }

# Create memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 300)  # Smaller for memory efficiency
config.set("region_count", 1)     # Fewer regions

result = memory_optimized_simulation(config, steps=2000)
print(f"Memory-optimized simulation completed: {result['final_spikes']} spikes")
```

### 4. Advanced Experiment Management

#### Experiment Tracking

```python
import pynlm
import json
import os
from datetime import datetime
from pathlib import Path

class ExperimentTracker:
    """Advanced experiment tracking and management."""
    
    def __init__(self, experiment_name, base_directory="experiments"):
        self.experiment_name = experiment_name
        self.base_directory = Path(base_directory)
        self.experiment_dir = self.base_directory / experiment_name
        self.experiment_dir.mkdir(parents=True, exist_ok=True)
        
        self.metadata = {
            'name': experiment_name,
            'start_time': datetime.now().isoformat(),
            'description': 'Advanced NLM experiment',
            'parameters': {},
            'results': [],
            'metrics': {}
        }
        
        # Save initial metadata
        with open(self.experiment_dir / 'metadata.json', 'w') as f:
            json.dump(self.metadata, f, indent=2)
    
    def log_parameters(self, **kwargs):
        """Log experiment parameters."""
        self.metadata['parameters'].update(kwargs)
        self._save_metadata()
    
    def log_result(self, iteration, metrics):
        """Log experiment results."""
        result = {
            'iteration': iteration,
            'timestamp': datetime.now().isoformat(),
            'metrics': metrics
        }
        
        self.metadata['results'].append(result)
        self._save_metadata()
        
        # Save individual result file
        with open(self.experiment_dir / f'iteration_{iteration:04d}.json', 'w') as f:
            json.dump(result, f, indent=2)
    
    def compute_metrics(self, metric_functions):
        """Compute aggregated metrics from all results."""
        if not self.metadata['results']:
            return
        
        for name, func in metric_functions.items():
            try:
                values = [r['metrics'].get(name, 0) for r in self.metadata['results']]
                self.metadata['metrics'][name] = {
                    'mean': sum(values) / len(values),
                    'std': (sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5,
                    'min': min(values),
                    'max': max(values),
                    'values': values
                }
            except Exception as e:
                self.metadata['metrics'][name] = {'error': str(e)}
        
        self._save_metadata()
    
    def generate_report(self):
        """Generate a comprehensive experiment report."""
        report = f"# Experiment Report: {self.experiment_name}\n\n"
        report += f"**Start Time:** {self.metadata['start_time']}\n\n"
        
        report += "## Parameters\n\n"
        for key, value in self.metadata['parameters'].items():
            report += f"- **{key}:** {value}\n"
        
        report += "\n## Results Summary\n\n"
        report += f"Total iterations: {len(self.metadata['results'])}\n\n"
        
        report += "## Aggregated Metrics\n\n"
        for metric_name, metric_data in self.metadata['metrics'].items():
            if 'error' in metric_data:
                report += f"### {metric_name}\nError: {metric_data['error']}\n\n"
            else:
                report += f"### {metric_name}\n"
                report += f"- **Mean:** {metric_data['mean']:.4f}\n"
                report += f"- **Std:** {metric_data['std']:.4f}\n"
                report += f"- **Min:** {metric_data['min']:.4f}\n"
                report += f"- **Max:** {metric_data['max']:.4f}\n\n"
        
        # Save report
        report_file = self.experiment_dir / 'report.md'
        with open(report_file, 'w') as f:
            f.write(report)
        
        return report
    
    def _save_metadata(self):
        """Save metadata to file."""
        with open(self.experiment_dir / 'metadata.json', 'w') as f:
            json.dump(self.metadata, f, indent=2)

# Example usage
experiment = ExperimentTracker("advanced_neural_experiment")
experiment.log_parameters(
    neuron_count=500,
    region_count=2,
    plasticity_rate=0.01,
    dopamine_baseline=0.1,
    simulation_steps=1000
)

# Run multiple iterations
for i in range(10):
    # Create brain and run simulation
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("random_seed", 42 + i)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
    
    # Log results
    metrics = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'final_stage': brain.getDevelopmentalStage()
    }
    
    experiment.log_result(i, metrics)

# Compute aggregated metrics
metric_functions = {
    'total_spikes': lambda x: x,
    'firing_rate': lambda x: x
}

experiment.compute_metrics(metric_functions)

# Generate report
report = experiment.generate_report()
print(report)
```

### 5. Configuration Management

#### Advanced Configuration System

```python
import pynlm
import json
import os
from pathlib import Path
from typing import Dict, Any, Optional

class AdvancedConfigManager:
    """Advanced configuration management for NLM experiments."""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
        # Default configurations
        self.default_configs = {
            'small': {
                'neuron_count': 100,
                'region_count': 1,
                'connection_probability': 0.1,
                'plasticity_learning_rate': 0.01,
                'dopamine_baseline': 0.1,
                'simulation_timestep': 0.001
            },
            'medium': {
                'neuron_count': 500,
                'region_count': 2,
                'connection_probability': 0.15,
                'plasticity_learning_rate': 0.01,
                'dopamine_baseline': 0.1,
                'simulation_timestep': 0.001
            },
            'large': {
                'neuron_count': 1000,
                'region_count': 3,
                'connection_probability': 0.2,
                'plasticity_learning_rate': 0.02,
                'dopamine_baseline': 0.2,
                'simulation_timestep': 0.001
            },
            'research': {
                'neuron_count': 2000,
                'region_count': 4,
                'connection_probability': 0.25,
                'plasticity_learning_rate': 0.03,
                'dopamine_baseline': 0.3,
                'simulation_timestep': 0.0005
            }
        }
    
    def create_config(self, base_config_name, variations=None, save_to_file=None):
        """Create configuration with variations."""
        if base_config_name not in self.default_configs:
            raise ValueError(f"Unknown config: {base_config_name}")
        
        # Start with base config
        config_dict = self.default_configs[base_config_name].copy()
        
        # Apply variations
        if variations:
            config_dict.update(variations)
        
        # Create pynlm config
        config = pynlm.createDefaultConfig()
        
        # Set all configuration values
        for key, value in config_dict.items():
            config.set(key, value)
        
        # Save if requested
        if save_to_file:
            self.save_config(save_to_file, config_dict)
        
        return config, config_dict
    
    def save_config(self, name, config_dict):
        """Save configuration to file."""
        filename = self.config_dir / f"{name}.json"
        
        # Add metadata
        config_data = {
            'name': name,
            'created': datetime.now().isoformat(),
            'config': config_dict
        }
        
        with open(filename, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        return filename
    
    def load_config(self, name):
        """Load configuration from file."""
        filename = self.config_dir / f"{name}.json"
        
        if not filename.exists():
            raise FileNotFoundError(f"Config file not found: {filename}")
        
        with open(filename, 'r') as f:
            config_data = json.load(f)
        
        return config_data['config']
    
    def get_presets(self):
        """Get available preset configurations."""
        return list(self.default_configs.keys())
    
    def create_experiment_config(self, experiment_name, param_ranges, num_experiments=10):
        """Create multiple experiment configurations with parameter variations."""
        experiments = []
        
        for i in range(num_experiments):
            config_dict = {}
            
            # Add variations for each parameter
            for param, range_values in param_ranges.items():
                if isinstance(range_values, tuple):  # (min, max)
                    import random
                    value = random.uniform(range_values[0], range_values[1])
                elif isinstance(range_values, list):  # discrete values
                    import random
                    value = random.choice(range_values)
                else:
                    value = range_values * (1 + random.normalvariate(0, 0.1))
                
                config_dict[param] = value
            
            # Add unique seed
            config_dict['random_seed'] = 42 + i
            
            # Create experiment directory
            exp_dir = self.config_dir / f"exp_{experiment_name}"
            exp_dir.mkdir(exist_ok=True)
            
            # Save experiment config
            exp_filename = exp_dir / f"config_{i:04d}.json"
            with open(exp_filename, 'w') as f:
                json.dump({
                    'name': f"{experiment_name}_exp_{i}",
                    'iteration': i,
                    'config': config_dict,
                    'created': datetime.now().isoformat()
                }, f, indent=2)
            
            experiments.append((i, config_dict))
        
        return experiments

# Example usage
config_manager = AdvancedConfigManager()

# Create medium-sized config
config, params = config_manager.create_config(
    'medium',
    variations={'neuron_count': 750},  # Modify neuron count
    save_to_file='custom_medium_config'
)

print(f"Created custom medium config with {params['neuron_count']} neurons")

# Create multiple experiment configurations
param_ranges = {
    'neuron_count': (500, 1500),
    'plasticity_learning_rate': (0.005, 0.05),
    'dopamine_baseline': (0.05, 0.5)
}

experiments = config_manager.create_experiment_config(
    'plasticity_study',
    param_ranges,
    num_experiments=5
)

print(f"Created {len(experiments)} experiment configurations")
```

### 6. Debugging and Monitoring

#### Advanced Debugging Tools

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
import traceback

class NLMDebugger:
    """Advanced debugging tools for NLM simulations."""
    
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = []
        self.step_count = 0
        
        # Performance tracking
        self.step_times = []
        self.spike_counts = []
        self.firing_rates = []
        
    def start_debug_session(self):
        """Start a debug session and log initial state."""
        self.debug_log.append({
            'timestamp': datetime.now().isoformat(),
            'event': 'debug_session_started',
            'initial_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'regions': self.brain.getRegionCount(),
                'plasticity_enabled': self.agent.enableStructuralPlasticity() if self.agent else None
            }
        })
    
    def log_step(self, step, current_time):
        """Log detailed information for a simulation step."""
        try:
            # Record performance metrics
            start_time = time.time()
            
            # Get current brain state
            brain_state = {
                'step': step,
                'timestamp': current_time,
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'regions': self.brain.getRegionCount(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'total_neurons': self.brain.getTotalNeuronCount()
            }
            
            # Log agent state if available
            if self.agent:
                agent_state = {
                    'novelty_level': self.agent.getNoveltyLevel(),
                    'curiosity_level': self.agent.getCuriosityLevel(),
                    'prediction_error': self.agent.getPredictionError(),
                    'reward_modulation': self.agent.isRewardModulationEnabled(),
                    'structural_plasticity': self.agent.isStructuralPlasticityEnabled(),
                    'development_enabled': self.agent.isDevelopmentEnabled(),
                    'curiosity_enabled': self.agent.isCuriosityEnabled()
                }
                brain_state['agent'] = agent_state
            
            # Log world state if available
            if self.world:
                world_state = {
                    'simulation_time': self.world.getSimulationTime(),
                    'agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
                    'agent_energy': self.world.getAgentBody().energy,
                    'agent_health': self.world.getAgentBody().health,
                    'object_count': len(self.world.getObjects()) if hasattr(self.world, 'getObjects') else 0
                }
                brain_state['world'] = world_state
            
            # Calculate performance
            step_time = time.time() - start_time
            self.step_times.append(step_time)
            
            brain_state['step_time'] = step_time
            brain_state['steps_per_second'] = 1.0 / step_time if step_time > 0 else 0
            
            # Log to debug history
            self.debug_log.append(brain_state)
            self.step_count = step
            
            # Record metrics for analysis
            self.spike_counts.append(brain_state['total_spikes'])
            self.firing_rates.append(brain_state['firing_neurons'])
            
            # Log warnings and errors
            if brain_state['total_spikes'] > 10000:
                self.debug_log.append({
                    'timestamp': datetime.now().isoformat(),
                    'event': 'warning',
                    'message': f"High spike count: {brain_state['total_spikes']}"
                })
            
            if brain_state['firing_neurons'] == 0 and step > 100:
                self.debug_log.append({
                    'timestamp': datetime.now().isoformat(),
                    'event': 'warning',
                    'message': f"No firing neurons after {step} steps"
                })
                
        except Exception as e:
            error_entry = {
                'timestamp': datetime.now().isoformat(),
                'event': 'error',
                'step': step,
                'error': str(e),
                'traceback': traceback.format_exc()
            }
            self.debug_log.append(error_entry)
            print(f"Debug error at step {step}: {e}")
    
    def generate_debug_report(self, output_file=None):
        """Generate a comprehensive debug report."""
        report = f"# NLM Debug Report\n\n"
        report += f"**Debug Session:** {self.debug_log[0]['timestamp'] if self.debug_log else 'N/A'}\n"
        report += f"**Total Steps:** {self.step_count}\n"
        report += f"**Total Debug Entries:** {len(self.debug_log)}\n\n"
        
        # Performance summary
        if self.step_times:
            report += "## Performance Summary\n\n"
            report += f"- **Average Step Time:** {np.mean(self.step_times)*1000:.2f} ms\n"
            report += f"- **Min Step Time:** {np.min(self.step_times)*1000:.2f} ms\n"
            report += f"- **Max Step Time:** {np.max(self.step_times)*1000:.2f} ms\n"
            report += f"- **Steps per Second (avg):** {1.0/np.mean(self.step_times):.2f}\n\n"
        
        # Spike activity summary
        if self.spike_counts:
            report += "## Spike Activity Summary\n\n"
            report += f"- **Final Spike Count:** {self.spike_counts[-1]}\n"
            report += f"- **Average Spikes per Step:** {np.mean(self.spike_counts):.2f}\n"
            report += f"- **Max Spikes at Single Step:** {np.max(self.spike_counts):.2f}\n"
            report += f"- **Min Spikes at Single Step:** {np.min(self.spike_counts):.2f}\n\n"
        
        # Firing rate summary
        if self.firing_rates:
            report += "## Firing Rate Summary\n\n"
            report += f"- **Average Firing Rate:** {np.mean(self.firing_rates):.2f} neurons\n"
            report += f"- **Max Firing Rate:** {np.max(self.firing_rates):.2f} neurons\n"
            report += f"- **Min Firing Rate:** {np.min(self.firing_rates):.2f} neurons\n\n"
        
        # Error and warning summary
        errors = [entry for entry in self.debug_log if entry.get('event') == 'error']
        warnings = [entry for entry in self.debug_log if entry.get('event') == 'warning']
        
        report += "## Issues Summary\n\n"
        report += f"- **Errors:** {len(errors)}\n"
        report += f"- **Warnings:** {len(warnings)}\n\n"
        
        if errors:
            report += "### Error Details\n\n"
            for i, error in enumerate(errors[:5]):  # Show first 5 errors
                report += f"{i+1}. **Step {error.get('step', 'N/A')}:** {error.get('error', 'Unknown error')}\n"
        
        if warnings:
            report += "\n### Warning Details\n\n"
            for i, warning in enumerate(warnings[:5]):  # Show first 5 warnings
                report += f"{i+1}. **{warning.get('timestamp', 'N/A')}:** {warning.get('message', 'No message')}\n"
        
        # Save report
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
            print(f"Debug report saved to {output_file}")
        
        return report
    
    def plot_activity(self, save_file=None):
        """Plot activity graphs for analysis."""
        if not self.spike_counts or not self.firing_rates:
            print("No data to plot")
            return
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
        
        # Plot spike count over time
        steps = list(range(len(self.spike_counts)))
        ax1.plot(steps, self.spike_counts, 'b-', alpha=0.7, linewidth=1)
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Total Spike Count')
        ax1.set_title('Spike Count Over Time')
        ax1.grid(True, alpha=0.3)
        
        # Plot firing rate over time
        ax2.plot(steps, self.firing_rates, 'r-', alpha=0.7, linewidth=1)
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Firing Neurons')
        ax2.set_title('Firing Rate Over Time')
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_file:
            plt.savefig(save_file, dpi=150, bbox_inches='tight')
            print(f"Activity plot saved to {save_file}")
        
        return fig
    
    def get_detailed_step_info(self, step_number):
        """Get detailed information for a specific step."""
        for entry in self.debug_log:
            if isinstance(entry, dict) and entry.get('step') == step_number:
                return entry
        return None

# Example usage
print("=== Advanced NLM Debugging Example ===")

# Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()

# Create debugger
debugger = NLMDebugger(brain)
debugger.start_debug_session()

# Run simulation with debugging
for step in range(200):
    brain.step(step)
    debugger.log_step(step, step * 0.001)  # Time = step * timestep
    
    # Add some artificial interesting events for demonstration
    if step % 50 == 0:
        print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons")

# Generate debug report
debug_report = debugger.generate_debug_report('simulation_debug_report.md')
print("\nDebug Report Summary:")
print(debug_report[:500] + "..." if len(debug_report) > 500 else debug_report)

# Plot activity
activity_fig = debugger.plot_activity('simulation_activity.png')
plt.show()  # Uncomment to display plot
```

---

## Advanced Configuration Examples

### 1. Research Configuration

```python
# Research configuration for scientific experiments
research_config = {
    'neuron_count': 2000,
    'region_count': 4,
    'connection_probability': 0.25,
    'plasticity_learning_rate': 0.03,
    'dopamine_baseline': 0.5,
    'simulation_timestep': 0.0005,
    'enable_reward_modulation': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'enable_curiosity': True,
    'world_width': 20.0,
    'world_height': 20.0,
    'vision_width': 12,
    'vision_height': 12,
    'max_steps': 5000
}
```

### 2. Real-time Application Configuration

```python
# Configuration for real-time applications (low latency)
real_time_config = {
    'neuron_count': 100,  # Smaller for speed
    'region_count': 1,
    'connection_probability': 0.1,
    'plasticity_learning_rate': 0.005,
    'dopamine_baseline': 0.1,
    'simulation_timestep': 0.01,  # Larger timestep for speed
    'enable_reward_modulation': False,  # Disable for speed
    'enable_structural_plasticity': False,  # Disable for speed
    'enable_development': False,  # Disable for speed
    'enable_curiosity': False,  # Disable for speed
    'world_width': 10.0,
    'world_height': 10.0,
    'vision_width': 8,
    'vision_height': 8,
    'max_steps': 1000,
    'log_interval': 10  # Less frequent logging
}
```

### 3. High-fidelity Simulation Configuration

```python
# High-fidelity configuration for detailed analysis
high_fidelity_config = {
    'neuron_count': 5000,
    'region_count': 5,
    'connection_probability': 0.3,
    'plasticity_learning_rate': 0.02,
    'dopamine_baseline': 0.5,
    'simulation_timestep': 0.0001,  # Very small for accuracy
    'enable_reward_modulation': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'enable_curiosity': True,
    'world_width': 50.0,
    'world_height': 50.0,
    'vision_width': 16,
    'vision_height': 16,
    'max_steps': 10000,
    'enable_profiling': True,
    'log_interval': 100,
    'enable_advanced_monitoring': True
}
```

---

## Advanced Command Line Interface

The enhanced CLI provides many advanced options:

```bash
# Run advanced simulation with all features enabled
python nlm_advanced_cli.py --simulation --max-steps 5000 --neuron-count 1000 --enable-all-features

# Run batch simulation for parameter sweep
python nlm_advanced_cli.py --batch --iterations 20 --neuron-count 500 --noise-level 0.1

# Run analysis on batch results
python nlm_advanced_cli.py --batch --iterations 10 --neuron-count 1000 --save my_experiment
python nlm_advanced_cli.py --analyze my_experiment

# Compare multiple experiments
python nlm_advanced_cli.py --compare exp1 exp2 exp3

# Load and run specific experiment
python nlm_advanced_cli.py --load my_experiment

# Run with custom configuration
python nlm_advanced_cli.py --simulation --max-steps 2000 --world-width 25 --world-height 25
```

---

## Getting Started with Advanced NLM

1. **Start with the Basic Guide**: Read `easy_usage.md` to understand the basics
2. **Explore the Extensibility Framework**: Study `src/nlm_extensibility.hpp` for custom implementations
3. **Try the Advanced CLI**: Use `nlm_advanced_cli.py` for enhanced functionality
4. **Experiment with Configurations**: Use the `AdvancedConfigManager` to create custom setups
5. **Monitor Performance**: Use `NLMPerformanceProfiler` for detailed analysis
6. **Integrate with ML**: Use `NLMMLPipeline` for machine learning integration
7. **Save and Compare Experiments**: Use the `ExperimentTracker` for systematic experimentation

---

## Advanced Resources

- **Source Code**: `src/` - Complete source code for all components
- **Documentation**: `docs/` - Scientific and architectural documentation
- **Examples**: `examples/` - Additional usage examples
- **Tests**: `tests/` - Comprehensive test suite
- **Configuration**: `configs/` - Configuration files and presets

---

**Happy experimenting with advanced NLM features!**

This guide provides the foundation for advanced NLM usage. As the project evolves, more features and capabilities will be added to support cutting-edge research in computational neuroscience and artificial intelligence.