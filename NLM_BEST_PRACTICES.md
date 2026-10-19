# NLM - Best Practices Guide

## Overview

This document provides comprehensive best practices for working with the enhanced NLM (Neural Learning Machine) project. It covers performance optimization, error handling, memory management, and simulation design patterns to help users create efficient, robust, and maintainable neural simulations.

## Table of Contents

- [Performance Optimization Strategies](#performance-optimization-strategies)
  - [Memory Management](#memory-management)
  - [CPU Optimization](#cpu-optimization)
  - [I/O Optimization](#io-optimization)

- [Error Handling Best Practices](#error-handling-best-practices)
  - [Exception Hierarchy](#exception-hierarchy)
  - [Error Recovery](#error-recovery)
  - [Logging and Monitoring](#logging-and-monitoring)

- [Memory Management Guidelines](#memory-management-guidelines)
  - [Memory Allocation](#memory-allocation)
  - [Memory Pooling](#memory-pooling)
  - [Garbage Collection](#garbage-collection)

- [Simulation Design Patterns](#simulation-design-patterns)
  - [Factory Pattern](#factory-pattern)
  - [Template Pattern](#template-pattern)
  - [Strategy Pattern](#strategy-pattern)

## Performance Optimization Strategies

### 1. Memory Management

#### Use Efficient Data Structures

```python
# Use numpy arrays instead of Python lists
import numpy as np

# Use float32 instead of float64 when possible
activity_matrix = np.zeros((num_neurons, num_synapses), dtype=np.float32)

# Use vectorized operations for better performance
np_array = np.random.rand(10000, 10)
result = np_array.mean(axis=1)  # Vectorized, faster than loops

# Use efficient serialization for large datasets
import h5py
with h5py.File('large_dataset.h5', 'w') as f:
    f.create_dataset('data', data=data, compression='gzip')
```

#### Use Memory Pools

```python
import numpy as np
from typing import List

class MemoryPool:
    """Memory pool for reusing allocated memory."""
    
    def __init__(self, dtype=np.float32, shape=(1000, 100)):
        self.dtype = dtype
        self.shape = shape
        self.pool = []
        self.max_pool_size = 10
    
    def acquire(self) -> np.ndarray:
        """Acquire memory from pool or allocate new."""
        if self.pool:
            return self.pool.pop()
        else:
            return np.zeros(self.shape, dtype=self.dtype)
    
    def release(self, array: np.ndarray):
        """Release memory back to pool."""
        if len(self.pool) < self.max_pool_size:
            array.fill(0)  # Clear array
            self.pool.append(array)

# Usage example
memory_pool = MemoryPool()
for i in range(100):
    arr = memory_pool.acquire()
    # Use array
    memory_pool.release(arr)
```

#### Monitor Memory Usage

```python
import psutil
import gc

def monitor_memory_usage():
    """Monitor and manage memory usage."""
    
    process = psutil.Process()
    memory_info = process.memory_info()
    
    print(f"Memory usage: RSS={memory_info.rss / 1024 / 1024:.1f} MB, "
          f"VMS={memory_info.vms / 1024 / 1024:.1f} MB")
    
    # Force garbage collection if memory usage is high
    if memory_info.rss > 2 * 1024 * 1024 * 1024:  # 2GB
        print("Memory usage high, forcing garbage collection...")
        gc.collect()
    
    return memory_info

# Continuous monitoring
def continuous_memory_monitor(interval=60):
    """Continuously monitor memory usage."""
    import time
    
    while True:
        monitor_memory_usage()
        time.sleep(interval)
```

### 2. CPU Optimization

#### Use Parallel Processing

```python
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import numpy as np

def parallel_simulation(param_sets, brain_factory):
    """
    Run simulations in parallel using thread pool.
    
    Args:
        param_sets: List of parameter sets
        brain_factory: Function to create brain with given parameters
        
    Returns:
        List of simulation results
    """
    def run_simulation(param_set):
        brain = brain_factory(param_set)
        brain.initialize()
        
        for step in range(200):
            brain.step(step, step * 0.01)
        
        return {
            'parameters': param_set,
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    # Use thread pool for I/O-bound operations
    with ThreadPoolExecutor(max_workers=4) as executor:
        results = list(executor.map(run_simulation, param_sets))
    
    return results

# Usage example
param_sets = [
    {'neuron_count': 5000, 'learning_rate': 0.01},
    {'neuron_count': 8000, 'learning_rate': 0.005},
    {'neuron_count': 3000, 'learning_rate': 0.02}
]

results = parallel_simulation(param_sets, brain_factory)
```

#### Use Vectorized Operations

```python
import numpy as np
import time

def vectorized_computation():
    """Demonstrate vectorized operations for performance."""
    
    # Large dataset
    n_samples, n_features = 10000, 100
    X = np.random.randn(n_samples, n_features)
    y = np.random.randn(n_samples)
    
    # Vectorized computation (fast)
    start_time = time.time()
    coefficients = np.linalg.lstsq(X, y, rcond=None)[0]
    vectorized_time = time.time() - start_time
    
    # Loop-based computation (slow)
    start_time = time.time()
    coefficients_loop = np.zeros(n_features)
    for i in range(n_samples):
        for j in range(n_features):
            coefficients_loop[j] += X[i, j] * y[i]
    loop_time = time.time() - start_time
    
    print(f"Vectorized time: {vectorized_time:.4f}s")
    print(f"Loop-based time: {loop_time:.4f}s")
    print(f"Speedup: {loop_time/vectorized_time:.1f}x")
    
    return coefficients, coefficients_loop

# Cache frequently used values
class ComputationCache:
    """Cache for expensive computations."""
    
    def __init__(self):
        self.cache = {}
    
    def compute(self, key, computation_func):
        """Compute value with caching."""
        if key not in self.cache:
            self.cache[key] = computation_func()
        return self.cache[key]
    
    def clear(self):
        """Clear cache."""
        self.cache.clear()

# Usage example
cache = ComputationCache()
result = cache.compute('mean_data', lambda: np.random.randn(1000).mean())
```

### 3. I/O Optimization

#### Batch File Operations

```python
import os
import glob
import h5py
import json

def batch_process_files(file_pattern, process_func):
    """
    Process multiple files in batch.
    
    Args:
        file_pattern: Pattern to match files
        process_func: Function to process each file
    """
    files = glob.glob(file_pattern)
    print(f"Processing {len(files)} files...")
    
    results = []
    for filepath in files:
        result = process_func(filepath)
        results.append(result)
    
    return results

# Example file processing function
def process_h5_file(filepath):
    """Process HDF5 file and return summary."""
    with h5py.File(filepath, 'r') as f:
        # Read data
        data = f['data'][:]
        
        # Compute statistics
        stats = {
            'filepath': filepath,
            'shape': data.shape,
            'mean': float(np.mean(data)),
            'std': float(np.std(data)),
            'min': float(np.min(data)),
            'max': float(np.max(data))
        }
    
    return stats

# Batch process HDF5 files
results = batch_process_files('./data/*.h5', process_h5_file)

# Write results to summary file
with open('./data/summary.json', 'w') as f:
    json.dump(results, f, indent=2)
```

#### Use Efficient File Formats

```python
import pandas as pd
import numpy as np

def save_large_dataset(data, filepath):
    """
    Save large dataset in efficient format.
    
    Args:
        data: Data to save
        filepath: Output file path
    """
    if isinstance(data, np.ndarray):
        # Use Parquet for columnar data (good for analytics)
        df = pd.DataFrame(data)
        df.to_parquet(filepath, index=False)
        
    elif isinstance(data, pd.DataFrame):
        # Save as Parquet (efficient for storage and query)
        data.to_parquet(filepath, index=False)
        
    else:
        # Save as CSV (universal format)
        pd.DataFrame(data).to_csv(filepath, index=False)

def load_large_dataset(filepath):
    """
    Load large dataset from efficient format.
    
    Args:
        filepath: Input file path
        
    Returns:
        Loaded data
    """
    # Try Parquet first (most efficient)
    if filepath.endswith('.parquet'):
        return pd.read_parquet(filepath)
    
    # Try CSV
    if filepath.endswith('.csv'):
        return pd.read_csv(filepath)
    
    # Try HDF5
    if filepath.endswith('.h5'):
        with h5py.File(filepath, 'r') as f:
            return f['data'][:]
    
    raise ValueError(f"Unsupported file format: {filepath}")
```

## Error Handling Best Practices

### 1. Exception Hierarchy

```python
from nlm_enhanced import *

# Define specific exception types for different error scenarios
class NLMSimulationError(NLMException):
    """Base class for simulation errors."""
    pass

class ConfigurationError(NLMException):
    """Errors related to configuration."""
    pass

class BrainInitializationError(ConfigurationError):
    """Errors during brain initialization."""
    pass

class SimulationStepError(NLMSimulationError):
    """Errors during simulation steps."""
    pass

class MemoryAllocationError(NLMSimulationError):
    """Errors during memory allocation."""
    pass

# Use specific exceptions in code
def create_brain_safely(config):
    """Create brain with comprehensive error handling."""
    
    try:
        # Validate configuration
        if not config.validate():
            raise ConfigurationError("Invalid configuration")
        
        # Create brain
        brain = createBrain(config)
        brain.initialize()
        return brain
        
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        # Recover by loading default configuration
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        return brain
        
    except BrainInitializationError as e:
        print(f"Brain initialization error: {e}")
        # Re-initialize with default settings
        brain.initialize()
        return brain
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        # Log error and return None
        log_error(e)
        return None
```

### 2. Error Recovery

```python
from nlm_enhanced import *

def robust_simulation_runner(brain_factory, config, max_retries=3):
    """
    Run simulation with robust error recovery.
    
    Args:
        brain_factory: Function to create brain
        config: Configuration for brain
        max_retries: Maximum number of retries
        
    Returns:
        Tuple of (success, brain, error_info)
    """
    
    for attempt in range(max_retries + 1):
        try:
            # Create brain
            brain = brain_factory(config)
            
            # Run simulation
            for step in range(1000):
                brain.step(step, step * 0.01)
            
            return True, brain, None
            
        except MemoryAllocationError as e:
            if attempt < max_retries:
                print(f"Memory error (attempt {attempt + 1}/{max_retries + 1}): {e}")
                print("Reducing network size and retrying...")
                config.set("brain.neuron_count", config.get("brain.neuron_count", 1000) // 2)
                continue
            else:
                print(f"Max retries reached for memory error: {e}")
                return False, None, {'type': 'memory', 'error': str(e), 'attempts': attempt}
                
        except BrainInitializationError as e:
            if attempt < max_retries:
                print(f"Brain initialization error (attempt {attempt + 1}/{max_retries + 1}): {e}")
                print("Re-initializing with default settings...")
                config = createDefaultConfig()
                continue
            else:
                print(f"Max retries reached for initialization error: {e}")
                return False, None, {'type': 'initialization', 'error': str(e), 'attempts': attempt}
                
        except Exception as e:
            if attempt < max_retries:
                print(f"Unexpected error (attempt {attempt + 1}/{max_retries + 1}): {e}")
                continue
            else:
                print(f"Max retries reached for error: {e}")
                return False, None, {'type': 'unknown', 'error': str(e), 'attempts': attempt}
    
    return False, None, {'type': 'max_retries_exceeded', 'attempts': max_retries}

# Usage example
success, brain, error_info = robust_simulation_runner(
    brain_factory=lambda config: createBrain(config),
    config=config,
    max_retries=3
)

if success:
    print("Simulation completed successfully!")
else:
    print(f"Simulation failed: {error_info}")
```

### 3. Logging and Monitoring

```python
import logging
import time
from datetime import datetime

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('simulation.log'),
        logging.StreamHandler()
    ]
)

class SimulationLogger:
    """Comprehensive simulation logger."""
    
    def __init__(self, log_file='simulation.log'):
        self.logger = logging.getLogger('simulation')
        self.log_file = log_file
        self.start_time = time.time()
        self.step_count = 0
        self.error_count = 0
    
    def log_simulation_step(self, step, time, brain):
        """Log simulation step with detailed metrics."""
        self.step_count += 1
        
        metrics = {
            'step': step,
            'time': time,
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'memory_usage': self.get_memory_usage(),
            'cpu_usage': self.get_cpu_usage()
        }
        
        self.logger.info(f"Step {step}: {metrics}")
        
        # Log to file for performance analysis
        with open(self.log_file, 'a') as f:
            f.write(f"{datetime.now().isoformat()} Step {step}: "
                   f"Active={metrics['active_neurons']}, "
                   f"Firing={metrics['firing_rate']:.3f}, "
                   f"E/I={metrics['e_i_ratio']:.3f}, "
                   f"Memory={metrics['memory_usage']:.1f}MB, "
                   f"CPU={metrics['cpu_usage']:.1f}%\n")
    
    def log_error(self, error, step=None):
        """Log error with context."""
        self.error_count += 1
        error_info = {
            'timestamp': time.time(),
            'step': step,
            'error_type': type(error).__name__,
            'error_message': str(error),
            'simulation_progress': f"Step {step}/{self.step_count}" if self.step_count > 0 else "Not started"
        }
        
        self.logger.error(f"Error: {error_info}")
    
    def get_memory_usage(self):
        """Get current memory usage in MB."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
    
    def get_cpu_usage(self):
        """Get current CPU usage in percent."""
        import psutil
        return psutil.cpu_percent()
    
    def generate_report(self):
        """Generate simulation report."""
        end_time = time.time()
        duration = end_time - self.start_time
        
        report = {
            'total_steps': self.step_count,
            'error_count': self.error_count,
            'duration_seconds': duration,
            'steps_per_second': self.step_count / duration if duration > 0 else 0,
            'start_time': datetime.fromtimestamp(self.start_time).isoformat(),
            'end_time': datetime.fromtimestamp(end_time).isoformat()
        }
        
        return report

# Usage example
logger = SimulationLogger()

# In simulation loop
for step in range(1000):
    time_val = step * 0.01
    brain.step(step, time_val)
    logger.log_simulation_step(step, time_val, brain)

# Generate final report
report = logger.generate_report()
print(f"Simulation completed: {report}")
```

## Memory Management Guidelines

### 1. Memory Allocation

```python
import numpy as np
import gc
from weakref import WeakValueDictionary

class SmartMemoryManager:
    """Smart memory manager for neural simulations."""
    
    def __init__(self, max_memory_mb=2048):
        self.max_memory_mb = max_memory_mb
        self.current_memory_mb = 0
        self.temporary_objects = WeakValueDictionary()
        self.large_arrays = []
    
    def allocate_array(self, shape, dtype=np.float32):
        """
        Allocate array with memory monitoring.
        
        Args:
            shape: Array shape
            dtype: Array data type
            
        Returns:
            Allocated array
        """
        # Estimate memory usage
        element_size = np.dtype(dtype).itemsize
        estimated_memory_mb = np.prod(shape) * element_size / (1024 * 1024)
        
        if self.current_memory_mb + estimated_memory_mb > self.max_memory_mb:
            print(f"Warning: Memory limit would be exceeded. "
                  f"Current: {self.current_memory_mb:.1f}MB, "
                  f"Estimated: {estimated_memory_mb:.1f}MB, "
                  f"Max: {self.max_memory_mb}MB")
            self.force_garbage_collection()
        
        # Allocate array
        array = np.zeros(shape, dtype=dtype)
        self.current_memory_mb += estimated_memory_mb
        self.large_arrays.append(array)
        
        return array
    
    def release_array(self, array):
        """
        Release array and free memory.
        
        Args:
            array: Array to release
        """
        if array in self.large_arrays:
            self.large_arrays.remove(array)
            # Note: numpy arrays don't need explicit free
            # They will be garbage collected when no references
    
    def register_temporary_object(self, key, obj):
        """
        Register temporary object for automatic cleanup.
        
        Args:
            key: Object key
            obj: Object to register
        """
        self.temporary_objects[key] = obj
    
    def cleanup_temporary_objects(self):
        """Cleanup temporary objects."""
        self.temporary_objects.clear()
        gc.collect()
    
    def force_garbage_collection(self):
        """Force garbage collection."""
        gc.collect()
        self.current_memory_mb = self.estimate_current_memory()
    
    def estimate_current_memory(self):
        """Estimate current memory usage."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / (1024 * 1024)
    
    def clear_all(self):
        """Clear all allocated memory."""
        self.large_arrays.clear()
        self.cleanup_temporary_objects()
        self.force_garbage_collection()

# Usage example
memory_manager = SmartMemoryManager(max_memory_mb=2048)

# Allocate arrays
for i in range(10):
    arr = memory_manager.allocate_array((1000, 1000), dtype=np.float32)
    # Use array
    memory_manager.release_array(arr)

print(f"Peak memory usage: {memory_manager.current_memory_mb:.1f}MB")
```

### 2. Memory Pooling

```python
import numpy as np
from typing import List, Optional

class MemoryPool:
    """Memory pool for reusing allocated memory."""
    
    def __init__(self, dtype=np.float32, max_pool_size=10):
        self.dtype = dtype
        self.max_pool_size = max_pool_size
        self.pool = []
    
    def acquire(self, shape: tuple) -> np.ndarray:
        """
        Acquire memory from pool or allocate new.
        
        Args:
            shape: Array shape
            
        Returns:
            Array from pool or new allocation
        """
        if self.pool and len(self.pool) > 0:
            # Reuse existing array
            array = self.pool.pop()
            array.fill(0)  # Clear array
            return array
        else:
            # Allocate new array
            return np.zeros(shape, dtype=self.dtype)
    
    def release(self, array: np.ndarray):
        """
        Release array back to pool.
        
        Args:
            array: Array to release
        """
        if len(self.pool) < self.max_pool_size:
            array.fill(0)  # Clear array
            self.pool.append(array)
    
    def clear(self):
        """Clear pool."""
        self.pool.clear()

# Usage example
float32_pool = MemoryPool(dtype=np.float32, max_pool_size=5)

# Acquire and release arrays
for i in range(10):
    arr = float32_pool.acquire((1000, 100))
    # Use array
    result = np.sum(arr)
    float32_pool.release(arr)

print(f"Pool size: {len(float32_pool.pool)}")
```

### 3. Garbage Collection

```python
import gc
import time
import numpy as np

class GarbageCollector:
    """Controlled garbage collection for simulations."""
    
    def __init__(self, auto_gc_interval=5.0):
        self.auto_gc_interval = auto_gc_interval
        self.last_gc_time = time.time()
        self.gc_count = 0
    
    def should_collect(self) -> bool:
        """
        Check if garbage collection should be performed.
        
        Returns:
            True if garbage collection should be performed
        """
        current_time = time.time()
        return (current_time - self.last_gc_time) > self.auto_gc_interval
    
    def collect(self, reason="auto"):
        """
        Perform garbage collection.
        
        Args:
            reason: Reason for garbage collection
        """
        print(f"Garbage collection: {reason}")
        start_time = time.time()
        
        # Force garbage collection
        collected = gc.collect()
        
        end_time = time.time()
        self.gc_count += 1
        self.last_gc_time = end_time
        
        print(f"  Collected {collected} objects in {end_time - start_time:.2f}s")
        return collected
    
    def collect_if_needed(self, reason="auto"):
        """
        Collect garbage if needed.
        
        Args:
            reason: Reason for potential collection
        """
        if self.should_collect():
            return self.collect(reason)
        return 0
    
    def get_statistics(self) -> dict:
        """
        Get garbage collection statistics.
        
        Returns:
            Statistics dictionary
        """
        return {
            'gc_count': self.gc_count,
            'last_gc_time': self.last_gc_time,
            'time_since_last_gc': time.time() - self.last_gc_time
        }

# Usage example
collector = GarbageCollector(auto_gc_interval=10.0)

# In simulation loop
for step in range(1000):
    # Run simulation
    brain.step(step, step * 0.01)
    
    # Collect garbage periodically
    collector.collect_if_needed("simulation_step")
    
    # Check memory usage
    import psutil
    memory_mb = psutil.Process().memory_info().rss / (1024 * 1024)
    
    if memory_mb > 2000:  # 2GB
        print(f"Memory usage high: {memory_mb:.1f}MB, forcing GC")
        collector.collect("memory_pressure")

# Get statistics
stats = collector.get_statistics()
print(f"GC statistics: {stats}")
```

## Simulation Design Patterns

### 1. Factory Pattern

```python
from nlm_enhanced import *

class BrainFactory:
    """Factory class for creating different brain types."""
    
    @staticmethod
    def create_standard_brain(config):
        """Create a standard brain."""
        brain = createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def create_large_brain(config):
        """Create a large-scale brain for research."""
        # Configure for large-scale simulation
        config.set("brain.neuron_count", 10000)
        config.set("brain.v_thresh", -55.0)
        brain = createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def create_dev_brain(config):
        """Create a developmental brain."""
        # Configure for developmental studies
        config.set("brain.neuron_count", 2000)
        config.set("development.enable", True)
        config.set("development.plastic_period", 100.0)
        brain = createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def create_custom_brain(config, custom_settings):
        """Create a custom brain with specific settings."""
        # Apply custom settings
        for key, value in custom_settings.items():
            config.set(key, value)
        
        brain = createBrain(config)
        brain.initialize()
        return brain

# Usage example
def create_research_brain():
    """Create research brain using factory."""
    config = createDefaultConfig()
    return BrainFactory.create_large_brain(config)

def create_performance_brain():
    """Create performance brain using factory."""
    config = createDefaultConfig()
    return BrainFactory.create_standard_brain(config)

def create_dev_study_brain():
    """Create developmental brain using factory."""
    config = createDefaultConfig()
    return BrainFactory.create_dev_brain(config)
```

### 2. Template Pattern

```python
from python.nlm_advanced.config import ConfigTemplate

class BrainTemplateFactory:
    """Factory for creating brain configuration templates."""
    
    @staticmethod
    def get_research_template():
        """Get research-oriented brain template."""
        template = ConfigTemplate()
        template.create_template(
            name="research",
            parameters={
                'network_size': 10000,
                'learning_rate': 0.001,
                'plasticity.stdp.enable': True,
                'neuromod.curiosity.enable': True,
                'development.enable': True,
                'memory.episodic.enable': True
            },
            description="Template optimized for research simulations",
            tags=['research', 'comprehensive']
        )
        return template
    
    @staticmethod
    def get_performance_template():
        """Get performance-optimized brain template."""
        template = ConfigTemplate()
        template.create_template(
            name="performance",
            parameters={
                'network_size': 5000,
                'learning_rate': 0.01,
                'plasticity.stdp.enable': True,
                'neuromod.curiosity.enable': False,
                'development.enable': False,
                'memory.episodic.enable': False
            },
            description="Template optimized for performance testing",
            tags=['performance', 'efficiency']
        )
        return template
    
    @staticmethod
    def get_development_template():
        """Get developmental brain template."""
        template = ConfigTemplate()
        template.create_template(
            name="development",
            parameters={
                'network_size': 2000,
                'learning_rate': 0.0001,
                'plasticity.stdp.enable': True,
                'neuromod.curiosity.enable': True,
                'development.enable': True,
                'plasticity.structural.enable': True,
                'memory.episodic.enable': True,
                'memory.working.enable': True,
                'prediction.enable': True,
                'cognition.enable': True
            },
            description="Template optimized for developmental studies",
            tags=['development', 'biological']
        )
        return template
    
    @staticmethod
    def apply_template(template_name, base_config, overrides=None):
        """
        Apply template to configuration.
        
        Args:
            template_name: Name of template to apply
            base_config: Base configuration
            overrides: Optional parameter overrides
            
        Returns:
            Applied configuration
        """
        templates = {
            'research': BrainTemplateFactory.get_research_template(),
            'performance': BrainTemplateFactory.get_performance_template(),
            'development': BrainTemplateFactory.get_development_template()
        }
        
        if template_name not in templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        template = templates[template_name]
        config = template.apply_template(template_name, overrides or {})
        
        return config

# Usage example
def create_research_simulation():
    """Create research simulation using template."""
    base_config = createDefaultConfig()
    return BrainTemplateFactory.apply_template('research', base_config)

def create_performance_test():
    """Create performance test using template."""
    base_config = createDefaultConfig()
    return BrainTemplateFactory.apply_template('performance', base_config)

def create_developmental_study():
    """Create developmental study using template."""
    base_config = createDefaultConfig()
    return BrainTemplateFactory.apply_template('development', base_config)
```

### 3. Strategy Pattern

```python
from abc import ABC, abstractmethod

class TimeSteppingStrategy(ABC):
    """Abstract base class for time-stepping strategies."""
    
    @abstractmethod
    def calculate_time_step(self, step, current_time, brain):
        """
        Calculate time step for simulation.
        
        Args:
            step: Current step
            current_time: Current simulation time
            brain: Brain object
            
        Returns:
            Time step value
        """
        pass

class FixedTimeStep(TimeSteppingStrategy):
    """Fixed time-stepping strategy."""
    
    def __init__(self, dt):
        self.dt = dt
    
    def calculate_time_step(self, step, current_time, brain):
        return self.dt

class AdaptiveTimeStep(TimeSteppingStrategy):
    """Adaptive time-stepping strategy."""
    
    def __init__(self, base_dt, activity_threshold):
        self.base_dt = base_dt
        self.activity_threshold = activity_threshold
    
    def calculate_time_step(self, step, current_time, brain):
        activity = brain.getActiveNeuronCount()
        
        if activity > self.activity_threshold:
            # High activity - smaller steps
            return self.base_dt * 0.5
        elif activity > self.activity_threshold // 2:
            # Medium activity
            return self.base_dt * 0.8
        else:
            # Low activity - larger steps
            return self.base_dt * 1.2

class EventDrivenTimeStep(TimeSteppingStrategy):
    """Event-driven time-stepping strategy."""
    
    def __init__(self, base_dt, event_threshold):
        self.base_dt = base_dt
        self.event_threshold = event_threshold
    
    def calculate_time_step(self, step, current_time, brain):
        # Check for events (spikes, state changes, etc.)
        spike_count = brain.getFiringNeuronCount()
        
        if spike_count > self.event_threshold:
            # Event-driven - smaller steps
            return self.base_dt * 0.1
        else:
            # No events - larger steps
            return self.base_dt

class TimeSteppingContext:
    """Context class for time-stepping strategies."""
    
    def __init__(self, strategy: TimeSteppingStrategy):
        self._strategy = strategy
    
    def set_strategy(self, strategy: TimeSteppingStrategy):
        """Set time-stepping strategy."""
        self._strategy = strategy
    
    def get_time_step(self, step, current_time, brain):
        """
        Get time step using current strategy.
        
        Args:
            step: Current step
            current_time: Current simulation time
            brain: Brain object
            
        Returns:
            Time step value
        """
        return self._strategy.calculate_time_step(step, current_time, brain)

# Usage example
def run_adaptive_simulation(config):
    """Run simulation with adaptive time-stepping."""
    brain = createBrain(config)
    brain.initialize()
    
    # Setup adaptive time-stepping
    context = TimeSteppingContext(AdaptiveTimeStep(0.01, 1000))
    
    # Run simulation
    for step in range(1000):
        time_val = step * 0.01
        dt = context.get_time_step(step, time_val, brain)
        
        brain.step(step, time_val)
    
    return brain

def run_event_driven_simulation(config):
    """Run simulation with event-driven time-stepping."""
    brain = createBrain(config)
    brain.initialize()
    
    # Setup event-driven time-stepping
    context = TimeSteppingContext(EventDrivenTimeStep(0.01, 100))
    
    # Run simulation
    for step in range(1000):
        time_val = step * 0.01
        dt = context.get_time_step(step, time_val, brain)
        
        brain.step(step, time_val)
    
    return brain
```

## Conclusion

This comprehensive best practices guide provides:

1. **Performance optimization strategies** for memory, CPU, and I/O
2. **Error handling best practices** with exception hierarchy and recovery
3. **Memory management guidelines** with allocation, pooling, and garbage collection
4. **Simulation design patterns** including factory, template, and strategy patterns

These best practices help users create efficient, robust, and maintainable neural simulations while following industry-standard design patterns and optimization techniques.

For further assistance or questions, please refer to the specific documentation sections or contact the NLM support team.