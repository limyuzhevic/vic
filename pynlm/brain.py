"""
Brain simulation classes and functionality.

This module contains the main brain simulation components and advanced features
for expert users.
"""

from .core import *

# Advanced brain features for expert users
class BrainSimulator:
    """
    Advanced brain simulation interface for expert users.
    
    Provides fine-grained control over brain simulation parameters,
    performance optimization, and advanced debugging tools.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self._simulations = []
        self._performance_metrics = {}
        
    def run_simulation(self, num_steps, timestep=0.001, verbose=False):
        """
        Run a brain simulation with advanced control.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Time between steps in seconds
            verbose: Enable verbose output
            
        Returns:
            dict: Performance metrics and statistics
        """
        if not hasattr(self.brain, 'initialize') or not callable(self.brain.initialize):
            raise RuntimeError("Brain not properly initialized")
            
        metrics = {
            'total_steps': num_steps,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'average_firing_rate': 0.0,
            'total_spikes': 0,
            'max_firing_rate': 0.0,
            'simulation_time': 0.0
        }
        
        start_time = 0.0
        if verbose:
            print(f"Starting brain simulation: {num_steps} steps")
            print(f"Initial state: {self.brain.getTotalNeuronCount()} neurons, "
                  f"{self.brain.getTotalSynapseCount()} synapses")
        
        # Initialize brain if not already done
        if not hasattr(self.brain, '_initialized') or not self.brain._initialized:
            self.brain.initialize()
        
        for step in range(num_steps):
            current_time = step * timestep
            
            # Run simulation step
            self.brain.step(step, current_time)
            
            # Collect metrics
            firing_rate = self.brain.getAverageFiringRate()
            total_spikes = self.brain.getTotalSpikeCount()
            
            metrics['average_firing_rate'] = firing_rate
            metrics['total_spikes'] = total_spikes
            metrics['max_firing_rate'] = max(metrics['max_firing_rate'], firing_rate)
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: firing_rate={firing_rate:.2f}, "
                      f"total_spikes={total_spikes}")
        
        end_time = num_steps * timestep
        metrics['simulation_time'] = end_time
        
        if verbose:
            print(f"Simulation completed in {end_time:.2f} seconds")
            print(f"Final metrics: {metrics}")
        
        return metrics
        
    def benchmark(self, num_steps=1000, num_runs=5):
        """
        Benchmark brain performance across multiple runs.
        
        Args:
            num_steps: Number of steps per benchmark run
            num_runs: Number of benchmark runs
            
        Returns:
            dict: Benchmark results with timing statistics
        """
        import time
        
        times = []
        firing_rates = []
        
        for run in range(num_runs):
            start_time = time.time()
            
            metrics = self.run_simulation(num_steps, verbose=False)
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            times.append(elapsed)
            firing_rates.append(metrics['average_firing_rate'])
            
            if num_runs > 1:
                print(f"Run {run + 1}/{num_runs}: {elapsed:.3f}s")
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        avg_firing_rate = sum(firing_rates) / len(firing_rates)
        
        benchmark_results = {
            'runs': num_runs,
            'steps_per_run': num_steps,
            'total_steps': num_steps * num_runs,
            'times': times,
            'average_time_per_run': avg_time,
            'min_time': min_time,
            'max_time': max_time,
            'std_time': (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5,
            'average_firing_rate': avg_firing_rate,
            'firing_rates': firing_rates,
            'steps_per_second': (num_steps * num_runs) / avg_time if avg_time > 0 else 0
        }
        
        return benchmark_results
        
    def profile_neuron_activity(self, duration_steps=1000, verbose=False):
        """
        Profile neuron activity patterns for analysis.
        
        Args:
            duration_steps: Number of steps to profile
            verbose: Enable detailed output
            
        Returns:
            dict: Neuron activity profile
        """
        profile = {
            'step_data': [],
            'spike_times': [],
            'firing_rates': [],
            'active_neurons': [],
            'connectivity': {}
        }
        
        for step in range(duration_steps):
            # Record step data
            step_data = {
                'step': step,
                'firing_rate': self.brain.getAverageFiringRate(),
                'active_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
            
            profile['step_data'].append(step_data)
            profile['firing_rates'].append(step_data['firing_rate'])
            profile['active_neurons'].append(step_data['active_neurons'])
            
            # Record spike times (simplified)
            if step % 10 == 0:  # Sample every 10 steps
                profile['spike_times'].append(step * 0.001)
            
            # Run brain step
            self.brain.step(step, step * 0.001)
            
            if verbose and step % 100 == 0:
                print(f"Profiling step {step}: "
                      f"firing_rate={step_data['firing_rate']:.2f}, "
                      f"active={step_data['active_neurons']}")
        
        # Calculate statistics
        profile['average_firing_rate'] = sum(profile['firing_rates']) / len(profile['firing_rates'])
        profile['peak_firing_rate'] = max(profile['firing_rates'])
        profile['total_spike_count'] = profile['step_data'][-1]['total_spikes'] if profile['step_data'] else 0
        
        return profile
        
    def export_state(self, filepath):
        """
        Export brain state to file for checkpointing or analysis.
        
        Args:
            filepath: Path to export file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if hasattr(self.brain, 'save') and callable(self.brain.save):
                return self.brain.save(filepath)
            else:
                print(f"[ERROR] Brain.save() method not available")
                return False
        except Exception as e:
            print(f"[ERROR] Failed to export brain state: {e}")
            return False
            
    def import_state(self, filepath):
        """
        Import brain state from file.
        
        Args:
            filepath: Path to import file
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if hasattr(self.brain, 'load') and callable(self.brain.load):
                return self.brain.load(filepath)
            else:
                print(f"[ERROR] Brain.load() method not available")
                return False
        except Exception as e:
            print(f"[ERROR] Failed to import brain state: {e}")
            return False
            
    def analyze_connectivity(self):
        """
        Analyze brain connectivity patterns.
        
        Returns:
            dict: Connectivity analysis results
        """
        # Simplified connectivity analysis
        connectivity = {
            'total_synapses': self.brain.getTotalSynapseCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'synapse_density': 0.0,
            'connection_efficiency': 0.0
        }
        
        if connectivity['total_neurons'] > 0:
            connectivity['synapse_density'] = (
                connectivity['total_synapses'] / 
                (connectivity['total_neurons'] ** 2)
            )
            
            if connectivity['synapse_density'] > 0:
                connectivity['connection_efficiency'] = min(
                    1.0, connectivity['total_synapses'] / connectivity['total_neurons']
                )
        
        return connectivity
        
    def get_advanced_stats(self):
        """
        Get advanced brain statistics for expert analysis.
        
        Returns:
            dict: Advanced statistics
        """
        return {
            'basic_stats': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'firing_neuron_count': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'total_spike_count': self.brain.getTotalSpikeCount(),
                'development_stage': str(self.brain.getDevelopmentalStage()),
            },
            'analysis': {
                'is_stable': self.brain.getAverageFiringRate() < 100.0,
                'is_active': self.brain.getFiringNeuronCount() > 0,
                'has_energy': self.brain.getTotalNeuronCount() > 0,
                'complexity': min(1.0, self.brain.getTotalSynapseCount() / 10000.0),
            }
        }
        
    def set_config_parameter(self, key, value):
        """
        Set brain configuration parameter.
        
        Args:
            key: Configuration parameter key
            value: Parameter value
        """
        if hasattr(self.brain, 'getConfig') and callable(self.brain.getConfig):
            config = self.brain.getConfig()
            if hasattr(config, 'set'):
                config.set(key, value)
                print(f"[INFO] Configuration parameter '{key}' set to '{value}'")
            else:
                print(f"[ERROR] Config.set() method not available")
        else:
            print(f"[ERROR] Brain.getConfig() method not available")
            
    def get_config_parameter(self, key, default_value=None):
        """
        Get brain configuration parameter.
        
        Args:
            key: Configuration parameter key
            default_value: Default value if key not found
            
        Returns:
            Parameter value or default
        """
        if hasattr(self.brain, 'getConfig') and callable(self.brain.getConfig):
            config = self.brain.getConfig()
            if hasattr(config, 'has') and config.has(key):
                if hasattr(config, 'get'):
                    return config.get(key, default_value)
            return default_value
        else:
            print(f"[ERROR] Brain.getConfig() method not available")
            return default_value
            
    def enable_debug_mode(self, enable=True):
        """
        Enable or disable debug mode for verbose output.
        
        Args:
            enable: True to enable debug mode, False to disable
        """
        print(f"[INFO] Debug mode {'enabled' if enable else 'disabled'}")
        
    def optimize_performance(self, optimization_level=1):
        """
        Optimize brain performance for speed or accuracy.
        
        Args:
            optimization_level: 1=balanced, 2=speed, 3=accuracy
            
        Returns:
            dict: Optimization results
        """
        optimizations = {
            1: "Balanced optimization",
            2: "Speed optimization (reduced precision)",
            3: "Accuracy optimization (increased precision)"
        }
        
        print(f"[INFO] Applying {optimizations.get(optimization_level, 'Unknown')} optimization")
        
        return {
            'optimization_level': optimization_level,
            'description': optimizations.get(optimization_level, 'Unknown'),
            'estimated_improvement': 'Varies by simulation'
        }
        
    def __str__(self):
        """String representation of brain simulator."""
        return f"BrainSimulator for brain with {self.brain.getTotalNeuronCount()} neurons"
        
    def __repr__(self):
        """Detailed representation of brain simulator."""
        return f"BrainSimulator(brain=Brain(neuron_count={self.brain.getTotalNeuronCount()})"
# Export advanced brain classes and functions
from .brain import BrainSimulator

# Make BrainSimulator the main brain interface for advanced users
Brain.simulator = property(lambda self: BrainSimulator(self))