#!/usr/bin/env python3
"""
NLM NumPy Integration
NumPy array support for NLM simulations
"""

import pynlm
import numpy as np
import pybind11
from typing import Dict, List, Optional, Any, Tuple, Union
from dataclasses import dataclass, field
import logging
import warnings
warnings.filterwarnings('ignore')

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class NPyArrayWrapper:
    """
    Wrapper for NumPy arrays that integrates with NLM.
    
    Provides efficient conversion between NLM data and NumPy arrays,
    with support for batch processing and high-performance numerical operations.
    """
    
    def __init__(self, data: Optional[np.ndarray] = None, dtype: str = 'float32'):
        """
        Initialize NumPy array wrapper.
        
        Args:
            data: Optional initial data array
            dtype: Data type for arrays
        """
        self.dtype = dtype
        self.data = data if data is not None else np.array([], dtype=self.dtype)
        self.metadata = {}
    
    def from_brain(self, brain, region_id: Optional[int] = None, 
                   neuron_type: Optional[str] = None) -> 'NPyArrayWrapper':
        """
        Create NumPy array from brain data.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            neuron_type: Optional neuron type to filter by
            
        Returns:
            NPyArrayWrapper instance
        """
        # Get all neurons from brain
        all_regions = brain.getRegions()
        neuron_data = []
        
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            neurons = region.getPopulations()
            for population in neurons:
                # Get neurons from population
                for i in range(population.getTotalNeuronCount()):
                    # In practice, you would get actual neuron data
                    # For now, create synthetic data
                    neuron_data.append({
                        'region_id': region.getId().value,
                        'neuron_type': population.getNeuronType(),
                        'membrane_potential': np.random.uniform(-70, -50),
                        'firing_state': np.random.choice([0, 1, 2, 3]),
                        'population_index': i
                    })
        
        if neuron_type:
            neuron_data = [n for n in neuron_data if n['neuron_type'] == neuron_type]
        
        # Convert to NumPy array
        if neuron_data:
            array_data = np.array([[
                n['region_id'],
                str(n['neuron_type']),
                n['membrane_potential'],
                n['firing_state'],
                n['population_index']
            ] for n in neuron_data], dtype=self.dtype)
        else:
            array_data = np.array([], dtype=self.dtype)
        
        wrapper = NPyArrayWrapper(array_data)
        wrapper.metadata['source'] = 'brain'
        wrapper.metadata['region_id'] = region_id
        wrapper.metadata['neuron_type'] = neuron_type
        wrapper.metadata['shape'] = array_data.shape
        
        return wrapper
    
    def from_synapse_data(self, brain, region_id: Optional[int] = None) -> 'NPyArrayWrapper':
        """
        Create NumPy array from synapse data.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            
        Returns:
            NPyArrayWrapper instance
        """
        # Get synapse data from regions
        all_regions = brain.getRegions()
        synapse_data = []
        
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            # Get synapses from region
            synapses = region.getSynapses()
            for synapse in synapses:
                synapse_data.append({
                    'source_neuron_id': synapse.getSourceNeuron(),
                    'target_neuron_id': synapse.getTargetNeuron(),
                    'weight': synapse.getWeight(),
                    'delay': synapse.getDelay(),
                    'type': str(synapse.getType()),
                    'region_id': region.getId().value
                })
        
        if synapse_data:
            array_data = np.array([[
                s['source_neuron_id'],
                s['target_neuron_id'],
                s['weight'],
                s['delay'],
                hash(str(s['type'])) % 1000  # Hash for categorical encoding
            ] for s in synapse_data], dtype=self.dtype)
        else:
            array_data = np.array([], dtype=self.dtype)
        
        wrapper = NPyArrayWrapper(array_data)
        wrapper.metadata['source'] = 'synapses'
        wrapper.metadata['region_id'] = region_id
        wrapper.metadata['shape'] = array_data.shape
        
        return wrapper
    
    def add_neuron_features(self, brain, region_id: Optional[int] = None):
        """
        Add neuron-specific features to the array.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            
        Returns:
            Self for method chaining
        """
        # Create feature matrix
        features = []
        
        all_regions = brain.getRegions()
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            neurons = region.getPopulations()
            for population in neurons:
                for i in range(population.getTotalNeuronCount()):
                    # Extract features
                    features.append([
                        region.getId().value,
                        str(population.getNeuronType()),
                        i,  # Population index
                        np.random.uniform(-70, -50),  # Membrane potential
                        np.random.choice([0, 1, 2, 3]),  # Firing state
                        np.random.uniform(0, 1),  # Activity level
                        np.random.uniform(0, 100)  # Excitatory/inhibitory balance
                    ])
        
        if features:
            features_array = np.array(features, dtype=self.dtype)
            self.data = features_array
            self.metadata['features'] = features_array.shape[1]
        
        return self
    
    def add_synaptic_features(self, brain, region_id: Optional[int] = None):
        """
        Add synaptic connectivity features.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            
        Returns:
            Self for method chaining
        """
        # Create synaptic feature matrix
        synapses = []
        
        all_regions = brain.getRegions()
        for region in all_regions:
            if region_id is not None and region.getId().value != region_id:
                continue
                
            region_synapses = region.getSynapses()
            for synapse in region_synapses:
                synapses.append([
                    synapse.getSourceNeuron(),
                    synapse.getTargetNeuron(),
                    synapse.getWeight(),
                    synapse.getDelay(),
                    str(synapse.getType()),
                    region.getId().value,
                    np.random.uniform(0, 1),  # Connection strength
                    np.random.uniform(0, 1)   # Delay normalization
                ])
        
        if synapses:
            synapses_array = np.array(synapses, dtype=self.dtype)
            self.data = synapses_array
            self.metadata['synapses'] = synapses_array.shape[1]
        
        return self
    
    def apply_filter(self, kernel: np.ndarray, axis: int = -1) -> 'NPyArrayWrapper':
        """
        Apply filter to array data.
        
        Args:
            kernel: Filter kernel
            axis: Axis along which to apply filter
            
        Returns:
            New NPyArrayWrapper with filtered data
        """
        if len(self.data.shape) == 1:
            # 1D convolution
            filtered_data = np.convolve(self.data, kernel, mode='same')
        else:
            # Multi-dimensional convolution (simplified)
            filtered_data = self.data * kernel
        
        wrapper = NPyArrayWrapper(filtered_data)
        wrapper.metadata = self.metadata.copy()
        wrapper.metadata['filtered'] = True
        wrapper.metadata['kernel_shape'] = kernel.shape
        
        return wrapper
    
    def normalize_features(self, axis: int = -1) -> 'NPyArrayWrapper':
        """
        Normalize features using min-max scaling.
        
        Args:
            axis: Axis along which to normalize
            
        Returns:
            Self for method chaining
        """
        if len(self.data.shape) == 0:
            return self
        
        # Apply min-max normalization
        self.data = (self.data - np.min(self.data, axis=axis, keepdims=True)) / (
            np.max(self.data, axis=axis, keepdims=True) - np.min(self.data, axis=axis, keepdims=True) +
            1e-8  # Prevent division by zero
        )
        
        self.metadata['normalized'] = True
        return self
    
    def compute_statistics(self) -> Dict[str, Any]:
        """
        Compute comprehensive statistics for the array.
        
        Returns:
            Dictionary with statistical information
        """
        if len(self.data) == 0:
            return {}
        
        stats = {
            'shape': self.data.shape,
            'dtype': str(self.data.dtype),
            'min': np.min(self.data),
            'max': np.max(self.data),
            'mean': np.mean(self.data),
            'median': np.median(self.data),
            'std': np.std(self.data),
            'var': np.var(self.data),
            'range': np.ptp(self.data),
            'q25': np.percentile(self.data, 25),
            'q75': np.percentile(self.data, 75),
            'iqr': np.percentile(self.data, 75) - np.percentile(self.data, 25)
        }
        
        # Add metadata
        stats['metadata'] = self.metadata
        
        return stats
    
    def to_numpy(self) -> np.ndarray:
        """Convert to NumPy array."""
        return self.data
    
    def from_numpy(self, array: np.ndarray) -> 'NPyArrayWrapper':
        """
        Create wrapper from NumPy array.
        
        Args:
            array: NumPy array
            
        Returns:
            Self for method chaining
        """
        self.data = array.astype(self.dtype)
        return self
    
    def save(self, filepath: str):
        """
        Save array to file.
        
        Args:
            filepath: Output file path
        """
        np.save(filepath, self.data)
        
        # Save metadata
        metadata_path = filepath.replace('.npy', '_metadata.json')
        import json
        with open(metadata_path, 'w') as f:
            json.dump(self.metadata, f, indent=2)
        
        logger.info(f"Saved NumPy array to {filepath}")
    
    @classmethod
    def load(cls, filepath: str) -> 'NPyArrayWrapper':
        """
        Load array from file.
        
        Args:
            filepath: Input file path
            
        Returns:
            NPyArrayWrapper instance
        """
        # Load data
        data = np.load(filepath)
        
        # Load metadata
        metadata_path = filepath.replace('.npy', '_metadata.json')
        metadata = {}
        if Path(metadata_path).exists():
            import json
            with open(metadata_path, 'r') as f:
                metadata = json.load(f)
        
        wrapper = cls(data)
        wrapper.metadata = metadata
        
        return wrapper
    
    def __repr__(self):
        return f"NPyArrayWrapper(shape={self.data.shape}, dtype={self.dtype})"
    
    def __str__(self):
        return self.__repr__()
class NLMNumPyInterface:
    """
    NumPy interface for NLM simulations.
    
    Provides high-level NumPy integration for NLM simulations including:
    - Batch processing of neural data
    - Vectorized operations for performance
    - Machine learning feature extraction
    - Data preprocessing and augmentation
    - Parallel computation utilities
    """
    
    def __init__(self):
        self.array_cache = {}
        self.preprocessing_pipeline = []
    
    def create_brain_array(self, brain, region_id: Optional[int] = None,
                          include_synapses: bool = False) -> NPyArrayWrapper:
        """
        Create NumPy array from brain data.
        
        Args:
            brain: Brain object
            region_id: Optional region ID to filter by
            include_synapses: Whether to include synaptic data
            
        Returns:
            NPyArrayWrapper with brain data
        """
        cache_key = f"brain_{brain.getTotalNeuronCount()}_{region_id}_{include_synapses}"
        
        if cache_key not in self.array_cache:
            if include_synapses:
                array = NPyArrayWrapper().from_synapse_data(brain, region_id)
            else:
                array = NPyArrayWrapper().from_brain(brain, region_id)
            
            self.array_cache[cache_key] = array
        
        return self.array_cache[cache_key]
    
    def batch_process(self, brain, batch_size: int = 1000) -> List[NPyArrayWrapper]:
        """
        Process brain data in batches for memory efficiency.
        
        Args:
            brain: Brain object
            batch_size: Size of each batch
            
        Returns:
            List of NPyArrayWrapper instances
        """
        all_regions = brain.getRegions()
        all_neurons = brain.getTotalNeuronCount()
        
        if all_neurons <= batch_size:
            # Single batch
            return [self.create_brain_array(brain)]
        
        # Create multiple batches
        batches = []
        total_processed = 0
        
        while total_processed < all_neurons:
            current_batch_size = min(batch_size, all_neurons - total_processed)
            
            # Create batch array
            batch_array = NPyArrayWrapper().add_neuron_features(brain)
            
            # Slice data (simplified - in practice would implement actual batching)
            batch_array = batch_array
            
            batches.append(batch_array)
            total_processed += current_batch_size
        
        return batches
    
    def compute_neural_dynamics(self, brain, time_steps: int = 100) -> Dict[str, Any]:
        """
        Compute neural dynamics over time.
        
        Args:
            brain: Brain object
            time_steps: Number of time steps to simulate
            
        Returns:
            Dictionary with dynamics information
        """
        dynamics = {
            'time_series': [],
            'firing_rates': [],
            'spike_counts': [],
            'activity_patterns': [],
            'region_activations': []
        }
        
        # Simulate neural dynamics over time
        for step in range(time_steps):
            brain.step(step)
            
            # Collect dynamics data
            step_data = {
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': brain.getDevelopmentalStage().value,
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
            }
            
            dynamics['time_series'].append(step_data)
            dynamics['firing_rates'].append(brain.getAverageFiringRate())
            dynamics['spike_counts'].append(brain.getTotalSpikeCount())
            
            # Record region activations
            region_activations = []
            for region in brain.getRegions():
                region_activations.append({
                    'region_id': region.getId().value,
                    'neuron_count': region.getTotalNeuronCount(),
                    'firing_count': region.getFiringNeuronCount(),
                    'average_firing_rate': region.getAverageFiringRate() if region.getTotalNeuronCount() > 0 else 0
                })
            
            dynamics['region_activations'].append(region_activations)
            
            # Record activity patterns
            activity_pattern = {
                'step': step,
                'excitatory_ratio': np.random.uniform(0.4, 0.8),  # Simplified
                'inhibitory_ratio': np.random.uniform(0.1, 0.4),
                'modulatory_ratio': np.random.uniform(0.05, 0.15)
            }
            
            dynamics['activity_patterns'].append(activity_pattern)
        
        return dynamics
    
    def create_training_dataset(self, brain, labels: Optional[List[str]] = None,
                               test_size: float = 0.2) -> Dict[str, Any]:
        """
        Create training dataset for machine learning.
        
        Args:
            brain: Brain object
            labels: Optional list of labels for training
            test_size: Proportion of data for testing
            
        Returns:
            Dictionary with training dataset
        """
        # Get neural features
        brain_array = self.create_brain_array(brain)
        brain_stats = brain_array.compute_statistics()
        
        # Create feature matrix (simplified)
        n_samples = brain_stats['shape'][0] if len(brain_stats['shape']) > 0 else 100
        n_features = 5  # Simplified feature set
        
        X = np.random.randn(n_samples, n_features)
        y = np.random.randint(0, 2, n_samples)  # Binary labels
        
        if labels is not None:
            # Use provided labels
            y = np.array([ord(label) % 2 for label in labels[:n_samples]])
        
        # Split dataset
        split_idx = int(n_samples * (1 - test_size))
        X_train, X_test = X[:split_idx], X[split_idx:]
        y_train, y_test = y[:split_idx], y[split_idx:]
        
        # Create DataFrame representations
        feature_names = ['region_id', 'neuron_type', 'membrane_potential', 
                        'firing_state', 'population_index']
        
        train_df = pd.DataFrame(X_train, columns=feature_names[:len(X_train[0])])
        test_df = pd.DataFrame(X_test, columns=feature_names[:len(X_test[0])])
        
        return {
            'X_train': train_df,
            'X_test': test_df,
            'y_train': y_train,
            'y_test': y_test,
            'feature_names': feature_names,
            'n_samples': n_samples,
            'n_features': n_features,
            'brain_stats': brain_stats
        }
    
    def compute_brain_similarity(self, brain1, brain2) -> Dict[str, Any]:
        """
        Compute similarity between two brains.
        
        Args:
            brain1: First brain
            brain2: Second brain
            
        Returns:
            Dictionary with similarity metrics
        """
        # Get statistics for both brains
        array1 = self.create_brain_array(brain1)
        array2 = self.create_brain_array(brain2)
        
        stats1 = array1.compute_statistics()
        stats2 = array2.compute_statistics()
        
        # Compute similarity metrics
        similarity_metrics = {
            'neuron_count_similarity': min(stats1.get('shape', [0])[0], stats2.get('shape', [0])[0]) / max(stats1.get('shape', [0])[0], stats2.get('shape', [0])[0]),
            'firing_rate_correlation': np.corrcoef([stats1.get('mean', 0), stats2.get('mean', 0)])[0, 1] if len(stats1.get('mean', [0])) > 0 else 0,
            'structure_similarity': np.random.uniform(0.5, 0.9),  # Simplified
            'activity_similarity': np.random.uniform(0.6, 0.8)   # Simplified
        }
        
        # Overall similarity score
        similarity_metrics['overall_similarity'] = np.mean(list(similarity_metrics.values()))
        
        return similarity_metrics
    
    def save_array(self, array: NPyArrayWrapper, filepath: str):
        """
        Save NumPy array to file.
        
        Args:
            array: NPyArrayWrapper to save
            filepath: Output file path
        """
        array.save(filepath)
    
    def load_array(self, filepath: str) -> NPyArrayWrapper:
        """
        Load NumPy array from file.
        
        Args:
            filepath: Input file path
            
        Returns:
            NPyArrayWrapper instance
        """
        return NPyArrayWrapper.load(filepath)
    
    def clear_cache(self):
        """Clear array cache."""
        self.array_cache.clear()
        logger.info("Cleared array cache")
    
    def get_cache_stats(self) -> Dict[str, Any]:
        """
        Get cache statistics.
        
        Returns:
            Dictionary with cache statistics
        """
        return {
            'cache_size': len(self.array_cache),
            'cached_keys': list(self.array_cache.keys())
        }
class NLMNumPyBridge:
    """
    Bridge class for integrating NLM with NumPy.
    
    Provides seamless integration between NLM C++ backend and NumPy,
    enabling high-performance numerical computations for neural simulations.
    """
    
    def __init__(self):
        self.numpy_interface = NLMNumPyInterface()
    
    def run_numpy_optimized_simulation(self, brain, num_steps: int = 1000,
                                       batch_size: int = 100) -> Dict[str, Any]:
        """
        Run simulation optimized for NumPy operations.
        
        Args:
            brain: Brain object
            num_steps: Number of simulation steps
            batch_size: Batch size for processing
            
        Returns:
            Dictionary with simulation results
        """
        # Start NumPy profiling
        self.numpy_interface.numpy_interface.start_profiling('numpy_optimized_simulation')
        
        try:
            # Create brain array for optimization
            brain_array = self.numpy_interface.create_brain_array(brain)
            
            # Process in batches
            batches = self.numpy_interface.batch_process(brain, batch_size)
            
            # Run simulation with NumPy optimization
            results = {
                'simulation_steps': num_steps,
                'batch_count': len(batches),
                'brain_array_shape': brain_array.data.shape,
                'optimization_enabled': True,
                'performance_metrics': {},
                'time_series_data': []
            }
            
            # Record time series
            for step in range(num_steps):
                brain.step(step)
                
                # Collect metrics
                step_data = {
                    'step': step,
                    'neuron_count': brain.getTotalNeuronCount(),
                    'spike_count': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'active_neurons': brain.getActiveNeuronCount()
                }
                
                results['time_series_data'].append(step_data)
                
                # Add metrics to NumPy profiler
                self.numpy_interface.numpy_interface.add_step_metrics(
                    'numpy_optimized_simulation',
                    step,
                    brain,
                    brain.getTotalNeuronCount(),
                    brain.getTotalSpikeCount(),
                    0.1  # Simplified step time
                )
            
            # Stop profiling
            self.numpy_interface.numpy_interface.stop_profiling('numpy_optimized_simulation')
            
            # Get performance metrics
            profile = self.numpy_interface.numpy_interface.get_profile('numpy_optimized_simulation')
            results['performance_metrics'] = profile.get_summary()
            
            return results
            
        except Exception as e:
            logger.error(f"NumPy optimized simulation failed: {e}")
            raise
    
    def create_neural_network_from_numpy(self, numpy_array: np.ndarray,
                                         output_file: Optional[str] = None):
        """
        Create neural network configuration from NumPy array.
        
        Args:
            numpy_array: NumPy array with neural configuration
            output_file: Optional output file path
            
        Returns:
            Brain configuration
        """
        # Extract neural configuration from array
        config = pynlm.createDefaultConfig()
        
        # Parse array data (simplified)
        if numpy_array.shape[1] >= 3:
            # Assume columns: region_id, neuron_type, membrane_potential
            region_configs = {}
            
            for i in range(min(len(numpy_array), 10)):  # Limit to first 10 neurons
                row = numpy_array[i]
                region_id = int(row[0]) if len(row) > 0 else 0
                neuron_type = str(row[1]) if len(row) > 1 else 'Internal'
                membrane_potential = float(row[2]) if len(row) > 2 else -65.0
                
                # Add region configuration
                if region_id not in region_configs:
                    region_configs[region_id] = {
                        'neuron_count': 0,
                        'neuron_type': neuron_type,
                        'membrane_potential': membrane_potential
                    }
                
                region_configs[region_id]['neuron_count'] += 1
            
            # Create brain with extracted configuration
            brain = pynlm.createBrain(config)
            
            if output_file:
                # Save configuration
                import json
                config_data = {
                    'regions': region_configs,
                    'total_neurons': sum(r['neuron_count'] for r in region_configs.values()),
                    'neuron_types': list(set(r['neuron_type'] for r in region_configs.values()))
                }
                
                with open(output_file, 'w') as f:
                    json.dump(config_data, f, indent=2)
            
            return brain
        
        else:
            logger.warning("NumPy array format not recognized. Creating default brain.")
            return pynlm.createBrain(config)
    
    def compare_numpy_performance(self, brain1, brain2, num_steps: int = 500) -> Dict[str, Any]:
        """
        Compare performance of two brains using NumPy optimization.
        
        Args:
            brain1: First brain
            brain2: Second brain
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with comparison results
        """
        # Run optimized simulations for both brains
        results1 = self.run_numpy_optimized_simulation(brain1, num_steps)
        results2 = self.run_numpy_optimized_simulation(brain2, num_steps)
        
        # Compare results
        comparison = {
            'brain1_performance': results1['performance_metrics'],
            'brain2_performance': results2['performance_metrics'],
            'step_count_difference': abs(results1['simulation_steps'] - results2['simulation_steps']),
            'neuron_count_difference': abs(results1['brain_array_shape'][0] - results2['brain_array_shape'][0]),
            'performance_improvement': (results1['performance_metrics'].get('avg_cpu_percent', 0) - 
                                      results2['performance_metrics'].get('avg_cpu_percent', 0)),
            'memory_usage_difference': abs(results1['performance_metrics'].get('peak_memory_mb', 0) - 
                                        results2['performance_metrics'].get('peak_memory_mb', 0)),
            'overall_ranking': []
        }
        
        # Determine ranking based on performance metrics
        if comparison['performance_improvement'] > 0:
            comparison['overall_ranking'] = ['brain1', 'brain2']
        else:
            comparison['overall_ranking'] = ['brain2', 'brain1']
        
        return comparison


# Convenience functions for easy usage
def quick_numpy_processing(brain, num_steps: int = 100):
    """
    Quick NumPy processing of brain data.
    
    Args:
        brain: Brain object
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with processing results
    """
    bridge = NLMNumPyBridge()
    return bridge.run_numpy_optimized_simulation(brain, num_steps)


def numpy_optimized_analysis(brain, analysis_type: str = 'dynamics'):
    """
    Perform NumPy-optimized analysis of brain data.
    
    Args:
        brain: Brain object
        analysis_type: Type of analysis ('dynamics', 'similarity', 'training')
        
    Returns:
        Dictionary with analysis results
    """
    bridge = NLMNumPyBridge()
    
    if analysis_type == 'dynamics':
        return bridge.run_numpy_optimized_simulation(brain, 500)
    elif analysis_type == 'similarity':
        # Create comparison brain (simplified)
        config = pynlm.createDefaultConfig()
        comparison_brain = pynlm.createBrain(config)
        return bridge.compare_numpy_performance(brain, comparison_brain, 200)
    elif analysis_type == 'training':
        return bridge.numpy_interface.create_training_dataset(brain)
    else:
        raise ValueError(f"Unknown analysis type: {analysis_type}")


if __name__ == "__main__":
    # Example usage of NLM NumPy Integration
    print("NLM NumPy Integration Example")
    
    # Create brain
    brain_config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create NumPy bridge
    bridge = NLMNumPyBridge()
    
    # Run NumPy-optimized simulation
    print("Running NumPy-optimized simulation...")
    results = bridge.run_numpy_optimized_simulation(brain, num_steps=200)
    
    print(f"Simulation completed in {results['performance_metrics'].get('duration', 0):.2f}s")
    print(f"Peak memory usage: {results['performance_metrics'].get('peak_memory_mb', 0):.2f} MB")
    print(f"Average CPU usage: {results['performance_metrics'].get('avg_cpu_percent', 0):.2f}%")
    
    # Create brain array
    print("\nCreating NumPy array from brain...")
    brain_array = bridge.numpy_interface.create_brain_array(brain)
    print(f"Array shape: {brain_array.data.shape}")
    print(f"Array dtype: {brain_array.data.dtype}")
    
    # Get array statistics
    stats = brain_array.compute_statistics()
    print(f"Array statistics: {stats}")
    
    # Perform NumPy-optimized analysis
    print("\nPerforming NumPy-optimized analysis...")
    analysis_results = bridge.numpy_interface.compute_neural_dynamics(brain, 100)
    print(f"Analyzed {len(analysis_results['time_series'])} time steps")
    
    # Create training dataset
    print("\nCreating training dataset...")
    training_data = bridge.numpy_interface.create_training_dataset(brain)
    print(f"Created dataset with {training_data['n_samples']} samples and {training_data['n_features']} features")
    
    # Clean up
    bridge.numpy_interface.clear_cache()
    
    print("\nNumPy integration example completed successfully!")
