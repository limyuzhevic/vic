# High-level brain simulation and development interface
from typing import Optional, List, Dict, Any, Generator, Union, Tuple
from enum import Enum
import time
from contextlib import contextmanager

from .types import *
from .config import Config, create_default_config, create_config

class BrainError(Exception):
    """Raised when brain operation fails."""
    pass

class Brain:
    """
    High-level brain simulation interface with Pythonic features.
    
    Provides a comprehensive interface for brain simulation with:
    - Type safety and validation
    - Pythonic context managers
    - Progress tracking and monitoring
    - Event-driven callbacks
    - Visualization and plotting integration
    - Advanced simulation features
    
    The Brain class wraps the C++ implementation to provide:
    - Simplified API for common operations
    - Enhanced monitoring and statistics
    - Progress tracking and callbacks
    - Visualization and plotting helpers
    - Simulation lifecycle management
    
    Examples:
        Basic simulation:
            from nlm import createBrain, create_default_config
            
            config = create_default_config()
            brain = createBrain(config)
            brain.initialize()
            
            for i in range(100):
                brain.step(i)
            
            print(f"Firing rate: {brain.get_average_firing_rate()}")
        
        Simulation with monitoring:
            brain = createBrain(config)
            brain.initialize()
            
            with brain.monitor() as monitor:
                for step in brain.steps(1000):
                    brain.step(step)
                    if step % 100 == 0:
                        print(f"Progress: {step}/1000")
            
            stats = monitor.get_statistics()
            print(f"Average firing rate: {stats['average_firing_rate']}")
        
        Simulation with callbacks:
            def on_step_complete(brain, step, stats):
                if step % 500 == 0:
                    print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
            
            brain = createBrain(config)
            brain.initialize()
            brain.add_step_callback(on_step_complete)
            
            for step in range(1000):
                brain.step(step)
    """
    
    def __init__(self, brain_instance: Any, config: Optional[Config] = None):
        """
        Initialize brain wrapper.
        
        Args:
            brain_instance: The underlying C++ Brain instance
            config: Configuration for this brain instance
        """
        self._brain = brain_instance
        self._config = config or create_default_config()
        self._callbacks: List[Callable] = []
        self._step_count = 0
        self._start_time = None
        self._monitor_data: List[Dict[str, Any]] = []
        
        # Get C++ type constants for forward references
        self._neuron_type = NeuronType
        self._synapse_type = SynapseType
        self._developmental_stage = DevelopmentalStage
        self._firing_state = FiringState
        self._action_type = ActionType
        self._motor_command = MotorCommand
    
    @property
    def brain(self) -> Any:
        """Get the underlying C++ Brain instance."""
        return self._brain
    
    @property
    def config(self) -> Config:
        """Get the brain configuration."""
        return self._config
    
    def initialize(self) -> 'Brain':
        """
        Initialize the brain.
        
        Returns:
            Self for method chaining
        """
        if not self._brain.initialize():
            raise BrainError("Failed to initialize brain")
        return self
    
    def step(self, step: Optional[SimulationStep] = None, time: Optional[Timestamp] = None) -> 'Brain':
        """
        Perform a single simulation step.
        
        Args:
            step: Step number (optional)
            time: Current simulation time (optional)
        
        Returns:
            Self for method chaining
        
        Raises:
            BrainError: If step fails
        """
        # Use provided parameters or defaults
        current_step = step if step is not None else self._step_count
        current_time = time if time is not None else current_step * 0.001
        
        try:
            if time is not None:
                self._brain.step(current_step, current_time)
            else:
                self._brain.step(current_step)
            
            self._step_count += 1
            
            # Record step data for monitoring
            step_data = self._collect_step_data(current_step, current_time)
            self._monitor_data.append(step_data)
            
            # Execute callbacks
            self._execute_callbacks(current_step, step_data)
            
        except Exception as e:
            raise BrainError(f"Step {current_step} failed: {e}")
        
        return self
    
    def reset(self) -> 'Brain':
        """
        Reset the brain to initial state.
        
        Returns:
            Self for method chaining
        """
        self._brain.reset()
        self._step_count = 0
        self._monitor_data.clear()
        return self
    
    def steps(self, num_steps: SimulationStep) -> Generator['Brain', None, None]:
        """
        Generate simulation steps with progress tracking.
        
        Args:
            num_steps: Number of steps to generate
        
        Yields:
            Self after each step
        """
        for step in range(num_steps):
            yield self.step(step)
    
    @contextmanager
    def monitor(self) -> 'BrainMonitor':
        """
        Context manager for monitoring brain simulation.
        
        Yields:
            BrainMonitor instance for collecting metrics
        """
        monitor = BrainMonitor(self)
        try:
            yield monitor
        finally:
            # Store monitor data
            if monitor.data:
                self._monitor_data.extend(monitor.data)
    
    def add_step_callback(self, callback: Callable[[SimulationStep, Dict[str, Any]], None]) -> 'Brain':
        """
        Add a callback to execute after each step.
        
        Args:
            callback: Function(step, step_data)
        
        Returns:
            Self for method chaining
        """
        self._callbacks.append(callback)
        return self
    
    def remove_step_callback(self, callback: Callable) -> 'Brain':
        """
        Remove a step callback.
        
        Args:
            callback: Callback to remove
        """
        if callback in self._callbacks:
            self._callbacks.remove(callback)
    
    def clear_step_callbacks(self) -> 'Brain':
        """Clear all step callbacks."""
        self._callbacks.clear()
        return self
    
    # Statistics and monitoring methods
    def get_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.get_total_neuron_count()
    
    def get_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.get_total_synapse_count()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of firing neurons in current step."""
        return self._brain.get_firing_neuron_count()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.get_average_firing_rate()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.get_total_spike_count()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self._brain.get_excitation_inhibition_ratio()
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        stage = self._brain.get_developmental_stage()
        return self._developmental_stage(stage)
    
    def get_regions(self) -> List['Region']:
        """Get all brain regions."""
        return [Region(region, self) for region in self._brain.get_regions()]
    
    def get_region(self, region_id: Union[str, RegionId]) -> Optional['Region']:
        """
        Get a specific region by ID.
        
        Args:
            region_id: Region ID (string or integer)
        
        Returns:
            Region instance or None if not found
        """
        if isinstance(region_id, str):
            # Try to parse as integer
            try:
                region_id = RegionId(int(region_id))
            except ValueError:
                return None
        
        regions = self._brain.get_regions()
        for region in regions:
            if region.get_id() == region_id:
                return Region(region, self)
        
        return None
    
    def inject_current(self, neuron_id: NeuronId, current: float) -> 'Brain':
        """
        Inject current into a specific neuron.
        
        Args:
            neuron_id: Target neuron ID
            current: Current value to inject
        
        Returns:
            Self for method chaining
        """
        self._brain.inject_current(neuron_id, current)
        return self
    
    def inject_current_to_neurons(self, neuron_type: NeuronType, current: float) -> 'Brain':
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to target
            current: Current value to inject
        
        Returns:
            Self for method chaining
        """
        self._brain.inject_current_to_neurons(neuron_type, current)
        return self
    
    def receive_sensory_input(self, sensory_input: Any) -> 'Brain':
        """
        Receive sensory input and inject into brain.
        
        Args:
            sensory_input: Sensory input data
        
        Returns:
            Self for method chaining
        """
        self._brain.receive_sensory_input(sensory_input)
        return self
    
    def produce_action(self) -> 'Action':
        """
        Produce an action based on current brain state.
        
        Returns:
            Action instance
        """
        from . import Action
        action_data = self._brain.produce_action()
        return Action(action_data, self)
    
    def save(self, filepath: str) -> 'Brain':
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save brain state
        
        Returns:
            Self for method chaining
        """
        if not self._brain.save(filepath):
            raise BrainError(f"Failed to save brain to {filepath}")
        return self
    
    def load(self, filepath: str) -> 'Brain':
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
        
        Returns:
            Self for method chaining
        """
        if not self._brain.load(filepath):
            raise BrainError(f"Failed to load brain from {filepath}")
        return self
    
    # Visualization and plotting methods
    def plot_firing_rate(self, save_path: Optional[str] = None) -> Any:
        """
        Plot firing rate over time.
        
        Args:
            save_path: Optional path to save plot
        
        Returns:
            Matplotlib figure object
        """
        try:
            import matplotlib.pyplot as plt
            
            if not self._monitor_data:
                raise BrainError("No monitoring data available. Run simulation first.")
            
            steps = [data['step'] for data in self._monitor_data]
            firing_rates = [data['average_firing_rate'] for data in self._monitor_data]
            
            fig, ax = plt.subplots(figsize=(10, 6))
            ax.plot(steps, firing_rates)
            ax.set_xlabel('Step')
            ax.set_ylabel('Average Firing Rate')
            ax.set_title('Brain Firing Rate Over Time')
            ax.grid(True, alpha=0.3)
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise BrainError("matplotlib not available. Install with: pip install matplotlib")
    
    def plot_spike_histogram(self, save_path: Optional[str] = None) -> Any:
        """
        Plot spike count histogram.
        
        Args:
            save_path: Optional path to save plot
        
        Returns:
            Matplotlib figure object
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Collect spike data from all regions
            spike_counts = []
            for region in self._brain.get_regions():
                # This would need to be implemented based on available C++ API
                pass
            
            # For now, create a placeholder plot
            fig, ax = plt.subplots(figsize=(10, 6))
            ax.hist(spike_counts, bins=50, alpha=0.7, edgecolor='black')
            ax.set_xlabel('Spike Count')
            ax.set_ylabel('Frequency')
            ax.set_title('Spike Count Distribution')
            ax.grid(True, alpha=0.3)
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise BrainError("matplotlib not available. Install with: pip install matplotlib")
    
    # Simulation statistics and analysis
    def get_simulation_statistics(self) -> Dict[str, Any]:
        """
        Get comprehensive simulation statistics.
        
        Returns:
            Dictionary with simulation statistics
        """
        if not self._monitor_data:
            return {}
        
        firing_rates = [data['average_firing_rate'] for data in self._monitor_data]
        spike_counts = [data['firing_neuron_count'] for data in self._monitor_data]
        
        return {
            'total_steps': len(self._monitor_data),
            'total_spikes': self.get_total_spike_count(),
            'average_firing_rate': np.mean(firing_rates) if firing_rates else 0.0,
            'std_firing_rate': np.std(firing_rates) if firing_rates else 0.0,
            'min_firing_rate': np.min(firing_rates) if firing_rates else 0.0,
            'max_firing_rate': np.max(firing_rates) if firing_rates else 0.0,
            'average_spike_count': np.mean(spike_counts) if spike_counts else 0.0,
            'total_firing_neurons': sum(spike_counts),
            'e_i_ratio': self.get_excitation_inhibition_ratio(),
            'development_stage': self.get_developmental_stage().value,
        }
    
    def print_status(self) -> None:
        """Print current brain status."""
        self._brain.log_status()
    
    def _collect_step_data(self, step: SimulationStep, time: Timestamp) -> Dict[str, Any]:
        """Collect monitoring data for current step."""
        return {
            'step': step,
            'time': time,
            'neuron_count': self.get_neuron_count(),
            'synapse_count': self.get_synapse_count(),
            'firing_neuron_count': self.get_firing_neuron_count(),
            'average_firing_rate': self.get_average_firing_rate(),
            'total_spike_count': self.get_total_spike_count(),
            'e_i_ratio': self.get_excitation_inhibition_ratio(),
            'development_stage': self.get_developmental_stage().value,
        }
    
    def _execute_callbacks(self, step: SimulationStep, step_data: Dict[str, Any]):
        """Execute all registered callbacks."""
        for callback in self._callbacks:
            try:
                callback(step, step_data)
            except Exception as e:
                print(f"Warning: Callback failed at step {step}: {e}")


class BrainMonitor:
    """
    Monitor brain simulation and collect metrics.
    
    Provides detailed monitoring of brain simulation with:
    - Real-time metric collection
    - Progress tracking
    - Statistical analysis
    - Data export capabilities
    
    Examples:
        Basic monitoring:
            brain = createBrain(config)
            brain.initialize()
            
            with brain.monitor() as monitor:
                for step in brain.steps(1000):
                    brain.step(step)
            
            stats = monitor.get_statistics()
            print(f"Average firing rate: {stats['average_firing_rate']}")
        
        Monitoring with thresholds:
            def alert_callback(step, data):
                if data['average_firing_rate'] > 10.0:
                    print(f"High firing rate alert at step {step}: {data['average_firing_rate']}")
            
            brain.add_step_callback(alert_callback)
            with brain.monitor() as monitor:
                # ... run simulation
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize brain monitor.
        
        Args:
            brain: Brain instance to monitor
        """
        self._brain = brain
        self.data: List[Dict[str, Any]] = []
        self._start_time = time.time()
        self._thresholds: Dict[str, float] = {}
        self._alert_callbacks: List[Callable] = []
    
    def set_threshold(self, metric: str, threshold: float):
        """
        Set a threshold for metric monitoring.
        
        Args:
            metric: Metric name ('firing_rate', 'spike_count', etc.)
            threshold: Threshold value
        """
        self._thresholds[metric] = threshold
    
    def add_alert_callback(self, callback: Callable[[str, float, Dict[str, Any]], None]):
        """
        Add an alert callback for threshold breaches.
        
        Args:
            callback: Function(metric, value, step_data)
        """
        self._alert_callbacks.append(callback)
    
    def check_thresholds(self, step_data: Dict[str, Any]):
        """Check if any thresholds are breached."""
        for metric, threshold in self._thresholds.items():
            if metric in step_data:
                value = step_data[metric]
                if value > threshold:
                    for callback in self._alert_callbacks:
                        callback(metric, value, step_data)
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get monitoring statistics.
        
        Returns:
            Dictionary with statistical summaries
        """
        if not self.data:
            return {}
        
        # Convert to numpy for calculations
        try:
            import numpy as np
        except ImportError:
            # Fallback without numpy
            return self._get_statistics_without_numpy()
        
        # Extract metrics
        steps = [d['step'] for d in self.data]
        firing_rates = [d['average_firing_rate'] for d in self.data]
        spike_counts = [d['firing_neuron_count'] for d in self.data]
        
        # Calculate statistics
        stats = {
            'total_steps': len(self.data),
            'total_spikes': sum(spike_counts),
            'average_firing_rate': float(np.mean(firing_rates)),
            'std_firing_rate': float(np.std(firing_rates)),
            'min_firing_rate': float(np.min(firing_rates)),
            'max_firing_rate': float(np.max(firing_rates)),
            'average_spike_count': float(np.mean(spike_counts)),
            'total_firing_neurons': sum(spike_counts),
            'duration_seconds': time.time() - self._start_time,
            'steps_per_second': len(self.data) / (time.time() - self._start_time) if (time.time() - self._start_time) > 0 else 0,
        }
        
        return stats
    
    def _get_statistics_without_numpy(self) -> Dict[str, Any]:
        """Get statistics without numpy."""
        if not self.data:
            return {}
        
        steps = [d['step'] for d in self.data]
        firing_rates = [d['average_firing_rate'] for d in self.data]
        spike_counts = [d['firing_neuron_count'] for d in self.data]
        
        # Simple calculations without numpy
        total_spikes = sum(spike_counts)
        
        def mean(values):
            return sum(values) / len(values) if values else 0.0
        
        def std(values, mean_val):
            if not values:
                return 0.0
            variance = sum((x - mean_val) ** 2 for x in values) / len(values)
            return (variance ** 0.5)
        
        def min_max(values):
            return min(values), max(values) if values else (0.0, 0.0)
        
        mean_firing_rate = mean(firing_rates)
        std_firing_rate = std(firing_rates, mean_firing_rate)
        min_fr, max_fr = min_max(firing_rates)
        
        return {
            'total_steps': len(self.data),
            'total_spikes': total_spikes,
            'average_firing_rate': mean_firing_rate,
            'std_firing_rate': std_firing_rate,
            'min_firing_rate': min_fr,
            'max_firing_rate': max_fr,
            'average_spike_count': mean(spike_counts),
            'total_firing_neurons': sum(spike_counts),
            'duration_seconds': time.time() - self._start_time,
            'steps_per_second': len(self.data) / (time.time() - self._start_time) if (time.time() - self._start_time) > 0 else 0,
        }
    
    def export_data(self, filepath: str, format: str = 'json') -> 'BrainMonitor':
        """
        Export monitoring data to file.
        
        Args:
            filepath: Path to export data
            format: Export format ('json', 'csv')
        
        Returns:
            Self for method chaining
        """
        if format == 'json':
            import json
            with open(filepath, 'w') as f:
                json.dump(self.data, f, indent=2)
        elif format == 'csv':
            import csv
            if self.data:
                with open(filepath, 'w', newline='') as f:
                    if self.data:
                        writer = csv.DictWriter(f, fieldnames=self.data[0].keys())
                        writer.writeheader()
                        writer.writerows(self.data)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return self
    
    def plot(self, save_path: Optional[str] = None) -> Any:
        """
        Create plots from monitoring data.
        
        Args:
            save_path: Optional path to save plot
        
        Returns:
            Matplotlib figure object
        """
        try:
            import matplotlib.pyplot as plt
            
            if not self.data:
                raise BrainError("No monitoring data available")
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('Brain Simulation Monitoring', fontsize=16)
            
            # Plot 1: Firing rate over time
            steps = [d['step'] for d in self.data]
            firing_rates = [d['average_firing_rate'] for d in self.data]
            axes[0, 0].plot(steps, firing_rates, linewidth=2)
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Average Firing Rate')
            axes[0, 0].set_title('Firing Rate Over Time')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Spike count histogram
            spike_counts = [d['firing_neuron_count'] for d in self.data]
            axes[0, 1].hist(spike_counts, bins=50, alpha=0.7, edgecolor='black')
            axes[0, 1].set_xlabel('Firing Neuron Count')
            axes[0, 1].set_ylabel('Frequency')
            axes[0, 1].set_title('Firing Neuron Count Distribution')
            axes[0, 1].grid(True, alpha=0.3)
            
            # Plot 3: Firing rate statistics
            ax = axes[1, 0]
            ax.hist(firing_rates, bins=50, alpha=0.7, edgecolor='black')
            ax.axvline(np.mean(firing_rates), color='red', linestyle='--', label=f"Mean: {np.mean(firing_rates):.2f}")
            ax.set_xlabel('Average Firing Rate')
            ax.set_ylabel('Frequency')
            ax.set_title('Firing Rate Statistics')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            # Plot 4: Progress (steps vs time)
            duration = len(self.data) / (time.time() - self._start_time) if (time.time() - self._start_time) > 0 else 0
            axes[1, 1].bar(['Steps', 'Duration (s)', 'Rate (steps/s)'], 
                          [len(self.data), time.time() - self._start_time, duration],
                          color=['skyblue', 'lightgreen', 'salmon'])
            axes[1, 1].set_ylabel('Value')
            axes[1, 1].set_title('Simulation Summary')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=300, bbox_inches='tight')
            
            return fig
            
        except ImportError:
            raise BrainError("matplotlib not available. Install with: pip install matplotlib")


class Region:
    """
    Brain region wrapper for Pythonic access.
    
    Provides Pythonic interface to brain regions with:
    - Property-based access
    - Statistical methods
    - Neuron and synapse management
    - Visual representation
    
    Examples:
        Region access:
            brain = createBrain(config)
            region = brain.get_region(1)
            
            print(f"Region has {region.get_neuron_count()} neurons")
            print(f"Average firing rate: {region.get_average_firing_rate()}")
            
            # Access neurons
            for neuron in region.get_neurons():
                print(f"Neuron {neuron.get_id()}: {neuron.get_state()}")
        
        Region statistics:
            stats = region.get_statistics()
            print(f"Region statistics: {stats}")
    """
    
    def __init__(self, region_instance: Any, brain: Brain):
        """
        Initialize region wrapper.
        
        Args:
            region_instance: The underlying C++ region instance
            brain: Parent brain instance
        """
        self._region = region_instance
        self._brain = brain
    
    def get_id(self) -> RegionId:
        """Get region ID."""
        return self._region.get_id()
    
    def get_name(self) -> str:
        """Get region name."""
        # This would need to be implemented based on C++ API
        return f"Region_{self.get_id()}"
    
    def get_neuron_count(self) -> int:
        """Get total neuron count in region."""
        return self._region.get_total_neuron_count()
    
    def get_synapse_count(self) -> int:
        """Get total synapse count in region."""
        return self._region.get_synapse_count()
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        return self._region.get_active_neuron_count()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate in region."""
        return self._region.get_average_firing_rate()
    
    def get_neurons(self) -> List['Neuron']:
        """Get all neurons in region."""
        # This would need to be implemented based on C++ API
        return []
    
    def get_populations(self) -> List['Population']:
        """Get all populations in region."""
        # This would need to be implemented based on C++ API
        return []
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get comprehensive region statistics.
        
        Returns:
            Dictionary with region statistics
        """
        return {
            'id': self.get_id(),
            'name': self.get_name(),
            'neuron_count': self.get_neuron_count(),
            'synapse_count': self.get_synapse_count(),
            'active_neuron_count': self.get_active_neuron_count(),
            'average_firing_rate': self.get_average_firing_rate(),
        }
    
    def print_status(self) -> None:
        """Print region status."""
        stats = self.get_statistics()
        print(f"Region {stats['id']} ({stats['name']}):")
        print(f"  Neurons: {stats['neuron_count']}")
        print(f"  Synapses: {stats['synapse_count']}")
        print(f"  Active neurons: {stats['active_neuron_count']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.4f}")


class Neuron:
    """
    Neuron wrapper with Pythonic interface.
    
    Provides Pythonic access to neuron properties and methods with:
    - Property-based access
    - State management
    - Event callbacks
    - Visualization support
    
    Examples:
        Neuron access:
            brain = createBrain(config)
            neuron = brain.get_region(1).get_neuron(1)
            
            print(f"Neuron ID: {neuron.get_id()}")
            print(f"Neuron type: {neuron.get_type()}")
            print(f"Membrane potential: {neuron.get_membrane_potential()}")
        
        Neuron control:
            neuron = brain.get_region(1).get_neuron(1)
            neuron.inject_current(10.0)
            neuron.set_membrane_potential(-65.0)
    """
    
    def __init__(self, neuron_instance: Any, region: Region, brain: Brain):
        """
        Initialize neuron wrapper.
        
        Args:
            neuron_instance: The underlying C++ neuron instance
            region: Parent region
            brain: Parent brain instance
        """
        self._neuron = neuron_instance
        self._region = region
        self._brain = brain
    
    def get_id(self) -> NeuronId:
        """Get neuron ID."""
        return self._neuron.get_id()
    
    def get_type(self) -> NeuronType:
        """Get neuron type."""
        type_value = self._neuron.get_type()
        return NeuronType(type_value)
    
    def get_state(self) -> Dict[str, Any]:
        """Get neuron state."""
        state = self._neuron.get_state()
        return {
            'membrane_potential': state.membranePotential,
            'threshold': state.threshold,
            'firing_state': state.firingState,
            'refractory_remaining': state.refractoryRemaining,
            'refractory_period': state.refractoryPeriod,
            'last_spike_time': state.lastSpikeTime,
            'resting_potential': state.restingPotential,
            'reset_potential': state.resetPotential,
            'leak_conductance': state.leakConductance,
        }
    
    def get_membrane_potential(self) -> float:
        """Get membrane potential."""
        return self._neuron.get_membrane_potential()
    
    def get_threshold(self) -> float:
        """Get firing threshold."""
        return self._neuron.get_threshold()
    
    def get_firing_state(self) -> FiringState:
        """Get firing state."""
        state = self._neuron.get_firing_state()
        return FiringState(state)
    
    def is_firing(self) -> bool:
        """Check if neuron is currently firing."""
        return self._neuron.is_firing()
    
    def inject_current(self, current: float) -> 'Neuron':
        """
        Inject current into neuron.
        
        Args:
            current: Current value to inject
        
        Returns:
            Self for method chaining
        """
        self._neuron.inject_current(current)
        return self
    
    def set_membrane_potential(self, potential: float) -> 'Neuron':
        """
        Set membrane potential.
        
        Args:
            potential: Membrane potential value
        
        Returns:
            Self for method chaining
        """
        self._neuron.set_membrane_potential(potential)
        return self
    
    def get_spike_history(self) -> List[Timestamp]:
        """Get spike history."""
        return self._neuron.get_spike_history()
    
    def get_last_spike_time(self) -> float:
        """Get last spike time."""
        return self._neuron.get_last_spike_time()


class Population:
    """
    Neuron population wrapper.
    
    Provides Pythonic interface to neuron populations with:
    - Population statistics
    - Neuron access
    - Activity monitoring
    - Visualization support
    """
    
    def __init__(self, population_instance: Any, region: Region):
        """
        Initialize population wrapper.
        
        Args:
            population_instance: The underlying C++ population instance
            region: Parent region
        """
        self._population = population_instance
        self._region = region
    
    def get_id(self) -> PopulationId:
        """Get population ID."""
        return self._population.get_id()
    
    def get_type(self) -> NeuronType:
        """Get neuron type for this population."""
        type_value = self._population.get_neuron_type()
        return NeuronType(type_value)
    
    def get_neuron_count(self) -> int:
        """Get number of neurons in population."""
        return self._population.get_neuron_count()
    
    def get_neurons(self) -> List[Neuron]:
        """Get all neurons in population."""
        # This would need to be implemented based on C++ API
        return []
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate of population."""
        return self._population.get_average_firing_rate()
    
    def get_activity_level(self) -> float:
        """Get overall activity level."""
        return self._population.get_activity_level()
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get population statistics."""
        return {
            'id': self.get_id(),
            'type': self.get_type().value,
            'neuron_count': self.get_neuron_count(),
            'average_firing_rate': self.get_average_firing_rate(),
            'activity_level': self.get_activity_level(),
        }


# Factory functions for backward compatibility
_brain_instance: Optional[Brain] = None
_world_instance: Optional[SimpleWorld] = None
_agent_instance: Optional[AgentBrain] = None
_config_instance: Optional[Config] = None

# Export all important classes and functions
__all__ = [
    # Core classes
    'Brain',
    'Config',
    'AgentBrain',
    'SimpleWorld',
    'Region',
    'Neuron',
    'Population',
    
    # Sensory and motor classes
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryPercept',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    
    # Enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Factory functions
    'create_default_config',
    'create_config',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Error classes
    'ConfigurationError',
    'BrainError',
    
    # Type definitions
    'ConfigKey',
    'ConfigValue',
    'SimulationStep',
    'Timestamp',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
]

# Create default instances (lazy initialization)
def get_default_config() -> Config:
    """Get default configuration instance."""
    global _config_instance
    if _config_instance is None:
        _config_instance = create_default_config()
    return _config_instance

def get_default_brain() -> Brain:
    """Get default brain instance (placeholder - should be created by user)."""
    # This is a placeholder - users should create their own brains
    raise NotImplementedError("Use createBrain() factory function instead")

def get_default_world() -> SimpleWorld:
    """Get default world instance."""
    global _world_instance
    if _world_instance is None:
        _world_instance = createSimpleWorld()
    return _world_instance

def get_default_agent() -> AgentBrain:
    """Get default agent instance (placeholder - should be created by user)."""
    # This is a placeholder - users should create their own agents
    raise NotImplementedError("Use createAgentBrain() factory function instead")
