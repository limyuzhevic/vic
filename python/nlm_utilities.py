# NLM Python Utilities
# Additional Python utilities for NLM users

import time
import numpy as np
from typing import Dict, List, Optional, Callable, Any
import sys

try:
    import matplotlib.pyplot as plt
    import matplotlib
    matplotlib.use('Agg')  # Use non-interactive backend
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False

from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    NeuronType, ActionType, MotorCommand, WorldObjectType, DevelopmentalStage,
    Vision, Audio, InternalSignals, SensoryPercept, Action, AgentBody,
    ActionResult, NeuronId, SynapseId, RegionId, PopulationId
)
class PerformanceMonitor:
    """
    Performance monitoring utilities for NLM simulations.
    
    This class provides timing, memory usage, and metrics tracking
    to help users understand simulation performance and identify bottlenecks.
    """
    
    def __init__(self, name: str = "NLM_Simulation"):
        """
        Initialize performance monitor.
        
        Args:
            name: Name for this performance monitor instance
        """
        self.name = name
        self.timings: Dict[str, List[float]] = {}
        self.memory_samples: List[Dict[str, Any]] = []
        self.metrics: Dict[str, Any] = {}
        self.start_time = time.time()
        
    def time_operation(self, operation_name: str) -> Callable:
        """
        Decorator to time specific operations.
        
        Args:
            operation_name: Name of the operation to time
            
        Returns:
            Decorator function
        """
        def decorator(func: Callable) -> Callable:
            def wrapper(*args, **kwargs):
                start_time = time.time()
                result = func(*args, **kwargs)
                end_time = time.time()
                
                if operation_name not in self.timings:
                    self.timings[operation_name] = []
                self.timings[operation_name].append(end_time - start_time)
                
                return result
            return wrapper
        return decorator
    
    def sample_memory(self, component_name: str = "brain"):
        """
        Sample memory usage for a component.
        
        Args:
            component_name: Name of the component being monitored
        """
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            sample = {
                'timestamp': time.time() - self.start_time,
                'memory_rss': memory_info.rss / 1024 / 1024,  # MB
                'memory_vms': memory_info.vms / 1024 / 1024,  # MB
                'memory_shared': getattr(memory_info, 'shared', 0) / 1024 / 1024,  # MB
                'component': component_name
            }
            self.memory_samples.append(sample)
        except ImportError:
            print("psutil not available for memory monitoring")
    
    def add_metric(self, name: str, value: Any, description: str = ""):
        """
        Add a custom metric to track.
        
        Args:
            name: Metric name
            value: Metric value
            description: Description of the metric
        """
        self.metrics[name] = {
            'value': value,
            'description': description,
            'timestamp': time.time() - self.start_time
        }
    
    def get_timing_stats(self, operation_name: str) -> Dict[str, float]:
        """
        Get statistics for a timed operation.
        
        Args:
            operation_name: Name of the operation
            
        Returns:
            Dictionary with timing statistics
        """
        if operation_name not in self.timings:
            return {'count': 0}
        
        times = self.timings[operation_name]
        return {
            'count': len(times),
            'min': min(times),
            'max': max(times),
            'mean': sum(times) / len(times),
            'total': sum(times)
        }
    
    def plot_timing(self, operation_name: str, save_path: str = None):
        """
        Plot timing statistics for an operation.
        
        Args:
            operation_name: Name of the operation
            save_path: Path to save the plot (None to show)
        """
        if not MATPLOTLIB_AVAILABLE:
            print("matplotlib not available for plotting")
            return
        
        stats = self.get_timing_stats(operation_name)
        if stats['count'] == 0:
            print(f"No timing data for {operation_name}")
            return
        
        plt.figure(figsize=(10, 6))
        
        # Create bars for different timing metrics
        metrics = ['min', 'max', 'mean', 'total']
        values = [stats[m] for m in metrics]
        
        plt.bar(metrics, values, color=['green', 'red', 'blue', 'orange'])
        plt.title(f"Timing Statistics: {operation_name}")
        plt.ylabel("Time (seconds)")
        plt.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
        else:
            plt.show()
    
    def plot_memory_usage(self, save_path: str = None):
        """
        Plot memory usage over time.
        
        Args:
            save_path: Path to save the plot (None to show)
        """
        if not MATPLOTLIB_AVAILABLE:
            print("matplotlib not available for plotting")
            return
        
        if not self.memory_samples:
            print("No memory samples to plot")
            return
        
        timestamps = [sample['timestamp'] for sample in self.memory_samples]
        memory_rss = [sample['memory_rss'] for sample in self.memory_samples]
        memory_vms = [sample['memory_vms'] for sample in self.memory_samples]
        
        plt.figure(figsize=(12, 6))
        
        plt.subplot(1, 2, 1)
        plt.plot(timestamps, memory_rss, 'b-', label='RSS Memory', linewidth=2)
        plt.xlabel('Time (seconds)')
        plt.ylabel('Memory (MB)')
        plt.title('Resident Set Size')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.subplot(1, 2, 2)
        plt.plot(timestamps, memory_vms, 'r-', label='VMS Memory', linewidth=2)
        plt.xlabel('Time (seconds)')
        plt.ylabel('Memory (MB)')
        plt.title('Virtual Memory Size')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
        else:
            plt.show()
    
    def get_report(self) -> str:
        """
        Generate a performance report.
        
        Returns:
            Formatted report string
        """
        report_lines = []
        report_lines.append(f"Performance Report: {self.name}")
        report_lines.append(f"Total time: {time.time() - self.start_time:.2f} seconds")
        report_lines.append("\n=== Timing Statistics ===")
        
        for op_name, times in self.timings.items():
            stats = self.get_timing_stats(op_name)
            report_lines.append(f"{op_name}:")
            report_lines.append(f"  Count: {stats['count']}")
            report_lines.append(f"  Min: {stats['min']:.4f}s")
            report_lines.append(f"  Max: {stats['max']:.4f}s")
            report_lines.append(f"  Mean: {stats['mean']:.4f}s")
            report_lines.append(f"  Total: {stats['total']:.4f}s")
        
        report_lines.append("\n=== Custom Metrics ===")
        for metric_name, metric_data in self.metrics.items():
            report_lines.append(f"{metric_name}: {metric_data['value']} - {metric_data['description']}")
        
        if self.memory_samples:
            report_lines.append("\n=== Memory Samples ===")
            report_lines.append(f"Number of samples: {len(self.memory_samples)}")
            latest = self.memory_samples[-1]
            report_lines.append(f"Latest memory usage: {latest['memory_rss']:.1f} MB RSS, {latest['memory_vms']:.1f} MB VMS")
        
        return "\n".join(report_lines)
    
    def print_report(self):
        """Print the performance report."""
        print(self.get_report())
class SimulationUtilities:
    """
    Utility functions for common NLM simulation patterns.
    
    This class provides convenience functions for running simulations,
    setting up environments, and processing results.
    """
    
    @staticmethod
    def create_reproduction_experiment(config_name: str = "standard"):
        """
        Create a standard reproduction experiment setup.
        
        Args:
            config_name: Name of the configuration to use
            
        Returns:
            Tuple of (brain, world, agent) ready for simulation
        """
        # Create configuration
        config = createDefaultConfig()
        
        # Configure based on config_name
        if config_name == "fast":
            config.neuronCount = 500
            config.regionCount = 1
            config.connectionProbability = 0.05
            config.maxSimulationSteps = 5000
        elif config_name == "standard":
            config.neuronCount = 1000
            config.regionCount = 2
            config.connectionProbability = 0.1
            config.maxSimulationSteps = 10000
        elif config_name == "detailed":
            config.neuronCount = 2000
            config.regionCount = 3
            config.connectionProbability = 0.15
            config.maxSimulationSteps = 20000
        else:
            # Default configuration
            config.neuronCount = 1000
            config.regionCount = 2
            config.connectionProbability = 0.1
            config.maxSimulationSteps = 10000
        
        # Create components
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable common features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        return brain, world, agent
    
    @staticmethod
    def run_learning_simulation(brain, world, agent, num_steps: int, 
                              monitor: Optional[PerformanceMonitor] = None,
                              callback: Optional[Callable] = None):
        """
        Run a standard learning simulation.
        
        Args:
            brain: NLM brain
            world: Simple world
            agent: Agent brain interface
            num_steps: Number of simulation steps
            monitor: Optional performance monitor
            callback: Optional callback function for each step
        """
        if monitor:
            monitor.add_metric("simulation_steps", num_steps, "Total steps in simulation")
        
        for step in range(num_steps):
            # Record start time if monitor available
            if monitor:
                step_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Call callback if provided
            if callback:
                result = callback(step, brain, world, agent, action)
                if result is not None:
                    # If callback returns a value, break
                    break
            
            # Record timing if monitor available
            if monitor:
                step_time = time.time() - step_start
                monitor.time_operation(f"step_{step}")(lambda: None)()  # Record time
                monitor.add_metric(f"step_{step}_time", step_time, f"Time for step {step}")
            
            # Sample memory occasionally
            if monitor and step % 1000 == 0:
                monitor.sample_memory("simulation")
        
        # Final memory sample
        if monitor:
            monitor.sample_memory("simulation_final")
        
        return True
    
    @staticmethod
    def create_benchmark_suite():
        """
        Create a benchmark suite for different configurations.
        
        Returns:
            Dictionary with benchmark configurations
        """
        return {
            'quick_test': {
                'config_name': 'fast',
                'steps': 1000,
                'description': 'Quick smoke test (500 neurons, 1000 steps)'
            },
            'standard_test': {
                'config_name': 'standard',
                'steps': 5000,
                'description': 'Standard test (1000 neurons, 5000 steps)'
            },
            'detailed_test': {
                'config_name': 'detailed',
                'steps': 10000,
                'description': 'Detailed test (2000 neurons, 10000 steps)'
            }
        }
    
    @staticmethod
    def run_benchmark_suite(monitor: Optional[PerformanceMonitor] = None):
        """
        Run a benchmark suite of different configurations.
        
        Args:
            monitor: Optional performance monitor
            
        Returns:
            Dictionary with benchmark results
        """
        if monitor:
            monitor.add_metric("benchmark_suite", 3, "Number of benchmarks to run")
        
        results = {}
        benchmark_suite = SimulationUtilities.create_benchmark_suite()
        
        for bench_name, bench_config in benchmark_suite.items():
            print(f"Running benchmark: {bench_name} - {bench_config['description']}")
            
            # Create experiment
            brain, world, agent = SimulationUtilities.create_reproduction_experiment(
                bench_config['config_name']
            )
            
            # Run simulation
            start_time = time.time()
            SimulationUtilities.run_learning_simulation(
                brain, world, agent, bench_config['steps'], monitor
            )
            end_time = time.time()
            
            # Record results
            results[bench_name] = {
                'config_name': bench_config['config_name'],
                'steps': bench_config['steps'],
                'duration': end_time - start_time,
                'brain_neurons': brain.getTotalNeuronCount(),
                'brain_spikes': brain.getTotalSpikeCount(),
                'agent_reward': agent.getNeuromodulationLevel()
            }
            
            print(f"  Completed in {end_time - start_time:.2f}s")
            print(f"  Neurons: {results[bench_name]['brain_neurons']}, Spikes: {results[bench_name]['brain_spikes']}")
        
        return results
class VisualizationUtilities:
    """
    Visualization utilities for NLM simulations.
    
    This class provides basic plotting and visualization capabilities
    for NLM simulation results.
    """
    
    @staticmethod
    def plot_brain_activity(brain, save_path: str = None):
        """
        Plot brain activity statistics.
        
        Args:
            brain: NLM brain instance
            save_path: Path to save the plot (None to show)
        """
        if not MATPLOTLIB_AVAILABLE:
            print("matplotlib not available for visualization")
            return
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        
        # Neuron count
        ax1.bar(['Total Neurons', 'Active Neurons', 'Firing Neurons'],
                [brain.getTotalNeuronCount(), brain.getActiveNeuronCount(),
                 brain.getFiringNeuronCount()], color=['skyblue', 'lightgreen', 'coral'])
        ax1.set_title('Neuron Statistics')
        ax1.set_ylabel('Count')
        ax1.grid(True, alpha=0.3)
        
        # Spike statistics
        spike_data = [brain.getTotalSpikeCount()]
        ax2.bar(['Total Spikes'], spike_data, color='gold', alpha=0.7)
        ax2.set_title('Spike Statistics')
        ax2.set_ylabel('Count')
        ax2.grid(True, alpha=0.3)
        
        # Firing rate
        firing_rate = brain.getAverageFiringRate()
        ax3.bar(['Average Firing Rate'], [firing_rate], color='violet', alpha=0.7)
        ax3.set_title('Firing Rate')
        ax3.set_ylabel('Hz')
        ax3.grid(True, alpha=0.3)
        
        # E/I ratio
        ei_ratio = brain.getExcitationInhibitionRatio()
        ax4.bar(['E/I Ratio'], [ei_ratio], color='orange', alpha=0.7)
        ax4.set_title('Excitatory/Inhibitory Ratio')
        ax4.set_ylabel('Ratio')
        ax4.grid(True, alpha=0.3)
        
        plt.suptitle('Brain Activity Statistics', fontsize=16)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
        else:
            plt.show()
    
    @staticmethod
    def plot_agent_metrics(agent, save_path: str = None):
        """
        Plot agent neuromodulation metrics.
        
        Args:
            agent: AgentBrain instance
            save_path: Path to save the plot (None to show)
        """
        if not MATPLOTLIB_AVAILABLE:
            print("matplotlib not available for visualization")
            return
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        
        # Neuromodulation levels
        metrics = ['Dopamine', 'Curiosity', 'Novelty', 'Prediction Error']
        values = [agent.getNeuromodulationLevel(), 
                  agent.getCuriosityLevel(),
                  agent.getNoveltyLevel(),
                  agent.getPredictionError()]
        
        colors = ['red', 'blue', 'green', 'orange']
        bars = ax1.bar(metrics, values, color=colors, alpha=0.7)
        ax1.set_title('Neuromodulation Levels')
        ax1.set_ylabel('Level (0-1)')
        ax1.set_ylim(0, 1)
        ax1.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.05,
                    f'{value:.3f}', ha='center', va='bottom')
        
        # Development stage
        stage = agent.getDevelopmentalStage()
        stages = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
        ax2.bar(['Development Stage'], [stage.value], color='purple', alpha=0.7)
        ax2.set_title('Development Stage')
        ax2.set_ylabel('Stage Value')
        ax2.set_ylim(0, 5)
        ax2.grid(True, alpha=0.3)
        
        # Feature flags
        features = ['Reward Modulation', 'Structural Plasticity', 'Development', 'Curiosity']
        flags = [agent.isRewardModulationEnabled(),
                 agent.isStructuralPlasticityEnabled(),
                 agent.isDevelopmentEnabled(),
                 agent.isCuriosityEnabled()]
        
        ax3.bar(features, flags, color=['green', 'blue', 'orange', 'red'], alpha=0.7)
        ax3.set_title('Enabled Features')
        ax3.set_ylabel('Enabled')
        ax3.set_ylim(0, 1)
        ax3.set_yticks([0, 1])
        ax3.set_yticklabels(['No', 'Yes'])
        ax3.grid(True, alpha=0.3)
        
        # Metrics summary
        ax4.axis('off')
        ax4.text(0.1, 0.8, f"Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}", fontsize=12)
        ax4.text(0.1, 0.6, f"Curiosity Level: {agent.getCuriosityLevel():.3f}", fontsize=12)
        ax4.text(0.1, 0.4, f"Novelty Level: {agent.getNoveltyLevel():.3f}", fontsize=12)
        ax4.text(0.1, 0.2, f"Prediction Error: {agent.getPredictionError():.3f}", fontsize=12)
        
        plt.suptitle('Agent Metrics', fontsize=16)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
        else:
            plt.show()
    
    @staticmethod
    def create_training_curves_plot(data: Dict[str, List[float]], save_path: str = None):
        """
        Create a training curves plot from simulation data.
        
        Args:
            data: Dictionary with metric names as keys and lists of values as values
            save_path: Path to save the plot (None to show)
        """
        if not MATPLOTLIB_AVAILABLE:
            print("matplotlib not available for visualization")
            return
        
        if not data:
            print("No data provided for training curves")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        axes = axes.flatten()
        
        for i, (metric_name, values) in enumerate(data.items()):
            if i >= len(axes):
                break
                
            ax = axes[i]
            
            # Plot metric over time (steps)
            steps = list(range(len(values)))
            ax.plot(steps, values, linewidth=2)
            ax.set_title(metric_name.replace('_', ' ').title())
            ax.set_xlabel('Step')
            ax.set_ylabel(metric_name.replace('_', ' '))
            ax.grid(True, alpha=0.3)
            
            # Add final value
            if values:
                ax.axhline(y=values[-1], color='red', linestyle='--', alpha=0.7, label=f'Final: {values[-1]:.3f}')
                ax.legend()
        
        # Hide unused subplots
        for i in range(len(data), len(axes)):
            axes[i].set_visible(False)
        
        plt.suptitle('Training Curves', fontsize=16)
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
        else:
            plt.show()
class ExperimentUtilities:
    """
    Utility functions for running and analyzing NLM experiments.
    
    This class provides helper functions for conducting experiments,
    analyzing results, and managing experimental configurations.
    """
    
    @staticmethod
    def create_experiment_config(base_config_name: str = "standard",
                                variations: Optional[Dict[str, Any]] = None):
        """
        Create an experiment configuration with variations.
        
        Args:
            base_config_name: Base configuration name
            variations: Dictionary of parameter variations
            
        Returns:
            Configuration dictionary
        """
        # Start with base configuration
        config = createDefaultConfig()
        
        # Set base configuration
        if base_config_name == "fast":
            config.neuronCount = 500
            config.regionCount = 1
            config.connectionProbability = 0.05
        elif base_config_name == "standard":
            config.neuronCount = 1000
            config.regionCount = 2
            config.connectionProbability = 0.1
        elif base_config_name == "detailed":
            config.neuronCount = 2000
            config.regionCount = 3
            config.connectionProbability = 0.15
        
        # Apply variations
        if variations:
            for key, value in variations.items():
                if hasattr(config, key):
                    setattr(config, key, value)
        
        return config
    
    @staticmethod
    def run_experiment(experiment_name: str, config_name: str = "standard",
                      steps: int = 10000, variations: Optional[Dict[str, Any]] = None,
                      monitor: Optional[PerformanceMonitor] = None,
                      save_results: bool = False) -> Dict[str, Any]:
        """
        Run a complete experiment.
        
        Args:
            experiment_name: Name of the experiment
            config_name: Base configuration name
            steps: Number of simulation steps
            variations: Parameter variations
            monitor: Optional performance monitor
            save_results: Whether to save results to files
            
        Returns:
            Dictionary with experiment results
        """
        print(f"Running experiment: {experiment_name}")
        
        # Create experiment configuration
        config = ExperimentUtilities.create_experiment_config(
            config_name, variations
        )
        
        # Create components
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable common features for learning experiments
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Record start time
        start_time = time.time()
        
        # Run simulation
        SimulationUtilities.run_learning_simulation(
            brain, world, agent, steps, monitor
        )
        
        # Record end time
        end_time = time.time()
        
        # Collect results
        results = {
            'experiment_name': experiment_name,
            'config_name': config_name,
            'steps': steps,
            'duration': end_time - start_time,
            'brain_neurons': brain.getTotalNeuronCount(),
            'brain_regions': brain.getRegionCount(),
            'brain_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'ei_ratio': brain.getExcitationInhibitionRatio(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'development_stage': agent.getDevelopmentalStage().value,
            'time_per_step': (end_time - start_time) / steps,
            'spikes_per_step': brain.getTotalSpikeCount() / steps
        }
        
        # Save results if requested
        if save_results:
            ExperimentUtilities.save_experiment_results(results, experiment_name)
        
        print(f"Experiment completed in {results['duration']:.2f}s")
        print(f"Results: {results['total_spikes']} spikes, {results['avg_firing_rate']:.2f} Hz firing rate")
        
        return results
    
    @staticmethod
    def save_experiment_results(results: Dict[str, Any], experiment_name: str,
                               directory: str = "./experiment_results"):
        """
        Save experiment results to files.
        
        Args:
            results: Experiment results dictionary
            experiment_name: Name of the experiment
            directory: Directory to save results
        """
        import os
        import json
        
        # Create directory if it doesn't exist
        os.makedirs(directory, exist_ok=True)
        
        # Save results as JSON
        results_file = os.path.join(directory, f"{experiment_name}_results.json")
        with open(results_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        # Save summary as text
        summary_file = os.path.join(directory, f"{experiment_name}_summary.txt")
        with open(summary_file, 'w') as f:
            f.write(f"Experiment: {experiment_name}\n")
            f.write(f"Duration: {results['duration']:.2f} seconds\n")
            f.write(f"Total Spikes: {results['total_spikes']}\n")
            f.write(f"Average Firing Rate: {results['avg_firing_rate']:.2f} Hz\n")
            f.write(f"E/I Ratio: {results['ei_ratio']:.3f}\n")
            f.write(f"Time per Step: {results['time_per_step']:.6f} seconds\n")
        
        print(f"Results saved to {directory}/")
    
    @staticmethod
    def compare_experiments(experiment_results: List[Dict[str, Any]]):
        """
        Compare multiple experiment results.
        
        Args:
            experiment_results: List of experiment results dictionaries
            
        Returns:
            Comparison dictionary
        """
        if not experiment_results:
            return {}
        
        comparison = {
            'experiment_names': [r['experiment_name'] for r in experiment_results],
            'durations': [r['duration'] for r in experiment_results],
            'total_spikes': [r['total_spikes'] for r in experiment_results],
            'avg_firing_rates': [r['avg_firing_rate'] for r in experiment_results],
            'spikes_per_step': [r['spikes_per_step'] for r in experiment_results]
        }
        
        # Calculate statistics
        comparison['mean_duration'] = sum(comparison['durations']) / len(comparison['durations'])
        comparison['mean_spikes'] = sum(comparison['total_spikes']) / len(comparison['total_spikes'])
        comparison['mean_firing_rate'] = sum(comparison['avg_firing_rates']) / len(comparison['avg_firing_rates'])
        
        return comparison
class NLMContextManager:
    """
    Context manager for NLM resources.
    
    This context manager ensures proper cleanup of NLM resources
    and provides a convenient way to work with NLM components.
    """
    
    def __init__(self, config_name: str = "standard"):
        """
        Initialize context manager.
        
        Args:
            config_name: Configuration name
        """
        self.config_name = config_name
        self.brain = None
        self.world = None
        self.agent = None
        self.monitor = None
    
    def __enter__(self):
        """Enter context and create NLM components."""
        # Create components
        self.brain = createBrain(createDefaultConfig())
        self.brain.initialize()
        
        self.world = createSimpleWorld()
        self.world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable common features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Create performance monitor
        self.monitor = PerformanceMonitor("ContextManager_Simulation")
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context and clean up resources."""
        # Clean up monitor
        if self.monitor:
            self.monitor.print_report()
        
        # Reset components
        if self.agent:
            self.agent.reset()
        
        # Clear references
        self.agent = None
        self.world = None
        self.brain = None
        self.monitor = None
    
    def run_simulation(self, steps: int, callback: Optional[Callable] = None):
        """
        Run a simulation within the context.
        
        Args:
            steps: Number of simulation steps
            callback: Optional callback function
            
        Returns:
            True if successful
        """
        if not all([self.brain, self.world, self.agent, self.monitor]):
            raise RuntimeError("NLM components not properly initialized")
        
        return SimulationUtilities.run_learning_simulation(
            self.brain, self.world, self.agent, steps, self.monitor, callback
        )
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get current statistics.
        
        Returns:
            Dictionary with current statistics
        """
        if not self.brain or not self.agent or not self.monitor:
            return {}
        
        return {
            'experiment_name': 'ContextManager_Simulation',
            'brain_stats': {
                'neurons': self.brain.getTotalNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            },
            'agent_stats': {
                'neuromodulation': self.agent.getNeuromodulationLevel(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel()
            },
            'performance_stats': self.monitor.get_timing_stats('step_0') if hasattr(self.monitor, 'get_timing_stats') else {}
        }
# Convenience functions for common patterns

def create_experiment_brain(config_name: str = "standard"):
    """
    Create a configured brain for experimentation.
    
    Args:
        config_name: Configuration name ("fast", "standard", "detailed")
        
    Returns:
        Configured brain instance
    """
    config = createDefaultConfig()
    
    if config_name == "fast":
        config.neuronCount = 500
        config.regionCount = 1
        config.connectionProbability = 0.05
    elif config_name == "standard":
        config.neuronCount = 1000
        config.regionCount = 2
        config.connectionProbability = 0.1
    elif config_name == "detailed":
        config.neuronCount = 2000
        config.regionCount = 3
        config.connectionProbability = 0.15
    
    brain = createBrain(config)
    brain.initialize()
    return brain
def quick_test(config_name: str = "fast", steps: int = 1000):
    """
    Run a quick test simulation.
    
    Args:
        config_name: Configuration name
        steps: Number of simulation steps
        
    Returns:
        Brain instance after simulation
    """
    brain = create_experiment_brain(config_name)
    world = createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    SimulationUtilities.run_learning_simulation(
        brain, world, agent, steps
    )
    
    return brain
def benchmark_configurations():
    """
    Run benchmarks for different configurations.
    
    Returns:
        Dictionary with benchmark results
    """
    monitor = PerformanceMonitor("Benchmark_Suite")
    
    print("Running benchmark suite...")
    results = SimulationUtilities.run_benchmark_suite(monitor)
    
    monitor.print_report()
    
    return results
# Example usage functions

def example_simple_simulation():
    """
    Example: Simple simulation with basic features.
    """
    print("=== Example 1: Simple Simulation ===")
    
    brain = create_experiment_brain("fast")
    world = createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    SimulationUtilities.run_learning_simulation(
        brain, world, agent, 500
    )
    
    print(f"Simulation completed!")
    print(f"Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
    print()

def example_learning_simulation():
    """
    Example: Learning simulation with curiosity and development.
    """
    print("=== Example 2: Learning Simulation ===")
    
    # Use standard configuration for learning
    brain = create_experiment_brain("standard")
    world = createSimpleWorld()
    world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run longer simulation
    SimulationUtilities.run_learning_simulation(
        brain, world, agent, 2000
    )
    
    print(f"Learning simulation completed!")
    print(f"Development stage: {agent.getDevelopmentalStage()}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()

def example_benchmark():
    """
    Example: Run benchmark suite.
    """
    print("=== Example 3: Benchmark Suite ===")
    
    results = benchmark_configurations()
    
    print("Benchmark completed!")
    for config_name, result in results.items():
        print(f"{config_name}: {result['duration']:.2f}s, {result['total_spikes']} spikes")
    print()

# Print available examples
print("NLM Python Utilities - Examples Available:")
print("1. Simple Simulation: example_simple_simulation()")
print("2. Learning Simulation: example_learning_simulation()")  
print("3. Benchmark Suite: example_benchmark()")
print()
print("For more information, see the documentation in easy_usage.md")
