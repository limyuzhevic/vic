# NLM Enhanced Python Bindings - Visualization and Analysis Module

## Overview

This module provides comprehensive visualization and analysis tools for NLM simulations, enabling users to monitor brain activity, analyze neural patterns, and create visualizations for research and debugging purposes.

## Module Structure

The visualization and analysis module includes:

1. **Neural Visualization Tools**
2. **Activity Monitoring Dashboards**
3. **Connection Inspection Tools**
4. **Performance Monitoring Utilities**
5. **Data Analysis Tools**
6. **Export and Integration Tools**

## 1. Neural Visualization Tools

### Neuron Activity Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def visualize_neuron_activity(brain, step_range=None, neuron_indices=None):
    """Visualize neural activity over time or across neurons."""
    
    if step_range is None:
        step_range = range(max(0, brain.get_total_step_count() - 1000), brain.get_total_step_count())
    
    fig, axes = plt.subplots(2, 1, figsize=(12, 8))
    
    # Plot 1: Firing rate over time
    firing_rates = []
    for step in step_range:
        brain.step(step)
        firing_rate = brain.get_firing_neuron_count() / max(brain.get_neuron_count(), 1)
        firing_rates.append(firing_rate)
    
    axes[0].plot(step_range, firing_rates)
    axes[0].set_xlabel('Step')
    axes[0].set_ylabel('Firing Rate')
    axes[0].set_title('Neural Activity Over Time')
    axes[0].grid(True, alpha=0.3)
    
    # Plot 2: Neuron activity distribution
    if neuron_indices is None:
        neuron_indices = list(range(min(100, brain.get_neuron_count())))
    
    # Get spike counts for selected neurons (simplified visualization)
    spike_counts = []
    for neuron_idx in neuron_indices:
        spike_count = brain.get_neuron_spike_count(neuron_idx) if hasattr(brain, 'get_neuron_spike_count') else 0
        spike_counts.append(spike_count)
    
    axes[1].bar(range(len(neuron_indices)), spike_counts)
    axes[1].set_xlabel('Neuron Index')
    axes[1].set_ylabel('Spike Count')
    axes[1].set_title('Spike Count Distribution')
    axes[1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig, axes

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation for some steps
for i in range(1000):
    brain.step(i)

fig, axes = visualize_neuron_activity(brain, step_range=range(900, 1000))
plt.show()
```

### Synaptic Connection Visualization

```python
import pynlm
import networkx as nx
import matplotlib.pyplot as plt

def visualize_synapse_network(brain, region_id=None, max_neurons=100):
    """Create a network visualization of synaptic connections."""
    
    try:
        # Get region or use first available region
        if region_id is None:
            region = brain.get_region(brain.get_region_ids()[0])
        else:
            region = brain.get_region(region_id)
            
        if not region:
            print("No region found for visualization")
            return None
            
        # Create network graph
        G = nx.Graph()
        
        # Add neurons as nodes
        neurons = region.get_all_neurons()
        neuron_ids = [neuron.get_id().get_value() for neuron in neurons[:max_neurons]]
        
        for neuron_id in neuron_ids:
            G.add_node(neuron_id)
            
        # Add synapses as edges
        for neuron in neurons[:max_neurons]:
            connections = neuron.get_connections()
            for synapse in connections:
                target_neuron = synapse.get_target_neuron()
                if target_neuron:
                    target_id = target_neuron.get_id().get_value()
                    if target_id in neuron_ids:
                        # Set edge weight based on synapse strength
                        G.add_edge(neuron_id, target_id, weight=synapse.get_weight())
        
        # Create visualization
        fig, ax = plt.subplots(figsize=(12, 10))
        
        # Use spring layout for better visualization
        pos = nx.spring_layout(G, k=1, iterations=50)
        
        # Draw nodes
        nx.draw_networkx_nodes(G, pos, node_color='lightblue', 
                             node_size=100, ax=ax)
        
        # Draw edges with width proportional to weight
        edge_weights = [G[u][v]['weight'] for u, v in G.edges()]
        max_weight = max(edge_weights) if edge_weights else 1
        
        nx.draw_networkx_edges(G, pos, width=[w/max_weight*3 for w in edge_weights],
                             alpha=0.6, ax=ax)
        
        # Draw labels for some nodes
        labels = {node_id: str(node_id) for node_id in list(G.nodes())[:20]}
        nx.draw_networkx_labels(G, pos, labels, ax=ax)
        
        ax.set_title(f"Synaptic Network Visualization (Region {region_id})")
        ax.axis('off')
        
        plt.tight_layout()
        return fig, G, pos
        
    except Exception as e:
        print(f"Error in network visualization: {e}")
        return None

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
fig, graph, positions = visualize_synapse_network(brain)
if fig:
    plt.show()
```

### Activity Heatmap Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def create_activity_heatmap(brain, time_window=100, neuron_window=50):
    """Create a heatmap of neural activity over time and neurons."""
    
    # Create activity matrix
    activity_matrix = np.zeros((time_window, neuron_window))
    
    # Record activity
    current_time = max(0, brain.get_total_step_count() - time_window)
    
    for t in range(time_window):
        step = current_time + t
        brain.step(step)
        
        # Get active neurons
        active_neurons = brain.get_active_neuron_ids()
        
        # Fill matrix
        for neuron_id in active_neurons[:neuron_window]:
            neuron_index = neuron_id.get_value() % neuron_window
            activity_matrix[t, neuron_index] += 1
    
    # Create heatmap
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Use imshow for heatmap
    im = ax.imshow(activity_matrix.T, aspect='auto', cmap='hot', interpolation='nearest')
    
    ax.set_xlabel('Neuron Index')
    ax.set_ylabel('Time Step')
    ax.set_title('Neural Activity Heatmap')
    
    # Add colorbar
    plt.colorbar(im, ax=ax, label='Activity Count')
    
    # Set ticks
    ax.set_xticks(np.arange(0, neuron_window, 10))
    ax.set_yticks(np.arange(0, time_window, 20))
    
    plt.tight_layout()
    return fig, ax, im

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation
for i in range(500):
    brain.step(i)

fig, ax, im = create_activity_heatmap(brain, time_window=200, neuron_window=100)
plt.show()
```

## 2. Activity Monitoring Dashboards

### Real-time Dashboard

```python
import pynlm
import matplotlib.pyplot as plt
import time
from matplotlib.animation import FuncAnimation

class NeuralActivityDashboard:
    """Real-time dashboard for monitoring neural activity."""
    
    def __init__(self, brain, update_interval=1000):
        self.brain = brain
        self.update_interval = update_interval
        self.step_count = 0
        self.history = {
            'firing_rate': [],
            'spike_count': [],
            'neuron_activity': [],
            'curiosity': [],
            'novelty': [],
            'prediction_error': [],
            'development_stage': []
        }
        
        self.fig, self.axes = plt.subplots(3, 3, figsize=(15, 10))
        self.fig.suptitle('NLM Neural Activity Dashboard', fontsize=16)
        
        # Initialize plots
        self.setup_plots()
    
    def setup_plots(self):
        """Setup all dashboard plots."""
        plot_configs = [
            ('Firing Rate', 'step', 'Firing Rate', 'red'),
            ('Total Spikes', 'step', 'Spikes', 'blue'),
            ('Curiosity Level', 'step', 'Curiosity', 'green'),
            ('Novelty Level', 'step', 'Novelty', 'orange'),
            ('Prediction Error', 'step', 'Error', 'purple'),
            ('Development Stage', 'step', 'Stage', 'brown'),
            ('Neuron Activity', 'step', 'Activity', 'cyan'),
            ('Learning Rate', 'step', 'Rate', 'magenta'),
            ('Energy Level', 'step', 'Energy', 'yellow')
        ]
        
        for idx, (title, xlabel, ylabel, color) in enumerate(plot_configs):
            row, col = idx // 3, idx % 3
            ax = self.axes[row, col]
            
            self.plot_lines = ax.plot([], [], color=color, label=title)
            ax.set_title(title)
            ax.set_xlabel(xlabel)
            ax.set_ylabel(ylabel)
            ax.grid(True, alpha=0.3)
            ax.legend()
    
    def update_dashboard(self, frame):
        """Update dashboard with new data."""
        # Run simulation step
        self.brain.step(self.step_count)
        self.step_count += 1
        
        # Collect data
        current_step = self.step_count - 1
        firing_rate = self.brain.firing_count / max(self.brain.neuron_count, 1)
        
        # Update history
        self.history['firing_rate'].append(firing_rate)
        self.history['spike_count'].append(self.brain.spike_count)
        self.history['neuron_activity'].append(self.brain.firing_count)
        
        # Get neuromodulation levels
        if hasattr(self.brain, 'get_curiosity_level'):
            self.history['curiosity'].append(self.brain.get_curiosity_level())
        else:
            self.history['curiosity'].append(0.0)
            
        if hasattr(self.brain, 'get_novelty_level'):
            self.history['novelty'].append(self.brain.get_novelty_level())
        else:
            self.history['novelty'].append(0.0)
            
        if hasattr(self.brain, 'get_prediction_error'):
            self.history['prediction_error'].append(self.brain.get_prediction_error())
        else:
            self.history['prediction_error'].append(0.0)
            
        if hasattr(self.brain, 'get_developmental_stage'):
            self.history['development_stage'].append(self.brain.get_developmental_stage())
        else:
            self.history['development_stage'].append(0)
        
        # Update each plot
        max_history = max(len(h) for h in self.history.values())
        
        for idx, (plot_type, data_key) in enumerate([
            ('firing_rate', 'firing_rate'),
            ('spike_count', 'spike_count'),
            ('curiosity', 'curiosity'),
            ('novelty', 'novelty'),
            ('prediction_error', 'prediction_error'),
            ('development_stage', 'development_stage'),
            ('neuron_activity', 'neuron_activity'),
            ('learning_rate', 'learning_rate'),
            ('energy_level', 'energy_level')
        ]):
            row, col = idx // 3, idx % 3
            ax = self.axes[row, col]
            
            x_data = list(range(len(self.history[data_key])))
            y_data = self.history[data_key]
            
            # Update plot data
            self.plot_lines[row*3 + col].set_data(x_data, y_data)
            
            # Adjust limits
            if x_data:
                ax.set_xlim(0, max_history - 1)
                
                if y_data:
                    max_val = max(y_data)
                    if max_val > 0:
                        ax.set_ylim(0, max_val * 1.1)
                    else:
                        ax.set_ylim(0, 1)
    
    def start_monitoring(self, interval=1000):
        """Start real-time monitoring."""
        anim = FuncAnimation(self.fig, self.update_dashboard, 
                           interval=interval, blit=False)
        plt.show()
        return anim

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Create and start dashboard
dashboard = NeuralActivityDashboard(brain, update_interval=100)
# dashboard.start_monitoring(interval=100)  # Uncomment to run
```

### Statistics Logger

```python
import pynlm
import json
import time
from datetime import datetime

class NeuralStatisticsLogger:
    """Log detailed statistics about neural simulation."""
    
    def __init__(self, brain, log_file="nlm_simulation_log.json"):
        self.brain = brain
        self.log_file = log_file
        self.start_time = time.time()
        self.log_data = {
            'simulation_info': {
                'start_time': datetime.now().isoformat(),
                'config_summary': brain.get_config().summary(),
                'neuron_count': brain.neuron_count,
                'synapse_count': brain.synapse_count,
                'region_count': brain.region_count
            },
            'simulation_steps': [],
            'system_states': [],
            'performance_metrics': {}
        }
    
    def log_step(self, step_number, current_time):
        """Log data for a single simulation step."""
        step_data = {
            'step': step_number,
            'timestamp': current_time,
            'firing_count': self.brain.firing_count,
            'spike_count': self.brain.spike_count,
            'active_neuron_count': self.brain.active_neuron_count,
            'average_firing_rate': self.brain.average_firing_rate,
            'excitation_inhibition_ratio': self.brain.excitation_inhibition_ratio,
            'development_stage': self.brain.get_developmental_stage() if hasattr(self.brain, 'get_developmental_stage') else None,
            'curiosity_level': self.brain.get_curiosity_level() if hasattr(self.brain, 'get_curiosity_level') else None,
            'novelty_level': self.brain.get_novelty_level() if hasattr(self.brain, 'get_novelty_level') else None,
            'prediction_error': self.brain.get_prediction_error() if hasattr(self.brain, 'get_prediction_error') else None,
            'neuromodulation_level': self.brain.get_neuromodulation_level() if hasattr(self.brain, 'get_neuromodulation_level') else None,
            'plastic': {
                'stdp_active': self.brain.get_stdp().get_plasticity_flags() if self.brain.get_stdp() else None,
                'hebbian_active': self.brain.get_hebbian().get_plasticity_flags() if self.brain.get_hebbian() else None,
                'structural_active': self.brain.get_structural_plasticity().get_plasticity_flags() if self.brain.get_structural_plasticity() else None
            }
        }
        
        self.log_data['simulation_steps'].append(step_data)
    
    def finalize(self):
        """Finalize logging and save to file."""
        end_time = time.time()
        
        self.log_data['simulation_info']['end_time'] = datetime.now().isoformat()
        self.log_data['simulation_info']['total_duration'] = end_time - self.start_time
        self.log_data['simulation_info']['total_steps'] = len(self.log_data['simulation_steps'])
        
        # Calculate performance metrics
        if self.log_data['simulation_steps']:
            steps = self.log_data['simulation_steps']
            
            # Average metrics
            self.log_data['performance_metrics']['average_firing_rate'] = np.mean([s['average_firing_rate'] for s in steps])
            self.log_data['performance_metrics']['average_spike_count'] = np.mean([s['spike_count'] for s in steps])
            self.log_data['performance_metrics']['max_firing_rate'] = np.max([s['average_firing_rate'] for s in steps])
            self.log_data['performance_metrics']['min_firing_rate'] = np.min([s['average_firing_rate'] for s in steps])
        
        # Save to file
        with open(self.log_file, 'w') as f:
            json.dump(self.log_data, f, indent=2, default=str)
        
        return self.log_file

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

logger = NeuralStatisticsLogger(brain)

# Run simulation with logging
for i in range(1000):
    brain.step(i)
    logger.log_step(i, i * 0.001)

log_file = logger.finalize()
print(f"Simulation logged to: {log_file}")
```

## 3. Connection Inspection Tools

### Synapse Inspector

```python
import pynlm
import networkx as nx
import matplotlib.pyplot as plt

def inspect_synapse_network(brain, min_weight=0.0, max_synapses=1000):
    """Inspect and analyze synaptic network properties."""
    
    inspector = {
        'total_synapses': 0,
        'synapse_types': {},
        'weight_stats': {},
        'connection_patterns': {},
        'plasticity_status': {},
        'network_metrics': {}
    }
    
    try:
        # Get all regions
        regions = brain.get_regions()
        
        for region in regions[:max_synapses//10]:  # Limit analysis
            region_id = region.get_id()
            inspector['total_synapses'] += region.get_synapse_count()
            
            # Analyze synapses
            synapses = region.get_synapses()
            
            for synapse in synapses[:max_synapses]:
                # Count synapse types
                synapse_type = synapse.get_type()
                inspector['synapse_types'][str(synapse_type)] = \
                    inspector['synapse_types'].get(str(synapse_type), 0) + 1
                
                # Collect weight statistics
                weight = synapse.get_weight()
                if 'weights' not in inspector['weight_stats']:
                    inspector['weight_stats'] = {'min': weight, 'max': weight, 'sum': weight, 'count': 1}
                else:
                    inspector['weight_stats']['min'] = min(inspector['weight_stats']['min'], weight)
                    inspector['weight_stats']['max'] = max(inspector['weight_stats']['max'], weight)
                    inspector['weight_stats']['sum'] += weight
                    inspector['weight_stats']['count'] += 1
                
                # Check plasticity status
                plasticity_flags = synapse.get_plasticity_flags()
                if region_id not in inspector['plasticity_status']:
                    inspector['plasticity_status'][str(region_id)] = {}
                
                if 'stdp' in str(plasticity_flags):
                    inspector['plasticity_status'][str(region_id)]['stdp'] = \
                        inspector['plasticity_status'][str(region_id)].get('stdp', 0) + 1
                if 'hebbian' in str(plasticity_flags):
                    inspector['plasticity_status'][str(region_id)]['hebbian'] = \
                        inspector['plasticity_status'][str(region_id)].get('hebbian', 0) + 1
                if 'structural' in str(plasticity_flags):
                    inspector['plasticity_status'][str(region_id)]['structural'] = \
                        inspector['plasticity_status'][str(region_id)].get('structural', 0) + 1
        
        # Calculate network metrics
        total_synapses = inspector['total_synapses']
        if total_synapses > 0:
            inspector['network_metrics']['synapse_density'] = total_synapses / (brain.neuron_count * (brain.neuron_count - 1) / 2)
            inspector['network_metrics']['avg_synapse_weight'] = inspector['weight_stats']['sum'] / inspector['weight_stats']['count']
            inspector['network_metrics']['weight_coefficient_of_variation'] = \
                (inspector['weight_stats']['max'] - inspector['weight_stats']['min']) / inspector['weight_stats']['avg_synapse_weight'] if inspector['weight_stats']['avg_synapse_weight'] > 0 else 0
        
        return inspector
        
    except Exception as e:
        print(f"Error during synapse inspection: {e}")
        return None

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

inspector = inspect_synapse_network(brain)
if inspector:
    print("=== Synapse Network Inspection ===")
    print(f"Total synapses: {inspector['total_synapses']}")
    print(f"Synapse types: {inspector['synapse_types']}")
    print(f"Weight stats: {inspector['weight_stats']}")
    print(f"Plasticity status: {inspector['plasticity_status']}")
    print(f"Network metrics: {inspector['network_metrics']}")
```

### Neuron Activity Inspector

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def inspect_neuron_activity(brain, neuron_ids=None, time_window=100):
    """Inspect and analyze neuron activity patterns."""
    
    inspector = {
        'most_active_neurons': [],
        'firing_patterns': {},
        'activity_clusters': {},
        'activity_statistics': {}
    }
    
    try:
        # Get current spike system for analysis
        spike_system = brain.get_spike_system()
        if spike_system:
            # Get most active neurons
            most_active = spike_system.get_most_active_neuron_ids(time_window)
            inspector['most_active_neurons'] = [(neuron_id.get_value(), spike_count) 
                                               for neuron_id, spike_count in most_active]
        
        # Analyze firing patterns for selected neurons
        if neuron_ids is None:
            # Get a sample of neurons for analysis
            region = brain.get_region(brain.get_region_ids()[0])
            neurons = region.get_all_neurons() if region else []
            neuron_ids = [neuron.get_id().get_value() for neuron in neurons[:50]]
        
        # Collect activity data
        for neuron_id in neuron_ids:
            # Get activity history (simplified)
            activity_history = []
            start_time = max(0, brain.get_total_step_count() - time_window)
            
            for step in range(start_time, brain.get_total_step_count()):
                brain.step(step)
                # Check if neuron is active
                is_active = neuron_id in brain.get_active_neuron_ids()
                activity_history.append(1 if is_active else 0)
            
            inspector['firing_patterns'][str(neuron_id)] = activity_history
            
            # Calculate statistics
            if activity_history:
                inspector['activity_statistics'][str(neuron_id)] = {
                    'mean_activity': np.mean(activity_history),
                    'std_activity': np.std(activity_history),
                    'total_active_steps': sum(activity_history),
                    'activity_ratio': sum(activity_history) / len(activity_history),
                    'burst_count': sum(1 for i in range(1, len(activity_history)) 
                                     if activity_history[i] == 1 and activity_history[i-1] == 0),
                    'max_consecutive_active': max(
                        sum(1 for j in range(i, len(activity_history)) 
                            if activity_history[j] == 1) 
                        for i in range(len(activity_history))
                    ) if any(activity_history) else 0
                }
        
        # Identify activity clusters
        inspector['activity_clusters'] = find_activity_clusters(inspector['firing_patterns'])
        
        return inspector
        
    except Exception as e:
        print(f"Error during neuron activity inspection: {e}")
        return None

def find_activity_clusters(activity_patterns):
    """Find clusters of neurons with similar activity patterns."""
    
    if not activity_patterns:
        return {}
    
    clusters = {}
    pattern_list = list(activity_patterns.items())
    
    for i, (neuron_id, pattern1) in enumerate(pattern_list):
        similar_neurons = []
        
        for j, (neuron_id2, pattern2) in enumerate(pattern_list[i+1:], i+1):
            # Calculate pattern similarity (normalized cross-correlation)
            if len(pattern1) == len(pattern2) and len(pattern1) > 0:
                correlation = np.corrcoef(pattern1, pattern2)[0, 1]
                if not np.isnan(correlation) and abs(correlation) > 0.7:
                    similar_neurons.append((neuron_id2, correlation))
        
        if similar_neurons:
            clusters[neuron_id] = similar_neurons
    
    return clusters

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation
for i in range(1000):
    brain.step(i)

inspector = inspect_neuron_activity(brain, time_window=200)
if inspector:
    print("=== Neuron Activity Inspection ===")
    print(f"Most active neurons: {inspector['most_active_neurons'][:10]}")
    print(f"Activity statistics for first 5 neurons:")
    for neuron_id, stats in list(inspector['activity_statistics'].items())[:5]:
        print(f"  {neuron_id}: {stats}")
```

## 4. Performance Monitoring Utilities

### Performance Profiler

```python
import pynlm
import time
import cProfile
import pstats
from io import StringIO

class NeuralPerformanceProfiler:
    """Profile and analyze NLM simulation performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = {
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'synapse_operations': [],
            'neuron_operations': []
        }
        self.step_count = 0
    
    def profile_step(self, step_number, current_time=0.0):
        """Profile a single simulation step."""
        start_time = time.time()
        
        # Profile the step
        try:
            self.brain.step(step_number, current_time)
            
            end_time = time.time()
            step_time = end_time - start_time
            
            # Record performance data
            self.performance_data['step_times'].append(step_time)
            self.performance_data['synapse_operations'].append(self.brain.get_total_synapse_count())
            self.performance_data['neuron_operations'].append(self.brain.get_total_neuron_count())
            
            self.step_count += 1
            
            return step_time
            
        except Exception as e:
            print(f"Error profiling step {step_number}: {e}")
            return None
    
    def run_profiling(self, num_steps, profile_internal=False):
        """Run simulation with profiling."""
        if profile_internal:
            pr = cProfile.Profile()
            pr.enable()
            
            for i in range(num_steps):
                self.profile_step(i, i * 0.001)
            
            pr.disable()
            
            # Generate profile statistics
            s = StringIO()
            ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
            ps.print_stats()
            
            return s.getvalue()
        else:
            # Simple timing without internal profiling
            total_time = 0
            for i in range(num_steps):
                step_time = self.profile_step(i, i * 0.001)
                if step_time:
                    total_time += step_time
            
            return total_time
    
    def analyze_performance(self):
        """Analyze and report performance metrics."""
        if not self.performance_data['step_times']:
            return {}
        
        analysis = {
            'total_steps': len(self.performance_data['step_times']),
            'total_time': sum(self.performance_data['step_times']),
            'average_step_time': np.mean(self.performance_data['step_times']),
            'median_step_time': np.median(self.performance_data['step_times']),
            'min_step_time': np.min(self.performance_data['step_times']),
            'max_step_time': np.max(self.performance_data['step_times']),
            'std_step_time': np.std(self.performance_data['step_times']),
            'steps_per_second': len(self.performance_data['step_times']) / sum(self.performance_data['step_times']) if sum(self.performance_data['step_times']) > 0 else 0,
            'memory_usage_per_step': np.mean(self.performance_data['memory_usage']) if self.performance_data['memory_usage'] else 0,
            'neuron_operations_per_step': np.mean(self.performance_data['neuron_operations']),
            'synapse_operations_per_step': np.mean(self.performance_data['synapse_operations'])
        }
        
        return analysis

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

profiler = NeuralPerformanceProfiler(brain)

# Run with profiling
total_time = profiler.run_profiling(1000, profile_internal=True)
print(f"Profiling results:\n{total_time}")

# Analyze performance
analysis = profiler.analyze_performance()
print("\nPerformance Analysis:")
for key, value in analysis.items():
    print(f"  {key}: {value:.6f}")
```

### Memory Usage Monitor

```python
import pynlm
import psutil
import time
import matplotlib.pyplot as plt

def monitor_memory_usage(brain, duration_seconds=60, interval=1.0):
    """Monitor memory usage during simulation."""
    
    memory_data = {
        'timestamps': [],
        'memory_usage_mb': [],
        'neuron_count': [],
        'synapse_count': [],
        'cpu_percent': []
    }
    
    start_time = time.time()
    
    try:
        while time.time() - start_time < duration_seconds:
            # Record current memory usage
            process = psutil.Process()
            memory_mb = process.memory_info().rss / 1024 / 1024
            
            # Get brain statistics
            memory_data['timestamps'].append(time.time() - start_time)
            memory_data['memory_usage_mb'].append(memory_mb)
            memory_data['neuron_count'].append(brain.neuron_count)
            memory_data['synapse_count'].append(brain.synapse_count)
            memory_data['cpu_percent'].append(process.cpu_percent())
            
            # Run simulation step
            brain.step(len(memory_data['timestamps']) - 1)
            
            # Wait for next measurement
            time.sleep(interval)
        
        return memory_data
        
    except Exception as e:
        print(f"Error during memory monitoring: {e}")
        return memory_data

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Monitor memory usage
memory_data = monitor_memory_usage(brain, duration_seconds=30, interval=0.5)

# Visualize results
if memory_data['timestamps']:
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Memory usage over time
    axes[0, 0].plot(memory_data['timestamps'], memory_data['memory_usage_mb'])
    axes[0, 0].set_xlabel('Time (s)')
    axes[0, 0].set_ylabel('Memory Usage (MB)')
    axes[0, 0].set_title('Memory Usage Over Time')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Neuron count over time
    axes[0, 1].plot(memory_data['timestamps'], memory_data['neuron_count'])
    axes[0, 1].set_xlabel('Time (s)')
    axes[0, 1].set_ylabel('Neuron Count')
    axes[0, 1].set_title('Neuron Count Over Time')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Synapse count over time
    axes[1, 0].plot(memory_data['timestamps'], memory_data['synapse_count'])
    axes[1, 0].set_xlabel('Time (s)')
    axes[1, 0].set_ylabel('Synapse Count')
    axes[1, 0].set_title('Synapse Count Over Time')
    axes[1, 0].grid(True, alpha=0.3)
    
    # CPU usage over time
    axes[1, 1].plot(memory_data['timestamps'], memory_data['cpu_percent'])
    axes[1, 1].set_xlabel('Time (s)')
    axes[1, 1].set_ylabel('CPU Usage (%)')
    axes[1, 1].set_title('CPU Usage Over Time')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
```

## 5. Data Analysis Tools

### Statistical Analysis

```python
import pynlm
import numpy as np
import scipy.stats as stats
import matplotlib.pyplot as plt
from scipy import signal

class NeuralDataAnalyzer:
    """Analyze neural simulation data with statistical methods."""
    
    def __init__(self, brain):
        self.brain = brain
        self.analysis_results = {}
    
    def analyze_firing_statistics(self, step_range):
        """Analyze firing statistics over time."""
        
        firing_rates = []
        spike_counts = []
        active_neuron_counts = []
        
        for step in step_range:
            self.brain.step(step)
            firing_rate = self.brain.firing_count / max(self.brain.neuron_count, 1)
            firing_rates.append(firing_rate)
            spike_counts.append(self.brain.spike_count)
            active_neuron_counts.append(self.brain.active_neuron_count)
        
        analysis = {
            'firing_rate_stats': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'median': np.median(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates),
                'skewness': stats.skew(firing_rates) if len(firing_rates) > 2 else 0,
                'kurtosis': stats.kurtosis(firing_rates) if len(firing_rates) > 2 else 0
            },
            'spike_count_stats': {
                'mean': np.mean(spike_counts),
                'std': np.std(spike_counts),
                'median': np.median(spike_counts),
                'min': np.min(spike_counts),
                'max': np.max(spike_counts)
            },
            'active_neuron_stats': {
                'mean': np.mean(active_neuron_counts),
                'std': np.std(active_neuron_counts),
                'median': np.median(active_neuron_counts),
                'min': np.min(active_neuron_counts),
                'max': np.max(active_neuron_counts)
            },
            'correlation_analysis': self._calculate_correlations(firing_rates, spike_counts, active_neuron_counts),
            'rhythmicity_analysis': self._detect_rhythmicity(firing_rates),
            'trend_analysis': self._detect_trends(firing_rates)
        }
        
        self.analysis_results['firing_statistics'] = analysis
        return analysis
    
    def _calculate_correlations(self, *series):
        """Calculate correlations between different time series."""
        
        correlations = {}
        series_names = ['firing_rate', 'spike_count', 'active_neuron_count']
        
        for i, (name1, series1) in enumerate(zip(series_names, series)):
            for j, (name2, series2) in enumerate(zip(series_names, series)):
                if i < j:
                    if len(series1) == len(series2) and len(series1) > 2:
                        corr, p_value = stats.pearsonr(series1, series2)
                        correlations[f'{name1}_vs_{name2}'] = {
                            'correlation': corr,
                            'p_value': p_value,
                            'significant': p_value < 0.05
                        }
        
        return correlations
    
    def _detect_rhythmicity(self, time_series):
        """Detect rhythmic patterns in time series data."""
        
        if len(time_series) < 10:
            return {'rhythmicity': 0, 'dominant_frequency': 0, 'confidence': 0}
        
        # Calculate power spectral density
        sampling_rate = 1.0  # Assuming steps are uniform
        freqs, psd = signal.periodogram(time_series, sampling_rate)
        
        # Find dominant frequency
        dominant_idx = np.argmax(psd[1:]) + 1  # Skip DC component
        dominant_freq = freqs[dominant_idx]
        dominant_power = psd[dominant_idx]
        
        # Calculate rhythmicity index (normalized power in rhythmic bands)
        rhythmic_bands = (freqs > 0.1) & (freqs < 10.0)  # 0.1-10 Hz band
        rhythmic_power = np.sum(psd[rhythmic_bands])
        total_power = np.sum(psd[1:])  # Exclude DC
        
        rhythmicity = rhythmic_power / total_power if total_power > 0 else 0
        
        return {
            'rhythmicity': rhythmicity,
            'dominant_frequency': dominant_freq,
            'dominant_power': dominant_power,
            'confidence': min(rhythmicity * 2, 1.0),  # Higher rhythmicity = higher confidence
            'power_spectrum': (freqs.tolist(), psd.tolist())
        }
    
    def _detect_trends(self, time_series):
        """Detect trends in time series data."""
        
        if len(time_series) < 3:
            return {'trend': 'insufficient_data', 'slope': 0, 'r_squared': 0}
        
        # Calculate linear regression trend
        x = np.arange(len(time_series))
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, time_series)
        
        # Determine trend direction
        if abs(slope) < 0.001:
            trend = 'stable'
        elif slope > 0:
            trend = 'increasing'
        else:
            trend = 'decreasing'
        
        return {
            'trend': trend,
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value,
            'significant': p_value < 0.05,
            'linear_regression': (x.tolist(), (slope * x + intercept).tolist())
        }
    
    def analyze_neuromodulation_dynamics(self):
        """Analyze neuromodulation system dynamics."""
        
        if hasattr(self.brain, 'get_curiosity_level'):
            # Collect neuromodulation data
            curiosity_data = []
            novelty_data = []
            prediction_error_data = []
            neuromodulation_data = []
            
            for i in range(100):  # Collect 100 data points
                self.brain.step(i)
                curiosity_data.append(self.brain.get_curiosity_level())
                novelty_data.append(self.brain.get_novelty_level() if hasattr(self.brain, 'get_novelty_level') else 0.0)
                prediction_error_data.append(self.brain.get_prediction_error() if hasattr(self.brain, 'get_prediction_error') else 0.0)
                neuromodulation_data.append(self.brain.get_neuromodulation_level() if hasattr(self.brain, 'get_neuromodulation_level') else 0.0)
            
            analysis = {
                'curiosity_dynamics': self._analyze_dynamics('curiosity', curiosity_data),
                'novelty_dynamics': self._analyze_dynamics('novelty', novelty_data),
                'prediction_error_dynamics': self._analyze_dynamics('prediction_error', prediction_error_data),
                'neuromodulation_dynamics': self._analyze_dynamics('neuromodulation', neuromodulation_data),
                'cross_correlations': self._calculate_modulation_correlations(
                    curiosity_data, novelty_data, prediction_error_data, neuromodulation_data
                )
            }
            
            self.analysis_results['neuromodulation'] = analysis
            return analysis
        
        return None
    
    def _analyze_dynamics(self, name, time_series):
        """Analyze dynamics of a time series."""
        
        if len(time_series) < 5:
            return None
        
        # Calculate various dynamic measures
        return {
            'mean': np.mean(time_series),
            'std': np.std(time_series),
            'cv': np.std(time_series) / np.mean(time_series) if np.mean(time_series) > 0 else 0,
            'skew': stats.skew(time_series) if len(time_series) > 2 else 0,
            'kurtosis': stats.kurtosis(time_series) if len(time_series) > 2 else 0,
            'trend_slope': self._calculate_linear_trend(time_series),
            'autocorrelation': self._calculate_autocorrelation(time_series),
            'power_spectrum': self._calculate_power_spectrum(time_series)
        }
    
    def _calculate_modulation_correlations(self, *series):
        """Calculate correlations between neuromodulation signals."""
        
        correlations = {}
        names = ['curiosity', 'novelty', 'prediction_error', 'neuromodulation']
        
        for i, (name1, series1) in enumerate(zip(names, series)):
            for j, (name2, series2) in enumerate(zip(names, series)):
                if i < j and len(series1) == len(series2) and len(series1) > 2:
                    corr, p_value = stats.pearsonr(series1, series2)
                    correlations[f'{name1}_vs_{name2}'] = {
                        'correlation': corr,
                        'p_value': p_value,
                        'significant': p_value < 0.05
                    }
        
        return correlations
    
    def _calculate_linear_trend(self, time_series):
        """Calculate linear trend slope."""
        
        if len(time_series) < 2:
            return 0
        
        x = np.arange(len(time_series))
        slope, _, _, _, _ = stats.linregress(x, time_series)
        return slope
    
    def _calculate_autocorrelation(self, time_series, max_lag=10):
        """Calculate autocorrelation function."""
        
        if len(time_series) < max_lag + 1:
            return {}
        
        autocorr = []
        for lag in range(max_lag + 1):
            if lag == 0:
                autocorr.append(1.0)
            else:
                corr = np.corrcoef(time_series[:-lag], time_series[lag:])[0, 1]
                autocorr.append(corr if not np.isnan(corr) else 0)
        
        return {
            'autocorrelation': autocorr,
            'lags': list(range(max_lag + 1))
        }
    
    def _calculate_power_spectrum(self, time_series):
        """Calculate power spectrum using FFT."""
        
        try:
            from scipy.fft import fft, fftfreq
            
            n = len(time_series)
            if n < 2:
                return {'frequencies': [], 'power': []}
            
            # Calculate FFT
            yf = fft(time_series)
            xf = fftfreq(n, 1.0)[:n//2]  # Only positive frequencies
            
            # Calculate power spectrum
            power = np.abs(yf[:n//2]) ** 2
            
            return {
                'frequencies': xf.tolist(),
                'power': power.tolist(),
                'dominant_frequency': xf[np.argmax(power)]
            }
        except ImportError:
            return {'error': 'FFT not available'}
    
    def generate_comprehensive_report(self):
        """Generate comprehensive analysis report."""
        
        report = {
            'brain_info': {
                'neuron_count': self.brain.neuron_count,
                'synapse_count': self.brain.synapse_count,
                'region_count': self.brain.region_count,
                'total_step_count': self.brain.get_total_step_count()
            },
            'analysis_results': self.analysis_results,
            'timestamp': time.time(),
            'analysis_summary': self._generate_summary()
        }
        
        return report
    
    def _generate_summary(self):
        """Generate summary of analysis results."""
        
        summary = []
        
        if 'firing_statistics' in self.analysis_results:
            stats = self.analysis_results['firing_statistics']
            firing_rate = stats['firing_rate_stats']['mean']
            if firing_rate < 0.01:
                summary.append("Low firing rate activity")
            elif firing_rate < 0.1:
                summary.append("Moderate firing rate activity")
            else:
                summary.append("High firing rate activity")
            
            trend = stats['trend_analysis']['trend']
            if trend == 'increasing':
                summary.append("Activity trending upward")
            elif trend == 'decreasing':
                summary.append("Activity trending downward")
            elif trend == 'stable':
                summary.append("Activity stable")
            
            rhythmicity = stats['rhythmicity_analysis']['rhythmicity']
            if rhythmicity > 0.5:
                summary.append("Strong rhythmic activity detected")
            elif rhythmicity > 0.2:
                summary.append("Moderate rhythmic activity")
        
        if 'neuromodulation' in self.analysis_results:
            neuromod = self.analysis_results['neuromodulation']
            curiosity = neuromod['curiosity_dynamics']['mean']
            if curiosity > 0.7:
                summary.append("High curiosity drive")
            elif curiosity > 0.3:
                summary.append("Moderate curiosity drive")
            else:
                summary.append("Low curiosity drive")
        
        return summary

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation
for i in range(1000):
    brain.step(i)

analyzer = NeuralDataAnalyzer(brain)

# Analyze firing statistics
step_range = range(900, 1000)
firing_stats = analyzer.analyze_firing_statistics(step_range)

# Analyze neuromodulation dynamics
neuromod_stats = analyzer.analyze_neuromodulation_dynamics()

# Generate comprehensive report
report = analyzer.generate_comprehensive_report()

print("=== Comprehensive Analysis Report ===")
print(f"Brain Summary: {report['brain_info']}")
print(f"Firing Statistics: {firing_stats['firing_rate_stats']['mean']:.4f} avg firing rate")
print(f"Analysis Summary: {', '.join(report['analysis_summary'])}")
```

### Pattern Recognition

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import StandardScaler

class NeuralPatternRecognizer:
    """Recognize and classify neural activity patterns."""
    
    def __init__(self, brain):
        self.brain = brain
        self.pattern_models = {}
        self.classifier = None
    
    def extract_neural_features(self, step_range):
        """Extract features from neural activity for pattern recognition."""
        
        features = []
        feature_labels = []
        
        for step in step_range:
            self.brain.step(step)
            
            # Extract firing-related features
            firing_rate = self.brain.firing_count / max(self.brain.neuron_count, 1)
            spike_count = self.brain.spike_count
            active_neuron_ratio = self.brain.active_neuron_count / max(self.brain.neuron_count, 1)
            
            # Extract neuromodulation features
            curiosity = self.brain.get_curiosity_level() if hasattr(self.brain, 'get_curiosity_level') else 0.0
            novelty = self.brain.get_novelty_level() if hasattr(self.brain, 'get_novelty_level') else 0.0
            prediction_error = self.brain.get_prediction_error() if hasattr(self.brain, 'get_prediction_error') else 0.0
            neuromodulation = self.brain.get_neuromodulation_level() if hasattr(self.brain, 'get_neuromodulation_level') else 0.0
            
            # Extract system state features
            developmental_stage = self.brain.get_developmental_stage() if hasattr(self.brain, 'get_developmental_stage') else 0
            
            step_features = [
                firing_rate, spike_count, active_neuron_ratio,
                curiosity, novelty, prediction_error, neuromodulation,
                developmental_stage
            ]
            
            features.append(step_features)
            feature_labels.append({
                'step': step,
                'firing_rate': firing_rate,
                'spike_count': spike_count,
                'activity_ratio': active_neuron_ratio,
                'curiosity': curiosity,
                'novelty': novelty,
                'prediction_error': prediction_error,
                'neuromodulation': neuromodulation,
                'developmental_stage': developmental_stage
            })
        
        return np.array(features), feature_labels
    
    def train_pattern_model(self, training_data, step_range):
        """Train pattern recognition model on training data."""
        
        # Extract features
        X, feature_labels = self.extract_neural_features(step_range)
        
        # Scale features
        scaler = StandardScaler()
        X_scaled = scaler.fit_transform(X)
        
        # Cluster patterns
        clustering = DBSCAN(eps=0.5, min_samples=5)
        cluster_labels = clustering.fit_predict(X_scaled)
        
        # Store pattern model
        self.pattern_models = {
            'clusters': cluster_labels,
            'feature_data': X,
            'scaler': scaler,
            'feature_labels': feature_labels,
            'cluster_centers': clustering.components_ if hasattr(clustering, 'components_') else None,
            'cluster_scores': clustering.labels_ if hasattr(clustering, 'labels_') else None
        }
        
        return self.pattern_models
    
    def recognize_pattern(self, test_data, step_range):
        """Recognize pattern in test data."""
        
        if not self.pattern_models:
            raise ValueError("Pattern model not trained. Call train_pattern_model() first.")
        
        # Extract features from test data
        X_test, test_labels = self.extract_neural_features(step_range)
        
        # Scale using trained scaler
        scaler = self.pattern_models['scaler']
        X_test_scaled = scaler.transform(X_test)
        
        # Predict clusters
        clustering = DBSCAN(eps=0.5, min_samples=5)
        cluster_labels = clustering.fit_predict(X_test_scaled)
        
        # Analyze pattern characteristics
        pattern_analysis = {}
        
        for i, cluster_id in enumerate(cluster_labels):
            if cluster_id != -1:  # Not noise
                pattern_features = X_test[i]
                pattern_analysis[f'pattern_{cluster_id}'] = {
                    'step': test_labels[i]['step'],
                    'features': pattern_features,
                    'characteristics': self._analyze_pattern_characteristics(pattern_features),
                    'confidence': self._calculate_pattern_confidence(pattern_features, cluster_id),
                    'similarity_to_model': self._calculate_similarity_to_model(pattern_features, cluster_id)
                }
        
        return pattern_analysis
    
    def _analyze_pattern_characteristics(self, features):
        """Analyze characteristics of neural pattern."""
        
        firing_rate, spike_count, active_neuron_ratio, curiosity, novelty, prediction_error, neuromodulation, stage = features
        
        characteristics = {
            'firing_regime': self._classify_firing_regime(firing_rate),
            'activity_level': self._classify_activity_level(spike_count),
            'neuromodulation_profile': self._classify_neuromodulation_profile(curiosity, novelty, prediction_error),
            'developmental_context': self._classify_developmental_context(stage),
            'stability': self._assess_pattern_stability(features)
        }
        
        return characteristics
    
    def _classify_firing_regime(self, firing_rate):
        """Classify firing rate regime."""
        
        if firing_rate < 0.001:
            return 'quiescent'
        elif firing_rate < 0.01:
            return 'low_activity'
        elif firing_rate < 0.05:
            return 'moderate_activity'
        elif firing_rate < 0.1:
            return 'high_activity'
        else:
            return 'hyperactive'
    
    def _classify_activity_level(self, spike_count):
        """Classify activity level based on spike count."""
        
        if spike_count < 10:
            return 'minimal'
        elif spike_count < 100:
            return 'low'
        elif spike_count < 1000:
            return 'moderate'
        elif spike_count < 10000:
            return 'high'
        else:
            return 'very_high'
    
    def _classify_neuromodulation_profile(self, curiosity, novelty, prediction_error):
        """Classify neuromodulation profile."""
        
        profile = []
        if curiosity > 0.7:
            profile.append('high_curiosity')
        elif curiosity > 0.3:
            profile.append('moderate_curiosity')
        else:
            profile.append('low_curiosity')
        
        if novelty > 0.7:
            profile.append('high_novelty')
        elif novelty > 0.3:
            profile.append('moderate_novelty')
        else:
            profile.append('low_novelty')
        
        if prediction_error > 0.7:
            profile.append('high_error')
        elif prediction_error > 0.3:
            profile.append('moderate_error')
        else:
            profile.append('low_error')
        
        return '_'.join(profile)
    
    def _classify_developmental_context(self, stage):
        """Classify developmental context."""
        
        developmental_stages = {
            0: 'initial',
            1: 'critical_period',
            2: 'maturation',
            3: 'adult',
            4: 'aging'
        }
        
        return developmental_stages.get(int(stage), 'unknown')
    
    def _assess_pattern_stability(self, features):
        """Assess stability of neural pattern."""
        
        # Calculate coefficient of variation
        cv = np.std(features) / np.mean(features) if np.mean(features) > 0 else 0
        
        if cv < 0.1:
            return 'stable'
        elif cv < 0.3:
            return 'moderately_stable'
        elif cv < 0.6:
            return 'unstable'
        else:
            return 'highly_unstable'
    
    def _calculate_pattern_confidence(self, features, cluster_id):
        """Calculate confidence of pattern classification."""
        
        # Simplified confidence calculation
        feature_mean = np.mean(features)
        feature_std = np.std(features)
        
        # Higher mean and lower std increase confidence
        confidence = min(feature_mean * 2, 1.0) * (1.0 - min(feature_std, 1.0))
        
        return confidence
    
    def _calculate_similarity_to_model(self, features, cluster_id):
        """Calculate similarity to stored pattern model."""
        
        if 'feature_data' not in self.pattern_models:
            return 0.0
        
        # Calculate similarity to closest model
        model_features = self.pattern_models['feature_data']
        
        distances = []
        for model_feature in model_features:
            if np.array_equal(cluster_id, model_feature):  # Simplified check
                distances.append(0.0)
            else:
                distance = np.linalg.norm(features - model_feature)
                distances.append(distance)
        
        if not distances:
            return 0.0
        
        return min(distances) / max(np.max(distances), 1.0)

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run training simulation
for i in range(1000):
    brain.step(i)

# Create pattern recognizer
recognizer = NeuralPatternRecognizer(brain)

# Train model on first 800 steps
train_step_range = range(200)
recognizer.train_pattern_model(None, train_step_range)

# Test on remaining steps
test_step_range = range(800, 1000)
pattern_analysis = recognizer.recognize_pattern(None, test_step_range)

print("=== Pattern Recognition Results ===")
print(f"Discovered patterns: {len(set([p['cluster_id'] for p in pattern_analysis.values() if 'cluster_id' in p]))}")
for pattern_id, analysis in pattern_analysis.items():
    print(f"{pattern_id}: {analysis['characteristics']}, confidence: {analysis['confidence']:.3f}")
```

## 6. Export and Integration Tools

### Data Export Utilities

```python
import pynlm
import json
import pickle
import numpy as np
from datetime import datetime

class NeuralDataExporter:
    """Export neural simulation data for external analysis."""
    
    def __init__(self, brain):
        self.brain = brain
        self.export_data = {}
    
    def export_simulation_data(self, step_range, format='json'):
        """Export simulation data in specified format."""
        
        data = {
            'metadata': {
                'brain_config': self.brain.get_config().summary(),
                'neuron_count': self.brain.neuron_count,
                'synapse_count': self.brain.synapse_count,
                'region_count': self.brain.region_count,
                'timestamp': datetime.now().isoformat(),
                'step_range': list(step_range)
            },
            'simulation_data': []
        }
        
        for step in step_range:
            self.brain.step(step)
            
            step_data = {
                'step': step,
                'timestamp': step * 0.001,  # Assuming 1ms timestep
                'firing_count': self.brain.firing_count,
                'spike_count': self.brain.spike_count,
                'active_neuron_count': self.brain.active_neuron_count,
                'average_firing_rate': self.brain.average_firing_rate,
                'excitation_inhibition_ratio': self.brain.excitation_inhibition_ratio,
                'development_stage': self.brain.get_developmental_stage() if hasattr(self.brain, 'get_developmental_stage') else None,
                'neuromodulation_data': {
                    'curiosity': self.brain.get_curiosity_level() if hasattr(self.brain, 'get_curiosity_level') else None,
                    'novelty': self.brain.get_novelty_level() if hasattr(self.brain, 'get_novelty_level') else None,
                    'prediction_error': self.brain.get_prediction_error() if hasattr(self.brain, 'get_prediction_error') else None,
                    'neuromodulation': self.brain.get_neuromodulation_level() if hasattr(self.brain, 'get_neuromodulation_level') else None
                },
                'system_states': {
                    'working_memory': self.brain.get_working_memory() is not None,
                    'episodic_memory': self.brain.get_episodic_memory() is not None,
                    'prediction_system': self.brain.get_prediction_system() is not None,
                    'planning_system': self.brain.get_planner() is not None,
                    'concept_formation': self.brain.get_concept_formation() is not None,
                    'attention_system': self.brain.get_attention() is not None,
                    'development_system': self.brain.get_development_system() is not None,
                    'dopamine_system': self.brain.get_dopamine() is not None,
                    'curiosity_system': self.brain.get_curiosity() is not None,
                    'novelty_system': self.brain.get_novelty() is not None,
                    'prediction_error_system': self.brain.get_prediction_error_signal() is not None
                }
            }
            
            data['simulation_data'].append(step_data)
        
        return data
    
    def export_to_file(self, step_range, filename, format='json'):
        """Export simulation data to file."""
        
        data = self.export_simulation_data(step_range, format)
        
        if format == 'json':
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        elif format == 'pkl':
            with open(filename, 'wb') as f:
                pickle.dump(data, f)
        elif format == 'csv':
            self._export_to_csv(data, filename)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return filename
    
    def _export_to_csv(self, data, filename):
        """Export data to CSV format."""
        
        import pandas as pd
        
        # Flatten simulation data
        flat_data = []
        for step_data in data['simulation_data']:
            row = {
                'step': step_data['step'],
                'timestamp': step_data['timestamp'],
                'firing_count': step_data['firing_count'],
                'spike_count': step_data['spike_count'],
                'active_neuron_count': step_data['active_neuron_count'],
                'average_firing_rate': step_data['average_firing_rate'],
                'excitation_inhibition_ratio': step_data['excitation_inhibition_ratio'],
                'development_stage': step_data['development_stage'],
                'curiosity': step_data['neuromodulation_data']['curiosity'],
                'novelty': step_data['neuromodulation_data']['novelty'],
                'prediction_error': step_data['neuromodulation_data']['prediction_error'],
                'neuromodulation': step_data['neuromodulation_data']['neuromodulation']
            }
            
            # Add system state indicators
            for system_name, active in step_data['system_states'].items():
                row[system_name] = 1 if active else 0
            
            flat_data.append(row)
        
        # Create DataFrame and save
        df = pd.DataFrame(flat_data)
        df.to_csv(filename, index=False)

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation
for i in range(1000):
    brain.step(i)

# Export data
exporter = NeuralDataExporter(brain)
step_range = range(800, 1000)

# Export to JSON
json_file = exporter.export_to_file(step_range, 'simulation_data.json', 'json')
print(f"Data exported to JSON: {json_file}")

# Export to pickle
pkl_file = exporter.export_to_file(step_range, 'simulation_data.pkl', 'pkl')
print(f"Data exported to pickle: {pkl_file}")

# Export to CSV
csv_file = exporter.export_to_file(step_range, 'simulation_data.csv', 'csv')
print(f"Data exported to CSV: {csv_file}")
```

### Integration with External Tools

```python
import pynlm
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

class ExternalToolIntegration:
    """Integrate NLM data with external analysis tools."""
    
    def __init__(self, brain):
        self.brain = brain
        self.external_data = {}
    
    def integrate_with_matplotlib(self, step_range, save_plots=True):
        """Integrate NLM data with matplotlib for visualization."""
        
        fig, axes = plt.subplots(2, 3, figsize=(18, 10))
        fig.suptitle('NLM External Tool Integration', fontsize=16)
        
        # Collect data
        steps = []
        firing_rates = []
        spike_counts = []
        curiosity_levels = []
        novelty_levels = []
        
        for step in step_range:
            self.brain.step(step)
            steps.append(step)
            firing_rates.append(self.brain.firing_count / max(self.brain.neuron_count, 1))
            spike_counts.append(self.brain.spike_count)
            
            if hasattr(self.brain, 'get_curiosity_level'):
                curiosity_levels.append(self.brain.get_curiosity_level())
            if hasattr(self.brain, 'get_novelty_level'):
                novelty_levels.append(self.brain.get_novelty_level())
        
        # Plot 1: Firing rate time series
        axes[0, 0].plot(steps, firing_rates, linewidth=1)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Rate')
        axes[0, 0].set_title('Firing Rate Time Series')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Spike count histogram
        axes[0, 1].hist(spike_counts, bins=50, alpha=0.7, density=True)
        axes[0, 1].set_xlabel('Spike Count')
        axes[0, 1].set_ylabel('Density')
        axes[0, 1].set_title('Spike Count Distribution')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Curiosity vs Novelty scatter
        if curiosity_levels and novelty_levels:
            axes[0, 2].scatter(curiosity_levels, novelty_levels, alpha=0.5)
            axes[0, 2].set_xlabel('Curiosity Level')
            axes[0, 2].set_ylabel('Novelty Level')
            axes[0, 2].set_title('Neuromodulation Levels')
            axes[0, 2].grid(True, alpha=0.3)
        
        # Plot 4: Activity heatmap
        heatmap_data = np.array(firing_rates).reshape(1, -1)
        axes[1, 0].imshow(heatmap_data.T, aspect='auto', cmap='hot', interpolation='nearest')
        axes[1, 0].set_xlabel('Time')
        axes[1, 0].set_ylabel('Metric')
        axes[1, 0].set_title('Activity Heatmap')
        
        # Plot 5: Correlation heatmap
        correlation_data = np.corrcoef([firing_rates, spike_counts] + 
                                      ([curiosity_levels] if curiosity_levels else []) +
                                      ([novelty_levels] if novelty_levels else []))
        
        sns.heatmap(correlation_data, annot=True, cmap='coolwarm', center=0,
                   xticklabels=['Firing Rate', 'Spike Count'] +
                               (['Curiosity'] if curiosity_levels else []) +
                               (['Novelty'] if novelty_levels else []),
                   yticklabels=['Firing Rate', 'Spike Count'] +
                               (['Curiosity'] if curiosity_levels else []) +
                               (['Novelty'] if novelty_levels else []),
                   ax=axes[1, 1])
        axes[1, 1].set_title('Correlation Matrix')
        
        # Plot 6: Development stage visualization
        development_stages = []
        for step in step_range:
            if hasattr(self.brain, 'get_developmental_stage'):
                development_stages.append(self.brain.get_developmental_stage())
            else:
                development_stages.append(0)
        
        unique_stages = set(development_stages)
        stage_counts = [development_stages.count(stage) for stage in sorted(unique_stages)]
        
        axes[1, 2].bar(range(len(unique_stages)), stage_counts)
        axes[1, 2].set_xlabel('Development Stage')
        axes[1, 2].set_ylabel('Count')
        axes[1, 2].set_title('Development Stage Distribution')
        axes[1, 2].set_xticks(range(len(unique_stages)))
        axes[1, 2].set_xticklabels([f'Stage {int(s)}' for s in sorted(unique_stages)])
        
        plt.tight_layout()
        
        if save_plots:
            plt.savefig('nlm_analysis_plots.png', dpi=300, bbox_inches='tight')
        
        return fig, axes
    
    def integrate_with_seaborn(self, step_range, save_plots=True):
        """Integrate NLM data with seaborn for statistical visualization."""
        
        # Collect data
        steps = []
        firing_rates = []
        spike_counts = []
        curiosity_levels = []
        novelty_levels = []
        prediction_error_levels = []
        
        for step in step_range:
            self.brain.step(step)
            steps.append(step)
            firing_rates.append(self.brain.firing_count / max(self.brain.neuron_count, 1))
            spike_counts.append(self.brain.spike_count)
            
            if hasattr(self.brain, 'get_curiosity_level'):
                curiosity_levels.append(self.brain.get_curiosity_level())
            if hasattr(self.brain, 'get_novelty_level'):
                novelty_levels.append(self.brain.get_novelty_level())
            if hasattr(self.brain, 'get_prediction_error'):
                prediction_error_levels.append(self.brain.get_prediction_error())
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('NLM Seaborn Integration', fontsize=16)
        
        # Box plot of firing rates by developmental stage
        if hasattr(self.brain, 'get_developmental_stage'):
            developmental_stages = []
            firing_by_stage = {}
            
            for step in step_range:
                self.brain.step(step)
                stage = self.brain.get_developmental_stage()
                firing_rate = self.brain.firing_count / max(self.brain.neuron_count, 1)
                
                if stage not in firing_by_stage:
                    firing_by_stage[stage] = []
                firing_by_stage[stage].append(firing_rate)
            
            axes[0, 0].boxplot(list(firing_by_stage.values()))
            axes[0, 0].set_xlabel('Development Stage')
            axes[0, 0].set_ylabel('Firing Rate')
            axes[0, 0].set_title('Firing Rate by Development Stage')
        
        # Pair plot of neuromodulation relationships
        neuromod_data = pd.DataFrame({
            'Curiosity': curiosity_levels,
            'Novelty': novelty_levels,
            'Prediction Error': prediction_error_levels,
            'Spike Count': spike_counts,
            'Firing Rate': firing_rates
        })
        
        # Remove None values
        neuromod_data = neuromod_data.dropna()
        
        if len(neuromod_data) > 1:
            sns.pairplot(neuromod_data, diag_kind='kde')
            axes[0, 1].set_title('Neuromodulation Relationships')
        
        # Time series with rolling statistics
        axes[1, 0].plot(steps, firing_rates, label='Firing Rate', alpha=0.7)
        
        # Calculate rolling statistics
        window = min(50, len(firing_rates) // 10)
        if window > 1:
            rolling_mean = np.convolve(firing_rates, np.ones(window)/window, mode='valid')
            rolling_std = np.array([np.std(firing_rates[max(0, i-window+1):i+1]) for i in range(len(firing_rates))])
            
            axes[1, 0].plot(steps[window-1:], rolling_mean, label='Rolling Mean', linewidth=2)
            axes[1, 0].fill_between(steps[window-1:], rolling_mean - rolling_std, 
                                   rolling_mean + rolling_std, alpha=0.3, label='Rolling Std')
        
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Firing Rate')
        axes[1, 0].set_title('Time Series with Rolling Statistics')
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3)
        
        # Violin plot of system activity
        system_activity = []
        system_labels = []
        
        for system_name in ['working_memory', 'episodic_memory', 'prediction_system', 
                           'planning_system', 'concept_formation', 'attention_system',
                           'development_system', 'dopamine_system', 'curiosity_system',
                           'novelty_system', 'prediction_error_system']:
            active = getattr(self.brain, f'get_{system_name}')() is not None
            system_activity.append(1 if active else 0)
            system_labels.append(system_name.replace('_', ' ').title())
        
        axes[1, 1].barh(range(len(system_activity)), system_activity)
        axes[1, 1].set_yticks(range(len(system_activity)))
        axes[1, 1].set_yticklabels(system_labels)
        axes[1, 1].set_xlabel('Active')
        axes[1, 1].set_title('System Activity Status')
        
        plt.tight_layout()
        
        if save_plots:
            plt.savefig('nlm_seaborn_analysis.png', dpi=300, bbox_inches='tight')
        
        return fig, axes

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()
# Run simulation
for i in range(1000):
    brain.step(i)

# Integration with matplotlib
integrator = ExternalToolIntegration(brain)
step_range = range(800, 1000)

fig1, axes1 = integrator.integrate_with_matplotlib(step_range)
plt.show()

fig2, axes2 = integrator.integrate_with_seaborn(step_range)
plt.show()
```

## Summary

The visualization and analysis module provides comprehensive tools for:

### 1. Neural Visualization Tools
- **Neuron Activity Visualization**: Time series plots and distribution analysis
- **Synaptic Network Visualization**: Graph-based network representations
- **Activity Heatmap**: 2D visualization of neural activity patterns

### 2. Activity Monitoring Dashboards
- **Real-time Dashboard**: Live monitoring of neural parameters
- **Statistics Logger**: Detailed logging of simulation metrics
- **Performance Monitoring**: System performance tracking and analysis

### 3. Connection Inspection Tools
- **Synapse Inspector**: Detailed analysis of synaptic network properties
- **Neuron Activity Inspector**: Pattern recognition and analysis of individual neurons

### 4. Performance Monitoring Utilities
- **Performance Profiler**: Comprehensive performance profiling
- **Memory Usage Monitor**: Memory usage tracking during simulation

### 5. Data Analysis Tools
- **Statistical Analysis**: Advanced statistical methods for neural data
- **Pattern Recognition**: Machine learning-based pattern recognition
- **Neural Data Analyzer**: Comprehensive neural data analysis framework

### 6. Export and Integration Tools
- **Data Export Utilities**: Export neural data in multiple formats
- **Integration with External Tools**: Seamless integration with matplotlib, seaborn, and other tools

This module enhances the NLM Python bindings by providing powerful visualization and analysis capabilities, making it easier to:

1. Monitor and debug neural simulations
2. Analyze neural patterns and dynamics
3. Visualize complex neural data
4. Integrate NLM data with external analysis tools
5. Export data for further analysis
6. Create comprehensive documentation and reports

These tools make NLM more accessible to researchers, educators, and practitioners working with artificial neural systems.
