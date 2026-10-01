# NLM Visualization and Analysis Tools

## Overview

This module provides enhanced visualization and analysis capabilities for NLM simulations, making it easier to understand neural dynamics, agent behavior, and learning progress.

## Installation

```bash
pip install matplotlib numpy scipy pandas seaborn
```

## Core Visualization Functions

### 1. Neural Dynamics Visualization

```python
from nlm.visualization import plot_neural_dynamics, plot_neuron_activity
import pynlm

def visualize_brain_dynamics(brain, output_file=None):
    """Comprehensive visualization of brain dynamics."""
    
    # Create figure with multiple subplots
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle('NLM Brain Dynamics Visualization', fontsize=16)
    
    # 1. Neuron count over time
    neuron_counts = []
    for step in range(100):
        brain.step(step)
        neuron_counts.append(brain.getTotalNeuronCount())
    
    axes[0, 0].plot(neuron_counts)
    axes[0, 0].set_title('Total Neuron Count')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Neuron Count')
    
    # 2. Firing rate distribution
    firing_rates = []
    for step in range(100):
        firing_rates.append(brain.getAverageFiringRate())
    
    axes[0, 1].hist(firing_rates, bins=20, alpha=0.7)
    axes[0, 1].set_title('Firing Rate Distribution')
    axes[0, 1].set_xlabel('Firing Rate')
    axes[0, 1].set_ylabel('Frequency')
    
    # 3. Spike activity over time
    spike_counts = []
    for step in range(100):
        spike_counts.append(brain.getTotalSpikeCount())
    
    axes[0, 2].plot(spike_counts, 'r-')
    axes[0, 2].set_title('Total Spike Count')
    axes[0, 2].set_xlabel('Step')
    axes[0, 2].set_ylabel('Spike Count')
    
    # 4. E/I ratio balance
    ei_ratios = []
    for step in range(100):
        ei_ratios.append(brain.getExcitationInhibitionRatio())
    
    axes[1, 0].plot(ei_ratios, 'g-')
    axes[1, 0].set_title('Excitation/Inhibition Ratio')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('E/I Ratio')
    axes[1, 0].axhline(y=1.0, color='r', linestyle='--', alpha=0.5)
    
    # 5. Active neuron count over time
    active_counts = []
    for step in range(100):
        active_counts.append(brain.getActiveNeuronCount())
    
    axes[1, 1].plot(active_counts, 'b-')
    axes[1, 1].set_title('Active Neuron Count')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Active Neurons')
    
    # 6. Developmental stage over time
    stages = []
    for step in range(100):
        stages.append(int(brain.getDevelopmentalStage()))
    
    axes[1, 2].plot(stages, 'm-')
    axes[1, 2].set_title('Developmental Stage')
    axes[1, 2].set_xlabel('Step')
    axes[1, 2].set_ylabel('Stage')
    
    plt.tight_layout()
    
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
    
    plt.show()
    return fig
```

### 2. Agent Performance Visualization

```python
from nlm.visualization import plot_agent_performance, plot_learning_curve
import pynlm
import numpy as np

def visualize_agent_learning(agent, world, num_steps=1000):
    """Visualize agent learning progress over time."""
    
    # Setup agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Record metrics
    rewards = []
    curiosity_levels = []
    novelty_levels = []
    firing_rates = []
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        internal = percept.getInternal()
        rewards.append(internal[0] if internal else 0.0)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        firing_rates.append(brain.getAverageFiringRate())
        
        if world.getAgentBody().health <= 0:
            break
    
    # Create comprehensive learning visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('Agent Learning Progress', fontsize=16)
    
    # 1. Learning curve (rewards)
    ax1.plot(rewards, alpha=0.7)
    ax1.set_title('Episode Rewards')
    ax1.set_xlabel('Step')
    ax1.set_ylabel('Reward')
    ax1.grid(True, alpha=0.3)
    
    # Add moving average
    window_size = 50
    if len(rewards) >= window_size:
        moving_avg = np.convolve(rewards, np.ones(window_size)/window_size, mode='valid')
        ax1.plot(moving_avg, 'r-', linewidth=2, label=f'{window_size}-step MA')
        ax1.legend()
    
    # 2. Curiosity and novelty
    ax2.plot(curiosity_levels, label='Curiosity', alpha=0.8)
    ax2.plot(novelty_levels, label='Novelty', alpha=0.8)
    ax2.set_title('Exploration Drives')
    ax2.set_xlabel('Step')
    ax2.set_ylabel('Level')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 3. Firing rate dynamics
    ax3.plot(firing_rates, 'g-', alpha=0.7)
    ax3.set_title('Brain Firing Rate')
    ax3.set_xlabel('Step')
    ax3.set_ylabel('Firing Rate (Hz)')
    ax3.grid(True, alpha=0.3)
    
    # 4. Correlation between curiosity and learning
    if len(rewards) > 10:
        ax4.scatter(rewards[-100:], curiosity_levels[-100:], 
                   alpha=0.5, s=10, c='blue')
        correlation = np.corrcoef(rewards[-100:], curiosity_levels[-100:])[0,1]
        ax4.set_title(f'Curiosity vs Reward Correlation: {correlation:.3f}')
        ax4.set_xlabel('Recent Reward')
        ax4.set_ylabel('Curiosity Level')
    else:
        ax4.text(0.5, 0.5, 'Not enough data', 
                horizontalalignment='center', verticalalignment='center',
                transform=ax4.transAxes)
    
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    return fig, {
        'rewards': rewards,
        'curiosity': curiosity_levels,
        'novelty': novelty_levels,
        'firing_rates': firing_rates
    }
```

### 3. World Dynamics Visualization

```python
from nlm.visualization import plot_world_dynamics
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def visualize_world_dynamics(world, duration=100, timestep=0.1):
    """Visualize the world dynamics over time."""
    
    # Setup world if not already configured
    if world.getWidth() == 0 or world.getHeight() == 0:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Record world state over time
    times = []
    agent_positions = []
    agent_energies = []
    agent_healths = []
    
    for step in range(duration):
        world.update(timestep)
        
        body = world.getAgentBody()
        times.append(world.getSimulationTime())
        agent_positions.append((body.x, body.y))
        agent_energies.append(body.energy)
        agent_healths.append(body.health)
        
        if body.health <= 0:
            break
    
    # Create visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('World Dynamics Visualization', fontsize=16)
    
    # 1. Agent trajectory
    positions = np.array(agent_positions)
    ax1.plot(positions[:, 0], positions[:, 1], 'b-', alpha=0.7, linewidth=2)
    ax1.scatter(positions[0, 0], positions[0, 1], c='green', s=100, label='Start', marker='o')
    ax1.scatter(positions[-1, 0], positions[-1, 1], c='red', s=100, label='End', marker='s')
    ax1.set_title('Agent Trajectory')
    ax1.set_xlabel('X Position')
    ax1.set_ylabel('Y Position')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.set_aspect('equal', adjustable='box')
    
    # 2. Agent energy over time
    ax2.plot(times, agent_energies, 'r-', linewidth=2)
    ax2.set_title('Agent Energy')
    ax2.set_xlabel('Time')
    ax2.set_ylabel('Energy')
    ax2.grid(True, alpha=0.3)
    
    # 3. Agent health over time
    ax3.plot(times, agent_healths, 'g-', linewidth=2)
    ax3.set_title('Agent Health')
    ax3.set_xlabel('Time')
    ax3.set_ylabel('Health')
    ax3.grid(True, alpha=0.3)
    ax3.set_ylim(0, 100)
    
    # 4. Combined state
    ax4.plot(times, agent_energies, label='Energy', linewidth=2)
    ax4.plot(times, agent_healths, label='Health', linewidth=2)
    ax4.set_title('Combined State')
    ax4.set_xlabel('Time')
    ax4.set_ylabel('Value')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    return fig
```

### 4. Comparative Analysis

```python
from nlm.visualization import plot_comparison
import pynlm
import matplotlib.pyplot as plt

def compare_experiment_runs(configs, environments, durations):
    """Compare multiple experiment runs."""
    
    results = {}
    
    for i, (config, world, duration) in enumerate(zip(configs, environments, durations)):
        print(f"Running experiment {i+1}/{len(configs)}...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        run_data = {
            'rewards': [],
            'curiosity': [],
            'final_stage': 0,
            'episodes_survived': 0
        }
        
        for step in range(duration):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record data
            internal = percept.getInternal()
            run_data['rewards'].append(internal[0] if internal else 0.0)
            run_data['curiosity'].append(agent.getCuriosityLevel())
            run_data['final_stage'] = brain.getDevelopmentalStage()
            
            if world.getAgentBody().health <= 0:
                run_data['episodes_survived'] = step
                break
        
        results[f'Config {i+1}'] = run_data
    
    # Create comparison visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('Experiment Comparison', fontsize=16)
    
    # Plot reward curves
    for name, data in results.items():
        axes[0, 0].plot(data['rewards'], label=name, alpha=0.7)
    
    axes[0, 0].set_title('Reward Curves')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Reward')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot curiosity levels
    for name, data in results.items():
        axes[0, 1].plot(data['curiosity'], label=name, alpha=0.7)
    
    axes[0, 1].set_title('Curiosity Levels')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Curiosity')
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3)
    
    # Final developmental stages
    stages = [data['final_stage'] for data in results.values()]
    axes[1, 0].bar(range(len(stages)), stages, alpha=0.7)
    axes[1, 0].set_title('Final Developmental Stages')
    axes[1, 0].set_xlabel('Experiment')
    axes[1, 0].set_ylabel('Stage')
    axes[1, 0].set_xticks(range(len(stages)))
    axes[1, 0].set_xticklabels(list(results.keys()))
    axes[1, 0].grid(True, alpha=0.3, axis='y')
    
    # Episodes survived
    survived = [data['episodes_survived'] for data in results.values()]
    axes[1, 1].bar(range(len(survived)), survived, alpha=0.7, color='green')
    axes[1, 1].set_title('Episodes Survived')
    axes[1, 1].set_xlabel('Experiment')
    axes[1, 1].set_ylabel('Episodes')
    axes[1, 1].set_xticks(range(len(survived)))
    axes[1, 1].set_xticklabels(list(results.keys()))
    axes[1, 1].grid(True, alpha=0.3, axis='y')
    
    plt.tight_layout()
    
    return fig, results
```

## Analysis Functions

### 1. Learning Performance Metrics

```python
from nlm.analysis import calculate_learning_metrics, analyze_brain_activity
import numpy as np
import pandas as pd

def analyze_learning_performance(rewards, curiosity_levels, firing_rates):
    """Calculate comprehensive learning metrics."""
    
    metrics = {}
    
    # Basic statistics
    metrics['total_episodes'] = len(rewards)
    metrics['mean_reward'] = np.mean(rewards)
    metrics['std_reward'] = np.std(rewards)
    metrics['max_reward'] = np.max(rewards)
    metrics['min_reward'] = np.min(rewards)
    
    # Learning curve analysis
    if len(rewards) > 50:
        # Calculate learning rate (slope of first 25% vs last 25%)
        first_quartile = rewards[:len(rewards)//4]
        last_quartile = rewards[-len(rewards)//4:]
        
        metrics['initial_performance'] = np.mean(first_quartile)
        metrics['final_performance'] = np.mean(last_quartile)
        metrics['learning_slope'] = (metrics['final_performance'] - 
                                    metrics['initial_performance']) / len(first_quartile)
    
    # Exploration analysis
    metrics['mean_curiosity'] = np.mean(curiosity_levels)
    metrics['max_curiosity'] = np.max(curiosity_levels)
    metrics['curiosity_variability'] = np.std(curiosity_levels)
    
    # Neural activity analysis
    metrics['mean_firing_rate'] = np.mean(firing_rates)
    metrics['firing_rate_variance'] = np.var(firing_rates)
    metrics['stability_index'] = 1.0 / (1.0 + metrics['firing_rate_variance'])
    
    # Skill acquisition (reward improvement)
    if len(rewards) > 20:
        first_10 = np.mean(rewards[:10])
        last_10 = np.mean(rewards[-10:])
        metrics['skill_improvement'] = last_10 - first_10
    
    return metrics

class ExperimentAnalyzer:
    """Analyze experimental data and generate insights."""
    
    def __init__(self):
        self.experiments = {}
    
    def add_experiment(self, name, data):
        """Add experimental data."""
        self.experiments[name] = data
    
    def compare_experiments(self, metric_name):
        """Compare a specific metric across experiments."""
        comparison = {}
        
        for name, data in self.experiments.items():
            if metric_name in data:
                comparison[name] = data[metric_name]
        
        return comparison
    
    def find_best_experiment(self, metric_name, higher_is_better=True):
        """Find the experiment with the best performance on a metric."""
        comparison = self.compare_experiments(metric_name)
        
        if higher_is_better:
            best_name = max(comparison, key=comparison.get)
            best_value = comparison[best_name]
        else:
            best_name = min(comparison, key=comparison.get)
            best_value = comparison[best_name]
        
        return best_name, best_value
    
    def generate_report(self, metrics_to_include=None):
        """Generate a text report of all experiments."""
        if metrics_to_include is None:
            metrics_to_include = ['mean_reward', 'skill_improvement', 'mean_curiosity']
        
        report = "NLM Experiment Analysis Report\n"
        report += "=" * 50 + "\n\n"
        
        for name, data in self.experiments.items():
            report += f"Experiment: {name}\n"
            report += "-" * 30 + "\n"
            
            for metric in metrics_to_include:
                if metric in data:
                    report += f"{metric}: {data[metric]:.4f}\n"
            
            report += "\n"
        
        return report
```

### 2. Statistical Analysis

```python
from nlm.analysis import statistical_tests, detect_regime_changes
import numpy as np
from scipy import stats
import warnings

class PerformanceAnalyzer:
    """Analyze agent performance with statistical methods."""
    
    @staticmethod
    def calculate_steady_state(rewards, window_size=100):
        """Calculate steady-state performance from reward time series."""
        if len(rewards) < window_size * 2:
            return None, None, None
        
        # Split into first and second halves
        first_half = rewards[:len(rewards)//2]
        second_half = rewards[len(rewards)//2:]
        
        # Check for stationarity using augmented Dickey-Fuller test
        try:
            _, _, _, p_value = stats.adfuller(first_half)
            is_stationary = p_value < 0.05
        except:
            is_stationary = False
        
        # Calculate means and confidence intervals
        mean_first = np.mean(first_half)
        mean_second = np.mean(second_half)
        
        std_first = np.std(first_half)
        std_second = np.std(second_half)
        
        se_first = std_first / np.sqrt(len(first_half))
        se_second = std_second / np.sqrt(len(second_half))
        
        ci_first = 1.96 * se_first
        ci_second = 1.96 * se_second
        
        # Determine if performance has changed significantly
        mean_diff = mean_second - mean_first
        pooled_se = np.sqrt((std_first**2 + std_second**2) / 2)
        pooled_n = len(first_half) + len(second_half)
        t_critical = stats.t.ppf(0.975, pooled_n - 2)
        
        has_significant_change = abs(mean_diff / pooled_se) > t_critical
        
        return {
            'stationary': is_stationary,
            'mean_change': mean_diff,
            'has_significant_change': has_significant_change,
            'first_half_mean': mean_first,
            'second_half_mean': mean_second,
            'confidence_intervals': (ci_first, ci_second)
        }
    
    @staticmethod
    def detect_plateau(rewards, min_plateau_length=50, tolerance=0.01):
        """Detect if the agent has reached a performance plateau."""
        
        if len(rewards) < min_plateau_length:
            return False, 0, 0
        
        # Calculate moving average
        moving_avg = np.convolve(rewards, 
                                np.ones(min_plateau_length)/min_plateau_length, 
                                mode='valid')
        
        # Check for plateau
        plateau_found = False
        plateau_start = 0
        plateau_value = 0
        
        for i in range(len(moving_avg) - min_plateau_length):
            current_window = moving_avg[i:i+min_plateau_length]
            window_mean = np.mean(current_window)
            window_std = np.std(current_window)
            
            # Check if window is stable
            is_stable = window_std / (abs(window_mean) + 1e-6) <= tolerance
            
            if is_stable:
                plateau_found = True
                plateau_start = i
                plateau_value = window_mean
                break
        
        return plateau_found, plateau_start, plateau_value
    
    @staticmethod
    def analyze_exploration_exploitation(rewards, curiosity_levels):
        """Analyze the balance between exploration and exploitation."""
        
        # Calculate correlation between curiosity and reward change
        if len(rewards) < 10:
            return None
        
        reward_changes = np.diff(rewards)
        
        # Align curiosity levels (remove last element)
        if len(curiosity_levels) > len(reward_changes):
            curiosity_levels = curiosity_levels[:-1]
        
        # Calculate correlation
        if len(reward_changes) == 0 or len(curiosity_levels) == 0:
            return None
        
        correlation = np.corrcoef(reward_changes, curiosity_levels)[0, 1]
        
        # Classify exploration-exploitation strategy
        if correlation > 0.3:
            strategy = "Exploitation-focused with occasional exploration"
        elif correlation > 0.1:
            strategy = "Balanced exploration-exploitation"
        elif correlation > -0.1:
            strategy = "Exploration-focused"
        else:
            strategy = "Chaotic/Undifferentiated"
        
        return {
            'curiosity_reward_correlation': correlation,
            'strategy': strategy,
            'reward_change_mean': np.mean(reward_changes),
            'curiosity_mean': np.mean(curiosity_levels)
        }
```

## Utility Functions

### 1. Data Processing

```python
from nlm.utils import smooth_data, interpolate_data, detect_anomalies
import numpy as np

def preprocess_simulation_data(rewards, curiosity_levels, firing_rates):
    """Preprocess simulation data for analysis."""
    
    processed_data = {}
    
    # Smooth noisy data
    window_size = 5
    processed_data['smoothed_rewards'] = smooth_data(rewards, window_size)
    processed_data['smoothed_curiosity'] = smooth_data(curiosity_levels, window_size)
    processed_data['smoothed_firing_rates'] = smooth_data(firing_rates, window_size)
    
    # Interpolate to uniform time steps
    processed_data['uniform_rewards'] = interpolate_data(
        np.arange(len(rewards)), rewards
    )
    
    # Detect anomalies (outliers)
    processed_data['reward_anomalies'] = detect_anomalies(
        rewards, threshold=3.0  # 3 sigma
    )
    
    processed_data['curiosity_anomalies'] = detect_anomalies(
        curiosity_levels, threshold=2.0
    )
    
    return processed_data
```

### 2. Configuration Helpers

```python
from nlm.utils import create_experiment_config, validate_config

def create_reproduction_config(base_config, **modifications):
    """Create a configuration for reproducible experiments."""
    
    config = pynlm.createDefaultConfig()
    
    # Apply base config settings
    for key, value in base_config.items():
        config.set(key, value)
    
    # Apply modifications
    for key, value in modifications.items():
        config.set(key, value)
    
    return config

def validate_experiment_config(config, required_keys=None):
    """Validate that all required configuration keys are present."""
    
    if required_keys is None:
        required_keys = [
            'brain.neuron_count',
            'brain.synapse_density',
            'plasticity.stdp.enable',
            'neuromod.dopamine.scale'
        ]
    
    missing_keys = []
    for key in required_keys:
        if not config.has(key):
            missing_keys.append(key)
    
    if missing_keys:
        raise ValueError(f"Missing required configuration keys: {missing_keys}")
    
    return True
```

## File Structure

```
# visualization module
nlm/visualization.py          # Core visualization functions
nlm/plots.py                  # Specialized plotting utilities
nlm/figures.py                # Figure generation helpers

# analysis module  
nlm/analysis.py              # Statistical analysis and metrics
nlm/metrics.py                # Performance metrics
nlm/statistics.py              # Statistical tests

# utils module
nlm/utils.py                  # General utility functions
nlm/helpers.py                # Helper functions
nlm/io.py                     # Input/output operations
```

## Usage Examples

### Complete Example

```python
import pynlm
from nlm.visualization import visualize_agent_learning
from nlm.analysis import calculate_learning_metrics

def run_and_analyze_experiment(config, world, num_steps=1000):
    """Complete example of running and analyzing an experiment."""
    
    # Run experiment
    fig, data = visualize_agent_learning(config, world, num_steps)
    
    # Analyze results
    metrics = calculate_learning_metrics(
        data['rewards'], 
        data['curiosity'], 
        data['firing_rates']
    )
    
    print("Experiment Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value:.4f}")
    
    return fig, data, metrics

# Usage
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

fig, data, metrics = run_and_analyze_experiment(config, world, 500)
plt.show()
```

### Batch Analysis

```python
from nlm.visualization import compare_experiment_runs
from nlm.analysis import ExperimentAnalyzer
import glob

# Load multiple experiment results
experiment_files = glob.glob('experiments/experiment_*.npy')

# Create analyzer
analyzer = ExperimentAnalyzer()

# Add experiments
for file_path in experiment_files:
    data = np.load(file_path, allow_pickle=True).item()
    name = file_path.split('/')[-1].replace('.npy', '')
    analyzer.add_experiment(name, data)

# Generate report
report = analyzer.generate_report()
print(report)

# Compare experiments
comparison = analyzer.compare_experiments('mean_reward')
print("Reward comparison:", comparison)

best_exp, best_score = analyzer.find_best_experiment('mean_reward')
print(f"Best experiment: {best_exp} (score: {best_score:.4f})")
```

## Dependencies

```bash
# Required dependencies
matplotlib>=3.5.0
numpy>=1.20.0
scipy>=1.7.0
pandas>=1.3.0
seaborn>=0.11.0

# Optional dependencies
plotly>=5.0.0        # Interactive plots
bokeh>=2.4.0         # Web visualizations
```

## Troubleshooting

### Missing Libraries

```bash
# Install required libraries
pip install matplotlib numpy scipy pandas seaborn
```

### Memory Issues

```python
# For large datasets, use memory-efficient processing
from nlm.utils import chunked_processing

# Process data in chunks
for chunk in chunked_processing(large_dataset, chunk_size=1000):
    # Process each chunk
    result = process_chunk(chunk)
```

### Visualization Issues

```python
# For systems without display
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

# Save plots to files
fig.savefig('experiment_results.png', dpi=300, bbox_inches='tight')
```

## License

MIT License

## Authors

NLM Research Team
