import numpy as np
import math
import random
import time
import json
import os
import sys
import warnings
from typing import List, Dict, Any, Tuple, Union, Optional
import logging

warnings.filterwarnings('ignore')

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class NLMUtilsError(Exception):
    """Exception raised for errors in the NLM utilities."""
    pass

class DataProcessor:
    """
    Utility class for processing neural data.
    """
    
    @staticmethod
    def normalize_data(data: np.ndarray, min_val: float = None, max_val: float = None) -> np.ndarray:
        """
        Normalize data to [0, 1] range.
        
        Args:
            data: Input data array
            min_val: Minimum value for normalization (if None, uses data min)
            max_val: Maximum value for normalization (if None, uses data max)
            
        Returns:
            Normalized data
        """
        if min_val is None:
            min_val = np.min(data)
        if max_val is None:
            max_val = np.max(data)
        
        if max_val == min_val:
            return np.zeros_like(data)
        
        return (data - min_val) / (max_val - min_val)
    
    @staticmethod
    def denormalize_data(data: np.ndarray, min_val: float, max_val: float) -> np.ndarray:
        """
        Denormalize data from [0, 1] range.
        
        Args:
            data: Normalized data array
            min_val: Original minimum value
            max_val: Original maximum value
            
        Returns:
            Denormalized data
        """
        return data * (max_val - min_val) + min_val
    
    @staticmethod
    def compute_statistics(data: np.ndarray) -> Dict[str, float]:
        """
        Compute basic statistics of data.
        
        Args:
            data: Input data array
            
        Returns:
            Dictionary containing statistics
        """
        return {
            'mean': np.mean(data),
            'std': np.std(data),
            'min': np.min(data),
            'max': np.max(data),
            'median': np.median(data),
            'variance': np.var(data),
            'range': np.max(data) - np.min(data)
        }
    
    @staticmethod
    def filter_data(data: np.ndarray, cutoff_freq: float, sampling_rate: float, filter_type: str = 'butterworth') -> np.ndarray:
        """
        Apply digital filter to data.
        
        Args:
            data: Input data array
            cutoff_freq: Cutoff frequency
            sampling_rate: Sampling rate
            filter_type: Type of filter ('butterworth', 'chebyshev', 'elliptic')
            
        Returns:
            Filtered data
        """
        from scipy import signal
        
        nyquist = 0.5 * sampling_rate
        normalized_cutoff = cutoff_freq / nyquist
        
        if filter_type == 'butterworth':
            b, a = signal.butter(4, normalized_cutoff, btype='low')
        elif filter_type == 'chebyshev':
            b, a = signal.cheby1(4, 0.5, normalized_cutoff, btype='low')
        elif filter_type == 'elliptic':
            b, a = signal.ellip(4, 0.5, normalized_cutoff, btype='low')
        else:
            raise ValueError(f"Unknown filter type: {filter_type}")
        
        return signal.filtfilt(b, a, data)
    
    @staticmethod
    def smooth_data(data: np.ndarray, window_size: int = 5) -> np.ndarray:
        """
        Apply moving average smoothing to data.
        
        Args:
            data: Input data array
            window_size: Size of smoothing window (must be odd)
            
        Returns:
            Smoothed data
        """
        if window_size % 2 == 0:
            window_size += 1
        
        return np.convolve(data, np.ones(window_size)/window_size, mode='same')
    
    @staticmethod
    def detect_spikes(data: np.ndarray, threshold: float, min_distance: int = 1) -> np.ndarray:
        """
        Detect spikes in data.
        
        Args:
            data: Input data array
            threshold: Threshold for spike detection
            min_distance: Minimum distance between spikes
            
        Returns:
            Array of spike indices
        """
        from scipy import signal
        
        # Find peaks above threshold
        peaks, _ = signal.find_peaks(data, height=threshold, distance=min_distance)
        return peaks

class TimeSeriesGenerator:
    """
    Utility class for generating time series data.
    """
    
    @staticmethod
    def generate_lif_activity(neuron_count: int, duration: int, firing_rate: float = 10.0, noise: float = 0.1) -> np.ndarray:
        """
        Generate Leaky Integrate-and-Fire (LIF) neuron activity.
        
        Args:
            neuron_count: Number of neurons
            duration: Duration of activity (time steps)
            firing_rate: Target firing rate (Hz)
            noise: Amount of noise to add
            
        Returns:
            2D array of shape (duration, neuron_count) with spike times
        """
        spike_times = np.zeros((duration, neuron_count), dtype=bool)
        
        for neuron in range(neuron_count):
            # Generate Poisson spike train
            expected_spikes = firing_rate * duration / 1000.0  # Convert Hz to per timestep
            spike_times[:, neuron] = np.random.poisson(expected_spikes, duration) > 0
            
            # Add some temporal correlations
            for t in range(1, duration):
                if spike_times[t, neuron] and spike_times[t-1, neuron]:
                    # Keep some spikes close together
                    if random.random() < 0.3:
                        spike_times[t, neuron] = True
        
        return spike_times
    
    @staticmethod
    def generate_synaptic_weights(neuron_count: int, density: float = 0.1, mean: float = 0.5, std: float = 0.1) -> np.ndarray:
        """
        Generate random synaptic weight matrix.
        
        Args:
            neuron_count: Number of neurons
            density: Connection density (0 to 1)
            mean: Mean weight value
            std: Standard deviation of weights
            
        Returns:
            Weight matrix
        """
        weights = np.zeros((neuron_count, neuron_count))
        
        # Create sparse connectivity matrix
        for i in range(neuron_count):
            for j in range(neuron_count):
                if i != j and random.random() < density:
                    weights[i, j] = random.gauss(mean, std)
        
        return weights

class ConfigurationValidator:
    """
    Utility class for validating configurations and data.
    """
    
    @staticmethod
    def validate_config(config_dict: Dict[str, Any]) -> Tuple[bool, List[str]]:
        """
        Validate configuration dictionary.
        
        Args:
            config_dict: Configuration dictionary
            
        Returns:
            Tuple of (is_valid, list_of_errors)
        """
        errors = []
        
        # Check for required sections
        required_sections = ['brain', 'plasticity', 'neuromod', 'cognition']
        for section in required_sections:
            if section not in config_dict:
                errors.append(f"Missing required section: {section}")
        
        # Validate brain configuration
        if 'brain' in config_dict:
            brain_config = config_dict['brain']
            if 'neuron_count' not in brain_config:
                errors.append("brain section must contain 'neuron_count'")
            elif not isinstance(brain_config['neuron_count'], (int, float)) or brain_config['neuron_count'] <= 0:
                errors.append("brain.neuron_count must be a positive number")
            
            if 'synapse_density' in brain_config and (brain_config['synapse_density'] < 0 or brain_config['synapse_density'] > 1):
                errors.append("brain.synapse_density must be between 0 and 1")
        
        # Validate plasticity configuration
        if 'plasticity' in config_dict:
            plasticity_config = config_dict['plasticity']
            if 'stdp' in plasticity_config and plasticity_config['stdp'].get('enable', False):
                if 'learning_rate' not in plasticity_config['stdp']:
                    errors.append("plasticity.stdp must contain 'learning_rate' when enabled")
        
        return len(errors) == 0, errors
    
    @staticmethod
    def generate_default_config() -> Dict[str, Any]:
        """
        Generate default configuration.
        
        Returns:
            Default configuration dictionary
        """
        return {
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'connection_probability': 0.05,
                'initial_weight_mean': 0.5,
                'initial_weight_std': 0.1,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
                'tau_mem': 20.0,
                'tau_ref': 2.0
            },
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.001,
                    'tau_plus': 20.0,
                    'tau_minus': 20.0
                },
                'hebbian': {
                    'enable': True,
                    'learning_rate': 0.001
                },
                'structural': {
                    'enable': True,
                    'growth_rate': 0.001,
                    'pruning_rate': 0.0001
                }
            },
            'neuromod': {
                'dopamine': {
                    'scale': 1.0
                },
                'curiosity': {
                    'enable': True,
                    'scale': 1.0
                },
                'novelty': {
                    'enable': True,
                    'threshold': 0.5
                },
                'prediction_error': {
                    'scale': 1.0
                }
            }
        }

class ProgressTracker:
    """
    Utility class for tracking progress of long-running operations.
    """
    
    def __init__(self, total_steps: int, update_interval: int = 100):
        """
        Initialize progress tracker.
        
        Args:
            total_steps: Total number of steps
            update_interval: Number of steps between progress updates
        """
        self.total_steps = total_steps
        self.update_interval = update_interval
        self.current_step = 0
        self.start_time = time.time()
        self.last_update_time = self.start_time
    
    def update(self, steps: int = 1):
        """
        Update progress.
        
        Args:
            steps: Number of steps completed
        """
        self.current_step += steps
        current_time = time.time()
        
        if (self.current_step % self.update_interval == 0 or 
            self.current_step == self.total_steps or
            current_time - self.last_update_time >= 5.0):
            
            progress_percent = (self.current_step / self.total_steps) * 100
            elapsed_time = current_time - self.start_time
            estimated_total_time = elapsed_time * (self.total_steps / self.current_step) if self.current_step > 0 else 0
            remaining_time = estimated_total_time - elapsed_time
            
            logger.info(f"Progress: {self.current_step}/{self.total_steps} ({progress_percent:.1f}%) "
                       f"Elapsed: {elapsed_time:.1f}s, Remaining: {remaining_time:.1f}s")
            
            self.last_update_time = current_time
    
    def complete(self):
        """Mark progress as complete."""
        self.current_step = self.total_steps
        elapsed_time = time.time() - self.start_time
        logger.info(f"Completed in {elapsed_time:.1f} seconds")

class CheckpointManager:
    """
    Utility class for managing checkpoints of simulation state.
    """
    
    def __init__(self, checkpoint_dir: str = "checkpoints"):
        """
        Initialize checkpoint manager.
        
        Args:
            checkpoint_dir: Directory to store checkpoints
        """
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
    
    def save_checkpoint(self, filename: str, data: Dict[str, Any]):
        """
        Save checkpoint data.
        
        Args:
            filename: Checkpoint filename
            data: Data to save
        """
        filepath = os.path.join(self.checkpoint_dir, filename)
        
        # Add metadata
        checkpoint = {
            'timestamp': time.time(),
            'version': '1.0',
            'data': data
        }
        
        with open(filepath, 'w') as f:
            json.dump(checkpoint, f, indent=2, default=str)
        
        logger.info(f"Checkpoint saved: {filepath}")
    
    def load_checkpoint(self, filename: str) -> Dict[str, Any]:
        """
        Load checkpoint data.
        
        Args:
            filename: Checkpoint filename
            
        Returns:
            Loaded checkpoint data
        """
        filepath = os.path.join(self.checkpoint_dir, filename)
        
        if not os.path.exists(filepath):
            raise NLMUtilsError(f"Checkpoint file not found: {filepath}")
        
        with open(filepath, 'r') as f:
            checkpoint = json.load(f)
        
        logger.info(f"Checkpoint loaded: {filepath}")
        return checkpoint['data']
    
    def list_checkpoints(self) -> List[str]:
        """
        List all checkpoints in directory.
        
        Returns:
            List of checkpoint filenames
        """
        if not os.path.exists(self.checkpoint_dir):
            return []
        
        return [f for f in os.listdir(self.checkpoint_dir) 
                if f.endswith('.json')]

class BatchProcessor:
    """
    Utility class for processing data in batches.
    """
    
    @staticmethod
    def process_batch(data: List[Any], processor_func, batch_size: int = 100) -> List[Any]:
        """
        Process data in batches.
        
        Args:
            data: Data to process
            processor_func: Function to apply to each batch
            batch_size: Size of each batch
            
        Returns:
            List of processed results
        """
        results = []
        
        for i in range(0, len(data), batch_size):
            batch = data[i:i + batch_size]
            batch_result = processor_func(batch)
            results.extend(batch_result)
            
            # Log progress
            if (i + batch_size) % (batch_size * 10) == 0:
                logger.info(f"Processed {i + batch_size}/{len(data)} items")
        
        return results

# Performance monitoring utilities
def get_memory_usage() -> float:
    """
    Get current memory usage in MB.
    
    Returns:
        Memory usage in MB
    """
    try:
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
    except ImportError:
        return 0.0

def get_cpu_usage() -> float:
    """
    Get current CPU usage percentage.
    
    Returns:
        CPU usage percentage
    """
    try:
        import psutil
        return psutil.cpu_percent()
    except ImportError:
        return 0.0

def time_operation(operation_func, *args, **kwargs):
    """
    Time execution of an operation.
    
    Args:
        operation_func: Function to time
        *args: Arguments to pass to function
        **kwargs: Keyword arguments to pass to function
        
    Returns:
        Tuple of (result, execution_time)
    """
    start_time = time.time()
    result = operation_func(*args, **kwargs)
    end_time = time.time()
    
    return result, end_time - start_time

# Context managers
class NLMContextManager:
    """
    Context manager for NLM operations with automatic cleanup.
    """
    
    def __init__(self, brain, world=None):
        """
        Initialize context manager.
        
        Args:
            brain: Brain instance
            world: World instance (optional)
        """
        self.brain = brain
        self.world = world
    
    def __enter__(self):
        """Enter context."""
        if not self.brain._initialized:
            self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context and cleanup."""
        # Reset brain for reuse
        self.brain.reset()
        if self.world:
            self.world.reset()


__all__ = [
    'DataProcessor',
    'TimeSeriesGenerator',
    'ConfigurationValidator',
    'ProgressTracker',
    'CheckpointManager',
    'BatchProcessor',
    'NLMUtilsError',
    'get_memory_usage',
    'get_cpu_usage',
    'time_operation',
    'NLMContextManager'
]
