# NLM Advanced Features for Power Users

## Overview

This document describes advanced features available in NLM for power users who want to go beyond the basic API and customize their neural network simulations.

## 1. Direct Access to Brain Components

Power users can access the internal brain components for advanced customization:

### Direct System Access

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access internal systems for advanced customization
s spike_system = brain.getSpikeSystem()
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
```

### Custom Learning Rules

```python
# Custom STDP implementation
class CustomSTDP:
    def __init__(self):
        self LTP_weight = 0.01
        self LTD_weight = 0.012
        self.tau = 20.0
    
    def update(self, synapse, pre_spike_time, post_spike_time, dt):
        # Custom STDP learning rule
        delta_t = post_spike_time - pre_spike_time
        
        if delta_t > 0:
            # Potentiation
            weight_change = self.LTP_weight * math.exp(-delta_t / self.tau)
        else:
            # Depression
            weight_change = -self.LTD_weight * math.exp(delta_t / self.tau)
        
        new_weight = synapse.getWeight() + weight_change
        synapse.setWeight(new_weight)
        return new_weight

# Replace brain's STDP system
brain.getSTDP().custom_stdp = CustomSTDP()
```

### Direct Neuron Manipulation

```python
# Direct access to neurons for advanced control
regions = brain.getRegions()
for region in regions:
    populations = region.getPopulations()
    for pop in populations:
        neurons = pop.getNeurons()
        for neuron in neurons:
            # Set custom neuron parameters
            neuron.setMembranePotential(-60.0)  # Custom initial potential
            neuron.setThreshold(-45.0)          # Custom threshold
```

## 2. Advanced Simulation Control

### Fine-Grained Simulation Steps

```python
import time

# Advanced simulation with custom control
start_time = time.time()
step_count = 0
last_log_time = start_time

for step in range(10000):
    # Custom step logic
    brain.step(step)
    
    # Custom time-based processing
    current_time = step * config.get("simulation.timestep", 0.001)
    
    # Performance monitoring
    if time.time() - last_log_time > 1.0:  # Log every second
        fps = step_count / (time.time() - start_time)
        print(f"Step {step}: {fps:.1f} FPS, {brain.getFiringNeuronCount()} firing")
        last_log_time = time.time()
    
    step_count += 1
    
    # Custom step conditions
    if step % 100 == 0:
        # Custom processing every 100 steps
        brain.getWorkingMemory().update(config.get("simulation.timestep", 0.001))
    
    if step % 1000 == 0:
        # Custom processing every 1000 steps
        brain.getEpisodicMemory().storeCurrentState(step)
```

### Variable Timestep Simulation

```python
# Variable timestep simulation for adaptive control
class AdaptiveTimestepSimulator:
    def __init__(self, brain, base_timestep=0.001):
        self.brain = brain
        self.base_timestep = base_timestep
        self.current_timestep = base_timestep
        self.step_count = 0
    
    def run_with_adaptive_timestep(self, max_steps=1000):
        for step in range(max_steps):
            # Calculate adaptive timestep based on neural activity
            firing_rate = self.brain.getAverageFiringRate()
            
            if firing_rate > 100.0:  # High activity - use smaller timestep
                self.current_timestep = self.base_timestep * 0.1
            elif firing_rate > 10.0:  # Medium activity
                self.current_timestep = self.base_timestep
            else:  # Low activity - can use larger timestep
                self.current_timestep = self.base_timestep * 2.0
            
            # Ensure timestep doesn't exceed maximum
            self.current_timestep = min(self.current_timestep, 0.01)
            
            # Run simulation step
            self.brain.step(step, step * self.current_timestep)
            self.step_count += 1

# Usage
simulator = AdaptiveTimestepSimulator(brain)
simulator.run_with_adaptive_timestep(5000)
```

### Parallel Simulation Execution

```python
import multiprocessing as mp
from multiprocessing import Pool
import itertools

# Parallel simulation execution
class ParallelSimulator:
    def __init__(self, config_template, num_processes=None):
        self.config_template = config_template
        self.num_processes = num_processes or mp.cpu_count()
        self.results = []
    
    def run_single_simulation(self, params):
        """Run a single simulation with given parameters."""
        config = pynlm.createDefaultConfig()
        
        # Apply simulation parameters
        config.set("brain.neuron_count", params.get('neuron_count', 1000))
        config.set("brain.connection_probability", params.get('conn_prob', 0.1))
        config.set("plasticity.stdp.learning_rate", params.get('stdp_rate', 0.001))
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(params.get('steps', 1000)):
            brain.step(step)
        
        # Collect results
        return {
            'params': params,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'region_count': brain.getRegionCount()
        }
    
    def run_parallel_experiments(self, param_grid):
        """Run simulations with different parameter combinations."""
        with Pool(self.num_processes) as pool:
            results = pool.map(self.run_single_simulation, param_grid)
        
        self.results = results
        return results

# Example parameter grid
param_grid = [
    {'neuron_count': 500, 'conn_prob': 0.05, 'stdp_rate': 0.001, 'steps': 1000},
    {'neuron_count': 1000, 'conn_prob': 0.1, 'stdp_rate': 0.001, 'steps': 1000},
    {'neuron_count': 2000, 'conn_prob': 0.2, 'stdp_rate': 0.002, 'steps': 1000},
    {'neuron_count': 500, 'conn_prob': 0.15, 'stdp_rate': 0.01, 'steps': 1000},
]

# Run parallel simulations
parallel_sim = ParallelSimulator(pynlm.createDefaultConfig())
results = parallel_sim.run_parallel_experiments(param_grid)

# Analyze results
for result in results:
    print(f"Params: {result['params']}")
    print(f"  Total spikes: {result['total_spikes']}")
    print(f"  Firing rate: {result['firing_rate']:.3f}")
    print(f"  Regions: {result['region_count']}")
```

## 3. Advanced Memory and Learning Features

### Custom Memory Systems

```python
# Custom memory system with advanced features
class AdvancedWorkingMemory:
    def __init__(self, brain, capacity=100):
        self.brain = brain
        self.capacity = capacity
        self.memory_traces = {}
        self.priority_scores = {}
    
    def store_with_priority(self, neuron_id, activation, priority):
        """Store neuron activation with priority scoring."""
        if len(self.memory_traces) >= self.capacity:
            # Remove lowest priority trace
            min_priority = min(self.priority_scores.values())
            for trace_id, trace_priority in self.priority_scores.items():
                if trace_priority == min_priority:
                    del self.memory_traces[trace_id]
                    del self.priority_scores[trace_id]
                    break
        
        self.memory_traces[neuron_id] = activation
        self.priority_scores[neuron_id] = priority
    
    def get_top_memories(self, top_n=10):
        """Get top N memories by priority."""
        sorted_items = sorted(self.priority_scores.items(), 
                            key=lambda x: x[1], reverse=True)
        return [(neuron_id, self.memory_traces[neuron_id]) 
                for neuron_id, priority in sorted_items[:top_n]]

# Replace brain's working memory with custom version
advanced_memory = AdvancedWorkingMemory(brain, capacity=200)
brain.getWorkingMemory().custom_memory = advanced_memory
```

### Episodic Memory Analysis

```python
# Advanced episodic memory analysis
class EpisodicMemoryAnalyzer:
    def __init__(self, brain):
        self.brain = brain
        self.episodic_memory = brain.getEpisodicMemory()
    
    def analyze_memory_patterns(self):
        """Analyze patterns in stored episodic memories."""
        episodes = self.episodic_memory.getEpisodes()
        
        if not episodes:
            return None
        
        # Calculate statistics
        pattern_stats = {
            'episode_count': len(episodes),
            'average_duration': sum(e.duration for e in episodes) / len(episodes),
            'reward_distribution': [e.reward for e in episodes],
            'pattern_complexity': self.calculate_pattern_complexity(episodes)
        }
        
        return pattern_stats
    
    def calculate_pattern_complexity(self, episodes):
        """Calculate complexity of memory patterns."""
        if not episodes:
            return 0.0
        
        # Calculate entropy of reward distribution
        rewards = [e.reward for e in episodes]
        from collections import Counter
        reward_counts = Counter(rewards)
        total = len(rewards)
        
        entropy = 0.0
        for count in reward_counts.values():
            prob = count / total
            entropy -= prob * math.log2(prob) if prob > 0 else 0
        
        return entropy
    
    def detect_memory_recurrences(self, min_recurrence=3):
        """Detect recurring memory patterns."""
        episodes = self.episodic_memory.getEpisodes()
        pattern_counts = {}
        
        for episode in episodes:
            # Create pattern signature from neuron activations
            pattern = tuple(sorted(episode.activeNeurons))
            pattern_counts[pattern] = pattern_counts.get(pattern, 0) + 1
        
        # Find recurring patterns
        recurring_patterns = {
            pattern: count for pattern, count in pattern_counts.items()
            if count >= min_recurrence
        }
        
        return recurring_patterns

# Use the analyzer
analyzer = EpisodicMemoryAnalyzer(brain)
patterns = analyzer.analyze_memory_patterns()
recurrences = analyzer.detect_memory_recurrences()
```

## 4. Advanced Neuromodulation

### Custom Neuromodulation Systems

```python
# Custom neuromodulation with advanced features
class AdvancedNeuromodulation:
    def __init__(self, brain):
        self.brain = brain
        self.dopamine = brain.getDopamine()
        self.curiosity = brain.getCuriosity()
        self.novelty = brain.getNovelty()
        
        # Advanced neuromodulation parameters
        self.dopamine_history = []
        self.curiosity_history = []
        self.novelty_history = []
        
        # Learning rate adapters
        self.dopamine_learning_rate = 0.01
        self.curiosity_learning_rate = 0.005
        self.novelty_learning_rate = 0.002
    
    def advanced_reward_modulation(self, reward, prediction_error):
        """Advanced reward modulation with temporal integration."""
        # Temporal integration of rewards
        integrated_reward = self.integrate_reward(reward)
        
        # Normalized prediction error
        normalized_error = self.normalize_prediction_error(prediction_error)
        
        # Combine reward and prediction error
        combined_modulation = (integrated_reward * 0.7 + 
                              normalized_error * 0.3)
        
        # Apply to dopamine system
        self.dopamine.setLevel(combined_modulation)
        self.dopamine_history.append(combined_modulation)
        
        return combined_modulation
    
    def integrate_reward(self, reward, window_size=100):
        """Integrate reward over time window."""
        self.dopamine_history.append(reward)
        
        if len(self.dopamine_history) > window_size:
            self.dopamine_history.pop(0)
        
        return sum(self.dopamine_history) / len(self.dopamine_history)
    
    def normalize_prediction_error(self, error):
        """Normalize prediction error to [0, 1] range."""
        # Adaptive normalization based on history
        if len(self.dopamine_history) > 10:
            historical_std = statistics.stdev(self.dopamine_history[-10:])
            if historical_std > 0:
                return max(0, min(1, error / historical_std))
        
        return max(0, min(1, error))

# Replace brain's neuromodulation with advanced version
advanced_mod = AdvancedNeuromodulation(brain)
brain.getNeuromodulation().custom_modulation = advanced_mod
```

### Neuromodulation Visualization

```python
import matplotlib.pyplot as plt
import numpy as np

# Visualize neuromodulation dynamics
class NeuromodulationVisualizer:
    def __init__(self, brain, duration=1000):
        self.brain = brain
        self.duration = duration
        self.dopamine_data = []
        self.curiosity_data = []
        self.novelty_data = []
        self.time_steps = []
    
    def record_neuromodulation(self):
        """Record neuromodulation levels over time."""
        for step in range(self.duration):
            self.brain.step(step)
            
            # Record levels
            dopamine = brain.getDopamine().getLevel()
            curiosity = brain.getCuriosity().getLevel()
            novelty = brain.getNovelty().getLevel()
            
            self.dopamine_data.append(dopamine)
            self.curiosity_data.append(curiosity)
            self.novelty_data.append(novelty)
            self.time_steps.append(step)
    
    def plot_dynamics(self):
        """Plot neuromodulation dynamics."""
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        
        # Dopamine dynamics
        ax1.plot(self.time_steps, self.dopamine_data, 'r-', alpha=0.7)
        ax1.set_title('Dopamine Dynamics')
        ax1.set_xlabel('Time Step')
        ax1.set_ylabel('Level')
        ax1.grid(True, alpha=0.3)
        
        # Curiosity dynamics
        ax2.plot(self.time_steps, self.curiosity_data, 'g-', alpha=0.7)
        ax2.set_title('Curiosity Dynamics')
        ax2.set_xlabel('Time Step')
        ax2.set_ylabel('Level')
        ax2.grid(True, alpha=0.3)
        
        # Novelty dynamics
        ax3.plot(self.time_steps, self.novelty_data, 'b-', alpha=0.7)
        ax3.set_title('Novelty Dynamics')
        ax3.set_xlabel('Time Step')
        ax3.set_ylabel('Level')
        ax3.grid(True, alpha=0.3)
        
        # Combined dynamics
        ax4.plot(self.time_steps, 
                np.array(self.dopamine_data) + np.array(self.curiosity_data) + np.array(self.novelty_data),
                'purple-', alpha=0.7)
        ax4.set_title('Combined Neuromodulation')
        ax4.set_xlabel('Time Step')
        ax4.set_ylabel('Total Level')
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()

# Usage
visualizer = NeuromodulationVisualizer(brain)
visualizer.record_neuromodulation()
visualizer.plot_dynamics()
```

## 5. Development and Learning Features

### Custom Development Schedules

```python
# Custom developmental schedules
class CustomDevelopment:
    def __init__(self, brain):
        self.brain = brain
        self.development_system = brain.getDevelopmentSystem()
    
    def set_custom_schedule(self, stages):
        """Set custom developmental schedule.
        
        stages: List of (stage_name, duration, plasticity_factor)
        """
        for stage_name, duration, plasticity_factor in stages:
            # Set stage properties
            self.development_system.setStageProperties(
                stage_name, duration, plasticity_factor
            )
    
    def accelerate_development(self, acceleration_factor=1.5):
        """Accelerate development process."""
        # Modify stage durations
        current_stages = self.development_system.getStages()
        accelerated_stages = []
        
        for stage_name, duration, plasticity_factor in current_stages:
            new_duration = max(10, int(duration / acceleration_factor))
            accelerated_stages.append((stage_name, new_duration, plasticity_factor))
        
        self.set_custom_schedule(accelerated_stages)
    
    def introduce_critical_period(self, stage_name, window_start, window_duration):
        """Introduce artificial critical period."""
        self.development_system.createCriticalPeriod(
            stage_name, window_start, window_duration
        )

# Usage
custom_dev = CustomDevelopment(brain)

# Custom schedule with extended critical period
custom_schedule = [
    ('Initial', 50, 1.0),
    ('Critical', 100, 0.5),  # Extended critical period
    ('Maturation', 150, 0.3),
    ('Adult', 200, 0.2),
    ('Aging', 100, 0.1)
]

custom_dev.set_custom_schedule(custom_schedule)
```

### Learning Analytics

```python
# Advanced learning analytics
class LearningAnalytics:
    def __init__(self, brain):
        self.brain = brain
        self.step_count = 0
        self.learning_data = []
        self.performance_metrics = {}
    
    def record_learning_event(self, event_type, value, context=None):
        """Record learning events for analysis."""
        self.learning_data.append({
            'step': self.step_count,
            'type': event_type,
            'value': value,
            'context': context or {},
            'timestamp': time.time()
        })
    
    def analyze_learning_curves(self):
        """Analyze learning curves and extract insights."""
        if not self.learning_data:
            return None
        
        # Group data by event type
        event_types = set(item['type'] for item in self.learning_data)
        
        analysis = {}
        for event_type in event_types:
            events = [item for item in self.learning_data if item['type'] == event_type]
            
            # Calculate statistics
            values = [e['value'] for e in events]
            analysis[event_type] = {
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values),
                'trend': self.calculate_trend(events)
            }
        
        return analysis
    
    def calculate_trend(self, events):
        """Calculate trend of events over time."""
        if len(events) < 2:
            return 'stable'
        
        steps = [e['step'] for e in events]
        values = [e['value'] for e in events]
        
        # Calculate linear regression slope
        coeffs = np.polyfit(steps, values, 1)
        slope = coeffs[0]
        
        if abs(slope) < 0.01:
            return 'stable'
        elif slope > 0.01:
            return 'increasing'
        else:
            return 'decreasing'
    
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        report = {
            'simulation_summary': {
                'total_steps': self.step_count,
                'final_spike_count': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage()
            },
            'learning_analysis': self.analyze_learning_curves(),
            'memory_stats': {
                'working_memory_active': self.brain.getWorkingMemory().getActiveTraces(),
                'episodic_memory_episodes': self.brain.getEpisodicMemory().getEpisodeCount()
            }
        }
        
        return report

# Usage
analytics = LearningAnalytics(brain)

# Record learning events
for step in range(1000):
    brain.step(step)
    
    # Record learning events
    if step % 10 == 0:
        analytics.record_learning_event(
            'spike', brain.getFiringNeuronCount(),
            {'step': step, 'region_count': brain.getRegionCount()}
        )
    
    if step % 50 == 0:
        analytics.record_learning_event(
            'learning_rate', 0.001,  # Example value
            {'step': step, 'development_stage': brain.getDevelopmentalStage()}
        )

# Generate report
report = analytics.generate_performance_report()
print(f"Performance Report:")
print(f"Total steps: {report['simulation_summary']['total_steps']}")
print(f"Final spike count: {report['simulation_summary']['final_spike_count']}")
print(f"Average firing rate: {report['simulation_summary']['average_firing_rate']:.3f}")
```

## 6. Batch and Parallel Processing

### Batch Simulation Manager

```python
# Batch simulation manager for large-scale experiments
class BatchSimulationManager:
    def __init__(self, num_processes=None):
        self.num_processes = num_processes or mp.cpu_count()
        self.results = []
        self.progress_callback = None
    
    def set_progress_callback(self, callback):
        """Set callback for progress updates."""
        self.progress_callback = callback
    
    def run_batch_experiment(self, experiment_config, num_repetitions=5):
        """Run batch experiment with multiple parameter sets."""
        print(f"Running batch experiment with {num_repetitions} repetitions...")
        
        # Generate parameter combinations
        param_sets = self.generate_parameter_combinations(
            experiment_config['parameters'],
            num_repetitions
        )
        
        # Run simulations
        with Pool(self.num_processes) as pool:
            # Use apply_async for progress tracking
            async_results = []
            for i, params in enumerate(param_sets):
                result_async = pool.apply_async(
                    self.run_single_simulation,
                    (params, experiment_config)
                )
                async_results.append(result_async)
                
                # Report progress
                if self.progress_callback:
                    progress = (i + 1) / len(param_sets) * 100
                    self.progress_callback(progress, f"Completed {i + 1}/{len(param_sets)}")
            
            # Collect results
            for i, result_async in enumerate(async_results):
                result = result_async.get()
                self.results.append(result)
                print(f"Completed experiment {i + 1}/{len(param_sets)}")
        
        return self.results
    
    def generate_parameter_combinations(self, base_params, num_repetitions):
        """Generate parameter combinations for batch experiment."""
        import itertools
        
        # Extract parameter names and values
        param_names = list(base_params.keys())
        param_values = list(base_params.values())
        
        # Generate all combinations
        combinations = list(itertools.product(*param_values))
        
        # Repeat combinations
        full_combinations = []
        for combo in combinations:
            for _ in range(num_repetitions):
                full_combinations.append(dict(zip(param_names, combo)))
        
        return full_combinations
    
    def run_single_simulation(self, params, experiment_config):
        """Run a single simulation with given parameters."""
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        for key, value in params.items():
            if '.' in key:
                # Nested parameter
                parts = key.split('.')
                # For simplicity, just set at top level
                config.set(key, value)
            else:
                config.set(key, value)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(experiment_config['steps']):
            brain.step(step)
            
            # Record intermediate results if needed
            if experiment_config.get('record_intermediate', False):
                self.record_simulation_step(brain, step)
        
        # Return results
        result = {
            'parameters': params,
            'final_spike_count': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'region_count': brain.getRegionCount(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        return result
    
    def analyze_batch_results(self):
        """Analyze batch experiment results."""
        if not self.results:
            return None
        
        # Calculate statistics for each parameter
        analysis = {}
        param_names = list(self.results[0]['parameters'].keys())
        
        for param in param_names:
            values = [r['parameters'][param] for r in self.results]
            spike_counts = [r['final_spike_count'] for r in self.results]
            
            analysis[param] = {
                'mean_value': np.mean(values),
                'std_value': np.std(values),
                'min_value': np.min(values),
                'max_value': np.max(values),
                'mean_spikes': np.mean(spike_counts),
                'std_spikes': np.std(spike_counts)
            }
        
        return analysis
    
    def find_optimal_parameters(self, metric='final_spike_count'):
        """Find optimal parameters based on metric."""
        if not self.results:
            return None
        
        # Find best result
        best_result = max(self.results, key=lambda x: x[metric])
        
        return {
            'optimal_parameters': best_result['parameters'],
            'optimal_value': best_result[metric],
            'all_results': self.results
        }

# Usage example
batch_manager = BatchSimulationManager(num_processes=4)

# Define experiment
experiment_config = {
    'steps': 1000,
    'parameters': {
        'brain.neuron_count': [500, 1000, 2000],
        'brain.connection_probability': [0.05, 0.1, 0.2],
        'plasticity.stdp.learning_rate': [0.001, 0.01, 0.1]
    }
}

# Run batch experiment
results = batch_manager.run_batch_experiment(experiment_config, num_repetitions=3)

# Analyze results
analysis = batch_manager.analyze_batch_results()
print("Batch Experiment Analysis:")
for param, stats in analysis.items():
    print(f"\n{param}:")
    print(f"  Mean value: {stats['mean_value']}")
    print(f"  Mean spikes: {stats['mean_spikes']:.1f} ± {stats['std_spikes']:.1f}")

# Find optimal parameters
optimal = batch_manager.find_optimal_parameters()
print(f"\nOptimal Parameters:")
for param, value in optimal['optimal_parameters'].items():
    print(f"  {param}: {value}")
print(f"Optimal spike count: {optimal['optimal_value']}")
```

## 7. Advanced Debugging and Monitoring

### Simulation Debug Tools

```python
# Advanced simulation debugging tools
class SimulationDebugger:
    def __init__(self, brain):
        self.brain = brain
        self.debug_log = []
        self.error_count = 0
        self.warning_count = 0
    
    def log_event(self, event_type, message, severity='info'):
        """Log debug event."""
        event = {
            'timestamp': time.time(),
            'step': self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else None,
            'type': event_type,
            'message': message,
            'severity': severity
        }
        self.debug_log.append(event)
        
        if severity == 'error':
            self.error_count += 1
        elif severity == 'warning':
            self.warning_count += 1
    
    def validate_simulation_state(self):
        """Validate simulation state for potential issues."""
        issues = []
        
        # Check for NaN or infinite values
        try:
            firing_rate = self.brain.getAverageFiringRate()
            if not np.isfinite(firing_rate):
                issues.append("Invalid firing rate: NaN or infinite")
        except:
            issues.append("Could not calculate firing rate")
        
        # Check neuron counts
        total_neurons = self.brain.getTotalNeuronCount()
        if total_neurons == 0:
            issues.append("No neurons in simulation")
        elif total_neurons > 100000:
            issues.append("Very large brain - may cause performance issues")
        
        # Check for memory issues
        try:
            active_neurons = self.brain.getActiveNeuronCount()
            if active_neurons > total_neurons * 0.9:
                issues.append("Almost all neurons active - possible saturation")
        except:
            pass
        
        # Check development stage
        try:
            stage = self.brain.getDevelopmentalStage()
            if stage.name == 'Aging' and total_neurons > 10000:
                issues.append("Large brain in aging stage - may be unstable")
        except:
            pass
        
        return issues
    
    def generate_debug_report(self):
        """Generate comprehensive debug report."""
        report = {
            'summary': {
                'total_events': len(self.debug_log),
                'error_count': self.error_count,
                'warning_count': self.warning_count,
                'simulation_step': self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else None
            },
            'events_by_type': self.get_events_by_type(),
            'validation_issues': self.validate_simulation_state(),
            'timeline': self.get_timeline_summary()
        }
        
        return report
    
    def get_events_by_type(self):
        """Get events grouped by type."""
        events_by_type = {}
        for event in self.debug_log:
            event_type = event['type']
            if event_type not in events_by_type:
                events_by_type[event_type] = []
            events_by_type[event_type].append(event)
        return events_by_type
    
    def get_timeline_summary(self):
        """Get timeline summary of key events."""
        if not self.debug_log:
            return []
        
        # Get critical events
        critical_events = [e for e in self.debug_log 
                          if e['severity'] in ('error', 'warning')]
        
        timeline = []
        for event in critical_events:
            timeline.append({
                'step': event['step'],
                'time': event['timestamp'],
                'type': event['type'],
                'message': event['message'],
                'severity': event['severity']
            })
        
        return timeline

# Usage
debugger = SimulationDebugger(brain)

# Simulate with debugging
for step in range(1000):
    # Validate state
    issues = debugger.validate_simulation_state()
    if issues:
        for issue in issues:
            debugger.log_event('validation', issue, 'warning')
    
    # Run simulation step
    brain.step(step)
    
    # Log events
    if step % 100 == 0:
        debugger.log_event('progress', f"Step {step} completed", 'info')
    
    if brain.getFiringNeuronCount() > 100:
        debugger.log_event('high_activity', 
                          f"High neuron activity: {brain.getFiringNeuronCount()} firing", 
                          'warning')
    
    # Check for errors
    try:
        firing_rate = brain.getAverageFiringRate()
        if not np.isfinite(firing_rate):
            debugger.log_event('error', f"Invalid firing rate: {firing_rate}", 'error')
    except:
        debugger.log_event('error', "Could not calculate firing rate", 'error')

# Generate debug report
debug_report = debugger.generate_debug_report()
print(f"Debug Report:")
print(f"Total events: {debug_report['summary']['total_events']}")
print(f"Errors: {debug_report['summary']['error_count']}")
print(f"Warnings: {debug_report['summary']['warning_count']}")
print(f"Validation issues: {len(debug_report['validation_issues'])}")
for issue in debug_report['validation_issues']:
    print(f"  - {issue}")
```

## 8. Export and Analysis Tools

### Data Export for Analysis

```python
# Export simulation data for external analysis
class DataExporter:
    def __init__(self, brain, output_dir='simulation_data'):
        self.brain = brain
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def export_spike_data(self, filename='spike_data.csv'):
        """Export spike data for analysis."""
        import csv
        
        filepath = os.path.join(self.output_dir, filename)
        
        with open(filepath, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['step', 'neuron_id', 'spike_time', 'region_id'])
            
            # Export spike data
            # Note: This assumes access to spike data
            # In practice, you would need to modify the spike system
            # to record spike events
            pass
    
    def export_neural_activity(self, filename='neural_activity.csv'):
        """Export neural activity data."""
        import csv
        import pandas as pd
        
        filepath = os.path.join(self.output_dir, filename)
        
        # Collect neural activity data
        activity_data = []
        
        for step in range(1000):  # Export first 1000 steps
            self.brain.step(step)
            
            # Collect activity data
            regions = self.brain.getRegions()
            for region in regions:
                neurons = region.getNeurons()
                for neuron in neurons:
                    activity_data.append({
                        'step': step,
                        'region_id': region.getId().index(),
                        'neuron_id': neuron.getId().index(),
                        'membrane_potential': neuron.getState().membranePotential,
                        'is_firing': neuron.isFiring()
                    })
        
        # Convert to DataFrame and save
        df = pd.DataFrame(activity_data)
        df.to_csv(filepath, index=False)
        
        return df
    
    def export_configuration(self, filename='configuration.json'):
        """Export configuration for reproducibility."""
        import json
        
        filepath = os.path.join(self.output_dir, filename)
        
        # Get configuration data
        config = self.brain.getConfig()
        
        # Convert configuration to dictionary
        config_dict = config.to_dict() if hasattr(config, 'to_dict') else {
            'brain': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'region_count': self.brain.getRegionCount()
            },
            'simulation_stats': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate()
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        return config_dict
    
    def export_complete_dataset(self, prefix='simulation_'):
        """Export complete dataset for analysis."""
        datasets = {}
        
        # Export spike data
        datasets['spike_data'] = self.export_spike_data(f"{prefix}spikes.csv")
        
        # Export neural activity
        datasets['neural_activity'] = self.export_neural_activity(f"{prefix}activity.csv")
        
        # Export configuration
        datasets['configuration'] = self.export_configuration(f"{prefix}config.json")
        
        # Export summary statistics
        datasets['summary'] = self.export_summary(f"{prefix}summary.json")
        
        return datasets
    
    def export_summary(self, filename='summary.json'):
        """Export summary statistics."""
        import json
        
        filepath = os.path.join(self.output_dir, filename)
        
        summary = {
            'simulation_parameters': {
                'total_steps': self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else 1000,
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'region_count': self.brain.getRegionCount()
            },
            'performance_metrics': {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
                'active_neuron_count': self.brain.getActiveNeuronCount()
            },
            'development_stats': {
                'developmental_stage': self.brain.getDevelopmentalStage().name,
                'checkpoint_count': self.brain.getCheckpointCount() if hasattr(self.brain, 'getCheckpointCount') else 0
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(summary, f, indent=2)
        
        return summary

# Usage
exporter = DataExporter(brain)

# Run simulation
datasets = exporter.export_complete_dataset()
print("Datasets exported:")
for name, data in datasets.items():
    print(f"  {name}: {len(data) if hasattr(data, '__len__') else 'N/A'} records")
```

## 9. Performance Optimization Tools

### Performance Profiling

```python
# Performance profiling tools for optimization
class PerformanceProfiler:
    def __init__(self, brain):
        self.brain = brain
        self.timing_data = {}
        self.memory_usage = []
        self.cpu_usage = []
    
    def profile_simulation_step(self, step_num, step_func):
        """Profile a single simulation step."""
        import time
        import psutil
        import os
        
        start_time = time.time()
        process = psutil.Process(os.getpid())
        
        # Record baseline memory
        baseline_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Execute step
        step_func()
        
        # Calculate metrics
        end_time = time.time()
        step_time = end_time - start_time
        
        # Get memory usage
        current_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_delta = current_memory - baseline_memory
        
        # Get CPU usage
        cpu_percent = process.cpu_percent()
        
        # Store metrics
        step_metrics = {
            'step': step_num,
            'step_time': step_time,
            'memory_delta': memory_delta,
            'cpu_percent': cpu_percent,
            'timestamp': end_time
        }
        
        if step_num not in self.timing_data:
            self.timing_data[step_num] = []
        self.timing_data[step_num].append(step_metrics)
        
        return step_metrics
    
    def run_profiled_simulation(self, num_steps, step_interval=10):
        """Run simulation with performance profiling."""
        print(f"Running profiled simulation for {num_steps} steps...")
        
        start_time = time.time()
        
        for step in range(num_steps):
            # Profile step execution
            metrics = self.profile_simulation_step(step, lambda: self.brain.step(step))
            
            # Print performance metrics at intervals
            if step % step_interval == 0:
                self.print_step_performance(step, metrics)
        
        total_time = time.time() - start_time
        self.print_simulation_summary(total_time, num_steps)
    
    def print_step_performance(self, step, metrics):
        """Print performance metrics for a step."""
        avg_step_time = sum(m['step_time'] for m in metrics) / len(metrics)
        avg_memory_delta = sum(m['memory_delta'] for m in metrics) / len(metrics)
        avg_cpu = sum(m['cpu_percent'] for m in metrics) / len(metrics)
        
        print(f"Step {step}:")
        print(f"  Avg step time: {avg_step_time*1000:.2f} ms")
        print(f"  Memory delta: {avg_memory_delta:.2f} MB")
        print(f"  Avg CPU: {avg_cpu:.1f}%")
    
    def print_simulation_summary(self, total_time, num_steps):
        """Print simulation performance summary."""
        print(f"\nSimulation Summary:")
        print(f"  Total time: {total_time:.2f} seconds")
        print(f"  Total steps: {num_steps}")
        print(f"  Steps per second: {num_steps/total_time:.1f}")
        
        if self.timing_data:
            all_metrics = []
            for step_metrics_list in self.timing_data.values():
                all_metrics.extend(step_metrics_list)
            
            if all_metrics:
                avg_step_time = sum(m['step_time'] for m in all_metrics) / len(all_metrics)
                max_step_time = max(m['step_time'] for m in all_metrics)
                min_step_time = min(m['step_time'] for m in all_metrics)
                
                print(f"  Avg step time: {avg_step_time*1000:.2f} ms")
                print(f"  Min step time: {min_step_time*1000:.2f} ms")
                print(f"  Max step time: {max_step_time*1000:.2f} ms")
                print(f"  Time std: {np.std([m['step_time'] for m in all_metrics])*1000:.2f} ms")
    
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        if not self.timing_data:
            return None
        
        # Aggregate metrics
        all_metrics = []
        for step_metrics_list in self.timing_data.values():
            all_metrics.extend(step_metrics_list)
        
        if not all_metrics:
            return None
        
        # Calculate statistics
        step_times = [m['step_time'] for m in all_metrics]
        memory_deltas = [m['memory_delta'] for m in all_metrics]
        cpu_percents = [m['cpu_percent'] for m in all_metrics]
        
        report = {
            'summary': {
                'total_steps_profiled': len(self.timing_data),
                'total_metric_samples': len(all_metrics),
                'simulation_time': sum(step_times),
                'steps_per_second': len(all_metrics) / sum(step_times) if sum(step_times) > 0 else 0
            },
            'performance': {
                'step_time_ms': {
                    'mean': np.mean(step_times) * 1000,
                    'median': np.median(step_times) * 1000,
                    'std': np.std(step_times) * 1000,
                    'min': np.min(step_times) * 1000,
                    'max': np.max(step_times) * 1000,
                    'percentile_95': np.percentile(step_times, 95) * 1000,
                    'percentile_99': np.percentile(step_times, 99) * 1000
                },
                'memory_delta_mb': {
                    'mean': np.mean(memory_deltas),
                    'median': np.median(memory_deltas),
                    'std': np.std(memory_deltas),
                    'min': np.min(memory_deltas),
                    'max': np.max(memory_deltas)
                },
                'cpu_percent': {
                    'mean': np.mean(cpu_percents),
                    'median': np.median(cpu_percents),
                    'std': np.std(cpu_percents),
                    'min': np.min(cpu_percents),
                    'max': np.max(cpu_percents)
                }
            }
        }
        
        return report

# Usage
profiler = PerformanceProfiler(brain)

# Run profiled simulation
profiler.run_profiled_simulation(1000, step_interval=50)

# Generate performance report
performance_report = profiler.generate_performance_report()
if performance_report:
    print(f"\nPerformance Report:")
    print(f"Steps per second: {performance_report['summary']['steps_per_second']:.1f}")
    print(f"Average step time: {performance_report['performance']['step_time_ms']['mean']:.2f} ms")
    print(f"Memory delta std: {performance_report['performance']['memory_delta_mb']['std']:.2f} MB")
```

## 10. Advanced Configuration Tools

### Configuration Generator

```python
# Advanced configuration tools for systematic exploration
class ConfigurationGenerator:
    def __init__(self):
        self.config_templates = {}
    
    def create_configuration_space(self, parameter_ranges):
        """Create configuration space from parameter ranges.
        
        parameter_ranges: Dictionary mapping parameter names to value ranges
        Example: {'brain.neuron_count': [100, 500, 1000, 2000]}
        """
        import itertools
        
        param_names = list(parameter_ranges.keys())
        param_values = list(parameter_ranges.values())
        
        # Generate all combinations
        combinations = list(itertools.product(*param_values))
        
        # Convert to dictionaries
        config_space = []
        for combo in combinations:
            config = {}
            for i, param_name in enumerate(param_names):
                config[param_name] = combo[i]
            config_space.append(config)
        
        return config_space
    
    def create_nested_configurations(self, base_config, parameter_modifications):
        """Create nested configurations by modifying base configuration."""
        configs = []
        
        for mod_name, modifications in parameter_modifications.items():
            config = pynlm.createDefaultConfig()
            
            # Apply base configuration
            for key, value in base_config.items():
                config.set(key, value)
            
            # Apply modifications
            for key, value in modifications.items():
                config.set(key, value)
            
            configs.append({
                'name': mod_name,
                'config': config,
                'parameters': modifications
            })
        
        return configs
    
    def create_development_schedules(self, schedule_templates):
        """Create different development schedules."""
        configs = []
        
        for template_name, template in schedule_templates.items():
            config = pynlm.createDefaultConfig()
            
            # Set development schedule
            config.set("development.stage_duration", template['stage_duration'])
            config.set("development.plasticity_decay_rate", template['plasticity_decay_rate'])
            
            configs.append({
                'name': template_name,
                'config': config,
                'template': template
            })
        
        return configs
    
    def create_neuromodulation_scenarios(self, neuromodulation_scenarios):
        """Create different neuromodulation scenarios."""
        configs = []
        
        for scenario_name, scenario in neuromodulation_scenarios.items():
            config = pynlm.createDefaultConfig()
            
            # Apply neuromodulation settings
            if 'dopamine' in scenario:
                config.set("neuromod.dopamine.scale", scenario['dopamine']['scale'])
            
            if 'curiosity' in scenario:
                config.set("neuromod.curiosity.enable", scenario['curiosity']['enable'])
                config.set("neuromod.curiosity.exploration_rate", scenario['curiosity']['exploration_rate'])
            
            if 'novelty' in scenario:
                config.set("neuromod.novelty.enable", scenario['novelty']['enable'])
                config.set("neuromod.novelty.threshold", scenario['novelty']['threshold'])
            
            configs.append({
                'name': scenario_name,
                'config': config,
                'scenario': scenario
            })
        
        return configs
    
    def generate_research_configurations(self):
        """Generate configurations for common research scenarios."""
        configurations = []
        
        # Get baseline configuration
        baseline = pynlm.createDefaultConfig()
        
        # Configuration 1: Minimal viable system
        minimal_config = pynlm.createDefaultConfig()
        minimal_config.set("brain.neuron_count", 100)
        minimal_config.set("brain.region_count", 1)
        configurations.append({
            'name': 'minimal_viable_system',
            'config': minimal_config,
            'description': 'Small, simple brain for testing and validation'
        })
        
        # Configuration 2: Development-focused
        dev_config = pynlm.createDefaultConfig()
        dev_config.set("development.stage_duration", [50, 100, 150, 200])
        dev_config.set("development.plasticity_decay_rate", 0.05)
        configurations.append({
            'name': 'development_focused',
            'config': dev_config,
            'description': 'Brain optimized for studying development and learning'
        })
        
        # Configuration 3: Neuromodulation-focused
        neuromod_config = pynlm.createDefaultConfig()
        neuromod_config.set("neuromod.curiosity.enable", True)
        neuromod_config.set("neuromod.curiosity.exploration_rate", 0.8)
        neuromod_config.set("neuromod.novelty.enable", True)
        neuromod_config.set("neuromod.novelty.threshold", 0.05)
        configurations.append({
            'name': 'neuromodulation_focused',
            'config': neuromod_config,
            'description': 'Brain optimized for studying neuromodulation effects'
        })
        
        # Configuration 4: Plasticity-focused
        plasticity_config = pynlm.createDefaultConfig()
        plasticity_config.set("plasticity.stdp.learning_rate", 0.01)
        plasticity_config.set("plasticity.hebbian.learning_rate", 0.05)
        plasticity_config.set("plasticity.structural.enable", True)
        configurations.append({
            'name': 'plasticity_focused',
            'config': plasticity_config,
            'description': 'Brain optimized for studying plasticity mechanisms'
        })
        
        return configurations

# Usage
config_generator = ConfigurationGenerator()

# Create configuration space
parameter_ranges = {
    'brain.neuron_count': [100, 500, 1000],
    'brain.connection_probability': [0.05, 0.1, 0.2],
    'plasticity.stdp.learning_rate': [0.001, 0.01]
}

config_space = config_generator.create_configuration_space(parameter_ranges)
print(f"Generated {len(config_space)} configuration combinations")

# Generate research configurations
research_configs = config_generator.generate_research_configurations()
print("\nResearch Configurations:")
for config in research_configs:
    print(f"\n{config['name']}:")
    print(f"  Description: {config['description']}")
    print(f"  Neuron count: {config['config'].get('brain.neuron_count', 'default')}")
```

## 11. Advanced API Extensions

### Custom Python Classes

```python
# Extend Python API with custom classes
class NLMExperiment:
    """Advanced experiment manager for systematic testing."""
    
    def __init__(self, config_generator, profiler=None, debugger=None):
        self.config_generator = config_generator
        self.profiler = profiler
        self.debugger = debugger
        self.results = []
        self.experiments = []
    
    def add_experiment(self, name, config_template, experiment_type='parameter_sweep'):
        """Add experiment to the experiment manager."""
        experiment = {
            'name': name,
            'config_template': config_template,
            'type': experiment_type,
            'start_time': None,
            'end_time': None,
            'results': None
        }
        self.experiments.append(experiment)
        return experiment
    
    def run_experiment(self, experiment_name, repetitions=5):
        """Run experiment with given name and repetitions."""
        # Find experiment
        experiment = next((e for e in self.experiments if e['name'] == experiment_name), None)
        if not experiment:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        print(f"Running experiment: {experiment_name} ({repetitions} repetitions)")
        
        # Generate configurations
        if experiment['type'] == 'parameter_sweep':
            param_ranges = experiment['config_template']['parameter_ranges']
            config_space = self.config_generator.create_configuration_space(param_ranges)
        elif experiment['type'] == 'preset':
            config_space = [experiment['config_template']]
        else:
            raise ValueError(f"Unknown experiment type: {experiment['type']}")
        
        # Run experiments
        experiment['start_time'] = time.time()
        
        for i, config_dict in enumerate(config_space):
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply configuration
            for key, value in config_dict.items():
                config.set(key, value)
            
            # Run repetitions
            repetition_results = []
            for rep in range(repetitions):
                # Create brain
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run simulation with profiling if available
                if self.profiler:
                    self.profiler.run_profiled_simulation(
                        experiment['config_template']['steps']
                    )
                
                # Run simulation
                for step in range(experiment['config_template']['steps']):
                    brain.step(step)
                
                # Collect results
                result = {
                    'config': config_dict,
                    'repetition': rep,
                    'final_spike_count': brain.getTotalSpikeCount(),
                    'final_firing_rate': brain.getAverageFiringRate(),
                    'development_stage': brain.getDevelopmentalStage().name
                }
                repetition_results.append(result)
            
            experiment['results'] = repetition_results
        
        experiment['end_time'] = time.time()
        
        return experiment['results']
    
    def run_batch_experiments(self):
        """Run all experiments in the experiment manager."""
        all_results = {}
        
        for experiment in self.experiments:
            results = self.run_experiment(experiment['name'])
            all_results[experiment['name']] = results
        
        self.results = all_results
        return all_results
    
    def analyze_experiments(self):
        """Analyze experiment results and generate insights."""
        if not self.results:
            return None
        
        analysis = {}
        
        for experiment_name, experiment_results in self.results.items():
            # Aggregate results
            all_spikes = []
            all_rates = []
            
            for result in experiment_results:
                all_spikes.append(result['final_spike_count'])
                all_rates.append(result['final_firing_rate'])
            
            # Calculate statistics
            experiment_analysis = {
                'total_experiments': len(experiment_results),
                'total_spikes': {
                    'mean': np.mean(all_spikes),
                    'std': np.std(all_spikes),
                    'min': np.min(all_spikes),
                    'max': np.max(all_spikes)
                },
                'firing_rates': {
                    'mean': np.mean(all_rates),
                    'std': np.std(all_rates),
                    'min': np.min(all_rates),
                    'max': np.max(all_rates)
                },
                'development_stages': list(set(r['development_stage'] for r in experiment_results))
            }
            
            analysis[experiment_name] = experiment_analysis
        
        return analysis
    
    def generate_experiment_report(self):
        """Generate comprehensive experiment report."""
        if not self.results:
            return None
        
        report = {
            'experiment_summary': {
                'total_experiments': len(self.experiments),
                'total_repetitions': sum(len(r) for r in self.results.values()),
                'start_time': min(e['start_time'] for e in self.experiments),
                'end_time': max(e['end_time'] for e in self.experiments)
            },
            'experiment_details': {},
            'analysis': self.analyze_experiments()
        }
        
        for experiment_name, experiment_results in self.results.items():
            report['experiment_details'][experiment_name] = {
                'name': experiment_name,
                'repetitions': len(experiment_results),
                'parameters_varied': list(experiment_results[0]['config'].keys()),
                'result_summary': {
                    'total_spikes': [r['final_spike_count'] for r in experiment_results],
                    'firing_rates': [r['final_firing_rate'] for r in experiment_results],
                    'development_stages': [r['development_stage'] for r in experiment_results]
                }
            }
        
        return report

# Usage
experiment_manager = NLMExperiment(config_generator, profiler, debugger)

# Add experiments
experiment_manager.add_experiment(
    'small_brain_test',
    {
        'parameter_ranges': {
            'brain.neuron_count': [100, 200, 500],
            'steps': [500]
        },
        'steps': 500
    }
)

experiment_manager.add_experiment(
    'plasticity_study',
    {
        'parameter_ranges': {
            'plasticity.stdp.learning_rate': [0.001, 0.01, 0.1],
            'steps': [1000]
        },
        'steps': 1000
    }
)

# Run all experiments
experiment_manager.run_batch_experiments()

# Generate report
experiment_report = experiment_manager.generate_experiment_report()
if experiment_report:
    print(f"\nExperiment Report:")
    print(f"Total experiments: {experiment_report['experiment_summary']['total_experiments']}")
    print(f"Total repetitions: {experiment_report['experiment_summary']['total_repetitions']}")
    
    for exp_name, exp_details in experiment_report['experiment_details'].items():
        print(f"\n{exp_name}:")
        print(f"  Repetitions: {exp_details['repetitions']}")
        print(f"  Parameters varied: {exp_details['parameters_varied']}")
        
        analysis = experiment_report['analysis'][exp_name]
        print(f"  Average spikes: {analysis['total_spikes']['mean']:.1f} ± {analysis['total_spikes']['std']:.1f}")
```

## 12. Integration with External Tools

### Jupyter Notebook Integration

```python
# Integration with Jupyter notebooks for interactive experimentation
class JupyterNotebookIntegration:
    """Integration tools for Jupyter notebooks."""
    
    def __init__(self, brain):
        self.brain = brain
        self.notebook_cells = []
        self.interactive_plots = []
    
    def create_interactive_simulation(self, steps=1000, interval=100):
        """Create interactive simulation with real-time plotting."""
        try:
            import matplotlib.pyplot as plt
            from matplotlib.animation import FuncAnimation
            import numpy as np
            
            # Setup figure
            fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))
            
            # Data storage
            time_data = []
            spike_count_data = []
            firing_rate_data = []
            
            # Create brain if not already created
            if not hasattr(self, 'brain_created'):
                self.brain.initialize()
                self.brain_created = True
            
            def update(frame):
                # Run simulation
                self.brain.step(frame)
                
                # Collect data
                time_data.append(frame)
                spike_count_data.append(self.brain.getTotalSpikeCount())
                firing_rate_data.append(self.brain.getAverageFiringRate())
                
                # Clear and plot
                ax1.clear()
                ax1.plot(time_data, spike_count_data, 'r-', label='Total Spikes')
                ax1.set_title('Spike Count Over Time')
                ax1.set_xlabel('Step')
                ax1.set_ylabel('Spike Count')
                ax1.legend()
                ax1.grid(True, alpha=0.3)
                
                ax2.clear()
                ax2.plot(time_data, firing_rate_data, 'g-', label='Firing Rate')
                ax2.set_title('Firing Rate Over Time')
                ax2.set_xlabel('Step')
                ax2.set_ylabel('Firing Rate (Hz)')
                ax2.legend()
                ax2.grid(True, alpha=0.3)
                
                # Update third subplot with neural statistics
                ax3.clear()
                regions = self.brain.getRegions()
                if regions:
                    region_names = [r.getName() for r in regions]
                    neuron_counts = [r.getTotalNeuronCount() for r in regions]
                    
                    bars = ax3.bar(region_names, neuron_counts)
                    ax3.set_title('Neuron Distribution by Region')
                    ax3.set_xlabel('Region')
                    ax3.set_ylabel('Neuron Count')
                    ax3.bar_label(bars, padding=3)
                
                plt.tight_layout()
            
            # Create animation
            anim = FuncAnimation(fig, update, frames=steps, interval=interval, cache_frame_data=False)
            
            self.interactive_plots.append(anim)
            
            plt.show()
            
            return anim
            
        except ImportError:
            print("Jupyter notebook integration requires matplotlib")
            return None
    
    def create_interactive_configuration_tool(self):
        """Create interactive configuration tool for Jupyter."""
        try:
            import ipywidgets as widgets
            from IPython.display import display, HTML
            
            # Configuration widgets
            config_widgets = {}
            
            # Neuron count slider
            config_widgets['neuron_count'] = widgets.IntSlider(
                value=1000,
                min=100,
                max=10000,
                step=100,
                description='Neuron Count:',
                continuous_update=True
            )
            
            # Region count slider
            config_widgets['region_count'] = widgets.IntSlider(
                value=1,
                min=1,
                max=10,
                description='Region Count:',
                continuous_update=True
            )
            
            # Connection probability slider
            config_widgets['connection_prob'] = widgets.FloatSlider(
                value=0.1,
                min=0.01,
                max=0.5,
                step=0.01,
                description='Conn Prob:',
                continuous_update=True
            )
            
            # Development stage selector
            config_widgets['development_stage'] = widgets.Dropdown(
                options=['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging'],
                value='Adult',
                description='Dev Stage:',
                continuous_update=True
            )
            
            # Create update button
            update_button = widgets.Button(description='Update Brain')
            
            def update_brain(b):
                # Update brain configuration
                config = pynlm.createDefaultConfig()
                config.set('brain.neuron_count', config_widgets['neuron_count'].value)
                config.set('brain.region_count', config_widgets['region_count'].value)
                config.set('brain.connection_probability', config_widgets['connection_prob'].value)
                
                # Create new brain with updated configuration
                new_brain = pynlm.createBrain(config)
                new_brain.initialize()
                
                # Update development stage
                stage_map = {
                    'Initial': nlm.DevelopmentalStage.Initial,
                    'CriticalPeriod': nlm.DevelopmentalStage.CriticalPeriod,
                    'Maturation': nlm.DevelopmentalStage.Maturation,
                    'Adult': nlm.DevelopmentalStage.Adult,
                    'Aging': nlm.DevelopmentalStage.Aging
                }
                
                new_brain.setDevelopmentalStage(stage_map[config_widgets['development_stage'].value])
                
                # Update global brain
                globals()['brain'] = new_brain
                
                print("Brain updated with new configuration")
            
            update_button.on_click(update_brain)
            
            # Display widgets
            display(HTML('<h3>Interactive Configuration Tool</h3>'))
            for widget in config_widgets.values():
                display(widget)
            display(update_button)
            
            return config_widgets
            
        except ImportError:
            print("Jupyter interactive widgets require ipywidgets")
            return None
    
    def create_notebook_template(self, template_name='research'):
        """Create Jupyter notebook template for research."""
        template = {
            'title': f'NLM Research Template - {template_name}',
            'description': f'Jupyter notebook template for {template_name} experiments',
            'cells': [
                {
                    'type': 'markdown',
                    'content': f'# NLM Research Template - {template_name}\n\nThis template provides a structured approach to conducting NLM research experiments.'
                },
                {
                    'type': 'code',
                    'content': '''import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Initialize NLM brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Set up figure for visualization
fig, axes = plt.subplots(2, 2, figsize=(15, 10))
fig.suptitle(f"{template_name.replace('_', ' ').title()} Experiment", fontsize=16)
'''
                },
                {
                    'type': 'markdown',
                    'content': '## Experiment Setup

This section sets up the experiment parameters and initializes the brain simulation.'
                },
                {
                    'type': 'code',
                    'content': '''# Configure experiment parameters
experiment_config = {
    'steps': 2000,
    'record_interval': 100,
    'enable_neuromodulation': True,
    'enable_development': True,
    'enable_curiosity': True
}

# Run simulation with progress monitoring
for step in range(experiment_config['steps']):
    brain.step(step)
    
    # Record statistics at intervals
    if step % experiment_config['record_interval'] == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
'''
                },
                {
                    'type': 'markdown',
                    'content': '## Results Analysis

This section analyzes the simulation results and generates visualizations.'
                },
                {
                    'type': 'code',
                    'content': '''# Analyze simulation results
results = {
    'total_steps': experiment_config['steps'],
    'total_spikes': brain.getTotalSpikeCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'region_count': brain.getRegionCount(),
    'development_stage': brain.getDevelopmentalStage().name
}

# Create visualization
fig2, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))

# Plot 1: Spike count over time
ax1.plot(range(0, experiment_config['steps'], experiment_config['record_interval']),
         [brain.getTotalSpikeCount() for _ in range(0, experiment_config['steps'], experiment_config['record_interval'])])
ax1.set_title('Total Spike Count')
ax1.set_xlabel('Step')
ax1.set_ylabel('Spike Count')

# Plot 2: Firing rate over time
ax2.plot(range(0, experiment_config['steps'], experiment_config['record_interval']),
         [brain.getAverageFiringRate() for _ in range(0, experiment_config['steps'], experiment_config['record_interval'])])
ax2.set_title('Average Firing Rate')
ax2.set_xlabel('Step')
ax2.set_ylabel('Firing Rate (Hz)')

# Plot 3: Region distribution
regions = brain.getRegions()
if regions:
    region_names = [r.getName() for r in regions]
    neuron_counts = [r.getTotalNeuronCount() for r in regions]
    ax3.bar(region_names, neuron_counts)
    ax3.set_title('Neuron Distribution by Region')
    ax3.set_xlabel('Region')
    ax3.set_ylabel('Neuron Count')

# Plot 4: Development stage
ax4.bar(['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging'], 
        [0, 0, 0, 1, 0])  # Current stage is Adult
ax4.set_title('Development Stage')
ax4.set_ylabel('Current Stage')

plt.tight_layout()
plt.show()

print("\nExperiment Results:")
for key, value in results.items():
    print(f"{key}: {value}")
'''
                },
                {
                    'type': 'markdown',
                    'content': '## Additional Analysis

This section provides additional analysis and export capabilities for research.'
                },
                {
                    'type': 'code',
                    'content': '''# Export data for further analysis
import json
import pandas as pd

# Export configuration
config_data = {
    'brain': {
        'neuron_count': brain.getTotalNeuronCount(),
        'region_count': brain.getRegionCount()
    },
    'simulation_stats': {
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate()
    }
}

with open('experiment_config.json', 'w') as f:
    json.dump(config_data, f, indent=2)

print("Configuration exported to experiment_config.json")

# Save brain state for reproducibility
brain.save(f"brain_checkpoint_{template_name}.bin")
print(f"Brain state saved to brain_checkpoint_{template_name}.bin")

# Generate summary report
report = {
    'experiment_name': template_name,
    'timestamp': pd.Timestamp.now().isoformat(),
    'results': results,
    'config_used': config_data
}

with open(f"experiment_report_{template_name}.json", 'w') as f:
    json.dump(report, f, indent=2)

print(f"Experiment report saved to experiment_report_{template_name}.json")
'''
                }
            ]
        }
        
        return template
    
    def save_notebook_template(self, template, filename='nlm_research_template.ipynb'):
        """Save notebook template to file."""
        try:
            from nbformat import v4 as nbformat
            
            notebook = nbformat.to_notebook([
                nbformat.new_code_cell(cell['content'])
                for cell in template['cells']
                if cell['type'] == 'code'
            ] + [
                nbformat.new_markdown_cell(cell['content'])
                for cell in template['cells']
                if cell['type'] == 'markdown'
            ])
            
            nbformat.write(notebook, filename)
            print(f"Notebook template saved to {filename}")
            
        except ImportError:
            print("Notebook template saving requires nbformat")
        except Exception as e:
            print(f"Error saving notebook template: {e}")

# Usage example
notebook_integration = JupyterNotebookIntegration(brain)

# Create interactive simulation
anim = notebook_integration.create_interactive_simulation(steps=1000, interval=100)

# Create interactive configuration tool
config_widgets = notebook_integration.create_interactive_configuration_tool()

# Create notebook template
template = notebook_integration.create_notebook_template('advanced_research')
notebook_integration.save_notebook_template(template, 'advanced_nlm_template.ipynb')
```

## 13. Performance Optimization and Scaling

### Large-Scale Simulation Manager

```python
# Large-scale simulation manager for high-performance computing
class LargeScaleSimulationManager:
    def __init__(self, num_workers=None, memory_limit_mb=2000):
        import multiprocessing as mp
        self.num_workers = num_workers or mp.cpu_count()
        self.memory_limit_mb = memory_limit_mb
        self.worker_pool = None
        self.job_queue = []
        self.results = []
    
    def __enter__(self):
        """Initialize worker pool."""
        import multiprocessing as mp
        import resource
        
        # Set memory limit
        resource.setrlimit(resource.RLIMIT_AS, 
                          (self.memory_limit_mb * 1024 * 1024, -1))
        
        # Create worker pool
        self.worker_pool = mp.Pool(
            processes=self.num_workers,
            initializer=self._worker_init,
            initargs=(self.memory_limit_mb,)
        )
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Clean up worker pool."""
        if self.worker_pool:
            self.worker_pool.close()
            self.worker_pool.join()
    
    @staticmethod
    def _worker_init(memory_limit_mb):
        """Worker initialization."""
        import resource
        resource.setrlimit(resource.RLIMIT_AS, 
                          (memory_limit_mb * 1024 * 1024, -1))
    
    def submit_job(self, config, steps, job_id=None):
        """Submit simulation job to worker pool."""
        if job_id is None:
            job_id = len(self.job_queue)
        
        job = {
            'id': job_id,
            'config': config,
            'steps': steps,
            'status': 'pending'
        }
        
        self.job_queue.append(job)
        return job_id
    
    def run_jobs(self, max_jobs=None):
        """Run submitted jobs in parallel."""
        if max_jobs is None:
            max_jobs = len(self.job_queue)
        
        # Submit jobs
        async_results = []
        for job in self.job_queue[:max_jobs]:
            result_async = self.worker_pool.apply_async(
                self._run_single_simulation,
                (job['config'], job['steps'])
            )
            async_results.append((job['id'], result_async))
        
        # Collect results
        self.results = []
        for job_id, result_async in async_results:
            try:
                result = result_async.get(timeout=3600)  # 1 hour timeout
                self.results.append({
                    'job_id': job_id,
                    'result': result,
                    'status': 'completed'
                })
                print(f"Job {job_id} completed successfully")
            except Exception as e:
                self.results.append({
                    'job_id': job_id,
                    'result': None,
                    'status': 'failed',
                    'error': str(e)
                })
                print(f"Job {job_id} failed: {e}")
    
    @staticmethod
    def _run_single_simulation(config_dict, steps):
        """Run single simulation (worker function)."""
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply configuration
            for key, value in config_dict.items():
                config.set(key, value)
            
            # Create and run brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(steps):
                brain.step(step)
            
            # Return results
            return {
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'region_count': brain.getRegionCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'development_stage': brain.getDevelopmentalStage().name,
                'success': True
            }
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e)
            }
    
    def analyze_large_scale_results(self):
        """Analyze large-scale simulation results."""
        if not self.results:
            return None
        
        # Filter successful results
        successful_results = [r for r in self.results if r['status'] == 'completed']
        
        if not successful_results:
            return None
        
        # Extract data
        results_data = [r['result'] for r in successful_results]
        
        # Calculate statistics
        analysis = {
            'total_jobs': len(self.job_queue),
            'successful_jobs': len(successful_results),
            'failed_jobs': len(self.results) - len(successful_results),
            'performance': {
                'spike_count': {
                    'mean': np.mean([r['total_spikes'] for r in results_data]),
                    'std': np.std([r['total_spikes'] for r in results_data]),
                    'min': np.min([r['total_spikes'] for r in results_data]),
                    'max': np.max([r['total_spikes'] for r in results_data])
                },
                'firing_rate': {
                    'mean': np.mean([r['average_firing_rate'] for r in results_data]),
                    'std': np.std([r['average_firing_rate'] for r in results_data]),
                    'min': np.min([r['average_firing_rate'] for r in results_data]),
                    'max': np.max([r['average_firing_rate'] for r in results_data])
                }
            },
            'configurations': list(set(tuple(r['config'].items()) for r in results_data))
        }
        
        return analysis
    
    def find_optimal_configurations(self, metric='total_spikes', top_n=5):
        """Find optimal configurations based on metric."""
        if not self.results:
            return None
        
        # Filter successful results
        successful_results = [r for r in self.results if r['status'] == 'completed']
        
        # Sort by metric
        sorted_results = sorted(
            successful_results,
            key=lambda x: x['result'][metric],
            reverse=True
        )
        
        # Return top N
        top_results = sorted_results[:top_n]
        
        return {
            'optimal_configurations': [
                {
                    'job_id': r['job_id'],
                    'metrics': r['result'],
                    'config': r['result']['config']
                }
                for r in top_results
            ],
            'ranking': [
                {
                    'job_id': r['job_id'],
                    'metric_value': r['result'][metric]
                }
                for r in sorted_results
            ]
        }

# Usage example
large_scale_manager = LargeScaleSimulationManager(num_workers=8, memory_limit_mb=4000)

# Create parameter space for large-scale experiment
parameter_ranges = {
    'brain.neuron_count': [1000, 5000, 10000, 20000],
    'brain.connection_probability': [0.01, 0.05, 0.1],
    'plasticity.stdp.learning_rate': [0.001, 0.01],
    'steps': [500, 1000]
}

# Generate parameter combinations
import itertools
param_names = list(parameter_ranges.keys())
param_values = list(parameter_ranges.values())

# Create subset for testing
combinations = []
for combo in itertools.product(*param_values):
    config = dict(zip(param_names, combo))
    combinations.append(config)
    
    # Limit to 20 combinations for testing
    if len(combinations) >= 20:
        break

# Submit jobs
with large_scale_manager as manager:
    for i, config in enumerate(combinations):
        manager.submit_job(config, config['steps'], job_id=i)
    
    # Run jobs
    manager.run_jobs(max_jobs=8)

# Analyze results
analysis = large_scale_manager.analyze_large_scale_results()
if analysis:
    print(f"\nLarge-Scale Analysis:")
    print(f"Total jobs: {analysis['total_jobs']}")
    print(f"Successful jobs: {analysis['successful_jobs']}")
    print(f"Failed jobs: {analysis['failed_jobs']}")
    print(f"Average spike count: {analysis['performance']['spike_count']['mean']:.1f} ± {analysis['performance']['spike_count']['std']:.1f}")
    print(f"Average firing rate: {analysis['performance']['firing_rate']['mean']:.2f} ± {analysis['performance']['firing_rate']['std']:.2f} Hz")

# Find optimal configurations
optimal = large_scale_manager.find_optimal_configurations('total_spikes', top_n=3)
if optimal:
    print(f"\nOptimal Configurations:")
    for i, config_info in enumerate(optimal['optimal_configurations']):
        print(f"{i+1}. Job {config_info['job_id']}:")
        print(f"   Metric value: {config_info['metrics']['total_spikes']}")
        print(f"   Firing rate: {config_info['metrics']['average_firing_rate']:.2f} Hz")
        print(f"   Parameters: {config_info['config']}")
```

## 14. Advanced API Customization

### Custom Python Extensions

```python
# Custom Python extensions for advanced NLM usage
class NLMExtensions:
    """Extension class for NLM with advanced functionality."""
    
    def __init__(self, brain, config_generator):
        self.brain = brain
        self.config_generator = config_generator
        self.custom_systems = {}
        self.performance_metrics = {}
        self.experimental_conditions = {}
    
    def add_custom_system(self, name, system):
        """Add custom system to brain."""
        self.custom_systems[name] = system
        print(f"Added custom system: {name}")
    
    def remove_custom_system(self, name):
        """Remove custom system from brain."""
        if name in self.custom_systems:
            del self.custom_systems[name]
            print(f"Removed custom system: {name}")
    
    def add_performance_metric(self, metric_name, metric_func, description=''):
        """Add custom performance metric."""
        self.performance_metrics[metric_name] = {
            'function': metric_func,
            'description': description
        }
        print(f"Added performance metric: {metric_name} - {description}")
    
    def remove_performance_metric(self, metric_name):
        """Remove performance metric."""
        if metric_name in self.performance_metrics:
            del self.performance_metrics[metric_name]
            print(f"Removed performance metric: {metric_name}")
    
    def add_experimental_condition(self, name, condition_config):
        """Add experimental condition."""
        self.experimental_conditions[name] = condition_config
        print(f"Added experimental condition: {name}")
    
    def remove_experimental_condition(self, name):
        """Remove experimental condition."""
        if name in self.experimental_conditions:
            del self.experimental_conditions[name]
            print(f"Removed experimental condition: {name}")
    
    def run_experimental_protocol(self, condition_name, trials=10):
        """Run experimental protocol with given condition."""
        if condition_name not in self.experimental_conditions:
            raise ValueError(f"Experimental condition '{condition_name}' not found")
        
        condition_config = self.experimental_conditions[condition_name]
        print(f"Running experimental protocol: {condition_name} ({trials} trials)")
        
        # Create base configuration
        config = pynlm.createDefaultConfig()
        
        # Apply condition configuration
        for key, value in condition_config.get('configuration', {}).items():
            config.set(key, value)
        
        # Run trials
        trial_results = []
        for trial in range(trials):
            print(f"  Trial {trial + 1}/{trials}")
            
            # Create brain
            trial_brain = pynlm.createBrain(config)
            trial_brain.initialize()
            
            # Run simulation
            for step in range(condition_config.get('steps', 1000)):
                trial_brain.step(step)
                
                # Record data if needed
                if condition_config.get('record_data', False):
                    self.record_trial_data(trial_brain, step)
            
            # Collect trial results
            trial_result = {
                'trial': trial + 1,
                'total_spikes': trial_brain.getTotalSpikeCount(),
                'average_firing_rate': trial_brain.getAverageFiringRate(),
                'development_stage': trial_brain.getDevelopmentalStage().name,
                'final_config': trial_brain.getConfig()
            }
            
            trial_results.append(trial_result)
        
        return trial_results
    
    def record_trial_data(self, brain, step):
        """Record trial data for analysis."""
        # Implement custom data recording
        pass
    
    def analyze_extensions_performance(self):
        """Analyze performance of all extensions."""
        analysis = {
            'custom_systems': list(self.custom_systems.keys()),
            'performance_metrics': list(self.performance_metrics.keys()),
            'experimental_conditions': list(self.experimental_conditions.keys()),
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'development_stage': self.brain.getDevelopmentalStage().name
            }
        }
        
        # Calculate performance metrics
        for metric_name, metric_info in self.performance_metrics.items():
            try:
                metric_value = metric_info['function'](self.brain)
                analysis[f'performance_{metric_name}'] = metric_value
            except Exception as e:
                analysis[f'performance_{metric_name}'] = f'Error: {e}'
        
        return analysis
    
    def export_extensions_state(self, filename='extensions_state.json'):
        """Export extensions state to file."""
        import json
        import datetime
        
        state = {
            'timestamp': datetime.datetime.now().isoformat(),
            'custom_systems': {name: str(system) for name, system in self.custom_systems.items()},
            'performance_metrics': {name: info['description'] for name, info in self.performance_metrics.items()},
            'experimental_conditions': {name: config for name, config in self.experimental_conditions.items()},
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'development_stage': self.brain.getDevelopmentalStage().name
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(state, f, indent=2)
        
        print(f"Extensions state exported to {filename}")
        return state
    
    def import_extensions_state(self, filename='extensions_state.json'):
        """Import extensions state from file."""
        try:
            with open(filename, 'r') as f:
                state = json.load(f)
            
            # Restore custom systems (simplified - in practice you would need to reconstruct objects)
            print(f"Extensions state imported from {filename}")
            
            return state
            
        except Exception as e:
            print(f"Error importing extensions state: {e}")
            return None

# Usage example
extensions = NLMExtensions(brain, config_generator)

# Add custom systems
custom_neuromod = CustomNeuromodulationSystem(brain)
extensions.add_custom_system('neuromodulation', custom_neuromod)

# Add performance metrics
extensions.add_performance_metric('firing_stability', 
                                 lambda b: b.getAverageFiringRate() / b.getTotalSpikeCount() if b.getTotalSpikeCount() > 0 else 0,
                                 'Measure firing rate stability')

# Add experimental condition
experimental_condition = {
    'name': 'high_curiosity',
    'configuration': {
        'neuromod.curiosity.enable': True,
        'neuromod.curiosity.exploration_rate': 0.8,
        'plasticity.stdp.learning_rate': 0.001
    },
    'steps': 2000,
    'record_data': False
}

extensions.add_experimental_condition('high_curiosity', experimental_condition)

# Run experimental protocol
experimental_results = extensions.run_experimental_protocol('high_curiosity', trials=3)

# Analyze performance
performance_analysis = extensions.analyze_extensions_performance()
print(f"\nPerformance Analysis:")
for key, value in performance_analysis.items():
    if key.startswith('performance_'):
        print(f"{key}: {value}")

# Export extensions state
extensions.export_extensions_state('my_extensions_state.json')

print(f"\nSuccessfully exported extensions state")
```

## 15. Advanced Error Handling and Recovery

### Robust Error Management

```python
# Advanced error handling and recovery for NLM
class NLMRobustnessManager:
    """Robust error handling and recovery for NLM simulations."""
    
    def __init__(self, brain, error_handlers=None):
        self.brain = brain
        self.error_handlers = error_handlers or self._default_error_handlers()
        self.error_log = []
        self.recovery_actions = []
        self.simulation_state = 'running'
    
    def _default_error_handlers(self):
        """Default error handlers."""
        return {
            'memory_error': self._handle_memory_error,
            'configuration_error': self._handle_configuration_error,
            'simulation_error': self._handle_simulation_error,
            'neuromodulation_error': self._handle_neuromodulation_error,
            'plasticity_error': self._handle_plasticity_error
        }
    
    def set_error_handler(self, error_type, handler):
        """Set custom error handler."""
        self.error_handlers[error_type] = handler
    
    def log_error(self, error_type, error_message, context=None):
        """Log error with context."""
        error_entry = {
            'timestamp': datetime.datetime.now().isoformat(),
            'type': error_type,
            'message': error_message,
            'context': context or {},
            'simulation_step': self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else None
        }
        
        self.error_log.append(error_entry)
        print(f"ERROR [{error_type}]: {error_message}")
        
        # Apply error handler
        if error_type in self.error_handlers:
            try:
                self.error_handlers[error_type](error_entry)
            except Exception as e:
                print(f"Error handler failed: {e}")
    
    def _handle_memory_error(self, error_entry):
        """Handle memory errors."""
        print(f"  Attempting memory recovery...")
        
        # Try to save current state
        try:
            self.brain.save(f"memory_error_backup_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.bin")
            print(f"  State saved to backup file")
        except Exception as e:
            print(f"  Failed to save state: {e}")
        
        # Reset brain to clean state
        try:
            self.brain.reset()
            print(f"  Brain reset to clean state")
            self.recovery_actions.append({
                'type': 'memory_recovery',
                'timestamp': datetime.datetime.now().isoformat(),
                'action': 'brain_reset'
            })
        except Exception as e:
            print(f"  Failed to reset brain: {e}")
    
    def _handle_configuration_error(self, error_entry):
        """Handle configuration errors."""
        print(f"  Attempting configuration recovery...")
        
        context = error_entry.get('context', {})
        original_config = context.get('original_config')
        
        if original_config:
            try:
                # Restore original configuration
                config = pynlm.createDefaultConfig()
                for key, value in original_config.items():
                    config.set(key, value)
                
                # Create new brain with corrected configuration
                new_brain = pynlm.createBrain(config)
                new_brain.initialize()
                
                # Replace current brain
                globals()['brain'] = new_brain
                print(f"  Configuration restored and brain recreated")
                
                self.recovery_actions.append({
                    'type': 'configuration_recovery',
                    'timestamp': datetime.datetime.now().isoformat(),
                    'action': 'config_restore'
                })
            except Exception as e:
                print(f"  Failed to restore configuration: {e}")
        else:
            print(f"  No original configuration available for recovery")
    
    def _handle_simulation_error(self, error_entry):
        """Handle simulation errors."""
        print(f"  Attempting simulation recovery...")
        
        context = error_entry.get('context', {})
        error_step = context.get('step', 0)
        
        try:
            # Reset simulation state
            self.brain.reset()
            print(f"  Simulation reset to step 0")
            
            # Continue from beginning
            for step in range(error_step):
                self.brain.step(step)
            
            self.recovery_actions.append({
                'type': 'simulation_recovery',
                'timestamp': datetime.datetime.now().isoformat(),
                'action': 'step_restart',
                'from_step': error_step
            })
        except Exception as e:
            print(f"  Failed to restart simulation: {e}")
    
    def _handle_neuromodulation_error(self, error_entry):
        """Handle neuromodulation errors."""
        print(f"  Attempting neuromodulation recovery...")
        
        try:
            # Reinitialize neuromodulation systems
            if hasattr(self.brain, 'getDopamine'):
                self.brain.getDopamine().reset()
            
            if hasattr(self.brain, 'getCuriosity'):
                self.brain.getCuriosity().reset()
            
            if hasattr(self.brain, 'getNovelty'):
                self.brain.getNovelty().reset()
            
            print(f"  Neuromodulation systems reinitialized")
            
            self.recovery_actions.append({
                'type': 'neuromodulation_recovery',
                'timestamp': datetime.datetime.now().isoformat(),
                'action': 'neuromod_reset'
            })
        except Exception as e:
            print(f"  Failed to reinitialize neuromodulation: {e}")
    
    def _handle_plasticity_error(self, error_entry):
        """Handle plasticity errors."""
        print(f"  Attempting plasticity recovery...")
        
        try:
            # Reset plasticity systems
            if hasattr(self.brain, 'getSTDP'):
                self.brain.getSTDP().reset()
            
            if hasattr(self.brain, 'getHebbian'):
                self.brain.getHebbian().reset()
            
            if hasattr(self.brain, 'getStructuralPlasticity'):
                self.brain.getStructuralPlasticity().reset()
            
            print(f"  Plasticity systems reset")
            
            self.recovery_actions.append({
                'type': 'plasticity_recovery',
                'timestamp': datetime.datetime.now().isoformat(),
                'action': 'plasticity_reset'
            })
        except Exception as e:
            print(f"  Failed to reset plasticity: {e}")
    
    def run_simulation_with_robustness(self, max_steps, error_recovery=True):
        """Run simulation with robust error handling."""
        print(f"Starting robust simulation with {max_steps} steps...")
        
        self.simulation_state = 'running'
        successful_steps = 0
        error_count = 0
        
        for step in range(max_steps):
            try:
                # Run simulation step
                self.brain.step(step)
                successful_steps += 1
                
                # Record performance metrics periodically
                if step % 100 == 0:
                    self._record_performance_metrics(step)
                
            except Exception as e:
                error_count += 1
                error_type = self._classify_error(e)
                context = {
                    'step': step,
                    'error': str(e),
                    'brain_state': self._get_brain_state_summary()
                }
                
                self.log_error(error_type, str(e), context)
                
                if not error_recovery:
                    print(f"Error recovery disabled - simulation stopping")
                    self.simulation_state = 'stopped'
                    break
                
                # Apply recovery
                if error_type in self.error_handlers:
                    print(f"Applying recovery for {error_type}")
                    try:
                        self.error_handlers[error_type]({
                            'type': error_type,
                            'message': str(e),
                            'context': context
                        })
                    except Exception as recovery_error:
                        print(f"Recovery failed: {recovery_error}")
                        self.simulation_state = 'recovered_failed'
                        break
        
        self.simulation_state = 'completed' if step == max_steps - 1 else 'interrupted'
        
        return {
            'successful_steps': successful_steps,
            'error_count': error_count,
            'total_steps': max_steps,
            'simulation_state': self.simulation_state,
            'recovery_actions': self.recovery_actions
        }
    
    def _classify_error(self, error):
        """Classify error type."""
        error_msg = str(error).lower()
        
        if 'memory' in error_msg or 'memory' in error_msg:
            return 'memory_error'
        elif 'config' in error_msg or 'configuration' in error_msg:
            return 'configuration_error'
        elif 'step' in error_msg or 'simulation' in error_msg:
            return 'simulation_error'
        elif 'neuromod' in error_msg or 'dopamine' in error_msg or 'curiosity' in error_msg:
            return 'neuromodulation_error'
        elif 'plasticity' in error_msg or 'synaptic' in error_msg:
            return 'plasticity_error'
        else:
            return 'unknown_error'
    
    def _get_brain_state_summary(self):
        """Get summary of current brain state."""
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'development_stage': self.brain.getDevelopmentalStage().name,
            'total_spikes': self.brain.getTotalSpikeCount()
        }
    
    def _record_performance_metrics(self, step):
        """Record performance metrics for analysis."""
        metrics = {
            'step': step,
            'timestamp': datetime.datetime.now().isoformat(),
            'brain_state': self._get_brain_state_summary(),
            'simulation_progress': step / max_steps if max_steps > 0 else 0,
            'error_count': len(self.error_log)
        }
        
        # In practice, you would use a proper metrics system
        pass
    
    def generate_robustness_report(self):
        """Generate comprehensive robustness report."""
        report = {
            'simulation_summary': {
                'total_errors': len(self.error_log),
                'recovery_actions': len(self.recovery_actions),
                'simulation_state': self.simulation_state,
                'final_brain_state': self._get_brain_state_summary()
            },
            'error_analysis': {
                'errors_by_type': self._get_errors_by_type(),
                'error_timeline': self._get_error_timeline(),
                'recovery_success_rate': self._calculate_recovery_success_rate()
            },
            'recommendations': self._generate_recommendations()
        }
        
        return report
    
    def _get_errors_by_type(self):
        """Get errors grouped by type."""
        errors_by_type = {}
        for error in self.error_log:
            error_type = error['type']
            if error_type not in errors_by_type:
                errors_by_type[error_type] = []
            errors_by_type[error_type].append(error)
        return errors_by_type
    
    def _get_error_timeline(self):
        """Get timeline of errors."""
        return sorted(self.error_log, key=lambda x: x['timestamp'])
    
    def _calculate_recovery_success_rate(self):
        """Calculate recovery success rate."""
        if not self.recovery_actions:
            return 0.0
        
        successful_recoveries = 0
        for action in self.recovery_actions:
            if action.get('status') == 'success':
                successful_recoveries += 1
        
        return successful_recoveries / len(self.recovery_actions)
    
    def _generate_recommendations(self):
        """Generate recommendations based on error analysis."""
        recommendations = []
        
        if len(self.error_log) > 10:
            recommendations.append("High error rate detected - consider reducing simulation complexity")
        
        memory_errors = len([e for e in self.error_log if e['type'] == 'memory_error'])
        if memory_errors > 0:
            recommendations.append("Memory errors detected - optimize memory usage or increase system resources")
        
        if len(self.recovery_actions) > 5:
            recommendations.append("Frequent recovery actions needed - review simulation parameters")
        
        return recommendations
    
    def export_error_log(self, filename='error_log.json'):
        """Export error log to file."""
        import json
        
        log_data = {
            'simulation_state': self.simulation_state,
            'total_errors': len(self.error_log),
            'recovery_actions': self.recovery_actions,
            'errors': self.error_log
        }
        
        with open(filename, 'w') as f:
            json.dump(log_data, f, indent=2)
        
        print(f"Error log exported to {filename}")
        return log_data

# Usage example
robustness_manager = NLMRobustnessManager(brain)

# Set custom error handler for simulation errors
def custom_simulation_error_handler(error_entry):
    print(f"  Custom simulation error handler: {error_entry['message']}")
    # Implement custom recovery logic
    robustness_manager.brain.reset()
    print(f"  Brain reset as custom recovery action")

robustness_manager.set_error_handler('simulation_error', custom_simulation_error_handler)

# Run simulation with robust error handling
print("Starting simulation with robust error handling...")
results = robustness_manager.run_simulation_with_robustness(max_steps=2000, error_recovery=True)

print(f"\nSimulation completed with {results['successful_steps']}/{results['total_steps']} successful steps")
print(f"Errors encountered: {results['error_count']}")
print(f"Recovery actions: {len(results['recovery_actions'])}")

# Generate robustness report
report = robustness_manager.generate_robustness_report()
print(f"\nRobustness Report:")
print(f"Simulation State: {report['simulation_summary']['simulation_state']}")
print(f"Total Errors: {report['simulation_summary']['total_errors']}")
print(f"Recovery Actions: {report['simulation_summary']['recovery_actions']}")

# Export error log
robustness_manager.export_error_log('simulation_error_log.json')

print(f"\nSuccessfully completed simulation with robust error handling")
```

## 16. Summary of Advanced Features

This comprehensive documentation covers the advanced features available in NLM for power users. The advanced features include:

### 1. Direct Component Access
- Access to internal brain components for customization
- Custom learning rules and plasticity mechanisms
- Direct neuron and synapse manipulation

### 2. Advanced Simulation Control
- Fine-grained simulation with variable timesteps
- Parallel and batch simulation execution
- Adaptive simulation control based on neural activity

### 3. Memory and Learning Features
- Custom memory systems with advanced features
- Episodic memory analysis and pattern detection
- Custom neuromodulation systems with temporal integration

### 4. Development and Learning Features
- Custom developmental schedules and critical periods
- Advanced learning analytics and performance tracking
- Experimental protocol management

### 5. Batch and Parallel Processing
- Large-scale parameter sweeps
- Parallel simulation execution
- Optimization for high-performance computing

### 6. Advanced Debugging and Monitoring
- Simulation debugging tools
- Performance profiling and optimization
- Real-time visualization and monitoring

### 7. Integration with External Tools
- Jupyter notebook integration
- Configuration generation tools
- Data export for external analysis

### 8. Performance Optimization
- Large-scale simulation management
- Memory and CPU optimization
- Performance profiling and analysis

### 9. Advanced API Customization
- Custom Python extensions
- Experimental condition management
- Advanced error handling and recovery

### 10. Robustness and Error Handling
- Comprehensive error classification and handling
- Automatic recovery mechanisms
- Simulation state management and rollback

These advanced features provide power users with the tools needed to conduct sophisticated research, optimize performance, and customize NLM for specific applications and experimental requirements.

For beginners, start with the basic API and gradually explore these advanced features as you become more familiar with the system. The advanced features are designed to provide maximum flexibility while maintaining the simplicity of the basic API.

The advanced features enable users to:
- Conduct large-scale parameter studies
- Implement custom learning algorithms
- Analyze neural activity patterns
- Optimize simulation performance
- Create reproducible research workflows
- Visualize and debug complex simulations
- Integrate with external analysis tools
- Scale simulations to large neural populations

These capabilities make NLM a powerful platform for neural network research and experimentation at all levels of complexity.
