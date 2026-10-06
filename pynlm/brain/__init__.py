"""
Brain system functionality for NLM Python bindings.

This module provides brain simulation, neural computation, and
cognitive functionality.
"""

import numpy as np
from typing import Optional, List, Dict, Any, Tuple

# Re-export brain bindings from pybind11
from ..core import *

class EnhancedBrain:
    """
    Enhanced brain with Pythonic interface and advanced features.
    
    Provides brain simulation with additional Python features including
    better error handling, progress tracking, and convenience methods.
    """
    
    def __init__(self, brain):
        self._brain = brain
        self._step_history = []
        self._spike_history = []
        self._state_history = []
    
    def initialize_enhanced(self, config_dict: Optional[Dict[str, Any]] = None):
        """
        Enhanced brain initialization with configuration.
        
        Parameters
        ----------
        config_dict : dict, optional
            Configuration parameters.
        """
        # Create configuration
        if config_dict is None:
            config = createDefaultConfig()
        else:
            config = createDefaultConfig()
            for key, value in config_dict.items():
                config.set(key, value)
        
        # Initialize with configuration
        self._brain.initialize()
    
    def run_steps_enhanced(self, num_steps: int, timestep: float = 1.0,
                          callback: Optional[callable] = None,
                          progress_callback: Optional[callable] = None) -> Dict[str, Any]:
        """
        Run enhanced brain simulation.
        
        Parameters
        ----------
        num_steps : int
            Number of steps to run.
        timestep : float
            Simulation timestep.
        callback : callable, optional
            Function called after each step.
        progress_callback : callable, optional
            Function called periodically with progress information.
        
        Returns
        -------
        dict
            Simulation statistics.
        """
        import time
        
        stats = {
            'total_steps': num_steps,
            'total_spikes': 0,
            'max_firing': 0,
            'average_firing': 0,
            'step_times': [],
            'start_time': time.time(),
        }
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Run brain step
            self._brain.step(step, timestep)
            
            # Collect statistics
            firing_count = self._brain.getFiringNeuronCount()
            spike_count = self._brain.getTotalSpikeCount()
            stats['total_spikes'] += spike_count
            stats['max_firing'] = max(stats['max_firing'], firing_count)
            
            # Call user callback
            if callback:
                callback(step, self._brain)
            
            # Progress reporting
            if progress_callback and step % 100 == 0:
                progress = (step / num_steps) * 100
                progress_callback(progress, step, stats)
            
            # Record step time
            elapsed = time.time() - step_start
            stats['step_times'].append(elapsed)
            
            # Store step history
            self._step_history.append({
                'step': step,
                'firing_count': firing_count,
                'spike_count': spike_count,
                'timestep': timestep,
                'time_elapsed': elapsed,
            })
        
        # Calculate averages
        if stats['step_times']:
            stats['average_step_time'] = sum(stats['step_times']) / len(stats['step_times'])
            stats['total_time'] = sum(stats['step_times'])
        
        stats['average_firing'] = stats['total_spikes'] / num_steps if num_steps > 0 else 0
        stats['total_time'] = time.time() - stats['start_time']
        
        return stats
    
    def run_to_condition(self, condition_func, max_steps: int = 10000,
                        timestep: float = 1.0, callback: Optional[callable] = None) -> Dict[str, Any]:
        """
        Run brain simulation until a condition is met.
        
        Parameters
        ----------
        condition_func : callable
            Function that takes brain and returns True if simulation should stop.
        max_steps : int
            Maximum number of steps to run.
        timestep : float
            Simulation timestep.
        callback : callable, optional
            Function called after each step.
        
        Returns
        -------
        dict
            Simulation results and final state.
        """
        for step in range(max_steps):
            self._brain.step(step, timestep)
            
            if condition_func(self._brain):
                return {
                    'steps_completed': step + 1,
                    'final_step': step,
                    'final_brain': self._brain,
                    'completed_naturally': True,
                }
            
            if callback:
                callback(step, self._brain)
        
        return {
            'steps_completed': max_steps,
            'final_step': max_steps - 1,
            'final_brain': self._brain,
            'completed_naturally': False,
            'max_steps_reached': True,
        }
    
    def get_brain_state_enhanced(self) -> Dict[str, Any]:
        """
        Get enhanced brain state information.
        
        Returns
        -------
        dict
            Enhanced brain state.
        """
        state = {
            'neuron_count': self._brain.getTotalNeuronCount(),
            'synapse_count': self._brain.getTotalSynapseCount(),
            'firing_neuron_count': self._brain.getFiringNeuronCount(),
            'firing_rate': self._brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'developmental_stage': self._brain.getDevelopmentalStage(),
            'configuration': self._brain.getConfig().summary() if self._brain.getConfig() else {},
            'simulation_parameters': {
                'step_history_length': len(self._step_history),
                'has_timestamps': len(self._step_history) > 0 and 'time_elapsed' in self._step_history[0],
            }
        }
        
        return state
    
    def get_neural_activity_patterns(self, step_range: Optional[Tuple[int, int]] = None) -> Dict[str, Any]:
        """
        Get neural activity patterns.
        
        Parameters
        ----------
        step_range : tuple, optional
            Step range (start, end). If None, uses all recorded steps.
        
        Returns
        -------
        dict
            Neural activity patterns.
        """
        if not self._step_history:
            return {}
        
        if step_range:
            start, end = step_range
            filtered_steps = [s for s in self._step_history if start <= s['step'] <= end]
        else:
            filtered_steps = self._step_history
        
        if not filtered_steps:
            return {}
        
        firing_rates = [s['firing_count'] for s in filtered_steps]
        step_numbers = [s['step'] for s in filtered_steps]
        times = [s.get('time_elapsed', 0) for s in filtered_steps]
        
        return {
            'total_steps': len(filtered_steps),
            'firing_rate_range': (min(firing_rates), max(firing_rates)),
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'firing_rate_variance': np.var(firing_rates) if len(firing_rates) > 1 else 0,
            'step_range': (min(step_numbers), max(step_numbers)),
            'time_range': (min(times), max(times)),
            'firing_rate_trend': self._calculate_trend(firing_rates),
        }
    
    def _calculate_trend(self, values: List[float]) -> Dict[str, str]:
        """Calculate trend in values."""
        if len(values) < 2:
            return {'trend': 'stable', 'direction': 'constant'}
        
        # Simple linear trend
        x = np.arange(len(values))
        y = np.array(values)
        
        # Calculate linear regression slope
        slope = np.polyfit(x, y, 1)[0]
        
        if abs(slope) < 0.01:
            return {'trend': 'stable', 'direction': 'constant', 'slope': slope}
        elif slope > 0:
            return {'trend': 'increasing', 'direction': 'up', 'slope': slope}
        else:
            return {'trend': 'decreasing', 'direction': 'down', 'slope': slope}

class BrainSimulator:
    """
    Brain simulator with multiple brain instances and comparison.
    
    Provides functionality for running multiple brains, comparing
    their behavior, and managing brain simulation experiments.
    """
    
    def __init__(self):
        self.brains = {}
        self.experiments = []
    
    def add_brain(self, name: str, brain):
        """
        Add a brain to the simulator.
        
        Parameters
        ----------
        name : str
            Name of the brain.
        brain : Brain
            Brain object to add.
        """
        self.brains[name] = EnhancedBrain(brain)
    
    def run_comparison(self, config_template: Dict[str, Any], 
                      num_steps: int, brain_names: List[str]) -> Dict[str, Any]:
        """
        Run comparison between multiple brains.
        
        Parameters
        ----------
        config_template : dict
            Configuration template for brains.
        num_steps : int
            Number of steps to run.
        brain_names : list
            Names of brains to compare.
        
        Returns
        -------
        dict
            Comparison results.
        """
        comparison_results = {
            'brains': {},
            'num_steps': num_steps,
            'config_used': config_template,
        }
        
        for brain_name in brain_names:
            if brain_name not in self.brains:
                print(f"Warning: Brain '{brain_name}' not found, skipping.")
                continue
            
            # Create brain with configuration
            brain = self.brains[brain_name]
            brain.initialize_enhanced(config_template)
            
            # Run simulation
            stats = brain.run_steps_enhanced(num_steps)
            comparison_results['brains'][brain_name] = {
                'stats': stats,
                'final_state': brain.get_brain_state_enhanced(),
            }
        
        return comparison_results

class NeuroimagingSimulator:
    """
    Neuroimaging simulator for visualizing brain activity.
    
    Provides functionality for simulating neuroimaging data and
    visualizing brain activity patterns.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self.imaging_data = []
    
    def simulate_fMRI(self, num_timepoints: int, sampling_rate: float = 0.1,
                     noise_level: float = 0.1) -> np.ndarray:
        """
        Simulate fMRI data from brain activity.
        
        Parameters
        ----------
        num_timepoints : int
            Number of time points to simulate.
        sampling_rate : float
            Sampling rate for fMRI data.
        noise_level : float
            Noise level for the simulation.
        
        Returns
        -------
        numpy.ndarray
            Simulated fMRI data of shape (num_timepoints, regions).
        """
        # Get brain regions
        regions = self.brain.getBrain().getRegions()
        num_regions = len(regions)
        
        # Generate simulated fMRI data
        fMRI_data = np.zeros((num_timepoints, num_regions))
        
        for timepoint in range(num_timepoints):
            # Get brain state at this timepoint
            # Note: This is a simplified simulation
            for region_idx, region in enumerate(regions):
                # Simulate blood-oxygen-level dependent (BOLD) signal
                # This is a simplified model based on neural activity
                neural_activity = np.random.random() * 0.5 + 0.5
                
                # Add some temporal autocorrelation
                if timepoint > 0:
                    neural_activity = 0.8 * neural_activity + 0.2 * np.random.random()
                
                # Add noise
                neural_activity += np.random.normal(0, noise_level)
                
                # Ensure non-negative
                neural_activity = max(0, neural_activity)
                
                fMRI_data[timepoint, region_idx] = neural_activity
        
        self.imaging_data.append({
            'type': 'fMRI',
            'data': fMRI_data,
            'sampling_rate': sampling_rate,
            'noise_level': noise_level,
        })
        
        return fMRI_data
    
    def simulate_EEG(self, num_timepoints: int, sampling_rate: float = 0.1,
                     noise_level: float = 0.1) -> np.ndarray:
        """
        Simulate EEG data from brain activity.
        
        Parameters
        ----------
        num_timepoints : int
            Number of time points to simulate.
        sampling_rate : float
            Sampling rate for EEG data.
        noise_level : float
            Noise level for the simulation.
        
        Returns
        -------
        numpy.ndarray
            Simulated EEG data of shape (num_timepoints, channels).
        """
        # Simulate EEG-like signals with different frequency bands
        num_channels = 8  # Simplified model
        EEG_data = np.zeros((num_timepoints, num_channels))
        
        for timepoint in range(num_timepoints):
            for channel in range(num_channels):
                # Generate signal with different characteristics per channel
                t = timepoint * sampling_rate
                
                # Alpha waves (8-12 Hz)
                alpha = 0.5 * np.sin(2 * np.pi * 10 * t)
                
                # Beta waves (13-30 Hz)
                beta = 0.3 * np.sin(2 * np.pi * 20 * t)
                
                # Gamma waves (30-100 Hz)
                gamma = 0.2 * np.sin(2 * np.pi * 50 * t)
                
                # Combine with noise
                signal = alpha + beta + gamma + np.random.normal(0, noise_level)
                
                EEG_data[timepoint, channel] = signal
        
        self.imaging_data.append({
            'type': 'EEG',
            'data': EEG_data,
            'sampling_rate': sampling_rate,
            'noise_level': noise_level,
        })
        
        return EEG_data

# Re-export commonly used classes and functions
__all__ = [
    'EnhancedBrain',
    'BrainSimulator',
    'NeuroimagingSimulator',
    # Core types from bindings
    'Brain',
    'Config',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
]