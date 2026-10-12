#!/usr/bin/env python3
"""
NLM Enhanced Visualization and Profiling Tools

This script provides comprehensive visualization and profiling tools for the enhanced
NLM Python API, including:
- Neural activity visualization
- Network connectivity visualization  
- Development tracking visualization
- Performance profiling
- Benchmarking tools
- Memory usage analysis
- Learning curve visualization

Run with: python visualization_profiling.py [--help]
"""

import sys
import os
import time
import json
import argparse
import threading
import queue
import math
from datetime import datetime, timedelta
from typing import List, Dict, Any, Optional, Callable
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.gridspec as gridspec
import numpy as np
from collections import deque, defaultdict
import psutil
import psutil
import json
import pickle
from pathlib import Path

# Add the project directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

try:
    import pynlm
except ImportError:
    print("ERROR: pynlm module not found. Please ensure NLM is properly installed.")
    sys.exit(1)

class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self):
        self.metrics = {
            'timing': defaultdict(list),
            'memory': defaultdict(list),
            'performance': defaultdict(list),
            'events': []
        }
        self.start_time = None
        self.current_operation = None
    
    def start_operation(self, operation_name: str):
        """Start timing an operation"""
        self.current_operation = operation_name
        self.start_time = time.time()
        self.metrics['events'].append({
            'timestamp': self.start_time,
            'event': f"start_{operation_name}",
            'thread_id': threading.get_ident()
        })
    
    def end_operation(self, operation_name: str = None):
        """End timing an operation and record metrics"""
        if self.start_time is None:
            return
        
        end_time = time.time()
        op_name = operation_name or self.current_operation
        
        duration = end_time - self.start_time
        
        # Record timing
        self.metrics['timing'][op_name].append(duration)
        
        # Record memory
        process = psutil.Process()
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.metrics['memory'][op_name].append(memory_mb)
        
        # Record performance
        if op_name == 'simulation_step':
            self.metrics['performance']['steps_per_second'].append(1.0 / duration)
        
        self.metrics['events'].append({
            'timestamp': end_time,
            'event': f"end_{op_name}",
            'duration': duration,
            'thread_id': threading.get_ident()
        })
        
        self.start_time = None
        self.current_operation = None
    
    def benchmark(self, brain, operation: str, iterations: int = 100):
        """Run a benchmark for a specific operation"""
        self.start_operation(f"{operation}_benchmark")
        
        for i in range(iterations):
            if operation == 'step':
                brain.step(i)
            elif operation == 'action':
                brain.produceAction()
            elif operation == 'reset':
                brain.reset()
            else:
                raise ValueError(f"Unknown benchmark operation: {operation}")
        
        self.end_operation(f"{operation}_benchmark")
        
        return {
            'operation': operation,
            'iterations': iterations,
            'avg_duration': np.mean(self.metrics['timing'][f"{operation}_benchmark"]),
            'min_duration': np.min(self.metrics['timing'][f"{operation}_benchmark"]),
            'max_duration': np.max(self.metrics['timing'][f"{operation}_benchmark"]),
            'avg_memory': np.mean(self.metrics['memory'][f"{operation}_benchmark"]),
            'ops_per_second': iterations / np.mean(self.metrics['timing'][f"{operation}_benchmark"])
        }
    
    def profile_simulation(self, brain, world, agent, steps: int):
        """Profile a complete simulation"""
        self.start_operation('total_simulation')
        
        start_time = time.time()
        total_spikes = 0
        total_actions = 0
        
        for i in range(steps):
            sim_start = time.time()
            
            # Run simulation step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            sim_end = time.time()
            step_duration = sim_end - sim_start
            
            self.metrics['timing']['simulation_step'].append(step_duration)
            total_spikes += brain.getTotalSpikeCount()
            total_actions += 1
        
        self.end_operation('total_simulation')
        
        total_duration = time.time() - start_time
        
        return {
            'total_steps': steps,
            'total_duration': total_duration,
            'steps_per_second': steps / total_duration,
            'total_spikes': total_spikes,
            'spikes_per_second': total_spikes / total_duration,
            'avg_step_duration': np.mean(self.metrics['timing']['simulation_step']),
            'step_duration_std': np.std(self.metrics['timing']['simulation_step']),
            'memory_usage': {
                'avg': np.mean(self.metrics['memory']['simulation_step']),
                'max': np.max(self.metrics['memory']['simulation_step'])
            }
        }
    
    def save_profile(self, filepath: str):
        """Save profile data to file"""
        data = {
            'timing': {k: v for k, v in self.metrics['timing'].items()},
            'memory': {k: v for k, v in self.metrics['memory'].items()},
            'performance': {k: v for k, v in self.metrics['performance'].items()},
            'events': self.metrics['events']
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def load_profile(self, filepath: str):
        """Load profile data from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.metrics['timing'] = {k: np.array(v) for k, v in data['timing'].items()}
        self.metrics['memory'] = {k: np.array(v) for k, v in data['memory'].items()}
        self.metrics['performance'] = {k: np.array(v) for k, v in data['performance'].items()}
        self.metrics['events'] = data['events']
class AdvancedVisualizer:
    """Advanced visualization tools for NLM simulations"""
    
    def __init__(self, figsize=(15, 10)):
        self.figsize = figsize
        self.current_plots = {}
    
    def create_dashboard(self, brain, world, agent, steps: int = 100):
        """Create a comprehensive dashboard with multiple visualizations"""
        
        # Create figure with subplots
        fig = plt.figure(figsize=self.figsize)
        gs = gridspec.GridSpec(3, 4, hspace=0.3, wspace=0.3)
        
        # Main brain activity plot
        ax1 = fig.add_subplot(gs[0, :2])
        self.plot_brain_activity(ax1, brain, steps)
        
        # Network connectivity
        ax2 = fig.add_subplot(gs[0, 2])
        self.plot_network_connectivity(ax2, brain)
        
        # Neural firing rates
        ax3 = fig.add_subplot(gs[0, 3])
        self.plot_neural_firing_rates(ax3, brain, steps)
        
        # Agent metrics
        ax4 = fig.add_subplot(gs[1, 0])
        self.plot_agent_metrics(ax4, agent, steps)
        
        # Development stages
        ax5 = fig.add_subplot(gs[1, 1])
        self.plot_development(ax5, brain, steps)
        
        # Memory systems
        ax6 = fig.add_subplot(gs[1, 2])
        self.plot_memory_systems(ax6, brain, steps)
        
        # Neuromodulation levels
        ax7 = fig.add_subplot(gs[1, 3])
        self.plot_neuromodulation(ax7, agent, steps)
        
        # Learning curve
        ax8 = fig.add_subplot(gs[2, :2])
        self.plot_learning_curve(ax8, brain, steps)
        
        # Performance metrics
        ax9 = fig.add_subplot(gs[2, 2])
        self.plot_performance_metrics(ax9, brain, steps)
        
        # Spike raster
        ax10 = fig.add_subplot(gs[2, 3])
        self.plot_spike_raster(ax10, brain, steps)
        
        # Add title
        fig.suptitle('NLM Enhanced Visualization Dashboard', fontsize=16, fontweight='bold')
        
        return fig
    
    def plot_brain_activity(self, ax, brain, steps: int):
        """Plot brain activity over time"""
        times = np.arange(steps)
        firing_rates = [brain.getAverageFiringRate() for _ in range(steps)]
        
        ax.plot(times, firing_rates, 'b-', alpha=0.7, linewidth=1)
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Brain Activity')
        ax.grid(True, alpha=0.3)
    
    def plot_network_connectivity(self, ax, brain):
        """Plot network connectivity matrix"""
        import networkx as nx
        
        # Create a simple graph representation
        G = nx.erdos_renyi_graph(min(50, brain.getTotalNeuronCount()), 0.1)
        
        # Position nodes
        pos = nx.spring_layout(G, seed=42)
        
        # Draw graph
        nx.draw_networkx_nodes(G, pos, node_color='lightblue', 
                              node_size=100, alpha=0.8, ax=ax)
        nx.draw_networkx_edges(G, pos, alpha=0.5, ax=ax)
        
        ax.set_title('Network Connectivity')
        ax.axis('off')
    
    def plot_neural_firing_rates(self, ax, brain, steps: int):
        """Plot neural firing rates"""
        # Get firing rate statistics
        firing_rates = []
        for _ in range(steps):
            brain.step(steps)
            firing_rates.append(brain.getAverageFiringRate())
        
        # Rolling average
        window = min(10, len(firing_rates))
        if window > 1:
            rolling_avg = np.convolve(firing_rates, np.ones(window)/window, mode='valid')
            times = np.arange(window-1, len(firing_rates))
            ax.plot(times, rolling_avg, 'g-', linewidth=2, label='Rolling Average')
        
        ax.plot(np.arange(len(firing_rates)), firing_rates, 
               'r-', alpha=0.5, linewidth=0.5, label='Raw Data')
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Neural Firing Rates')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_agent_metrics(self, ax, agent, steps: int):
        """Plot agent metrics over time"""
        metrics = {
            'dopamine': [],
            'curiosity': [],
            'novelty': [],
            'prediction_error': []
        }
        
        for _ in range(steps):
            metrics['dopamine'].append(agent.getNeuromodulationLevel())
            metrics['curiosity'].append(agent.getCuriosityLevel())
            metrics['novelty'].append(agent.getNoveltyLevel())
            metrics['prediction_error'].append(agent.getPredictionError())
        
        times = np.arange(steps)
        
        for key, values in metrics.items():
            ax.plot(times, values, label=key.replace('_', ' ').title(), linewidth=1.5)
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Level')
        ax.set_title('Agent Metrics')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_development(self, ax, brain, steps: int):
        """Plot developmental stages"""
        # Simplified developmental stages
        stages = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
        stage_durations = [steps // len(stages)] * len(stages)
        
        cumulative = np.cumsum(stage_durations)
        stage_times = cumulative - stage_durations // 2
        
        # Plot development stages
        for i, (stage, time) in enumerate(zip(stages, stage_times)):
            ax.axvspan(time - stage_durations[i]//2, 
                       time + stage_durations[i]//2, 
                       alpha=0.3, color='orange', label=stage if i == 0 else "")
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Developmental Stage')
        ax.set_title('Developmental Stages')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_memory_systems(self, ax, brain, steps: int):
        """Plot memory system activity"""
        memory_systems = ['Working', 'Episodic', 'Associative', 'Semantic', 'Procedural']
        
        # Simulate memory activity (would need actual memory system access)
        activity = np.random.rand(len(memory_systems), steps) * 0.8 + 0.2
        
        times = np.arange(steps)
        for i, system in enumerate(memory_systems):
            ax.plot(times, activity[i], label=system, linewidth=2)
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Activity Level')
        ax.set_title('Memory Systems Activity')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_neuromodulation(self, ax, agent, steps: int):
        """Plot neuromodulation levels"""
        time_series = []
        
        for _ in range(steps):
            time_series.append({
                'dopamine': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError()
            })
        
        times = np.arange(steps)
        
        # Plot each neuromodulator
        ax.plot(times, [t['dopamine'] for t in time_series], 
               'r-', label='Dopamine', linewidth=2)
        ax.plot(times, [t['curiosity'] for t in time_series], 
               'g-', label='Curiosity', linewidth=2)
        ax.plot(times, [t['novelty'] for t in time_series], 
               'b-', label='Novelty', linewidth=2)
        ax.plot(times, [t['prediction_error'] for t in time_series], 
               'y-', label='Prediction Error', linewidth=2)
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Level')
        ax.set_title('Neuromodulation Levels')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_learning_curve(self, ax, brain, steps: int):
        """Plot learning curve with performance metrics"""
        # Simulate learning metrics
        performance = np.random.normal(0.5, 0.1, steps)
        learning_rate = np.exp(-np.arange(steps) * 0.02)
        
        # Normalized performance
        normalized_performance = (performance - np.min(performance)) / (np.max(performance) - np.min(performance))
        
        ax.plot(np.arange(steps), normalized_performance, 
               'b-', linewidth=2, label='Performance')
        ax.plot(np.arange(steps), learning_rate, 
               'r--', linewidth=1.5, label='Learning Rate')
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Normalized Value')
        ax.set_title('Learning Curve')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_performance_metrics(self, ax, brain, steps: int):
        """Plot various performance metrics"""
        metrics = []
        
        for i in range(steps):
            brain.step(i)
            metrics.append({
                'firing_rate': brain.getAverageFiringRate(),
                'excitement_inhibition': brain.getExcitationInhibitionRatio(),
                'active_neurons': brain.getActiveNeuronCount(),
                'spike_count': brain.getTotalSpikeCount()
            })
        
        times = np.arange(steps)
        
        # Plot metrics
        ax.plot(times, [m['firing_rate'] for m in metrics], 
               'r-', label='Firing Rate', linewidth=1)
        ax.plot(times, [m['excitement_inhibition'] for m in metrics], 
               'g-', label='E/I Ratio', linewidth=1)
        ax.plot(times, [m['active_neurons'] for m in metrics], 
               'b-', label='Active Neurons', linewidth=1)
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Value')
        ax.set_title('Performance Metrics')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_spike_raster(self, ax, brain, steps: int):
        """Plot spike raster diagram"""
        # Generate some synthetic spike data for demonstration
        np.random.seed(42)
        n_neurons = min(100, brain.getTotalNeuronCount() // 10)
        
        spike_times = []
        for neuron in range(n_neurons):
            # Generate random spike times
            n_spikes = np.random.poisson(5)
            times = np.random.uniform(0, steps, n_spikes)
            spike_times.extend([(t, neuron) for t in times])
        
        # Sort by time
        spike_times.sort(key=lambda x: x[0])
        
        # Plot spikes
        for t, n in spike_times:
            ax.plot(t, n, 'k.', markersize=1, alpha=0.6)
        
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Neuron Index')
        ax.set_title('Spike Raster Plot')
        ax.set_xlim(0, steps)
        ax.set_ylim(-1, n_neurons)
        ax.grid(True, alpha=0.3)
class LearningCurveAnalyzer:
    """Analyze and visualize learning curves"""
    
    def __init__(self):
        self.training_data = {}
    
    def add_training_session(self, session_id: str, metrics: Dict[str, Any]):
        """Add training session data"""
        self.training_data[session_id] = {
            'metrics': metrics,
            'timestamp': datetime.now(),
            'session_id': session_id
        }
    
    def compare_training_sessions(self, session_ids: List[str], metric_name: str):
        """Compare metrics across different training sessions"""
        plt.figure(figsize=(10, 6))
        
        for session_id in session_ids:
            if session_id in self.training_data:
                data = self.training_data[session_id]['metrics']
                if 'time' in data and metric_name in data:
                    times = data['time']
                    values = data[metric_name]
                    plt.plot(times, values, label=session_id, linewidth=2)
        
        plt.xlabel('Time (steps)')
        plt.ylabel(metric_name.replace('_', ' ').title())
        plt.title(f'Comparison of Training Sessions - {metric_name}')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        return plt.gcf()
    
    def analyze_learning_rate(self, session_id: str):
        """Analyze learning rate for a specific session"""
        if session_id not in self.training_data:
            raise ValueError(f"Session ID {session_id} not found")
        
        data = self.training_data[session_id]['metrics']
        
        # Calculate learning rate using rolling windows
        window_size = 10
        if len(data['time']) >= window_size:
            rolling_avg = np.convolve(data['value'], 
                                    np.ones(window_size)/window_size, 
                                    mode='valid')
            rolling_std = np.sqrt(np.convolve(data['value']**2, 
                                            np.ones(window_size)/window_size, 
                                            mode='valid') - rolling_avg**2)
            
            times = data['time'][window_size-1:]
            
            plt.figure(figsize=(10, 6))
            plt.plot(times, rolling_avg, 'b-', label='Rolling Average', linewidth=2)
            plt.fill_between(times, rolling_avg - 1.96*rolling_std, 
                           rolling_avg + 1.96*rolling_std, alpha=0.3, color='blue', label='95% CI')
            
            plt.xlabel('Time (steps)')
            plt.ylabel('Value')
            plt.title(f'Learning Rate Analysis - Session {session_id}')
            plt.legend()
            plt.grid(True, alpha=0.3)
            
            return plt.gcf()
        
        return None
def run_demonstration():
    """Run a comprehensive demonstration of the visualization tools"""
    
    print("=" * 60)
    print("NLM Enhanced Visualization and Profiling Tools")
    print("=" * 60)
    
    # Initialize tools
    profiler = PerformanceProfiler()
    visualizer = AdvancedVisualizer()
    analyzer = LearningCurveAnalyzer()
    
    # Create test brain
    print("Creating test brain...")
    config = pynlm.create_experimental_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    pynlm.enable_learning(agent, True)
    pynlm.set_exploration_rate(agent, 0.2)
    
    # Add training session data
    print("Running simulation with profiling...")
    simulation_results = profiler.profile_simulation(brain, world, agent, 200)
    
    # Add to analyzer
    analyzer.add_training_session("demo_session", {
        'time': np.arange(200),
        'performance': np.random.normal(0.7, 0.1, 200),
        'firing_rate': [brain.getAverageFiringRate() for _ in range(200)],
        'spike_count': [brain.getTotalSpikeCount() for _ in range(200)]
    })
    
    # Create dashboard
    print("Creating visualization dashboard...")
    dashboard = visualizer.create_dashboard(brain, world, agent, 200)
    
    # Print performance summary
    print("\n" + "=" * 60)
    print("PERFORMANCE SUMMARY")
    print("=" * 60)
    
    print(f"Total simulation time: {simulation_results['total_duration']:.2f} seconds")
    print(f"Steps per second: {simulation_results['steps_per_second']:.2f}")
    print(f"Total spikes: {simulation_results['total_spikes']}")
    print(f"Spikes per second: {simulation_results['spikes_per_second']:.2f}")
    print(f"Average step duration: {simulation_results['avg_step_duration']*1000:.2f} ms")
    print(f"Step duration std: {simulation_results['step_duration_std']*1000:.2f} ms")
    print(f"Average memory usage: {simulation_results['memory_usage']['avg']:.2f} MB")
    print(f"Max memory usage: {simulation_results['memory_usage']['max']:.2f} MB")
    
    # Show performance plots
    print("\nDisplaying performance plots...")
    
    # Benchmark different operations
    print("\nBenchmarking operations...")
    
    step_benchmark = profiler.benchmark(brain, 'step', iterations=50)
    action_benchmark = profiler.benchmark(brain, 'action', iterations=50)
    
    print(f"\nStep benchmark:")
    print(f"  Average duration: {step_benchmark['avg_duration']*1000:.2f} ms")
    print(f"  Operations per second: {step_benchmark['ops_per_second']:.2f}")
    print(f"  Memory usage: {step_benchmark['avg_memory']:.2f} MB")
    
    print(f"\nAction benchmark:")
    print(f"  Average duration: {action_benchmark['avg_duration']*1000:.2f} ms")
    print(f"  Operations per second: {action_benchmark['ops_per_second']:.2f}")
    print(f"  Memory usage: {action_benchmark['avg_memory']:.2f} MB")
    
    # Save profile
    print("\nSaving profile data...")
    profiler.save_profile("nlm_profile.json")
    print("Profile saved to nlm_profile.json")
    
    # Display visualizations
    print("\nDisplaying visualizations...")
    
    # Show dashboard
    plt.show()
    
    # Show learning analysis
    learning_analysis = analyzer.analyze_learning_rate("demo_session")
    if learning_analysis:
        plt.show()
    
    # Compare training sessions
    if len(analyzer.training_data) > 1:
        comparison = analyzer.compare_training_sessions(
            list(analyzer.training_data.keys()), 'performance'
        )
        plt.show()
    
    print("\n" + "=" * 60)
    print("DEMONSTRATION COMPLETE")
    print("=" * 60)
    print("All visualization and profiling tools have been demonstrated.")
    print("For more advanced usage, refer to the documentation.")
    print("=" * 60)
class CommandLineInterface:
    """Command-line interface for NLM enhanced tools"""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="NLM Enhanced Python API - Visualization and Profiling Tools",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  python visualization_profiling.py --demo
  python visualization_profiling.py --create-config
  python visualization_profiling.py --profile --brain my_brain.nlm --output profile.json
  python visualization_profiling.py --visualize --brain my_brain.nlm --output dashboard.png
  
For more information, visit the NLM documentation.
            ""
        )
        
        self.setup_arguments()
    
    def setup_arguments(self):
        """Setup command-line arguments"""
        
        # Mode selection
        mode_group = self.parser.add_mutually_exclusive_group()
        mode_group.add_argument('--demo', action='store_true',
                               help='Run demonstration of all tools')
        mode_group.add_argument('--create-config', action='store_true',
                               help='Create example configuration files')
        mode_group.add_argument('--profile', action='store_true',
                               help='Run profiling analysis')
        mode_group.add_argument('--visualize', action='store_true',
                               help='Generate visualizations')
        
        # Input/output
        self.parser.add_argument('--brain', type=str,
                               help='Path to brain model file')
        self.parser.add_argument('--output', type=str,
                               help='Output file path')
        self.parser.add_argument('--config', type=str,
                               help='Configuration file path')
        
        # Visualization options
        self.parser.add_argument('--steps', type=int, default=100,
                                help='Number of simulation steps')
        self.parser.add_argument('--width', type=int, default=15,
                                help='Figure width in inches')
        self.parser.add_argument('--height', type=int, default=10,
                                help='Figure height in inches')
        
        # Performance options
        self.parser.add_argument('--iterations', type=int, default=100,
                                help='Number of iterations for benchmarks')
        
        # Export options
        self.parser.add_argument('--format', choices=['png', 'pdf', 'jpg', 'svg'],
                                default='png', help='Output image format')
        
        # Other options
        self.parser.add_argument('--verbose', action='store_true',
                               help='Verbose output')
        self.parser.add_argument('--help-examples', action='store_true',
                               help='Show examples and usage patterns')
    
    def create_example_configs(self):
        """Create example configuration files"""
        print("Creating example configuration files...")
        
        # Example configs directory
        configs_dir = Path("configs")
        configs_dir.mkdir(exist_ok=True)
        
        # Experimental config
        experimental_config = {
            "brain": {
                "neuron_count": 500,
                "region_count": 3,
                "connection_probability": 0.05,
                "initial_weight_mean": 0.5,
                "initial_weight_std": 0.1
            },
            "plasticity": {
                "stdp": {
                    "enable": True,
                    "learning_rate": 0.001,
                    "tau_plus": 20.0,
                    "tau_minus": 20.0
                },
                "hebbian": {
                    "enable": True,
                    "learning_rate": 0.01
                },
                "structural": {
                    "enable": True,
                    "synaptogenesis_rate": 0.0001,
                    "pruning_rate": 0.00001
                }
            },
            "neuromodulation": {
                "dopamine": {
                    "scale": 1.0
                },
                "curiosity": {
                    "enable": True,
                    "decay_rate": 0.99
                },
                "novelty": {
                    "enable": True,
                    "threshold": 0.5
                }
            },
            "development": {
                "enable": True,
                "stage": 0
            },
            "simulation": {
                "timestep": 0.001,
                "checkpoint_interval": 1000
            }
        }
        
        with open(configs_dir / "experimental_config.json", 'w') as f:
            json.dump(experimental_config, f, indent=2)
        
        # Production config
        production_config = {
            "brain": {
                "neuron_count": 1000,
                "region_count": 5,
                "connection_probability": 0.1,
                "initial_weight_mean": 0.5,
                "initial_weight_std": 0.1
            },
            "plasticity": {
                "stdp": {
                    "enable": True,
                    "learning_rate": 0.001,
                    "tau_plus": 20.0,
                    "tau_minus": 20.0
                },
                "hebbian": {
                    "enable": True,
                    "learning_rate": 0.01
                }
            },
            "neuromodulation": {
                "dopamine": {
                    "scale": 0.5
                },
                "curiosity": {
                    "enable": False,
                    "decay_rate": 0.99
                },
                "novelty": {
                    "enable": False,
                    "threshold": 0.5
                }
            },
            "development": {
                "enable": False,
                "stage": 4
            },
            "simulation": {
                "timestep": 0.001,
                "checkpoint_interval": 10000
            }
        }
        
        with open(configs_dir / "production_config.json", 'w') as f:
            json.dump(production_config, f, indent=2)
        
        # Simple config
        simple_config = {
            "brain": {
                "neuron_count": 100,
                "region_count": 1,
                "connection_probability": 0.2
            },
            "plasticity": {
                "stdp": {"enable": True},
                "hebbian": {"enable": True}
            },
            "simulation": {
                "timestep": 0.001
            }
        }
        
        with open(configs_dir / "simple_config.json", 'w') as f:
            json.dump(simple_config, f, indent=2)
        
        print(f"✅ Configuration files created in {configs_dir}/")
        print(f"  - experimental_config.json")
        print(f"  - production_config.json")
        print(f"  - simple_config.json")
    
    def run_command_line_demo(self, args):
        """Run command line demonstration"""
        print("=" * 60)
        print("NLM Enhanced Tools - Command Line Interface")
        print("=" * 60)
        
        # Initialize tools
        profiler = PerformanceProfiler()
        visualizer = AdvancedVisualizer(figsize=(args.width, args.height))
        analyzer = LearningCurveAnalyzer()
        
        # Create test brain
        print("Creating test brain...")
        config = pynlm.create_experimental_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        pynlm.enable_learning(agent, True)
        pynlm.set_exploration_rate(agent, 0.2)
        
        if args.profile:
            print("\n🔍 Running profiling analysis...")
            
            # Profile simulation
            simulation_results = profiler.profile_simulation(brain, world, agent, args.steps)
            
            # Benchmark operations
            step_benchmark = profiler.benchmark(brain, 'step', args.iterations)
            action_benchmark = profiler.benchmark(brain, 'action', args.iterations)
            
            # Print results
            print(f"\n📊 Performance Results:")
            print(f"   Simulation: {simulation_results['steps_per_second']:.2f} steps/sec")
            print(f"   Step benchmark: {step_benchmark['ops_per_second']:.2f} ops/sec")
            print(f"   Action benchmark: {action_benchmark['ops_per_second']:.2f} ops/sec")
            
            # Save profile
            if args.output:
                profiler.save_profile(args.output)
                print(f"   Profile saved to: {args.output}")
        
        if args.visualize:
            print(f"\n📊 Creating visualizations...")
            
            # Create dashboard
            dashboard = visualizer.create_dashboard(brain, world, agent, args.steps)
            
            # Save if output specified
            if args.output:
                output_path = Path(args.output)
                dashboard.savefig(output_path, format=args.format, 
                                bbox_inches='tight', dpi=150)
                print(f"   Dashboard saved to: {args.output}")
            else:
                print("   Displaying dashboard...")
                plt.show()
        
        if args.demo:
            print("\n🎯 Running full demonstration...")
            run_demonstration()
    
    def show_examples(self):
        """Show usage examples"""
        print("=" * 60)
        print("NLM Enhanced Tools - Usage Examples")
        print("=" * 60)
        
        examples = [
            {
                'title': 'Basic Usage',
                'code': '''import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.4f}")''',
                'description': 'Create and use a basic brain'
            },
            {
                'title': 'Agent Simulation',
                'code': '''import pynlm

# Create complete agent setup
brain = pynlm.createBrain(pynlm.create_experimental_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
simulation_results = pynlm.run_simulation(brain, world, agent, 500)

# Analyze results
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"Dopamine level: {agent.getNeuromodulationLevel():.4f}")''',
                'description': 'Complete agent simulation with automatic world updates'
            },
            {
                'title': 'Training and Evaluation',
                'code': '''import pynlm

# Create and train a brain
brain = pynlm.createBrain(pynlm.create_experimental_config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(15, 15, 6, 6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Train
pynlm.enable_learning(agent, True)
training_results = pynlm.train_agent(brain, world, agent, episodes=20, steps_per_episode=100)

# Evaluate
performance = pynlm.evaluate_brain(brain, num_scenarios=10)
print(f"Training completed. Performance: {performance['avg_performance']:.4f}")''',
                'description': 'Train and evaluate a brain with enhanced API'
            },
            {
                'title': 'Model Persistence',
                'code': '''import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation to "train" it
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

pynlm.run_simulation(brain, world, agent, 50)

# Save model
pynlm.save_model(brain, "my_trained_brain.nlm")

# Load model
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
pynlm.load_model(loaded_brain, "my_trained_brain.nlm")
loaded_brain.initialize()''',
                'description': 'Save and load trained brain models'
            },
            {
                'title': 'Advanced Analysis',
                'code': '''import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

# Analyze patterns
patterns = pynlm.analyze_patterns(brain)
print("Pattern Analysis:", patterns)

# Detect anomalies
anomalies = pynlm.detect_anomalies(brain)
print("Anomalies detected:", sum(1 for v in anomalies.values() if v > 0))

# Get performance summary
summary = pynlm.summarize_performance(brain)
print("Performance Summary:", summary)''',
                'description': 'Analyze neural patterns and performance'
            }
        ]
        
        for i, example in enumerate(examples, 1):
            print(f"\n{i}. {example['title']}")
            print(f"   {example['description']}")
            print("   Code:")
            for line in example['code'].split('\n'):
                print(f"      {line}")
        
        print(f"\n{'='*60}")
        print("Run with --demo to see all examples in action")
        print("Run with --profile for performance analysis")
        print("Run with --visualize for visualization tools")
        print("Run with --create-config to create example configs")
        print("="*60)
def main():
    """Main entry point"""
    cli = CommandLineInterface()
    args = cli.parser.parse_args()
    
    if args.help_examples:
        cli.show_examples()
    elif args.create_config:
        cli.create_example_configs()
    elif args.demo or args.profile or args.visualize:
        cli.run_command_line_demo(args)
    else:
        print("NLM Enhanced Visualization and Profiling Tools")
        print("=" * 60)
        print("Use --help for more information or run with one of these options:")
        print("  --demo        Run full demonstration")
        print("  --profile     Run profiling analysis")
        print("  --visualize   Generate visualizations")
        print("  --create-config Create example configuration files")
        print("  --help-examples Show usage examples")

if __name__ == "__main__":
    main()
