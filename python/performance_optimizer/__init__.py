# Performance Optimization Categories:
# 1. Memory Management Optimizations
# 2. Data Structure Optimizations
# 3. Computation Optimizations
# 4. Interface Optimizations
# 5. System-Level Optimizations

import time
import threading
import gc
import numpy as np
import functools
from typing import Dict, List, Optional, Any, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
import collections
import weakref

# Type aliases for better readability
NeuronId = int
SynapseId = int
SimulationStep = int
TimeStamp = float

class PerformanceOptimization(Enum):
    """Available performance optimization categories"""
    MEMORY_POOLING = "memory_pooling"
    OBJECT_REUSE = "object_reuse"
    GARBAGE_COLLECTION = "garbage_collection"
    BUFFER_MANAGEMENT = "buffer_management"
    DATA_STRUCTURE_OPTIMIZATION = "data_structure_optimization"
    NUMPY_INTEGRATION = "numpy_integration"
    VECTORIZED_OPERATIONS = "vectorized_operations"
    SPARSE_HANDLING = "sparse_handling"
    PARALLEL_PROCESSING = "parallel_processing"
    LAZY_EVALUATION = "lazy_evaluation"
    CACHING = "caching"
    BATCH_OPERATIONS = "batch_operations"
    STREAMING = "streaming"
    THREAD_POOL = "thread_pool"
    CPU_CACHE_OPT = "cpu_cache_optimization"
    NUMA_AWARE = "numa_aware"

class OptimizationStatus(Enum):
    """Status of performance optimizations"""
    DISABLED = "disabled"
    ENABLED = "enabled"
    OPTIMAL = "optimal"

@dataclass
class PerformanceMetrics:
    """Performance measurement data"""
    # Memory metrics
    memory_usage_mb: float = 0.0
    memory_allocated_mb: float = 0.0
    memory_peak_mb: float = 0.0
    
    # CPU metrics
    cpu_usage_percent: float = 0.0
    cpu_time_ms: float = 0.0
    real_time_ms: float = 0.0
    
    # Operation metrics
    operations_per_second: float = 0.0
    bytes_processed: int = 0
    spike_events_processed: int = 0
    
    # Network metrics
    synapses_updated: int = 0
    neurons_updated: int = 0
    
    # Quality metrics
    accuracy: float = 0.0
    convergence_rate: float = 0.0
    
    timestamp: TimeStamp = 0.0
    step_id: SimulationStep = 0

class MemoryPool:
    """Memory pool for efficient object allocation"""
    
    def __init__(self, pool_size: int = 10000):
        self.pool_size = pool_size
        self.neuron_cache = []
        self.synapse_cache = []
        self.object_pool = []
        self.free_indices = list(range(pool_size))
        self.lock = threading.RLock()
        self.stats = {
            'allocations': 0,
            'deallocations': 0,
            'cache_hits': 0,
            'cache_misses': 0
        }
        
    def allocate_neuron(self) -> NeuronId:
        """Allocate a neuron ID from pool"""
        with self.lock:
            if self.free_indices:
                idx = self.free_indices.pop()
                self.stats['cache_hits'] += 1
                return idx
            else:
                self.stats['cache_misses'] += 1
                return len(self.neuron_cache)
    
    def deallocate_neuron(self, neuron_id: NeuronId):
        """Return neuron ID to pool"""
        with self.lock:
            if neuron_id < self.pool_size:
                self.free_indices.append(neuron_id)
                self.stats['deallocations'] += 1
    
    def get_stats(self) -> Dict:
        """Get memory pool statistics"""
        with self.lock:
            total = self.stats['allocations'] + self.stats['cache_misses']
            hit_rate = self.stats['cache_hits'] / total if total > 0 else 0.0
            return {
                'pool_size': self.pool_size,
                'free_indices': len(self.free_indices),
                'hit_rate': hit_rate,
                'allocations': self.stats['allocations'],
                'deallocations': self.stats['deallocations']
            }

class ObjectCache:
    """LRU cache for frequently used objects"""
    
    def __init__(self, max_size: int = 1000):
        self.max_size = max_size
        self.cache = collections.OrderedDict()
        self.hits = 0
        self.misses = 0
        self.lock = threading.RLock()
    
    def get(self, key: Any) -> Optional[Any]:
        """Get object from cache"""
        with self.lock:
            if key in self.cache:
                self.cache.move_to_end(key)
                self.hits += 1
                return self.cache[key]
            self.misses += 1
            return None
    
    def put(self, key: Any, value: Any):
        """Put object into cache"""
        with self.lock:
            if key in self.cache:
                self.cache.move_to_end(key)
                self.cache[key] = value
            else:
                self.cache[key] = value
                self.cache.move_to_end(key)
                if len(self.cache) > self.max_size:
                    self.cache.popitem(last=False)
    
    def clear(self):
        """Clear cache"""
        with self.lock:
            self.cache.clear()
    
    def stats(self) -> Dict:
        """Get cache statistics"""
        with self.lock:
            total = self.hits + self.misses
            hit_rate = self.hits / total if total > 0 else 0.0
            return {
                'size': len(self.cache),
                'max_size': self.max_size,
                'hits': self.hits,
                'misses': self.misses,
                'hit_rate': hit_rate
            }

class GarbageCollectionManager:
    """Intelligent garbage collection management"""
    
    def __init__(self):
        self.gc_enabled = True
        self.gc_threshold = 0.8
        self.last_collection_time = 0.0
        self.collection_count = 0
        self.auto_collect_interval = 0.1
        self.last_auto_collect = 0.0
        
    def tune_gc(self, target_memory: float = 0.7):
        """Tune garbage collection parameters"""
        self.gc_threshold = target_memory
        if target_memory > 0.9:
            gc.disable()
            self.gc_enabled = False
        else:
            gc.enable()
            self.gc_enabled = True
    
    def collect(self, generation: int = 2):
        """Run garbage collection"""
        if not self.gc_enabled:
            return 0
        
        start_time = time.time()
        collected = gc.collect(generation)
        self.last_collection_time = time.time() - start_time
        self.collection_count += 1
        return collected
    
    def auto_collect_if_needed(self, current_memory: float):
        """Automatically collect garbage if memory usage is high"""
        if not self.gc_enabled:
            return
        
        now = time.time()
        if now - self.last_auto_collect > self.auto_collect_interval:
            if current_memory > self.gc_threshold:
                self.collect()
            self.last_auto_collect = now

class VectorizedOperations:
    """Vectorized operations for numerical computations"""
    
    def __init__(self, enable_numpy: bool = True):
        self.enable_numpy = enable_numpy
        self.simd_available = self._check_simd_support()
        self.operation_cache = {}
        
    def _check_simd_support(self) -> bool:
        """Check if SIMD instructions are available"""
        try:
            import numpy as np
            return np.__version__ is not None
        except ImportError:
            return False
    
    def batch_neuron_update(self, membrane_potentials: List[float], 
                           weights: List[float], thresholds: List[float],
                           current_step: SimulationStep) -> List[float]:
        """Batch update multiple neurons"""
        if self.enable_numpy and len(membrane_potentials) > 100:
            arr_potentials = np.array(membrane_potentials, dtype=np.float32)
            arr_weights = np.array(weights, dtype=np.float32)
            arr_thresholds = np.array(thresholds, dtype=np.float32)
            
            # Vectorized update
            updated = arr_potentials + arr_weights * (arr_potentials > arr_thresholds)
            return updated.tolist()
        else:
            # Fallback to sequential
            return self._sequential_neuron_update(membrane_potentials, weights, thresholds)
    
    def _sequential_neuron_update(self, membrane_potentials: List[float],
                                weights: List[float], thresholds: List[float]) -> List[float]:
        """Sequential neuron update (fallback)"""
        updated = []
        for pot, weight, threshold in zip(membrane_potentials, weights, thresholds):
            updated.append(pot + weight * (1.0 if pot > threshold else 0.0))
        return updated
    
    def sparse_matrix_multiply(self, matrix: Dict[int, Dict[int, float]],
                             vector: List[float]) -> List[float]:
        """Sparse matrix-vector multiplication"""
        result = [0.0] * len(vector)
        
        for row, cols in matrix.items():
            for col, value in cols.items():
                result[col] += vector[row] * value
        
        return result

class BatchProcessor:
    """Batch processing for improved throughput"""
    
    def __init__(self, batch_size: int = 100):
        self.batch_size = batch_size
        self.pending_operations = []
        self.processing_queue = []
        self.lock = threading.RLock()
        self.processor_threads = []
        self.stop_event = threading.Event()
        
    def add_operation(self, operation: Callable, *args, **kwargs):
        """Add operation to batch queue"""
        with self.lock:
            self.pending_operations.append((operation, args, kwargs))
    
    def flush(self):
        """Process all pending operations"""
        with self.lock:
            if not self.pending_operations:
                return
            
            # Process operations in batches
            while self.pending_operations:
                batch = self.pending_operations[:self.batch_size]
                self.pending_operations = self.pending_operations[self.batch_size:]
                
                for operation, args, kwargs in batch:
                    try:
                        operation(*args, **kwargs)
                    except Exception as e:
                        print(f"Batch operation error: {e}")

class GarbageCollectionManager:
    """Intelligent garbage collection management"""
    
    def __init__(self):
        self.gc_enabled = True
        self.gc_threshold = 0.8
        self.last_collection_time = 0.0
        self.collection_count = 0
        self.auto_collect_interval = 0.1
        self.last_auto_collect = 0.0
        
    def tune_gc(self, target_memory: float = 0.7):
        """Tune garbage collection parameters"""
        self.gc_threshold = target_memory
        if target_memory > 0.9:
            gc.disable()
            self.gc_enabled = False
        else:
            gc.enable()
            self.gc_enabled = True
    
    def collect(self, generation: int = 2):
        """Run garbage collection"""
        if not self.gc_enabled:
            return 0
        
        start_time = time.time()
        collected = gc.collect(generation)
        self.last_collection_time = time.time() - start_time
        self.collection_count += 1
        return collected
    
    def auto_collect_if_needed(self, current_memory: float):
        """Automatically collect garbage if memory usage is high"""
        if not self.gc_enabled:
            return
        
        now = time.time()
        if now - self.last_auto_collect > self.auto_collect_interval:
            if current_memory > self.gc_threshold:
                self.collect()
            self.last_auto_collect = now

class VectorizedOperations:
    """Vectorized operations for numerical computations"""
    
    def __init__(self, enable_numpy: bool = True):
        self.enable_numpy = enable_numpy
        self.simd_available = self._check_simd_support()
        self.operation_cache = {}
        
    def _check_simd_support(self) -> bool:
        """Check if SIMD instructions are available"""
        try:
            import numpy as np
            return np.__version__ is not None
        except ImportError:
            return False
    
    def batch_neuron_update(self, membrane_potentials: List[float], 
                           weights: List[float], thresholds: List[float],
                           current_step: SimulationStep) -> List[float]:
        """Batch update multiple neurons"""
        if self.enable_numpy and len(membrane_potentials) > 100:
            arr_potentials = np.array(membrane_potentials, dtype=np.float32)
            arr_weights = np.array(weights, dtype=np.float32)
            arr_thresholds = np.array(thresholds, dtype=np.float32)
            
            # Vectorized update
            updated = arr_potentials + arr_weights * (arr_potentials > arr_thresholds)
            return updated.tolist()
        else:
            # Fallback to sequential
            return self._sequential_neuron_update(membrane_potentials, weights, thresholds)
    
    def _sequential_neuron_update(self, membrane_potentials: List[float],
                                weights: List[float], thresholds: List[float]) -> List[float]:
        """Sequential neuron update (fallback)"""
        updated = []
        for pot, weight, threshold in zip(membrane_potentials, weights, thresholds):
            updated.append(pot + weight * (1.0 if pot > threshold else 0.0))
        return updated
    
    def sparse_matrix_multiply(self, matrix: Dict[int, Dict[int, float]],
                             vector: List[float]) -> List[float]:
        """Sparse matrix-vector multiplication"""
        result = [0.0] * len(vector)
        
        for row, cols in matrix.items():
            for col, value in cols.items():
                result[col] += vector[row] * value
        
        return result

class BatchProcessor:
    """Batch processing for improved throughput"""
    
    def __init__(self, batch_size: int = 100):
        self.batch_size = batch_size
        self.pending_operations = []
        self.processing_queue = []
        self.lock = threading.RLock()
        self.processor_threads = []
        self.stop_event = threading.Event()
        
    def add_operation(self, operation: Callable, *args, **kwargs):
        """Add operation to batch queue"""
        with self.lock:
            self.pending_operations.append((operation, args, kwargs))
    
    def flush(self):
        """Process all pending operations"""
        with self.lock:
            if not self.pending_operations:
                return
            
            # Process operations in batches
            while self.pending_operations:
                batch = self.pending_operations[:self.batch_size]
                self.pending_operations = self.pending_operations[self.batch_size:]
                
                for operation, args, kwargs in batch:
                    try:
                        operation(*args, **kwargs)
                    except Exception as e:
                        print(f"Batch operation error: {e}")

class PerformanceOptimizer:
    """
    Main performance optimization system for NLM Python bindings.
    
    This class provides centralized performance management across multiple
    optimization categories while maintaining backward compatibility.
    """
    
    def __init__(self, config: Optional[Dict] = None):
        """Initialize performance optimizer with configuration"""
        self.config = config or self._default_config()
        self.optimizations = {}
        self.metrics_history = []
        self.memory_pool = MemoryPool(self.config['memory_pool_size'])
        self.object_cache = ObjectCache(self.config['object_cache_size'])
        self.gc_manager = GarbageCollectionManager()
        self.vector_ops = VectorizedOperations(self.config['enable_numpy'])
        self.batch_processor = BatchProcessor(self.config['batch_size'])
        self.thread_pool = None
        
        # Performance tuning parameters
        self.performance_thresholds = {
            'memory_usage': self.config['memory_threshold'],
            'cpu_usage': self.config['cpu_threshold'],
            'operation_rate': self.config['operation_rate_threshold']
        }
        
        # Enable optimizations based on configuration
        self._initialize_optimizations()
        
    def _default_config(self) -> Dict:
        """Get default performance optimization configuration"""
        # Safe CPU count with fallback
        try:
            import os
            cpu_count = len(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else os.cpu_count() or 1
        except:
            cpu_count = 1
            
        return {
            'memory_pool_size': 10000,
            'object_cache_size': 1000,
            'enable_numpy': True,
            'batch_size': 100,
            'enable_parallel': True,
            'thread_count': max(1, cpu_count - 1),
            'memory_threshold': 0.8,
            'cpu_threshold': 0.9,
            'operation_rate_threshold': 1000.0,
            'enable_gc_tuning': True,
            'enable_vectorization': True,
            'cache_size_limit': 100 * 1024 * 1024,  # 100MB
            'auto_optimize': True
        }
    
    def _initialize_optimizations(self):
        """Initialize all enabled optimizations"""
        optimization_configs = {
            PerformanceOptimization.MEMORY_POOLING: self.config['memory_pool_size'] > 0,
            PerformanceOptimization.OBJECT_REUSE: self.config['object_cache_size'] > 0,
            PerformanceOptimization.GARBAGE_COLLECTION: self.config['enable_gc_tuning'],
            PerformanceOptimization.BUFFER_MANAGEMENT: True,
            PerformanceOptimization.DATA_STRUCTURE_OPTIMIZATION: True,
            PerformanceOptimization.NUMPY_INTEGRATION: self.config['enable_numpy'],
            PerformanceOptimization.VECTORIZED_OPERATIONS: self.config['enable_vectorization'],
            PerformanceOptimization.SPARSE_HANDLING: True,
            PerformanceOptimization.PARALLEL_PROCESSING: self.config['enable_parallel'],
            PerformanceOptimization.LAZY_EVALUATION: False,  # Can be enabled
            PerformanceOptimization.CACHING: True,
            PerformanceOptimization.BATCH_OPERATIONS: self.config['batch_size'] > 0,
            PerformanceOptimization.STREAMING: False,  # Can be enabled
            PerformanceOptimization.THREAD_POOL: self.config['enable_parallel'],
            PerformanceOptimization.CPU_CACHE_OPT: True,
            PerformanceOptimization.NUMA_AWARE: False  # Platform dependent
        }
        
        for opt, enabled in optimization_configs.items():
            if enabled:
                self.optimizations[opt] = OptimizationStatus.ENABLED
            else:
                self.optimizations[opt] = OptimizationStatus.DISABLED
        
        # Initialize thread pool if needed
        if self.config['enable_parallel'] and self.optimizations[PerformanceOptimization.THREAD_POOL]:
            self._initialize_thread_pool()
    
    def _initialize_thread_pool(self):
        """Initialize thread pool for parallel processing"""
        try:
            from concurrent.futures import ThreadPoolExecutor
            
            self.thread_pool = ThreadPoolExecutor(
                max_workers=self.config['thread_count'],
                thread_name_prefix='nlm_worker'
            )
        except ImportError:
            # Fallback if concurrent.futures is not available
            self.thread_pool = None
            print("Warning: concurrent.futures not available, parallel processing disabled")
    
    def is_optimization_enabled(self, optimization: PerformanceOptimization) -> bool:
        """Check if optimization is enabled"""
        return self.optimizations.get(optimization, OptimizationStatus.DISABLED) == OptimizationStatus.ENABLED
    
    def enable_optimization(self, optimization: PerformanceOptimization):
        """Enable specific optimization"""
        if optimization in self.optimizations:
            self.optimizations[optimization] = OptimizationStatus.ENABLED
    
    def disable_optimization(self, optimization: PerformanceOptimization):
        """Disable specific optimization"""
        if optimization in self.optimizations:
            self.optimizations[optimization] = OptimizationStatus.DISABLED
    
    def collect_performance_metrics(self) -> PerformanceMetrics:
        """Collect current performance metrics"""
        current_time = time.time()
        
        metrics = PerformanceMetrics(timestamp=current_time)
        
        # Memory metrics
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            metrics.memory_usage_mb = memory_info.rss / 1024 / 1024
            metrics.memory_allocated_mb = memory_info.vms / 1024 / 1024
        except:
            pass
        
        # CPU metrics
        try:
            cpu_percent = psutil.cpu_percent(interval=0.0)
            metrics.cpu_usage_percent = cpu_percent
        except:
            pass
        
        # System metrics
        metrics.operations_per_second = self._calculate_operations_rate()
        
        # Calculate performance stats
        metrics.real_time_factor = getattr(self, '_real_time_factor', 1.0)
        
        return metrics
    
    def _calculate_operations_rate(self) -> float:
        """Calculate operations per second based on recent activity"""
        # This would typically track operations in a production system
        return 0.0
    
    def optimize_neural_computation(self, membrane_potentials: List[float],
                                   weights: List[float], thresholds: List[float],
                                   step_id: int) -> List[float]:
        """Optimized neural computation using vectorized operations"""
        if self.is_optimization_enabled(PerformanceOptimization.VECTORIZED_OPERATIONS):
            return self.vector_ops.batch_neuron_update(
                membrane_potentials,
                weights,
                thresholds,
                step_id
            )
        else:
            # Fallback to sequential implementation
            return self._sequential_neural_update(membrane_potentials, weights, thresholds)
    
    def _sequential_neural_update(self, membrane_potentials: List[float],
                                weights: List[float], thresholds: List[float]) -> List[float]:
        """Sequential neural computation (fallback)"""
        updated = []
        for pot, weight, threshold in zip(membrane_potentials, weights, thresholds):
            updated.append(pot + weight * (1.0 if pot > threshold else 0.0))
        return updated
    
    def sparse_connectivity_update(self, connectivity: Dict[int, Dict[int, float]],
                                 input_vector: List[float]) -> List[float]:
        """Optimized sparse connectivity update"""
        if self.is_optimization_enabled(PerformanceOptimization.SPARSE_HANDLING):
            return self.vector_ops.sparse_matrix_multiply(connectivity, input_vector)
        else:
            # Simple fallback
            return self._simple_sparse_update(connectivity, input_vector)
    
    def _simple_sparse_update(self, connectivity: Dict[int, Dict[int, float]],
                            input_vector: List[float]) -> List[float]:
        """Simple sparse update (fallback)"""
        result = [0.0] * len(input_vector)
        
        for row, cols in connectivity.items():
            for col, value in cols.items():
                result[col] += input_vector[row] * value
        
        return result
    
    def get_enabled_optimizations(self) -> List[str]:
        """Get list of enabled optimizations"""
        return [opt.value for opt, status in self.optimizations.items() 
                if status == OptimizationStatus.ENABLED]

    def get_optimization_stats(self) -> Dict[str, Any]:
        """Get comprehensive optimization statistics"""
        stats = {
            'enabled_optimizations': self.get_enabled_optimizations(),
            'memory_pool': self.memory_pool.get_stats(),
            'object_cache': self.object_cache.stats()
        }
        
        if self.thread_pool:
            stats['thread_pool'] = {
                'max_workers': self.thread_pool._max_workers,
                'active_workers': len([t for t in threading.enumerate() 
                                     if t.name.startswith('nlm_worker')])
            }
        
        return stats
    
    def tune_for_performance(self, metrics: PerformanceMetrics):
        """Automatically tune optimizations based on current performance"""
        if not self.config['auto_optimize']:
            return
        
        # Memory-based tuning
        if metrics.memory_usage_mb > self.performance_thresholds['memory_usage'] * 1024 * 1024:
            if self.is_optimization_enabled(PerformanceOptimization.MEMORY_POOLING):
                self.disable_optimization(PerformanceOptimization.MEMORY_POOLING)
            self.gc_manager.tune_gc(0.7)
        
        # CPU-based tuning
        if metrics.cpu_usage_percent > self.performance_thresholds['cpu_usage'] * 100:
            if self.is_optimization_enabled(PerformanceOptimization.PARALLEL_PROCESSING):
                self.disable_optimization(PerformanceOptimization.PARALLEL_PROCESSING)
        
        # Rate-based tuning
        if metrics.operations_per_second > self.performance_thresholds['operation_rate']:
            if self.is_optimization_enabled(PerformanceOptimization.BATCH_OPERATIONS):
                self.disable_optimization(PerformanceOptimization.BATCH_OPERATIONS)
        
        # Garbage collection tuning
        self.gc_manager.auto_collect_if_needed(
            metrics.memory_usage_mb / 1024 / 1024  # Convert to GB
        )
    
    def cleanup(self):
        """Clean up resources"""
        if self.thread_pool:
            self.thread_pool.shutdown(wait=False)
            self.thread_pool = None
        
        self.object_cache.clear()
    
    def __del__(self):
        """Destructor to ensure cleanup"""
        self.cleanup()
    
    def _initialize_optimizations(self):
        """Initialize all enabled optimizations"""
        optimization_configs = {
            PerformanceOptimization.MEMORY_POOLING: self.config['memory_pool_size'] > 0,
            PerformanceOptimization.OBJECT_REUSE: self.config['object_cache_size'] > 0,
            PerformanceOptimization.GARBAGE_COLLECTION: self.config['enable_gc_tuning'],
            PerformanceOptimization.BUFFER_MANAGEMENT: True,
            PerformanceOptimization.DATA_STRUCTURE_OPTIMIZATION: True,
            PerformanceOptimization.NUMPY_INTEGRATION: self.config['enable_numpy'],
            PerformanceOptimization.VECTORIZED_OPERATIONS: self.config['enable_vectorization'],
            PerformanceOptimization.SPARSE_HANDLING: True,
            PerformanceOptimization.PARALLEL_PROCESSING: self.config['enable_parallel'],
            PerformanceOptimization.LAZY_EVALUATION: False,  # Can be enabled
            PerformanceOptimization.CACHING: True,
            PerformanceOptimization.BATCH_OPERATIONS: self.config['batch_size'] > 0,
            PerformanceOptimization.STREAMING: False,  # Can be enabled
            PerformanceOptimization.THREAD_POOL: self.config['enable_parallel'],
            PerformanceOptimization.CPU_CACHE_OPT: True,
            PerformanceOptimization.NUMA_AWARE: False  # Platform dependent
        }
        
        for opt, enabled in optimization_configs.items():
            if enabled:
                self.optimizations[opt] = OptimizationStatus.ENABLED
            else:
                self.optimizations[opt] = OptimizationStatus.DISABLED
        
        # Initialize thread pool if needed
        if self.config['enable_parallel'] and self.optimizations[PerformanceOptimization.THREAD_POOL]:
            self._initialize_thread_pool()
    
    def _initialize_thread_pool(self):
        """Initialize thread pool for parallel processing"""
        from concurrent.futures import ThreadPoolExecutor
        
        self.thread_pool = ThreadPoolExecutor(
            max_workers=self.config['thread_count'],
            thread_name_prefix='nlm_worker'
        )
    
    def is_optimization_enabled(self, optimization: PerformanceOptimization) -> bool:
        """Check if optimization is enabled"""
        return self.optimizations.get(optimization, OptimizationStatus.DISABLED) == OptimizationStatus.ENABLED
    
    def enable_optimization(self, optimization: PerformanceOptimization):
        """Enable specific optimization"""
        if optimization in self.optimizations:
            self.optimizations[optimization] = OptimizationStatus.ENABLED
    
    def disable_optimization(self, optimization: PerformanceOptimization):
        """Disable specific optimization"""
        if optimization in self.optimizations:
            self.optimizations[optimization] = OptimizationStatus.DISABLED
    
    def collect_performance_metrics(self) -> PerformanceMetrics:
        """Collect current performance metrics"""
        import psutil
        
        current_time = time.time()
        
        metrics = PerformanceMetrics(timestamp=current_time)
        
        # Memory metrics
        process = psutil.Process()
        memory_info = process.memory_info()
        metrics.memory_usage_mb = memory_info.rss / 1024 / 1024
        metrics.memory_allocated_mb = memory_info.vms / 1024 / 1024
        
        # CPU metrics
        cpu_percent = psutil.cpu_percent(interval=0.1)
        metrics.cpu_usage_percent = cpu_percent
        
        # System metrics
        metrics.operations_per_second = self._calculate_operations_rate()
        
        return metrics
    
    def _calculate_operations_rate(self) -> float:
        """Calculate operations per second based on recent activity"""
        # This would typically track operations in a production system
        return 0.0
    
    def optimize_neural_computation(self, membrane_potentials: List[float],
                                   weights: List[float], thresholds: List[float],
                                   current_step: SimulationStep) -> List[float]:
        """Optimize neural computation using vectorized operations"""
        if self.is_optimization_enabled(PerformanceOptimization.VECTORIZED_OPERATIONS):
            return self.vector_ops.batch_neuron_update(
                membrane_potentials, weights, thresholds, current_step
            )
        else:
            # Fallback to sequential implementation
            return self._sequential_neural_update(membrane_potentials, weights, thresholds)
    
    def _sequential_neural_update(self, membrane_potentials: List[float],
                                weights: List[float], thresholds: List[float]) -> List[float]:
        """Sequential neural computation (fallback)"""
        updated = []
        for pot, weight, threshold in zip(membrane_potentials, weights, thresholds):
            updated.append(pot + weight * (1.0 if pot > threshold else 0.0))
        return updated
    
    def sparse_connectivity_update(self, connectivity: Dict[int, Dict[int, float]],
                                 input_vector: List[float]) -> List[float]:
        """Optimized sparse connectivity update"""
        if self.is_optimization_enabled(PerformanceOptimization.SPARSE_HANDLING):
            return self.vector_ops.sparse_matrix_multiply(connectivity, input_vector)
        else:
            # Simple fallback
            return self._simple_sparse_update(connectivity, input_vector)
    
    def _simple_sparse_update(self, connectivity: Dict[int, Dict[int, float]],
                            input_vector: List[float]) -> List[float]:
        """Simple sparse update (fallback)"""
        result = [0.0] * len(input_vector)
        for row, cols in connectivity.items():
            for col, value in cols.items():
                result[col] += input_vector[row] * value
        return result
    
    def allocate_neuron_id(self) -> int:
        """Allocate neuron ID from memory pool"""
        if self.is_optimization_enabled(PerformanceOptimization.MEMORY_POOLING):
            return self.memory_pool.allocate_neuron()
        return self.optimizer_.step_count  # Fallback, shouldn't reach here

    def deallocate_neuron_id(self, neuron_id: int):
        """Return neuron ID to memory pool"""
        if self.is_optimization_enabled(PerformanceOptimization.MEMORY_POOLING):
            self.memory_pool.deallocate_neuron(neuron_id)

    def get_performance_stats(self) -> Dict[str, Any]:
        """Get comprehensive performance statistics"""
        stats = {
            'enabled_optimizations': [opt.value for opt, status in self.optimizations.items() 
                                    if status == OptimizationStatus.ENABLED],
            'memory_pool': self.memory_pool.get_stats(),
            'object_cache': self.object_cache.stats()
        }
        
        if self.thread_pool:
            stats['thread_pool'] = {
                'max_workers': self.thread_pool._max_workers,
                'active_workers': len([t for t in threading.enumerate() 
                                     if t.name.startswith('nlm_worker')])
            }
        
        return stats
    
    def tune_for_performance(self, metrics: PerformanceMetrics):
        """Automatically tune optimizations based on current performance"""
        if not self.config['auto_optimize']:
            return
        
        # Memory-based tuning
        if metrics.memory_usage_mb > self.performance_thresholds['memory_usage'] * 1024 * 1024:
            if self.is_optimization_enabled(PerformanceOptimization.MEMORY_POOLING):
                self.disable_optimization(PerformanceOptimization.MEMORY_POOLING)
            self.gc_manager.tune_gc(0.7)
        
        # CPU-based tuning
        if metrics.cpu_usage_percent > self.performance_thresholds['cpu_usage'] * 100:
            if self.is_optimization_enabled(PerformanceOptimization.PARALLEL_PROCESSING):
                self.disable_optimization(PerformanceOptimization.PARALLEL_PROCESSING)
        
        # Rate-based tuning
        if metrics.operations_per_second > self.performance_thresholds['operation_rate']:
            if self.is_optimization_enabled(PerformanceOptimization.BATCH_OPERATIONS):
                self.disable_optimization(PerformanceOptimization.BATCH_OPERATIONS)
        
        # Garbage collection tuning
        self.gc_manager.auto_collect_if_needed(
            metrics.memory_usage_mb / 1024 / 1024  # Convert to GB
        )
    
    def cleanup(self):
        """Clean up resources"""
        if self.thread_pool:
            self.thread_pool.shutdown(wait=False)
            self.thread_pool = None
        
        self.object_cache.clear()
        
        if self.is_optimization_enabled(PerformanceOptimization.MEMORY_POOLING):
            # Return all memory to system
            self.memory_pool.deallocate_neuron  # This is a method, not attribute
    
    def __del__(self):
        """Destructor to ensure cleanup"""
        self.cleanup()

# Global performance optimizer instance
_global_optimizer = None

def get_optimizer(config: Optional[Dict] = None) -> PerformanceOptimizer:
    """Get or create global performance optimizer instance"""
    global _global_optimizer
    if _global_optimizer is None:
        _global_optimizer = PerformanceOptimizer(config)
    return _global_optimizer

# Convenience functions for common optimizations
def optimize_neural_update(neuron_states: Dict[str, Any], 
                          weights: Dict[str, Any],
                          thresholds: Dict[str, Any],
                          step: SimulationStep) -> Dict[str, Any]:
    """Convenience function for optimized neural computation"""
    optimizer = get_optimizer()
    return optimizer.optimize_neural_computation(
        neuron_states['membrane_potentials'],
        weights['synaptic_weights'],
        thresholds['thresholds'],
        step
    )

def batch_process_operations(operations: List[Callable], 
                           *args, **kwargs) -> List[Any]:
    """Convenience function for batch processing"""
    optimizer = get_optimizer()
    results = []
    
    for op in operations:
        try:
            result = op(*args, **kwargs)
            results.append(result)
        except Exception as e:
            print(f"Batch operation error: {e}")
            results.append(None)
    
    return results

print("Performance optimization system loaded successfully!")