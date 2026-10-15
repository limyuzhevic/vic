# Advanced utilities and simulation tools
# This module provides advanced utilities for working with NLM simulations

import time
import numpy as np
from typing import Dict, Any, List, Optional, Callable, Tuple
from enum import Enum
from .core import NLMError, ValidationError, SimulationError
from .brain import BrainSession, PythonicBrain
from .world import SensoryData
from .agent import ActionInterface
class SimulationRunner:
    """
    Advanced simulation runner with progress tracking and monitoring.
    
    This class provides advanced simulation capabilities including:
    - Progress tracking and reporting
    - Real-time monitoring
    - Performance benchmarking
    - Checkpoint management
    """
    
    def __init__(self, progress_callback: Optional[Callable] = None):
        """
        Initialize simulation runner.
        
        Args:
            progress_callback: Optional callback for progress updates
        """
        self.progress_callback = progress_callback
        self.checkpoint_interval = 100
        
    def run_with_monitoring(self, brain, world, agent, steps: int, 
                           timestep: float = 0.1, **kwargs) -> Dict[str, Any]:
        """
        Run simulation with comprehensive monitoring.
        
        Args:
            brain: Brain object
            world: World object
            agent: AgentBrain object
            steps: Number of simulation steps
            timestep: Time step duration
            **kwargs: Additional configuration
            
        Returns:
            Dictionary with monitoring data
        """
        start_time = time.time()
        session = BrainSession(brain, world, agent)
        
        # Setup monitoring
        monitoring_data = {
            'steps': steps,
            'timestep': timestep,
            'start_time': start_time,
            'checkpoint_interval': self.checkpoint_interval,
            'progress': [],
            'checkpoints': [],
        }
        
        # Run simulation with monitoring
        for i in range(steps):
            # Update world
            world.update(timestep)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input and get action
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics at checkpoints
            if (i + 1) % self.checkpoint_interval == 0:
                checkpoint_data = {
                    'step': i + 1,
                    'time': i * timestep,
                    'neurons_firing': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'simulation_time': world.getSimulationTime(),
                }
                monitoring_data['checkpoints'].append(checkpoint_data)
                
            # Update progress
            if self.progress_callback:
                progress = (i + 1) / steps
                self.progress_callback(i + 1, progress, monitoring_data['checkpoints'][-1] if monitoring_data['checkpoints'] else None)
                
        # Calculate final statistics
        end_time = time.time()
        duration = end_time - start_time
        
        monitoring_data.update({
            'end_time': end_time,
            'duration': duration,
            'steps_per_second': steps / duration if duration > 0 else 0,
            'final_stats': session.get_stats(),
        })
        
        return monitoring_data
        
    def run_episode_with_callbacks(self, brain, world, agent, steps: int,
                                   timestep: float = 0.1, **kwargs) -> Dict[str, Any]:
        """
        Run episode with various callbacks and hooks.
        
        Args:
            brain: Brain object
            world: World object
            agent: AgentBrain object
            steps: Number of simulation steps
            timestep: Time step duration
            **kwargs: Additional configuration including callbacks
            
        Returns:
            Dictionary with episode results
        """
        on_step_callback = kwargs.get('on_step', lambda step, brain, world, agent: None)
        on_checkpoint_callback = kwargs.get('on_checkpoint', lambda step, stats: None)
        before_simulation_callback = kwargs.get('before_simulation', lambda: None)
        after_simulation_callback = kwargs.get('after_simulation', lambda stats: None)
        
        before_simulation_callback()
        
        episode_data = {
            'steps': steps,
            'timestep': timestep,
            'events': [],
            'checkpoints': [],
            'final_stats': None,
        }
        
        for i in range(steps):
            # Pre-step callback
            step_data = on_step_callback(i, brain, world, agent)
            if step_data:
                episode_data['events'].append({
                    'type': 'step_callback',
                    'step': i,
                    'data': step_data,
                })
            
            # Run simulation step
            world.update(timestep)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Checkpoint callback
            if (i + 1) % self.checkpoint_interval == 0:
                checkpoint_data = {
                    'step': i + 1,
                    'time': i * timestep,
                    'neurons_firing': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                }
                episode_data['checkpoints'].append(checkpoint_data)
                on_checkpoint_callback(i + 1, checkpoint_data)
                
        after_simulation_callback(episode_data['final_stats'])
        
        return episode_data
class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    This class provides detailed performance profiling including:
    - Time profiling
    - Memory profiling
    - Bottleneck analysis
    - Optimization recommendations
    """
    
    def __init__(self):
        """Initialize performance profiler."""
        self.timings = {}
        self.memory_usage = []
        
    def profile_operation(self, name: str, func: Callable, *args, **kwargs) -> Any:
        """
        Profile execution of a single operation.
        
        Args:
            name: Name of operation
            func: Function to profile
            *args: Arguments to pass to function
            **kwargs: Keyword arguments to pass to function
            
        Returns:
            Result of function execution
        """
        start_time = time.time()
        
        try:
            result = func(*args, **kwargs)
            
            end_time = time.time()
            duration = end_time - start_time
            
            self.timings[name] = duration
            
            return result
            
        except Exception as e:
            end_time = time.time()
            duration = end_time - start_time
            
            self.timings[name] = duration
            raise e
            
    def get_timing_report(self) -> Dict[str, Any]:
        """
        Get timing report.
        
        Returns:
            Dictionary with timing information
        """
        if not self.timings:
            return {'message': 'No timing data available'}
            
        total_time = sum(self.timings.values())
        
        report = {
            'total_time': total_time,
            'operations': len(self.timings),
            'average_time': total_time / len(self.timings) if self.timings else 0,
            'operations': self.timings,
        }
        
        # Identify potential bottlenecks
        if total_time > 0:
            slowest_ops = sorted(self.timings.items(), key=lambda x: x[1], reverse=True)[:3]
            report['slowest_operations'] = [
                {'name': name, 'time': time, 'percentage': (time / total_time) * 100}
                for name, time in slowest_ops
            ]
            
        return report
        
    def reset(self):
        """Reset profiler data."""
        self.timings.clear()
        self.memory_usage.clear()
class ExperimentManager:
    """
    Experiment manager for running controlled experiments.
    
    This class provides tools for running systematic experiments including:
    - Parameter sweeps
    - Random sampling
    - Controlled experiments
    - Result aggregation
    """
    
    def __init__(self):
        """Initialize experiment manager."""
        self.experiments = []
        
    def add_parameter(self, name: str, values: List[Any]):
        """
        Add a parameter for experiments.
        
        Args:
            name: Parameter name
            values: List of values to test
        """
        # This would be used to define experiment parameters
        pass
        
    def run_grid_search(self, brain_factory: Callable, world_factory: Callable,
                       agent_factory: Callable, param_grid: Dict[str, List[Any]],
                       evaluation_metric: str = 'final_spike_count') -> List[Dict[str, Any]]:
        """
        Run grid search experiment.
        
        Args:
            brain_factory: Factory function for creating brain objects
            world_factory: Factory function for creating world objects
            agent_factory: Factory function for creating agent objects
            param_grid: Dictionary mapping parameter names to lists of values
            evaluation_metric: Metric to optimize
            
        Returns:
            List of experiment results
        """
        from itertools import product
        
        # Generate all combinations of parameters
        param_names = list(param_grid.keys())
        param_values = list(product(*param_grid.values()))
        
        results = []
        
        for i, param_combo in enumerate(param_values):
            # Create parameter dictionary
            params = dict(zip(param_names, param_combo))
            
            # Create experiment objects
            brain = brain_factory(params)
            world = world_factory(params)
            agent = agent_factory(params)
            
            # Run experiment
            session = BrainSession(brain, world, agent)
            try:
                session.run_simulation(params.get('steps', 100), params.get('timestep', 0.1))
                stats = session.get_stats()
                
                # Record result
                result = {
                    'parameters': params,
                    'metrics': stats,
                    'success': True,
                }
                
                if evaluation_metric in stats:
                    result[f'optimized_{evaluation_metric}'] = stats[evaluation_metric]
                    
                results.append(result)
                
            except Exception as e:
                # Record failed experiment
                results.append({
                    'parameters': params,
                    'error': str(e),
                    'success': False,
                })
                
        return results
        
    def run_random_sampling(self, brain_factory: Callable, world_factory: Callable,
                           agent_factory: Callable, n_samples: int,
                           param_distributions: Dict[str, Callable],
                           evaluation_metric: str = 'final_spike_count') -> List[Dict[str, Any]]:
        """
        Run random sampling experiment.
        
        Args:
            brain_factory: Factory function for creating brain objects
            world_factory: Factory function for creating world objects
            agent_factory: Factory function for creating agent objects
            n_samples: Number of samples to generate
            param_distributions: Dictionary mapping parameter names to distribution functions
            evaluation_metric: Metric to optimize
            
        Returns:
            List of experiment results
        """
        import random
        
        results = []
        
        for i in range(n_samples):
            # Sample parameters from distributions
            params = {}
            for name, distribution in param_distributions.items():
                params[name] = distribution()
                
            # Create experiment objects
            brain = brain_factory(params)
            world = world_factory(params)
            agent = agent_factory(params)
            
            # Run experiment
            session = BrainSession(brain, world, agent)
            try:
                session.run_simulation(params.get('steps', 100), params.get('timestep', 0.1))
                stats = session.get_stats()
                
                # Record result
                result = {
                    'parameters': params,
                    'metrics': stats,
                    'success': True,
                }
                
                if evaluation_metric in stats:
                    result[f'evaluated_{evaluation_metric}'] = stats[evaluation_metric]
                    
                results.append(result)
                
            except Exception as e:
                # Record failed experiment
                results.append({
                    'parameters': params,
                    'error': str(e),
                    'success': False,
                })
                
        return results
class SimulationVisualizer:
    """
    Advanced visualization tools for NLM simulations.
    
    This class provides visualization capabilities including:
    - Neural activity plots
    - Spike raster plots
    - Network connectivity visualization
    - Performance monitoring
    """
    
    @staticmethod
    def create_spike_raster_plot(brain, filename: str = 'spike_raster.png'):
        """
        Create spike raster plot.
        
        Args:
            brain: Brain object
            filename: Output filename
            
        Returns:
            Matplotlib figure (if matplotlib available)
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Get spike data
            # This would get actual spike data from the brain
            # For now, return None to indicate not implemented
            return None
            
        except ImportError:
            print("matplotlib not available for spike raster plot")
            return None
            
    @staticmethod
    def create_neural_activity_plot(brain, neurons: Optional[List[int]] = None, 
                                   filename: str = 'neural_activity.png'):
        """
        Create neural activity plot.
        
        Args:
            brain: Brain object
            neurons: List of specific neurons to plot (if None, plot all)
            filename: Output filename
            
        Returns:
            Matplotlib figure (if matplotlib available)
        """
        try:
            import matplotlib.pyplot as plt
            
            # Create activity plot
            # This would create actual neural activity plot
            # For now, return None to indicate not implemented
            return None
            
        except ImportError:
            print("matplotlib not available for neural activity plot")
            return None
            
    @staticmethod
    def create_network_connectivity_plot(brain, region_id=None, filename: str = 'network_connectivity.png'):
        """
        Create network connectivity plot.
        
        Args:
            brain: Brain object
            region_id: Specific region to visualize (if None, show whole network)
            filename: Output filename
            
        Returns:
            Matplotlib figure (if matplotlib available)
        """
        try:
            import matplotlib.pyplot as plt
            import networkx as nx
            
            # Create network visualization
            # This would create actual network connectivity plot
            # For now, return None to indicate not implemented
            return None
            
        except ImportError:
            print("matplotlib or networkx not available for network connectivity plot")
            return None
            
    @staticmethod
    def create_performance_monitor(brain, world, agent, duration: float = 10.0, 
                                   update_interval: float = 0.1):
        """
        Create performance monitoring visualization.
        
        Args:
            brain: Brain object
            world: World object
            agent: AgentBrain object
            duration: Duration to monitor in seconds
            update_interval: Update interval in seconds
            
        Returns:
            Dictionary with monitoring data
        """
        start_time = time.time()
        monitoring_data = {
            'time': [],
            'neurons_firing': [],
            'total_spikes': [],
            'firing_rate': [],
            'simulation_time': [],
        }
        
        steps = int(duration / update_interval)
        
        for i in range(steps):
            current_time = time.time() - start_time
            
            # Run simulation step
            world.update(update_interval)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            monitoring_data['time'].append(current_time)
            monitoring_data['neurons_firing'].append(brain.getFiringNeuronCount())
            monitoring_data['total_spikes'].append(brain.getTotalSpikeCount())
            monitoring_data['firing_rate'].append(brain.getAverageFiringRate())
            monitoring_data['simulation_time'].append(world.getSimulationTime())
            
        return monitoring_data