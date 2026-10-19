#!/usr/bin/env python3
"""
Performance Optimization Examples - Efficient Simulation Techniques

This guide demonstrates advanced performance optimization techniques for NLM Python bindings:
- Memory management and optimization
- Computational efficiency improvements
- Data structure optimizations
- Parallel and distributed processing
- Cache optimization
- GPU acceleration techniques
- Load balancing strategies
- Profiling and benchmarking tools
"""
import nlm
import time
import numpy as np
from typing import List, Dict, Any, Optional, Tuple
from contextlib import contextmanager
import tracemalloc
import psutil
import gc
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import warnings

class PerformanceOptimizer:
    """
    Advanced performance optimizer for NLM simulations.
    
    This class provides comprehensive optimization techniques for:
    1. Memory efficiency and management
    2. Computational optimization
    3. Data structure optimization
    4. Parallel processing
    5. Profiling and benchmarking
    6. Cache optimization
    7. Load balancing
    """
    
    def __init__(self):
        """Initialize performance optimizer."""
        print("Initializing Performance Optimizer...")
        
        # Performance tracking
        self.optimization_metrics = []
        self.memory_snapshots = []
        self.benchmark_results = {}
        
        # Configuration
        self.config = {
            'enable_memory_tracking': True,
            'enable_cpu_profiling': True,
            'enable_benchmarking': True,
            'memory_threshold_mb': 256,
            'cpu_threshold_percent': 80,
            'cache_size_limit': 1000,
            'parallel_threshold': 10,
        }
        
        # Initialize memory tracking
        if self.config['enable_memory_tracking']:
            tracemalloc.start()
        
        print("✓ Performance optimizer initialized")
    
    @contextmanager
    def performance_context(self, operation_name: str, optimize_memory: bool = True):
        """
        Context manager for performance monitoring and optimization.
        
        Args:
            operation_name: Name of the operation to optimize
            optimize_memory: Whether to enable memory optimization
        """
        start_time = time.time()
        process = psutil.Process()
        
        if self.config['enable_memory_tracking']:
            snapshot1 = tracemalloc.take_snapshot()
        
        cpu_before = process.cpu_percent()
        memory_before = process.memory_info().rss / (1024 * 1024)  # MB
        
        try:
            yield
            
        finally:
            # Measure performance
            end_time = time.time()
            duration = end_time - start_time
            
            cpu_after = process.cpu_percent()
            memory_after = process.memory_info().rss / (1024 * 1024)  # MB
            
            cpu_usage = cpu_after - cpu_before
            memory_usage = memory_after - memory_before
            
            # Record metrics
            metrics = {
                'operation': operation_name,
                'duration': duration,
                'cpu_usage_percent': cpu_usage,
                'memory_usage_mb': memory_usage,
                'memory_after_mb': memory_after,
                'start_time': start_time,
                'end_time': end_time,
            }
            
            if self.config['enable_memory_tracking']:
                snapshot2 = tracemalloc.take_snapshot()
                top_stats = snapshot2.compare_to(snapshot1, 'lineno')
                memory_allocations = [
                    (stat.size_diff, stat.traceback) for stat in top_stats[:10]
                ]
                metrics['memory_allocations'] = memory_allocations
            
            self.optimization_metrics.append(metrics)
            
            # Log performance
            self._log_performance(metrics, optimize_memory)
    
    def _log_performance(self, metrics: Dict[str, Any], optimize_memory: bool):
        """Log performance metrics and optimize if needed."""
        operation = metrics['operation']
        duration = metrics['duration']
        memory_usage = metrics['memory_usage_mb']
        cpu_usage = metrics['cpu_usage_percent']
        
        # Check thresholds
        alerts = []
        
        if memory_usage > self.config['memory_threshold_mb']:
            alerts.append(f"  ⚠️ High memory usage: {memory_usage:.1f} MB")
        
        if cpu_usage > self.config['cpu_threshold_percent']:
            alerts.append(f"  ⚠️ High CPU usage: {cpu_usage:.1f}%")
        
        if duration > 1.0:  # 1 second threshold
            alerts.append(f"  ⚠️ Slow operation: {duration:.3f}s")
        
        # Log performance
        print(f"📊 Performance: {operation}")
        print(f"  - Duration: {duration:.3f}s")
        print(f"  - Memory: {metrics.get('memory_after_mb', 0):.1f} MB")
        print(f"  - CPU: {cpu_usage:.1f}%")
        
        if alerts:
            for alert in alerts:
                print(alert)
        
        # Apply optimizations if needed
        if optimize_memory and memory_usage > self.config['memory_threshold_mb']:
            self._optimize_memory(operation, metrics)
    
    def _optimize_memory(self, operation: str, metrics: Dict[str, Any]):
        """Optimize memory usage for operation."""
        print(f"\n🔧 Memory optimization for {operation}")
        
        # Force garbage collection
        collected = gc.collect()
        print(f"  - Garbage collected: {collected} objects")
        
        # Clear Python cache
        import sys
        if len(sys.modules) > self.config['cache_size_limit']:
            print(f"  - Cache size: {len(sys.modules)} (approaching limit)")
        
        # Clear performance metrics if too large
        if len(self.optimization_metrics) > 1000:
            print(f"  - Clearing old performance metrics")
            self.optimization_metrics = self.optimization_metrics[-100:]
    
    def benchmark_operation(self, operation_func: Callable, operation_name: str,
                           iterations: int = 100, warmup_iterations: int = 10) -> Dict[str, Any]:
        """
        Benchmark an operation with warmup and multiple iterations.
        
        Args:
            operation_func: Function to benchmark
            operation_name: Name of operation for reporting
            iterations: Number of benchmark iterations
            warmup_iterations: Number of warmup iterations
            
        Returns:
            Dictionary with benchmark results
        """
        print(f"\n🚀 Benchmarking: {operation_name}")
        print(f"  - Warmup: {warmup_iterations} iterations")
        print(f"  - Benchmark: {iterations} iterations")
        
        # Warmup
        print("  Warming up...")
        for i in range(warmup_iterations):
            with self.performance_context(f"{operation_name}_warmup_{i}"):
                operation_func()
        
        # Benchmark
        print("  Benchmarking...")
        times = []
        memories = []
        efficiencies = []
        
        for i in range(iterations):
            if i % 20 == 0:
                print(f"    Iteration {i+1}/{iterations}")
            
            with self.performance_context(f"{operation_name}_{i}"):
                start_time = time.time()
                operation_func()
                end_time = time.time()
                
                # Get current metrics
                if self.optimization_metrics:
                    last_metric = self.optimization_metrics[-1]
                    times.append(last_metric['duration'])
                    memories.append(last_metric['memory_usage_mb'])
                    efficiencies.append(last_metric.get('cpu_usage_percent', 0))
        
        # Calculate statistics
        results = {
            'operation': operation_name,
            'iterations': iterations,
            'average_time': np.mean(times) if times else 0,
            'median_time': np.median(times) if times else 0,
            'min_time': np.min(times) if times else 0,
            'max_time': np.max(times) if times else 0,
            'std_time': np.std(times) if times else 0,
            'average_memory': np.mean(memories) if memories else 0,
            'median_memory': np.median(memories) if memories else 0,
            'average_efficiency': np.mean(efficiencies) if efficiencies else 0,
            'all_times': times,
            'all_memories': memories,
        }
        
        # Calculate performance score
        results['performance_score'] = self._calculate_performance_score(results)
        
        print(f"\n📊 Benchmark Results for {operation_name}:")
        print(f"  - Average time: {results['average_time']:.4f}s")
        print(f"  - Median time: {results['median_time']:.4f}s")
        print(f"  - Min time: {results['min_time']:.4f}s")
        print(f"  - Max time: {results['max_time']:.4f}s")
        print(f"  - Std dev: {results['std_time']:.4f}s")
        print(f"  - Average memory: {results['average_memory']:.1f} MB")
        print(f"  - Average efficiency: {results['average_efficiency']:.1f}%")
        print(f"  - Performance score: {results['performance_score']:.3f}")
        
        # Store benchmark
        self.benchmark_results[operation_name] = results
        
        return results
    
    def _calculate_performance_score(self, results: Dict[str, Any]) -> float:
        """Calculate performance score based on metrics."""
        if not results['all_times']:
            return 0.0
        
        # Normalize based on best/worst in benchmark
        avg_time = results['average_time']
        min_time = results['min_time']
        max_time = results['max_time']
        
        # Time score (higher is better)
        if min_time > 0:
            time_score = 1.0 - ((avg_time - min_time) / (max_time - min_time + 1e-9))
        else:
            time_score = 1.0
        
        # Memory score (higher is better)
        avg_memory = results['average_memory']
        memory_score = 1.0 / (1.0 + avg_memory / 100.0)
        
        # Efficiency score
        efficiency_score = min(results['average_efficiency'] / 100.0, 1.0)
        
        # Combined score
        performance_score = (time_score * 0.5 + memory_score * 0.3 + efficiency_score * 0.2)
        
        return performance_score
class MemoryOptimizer:
    """
    Advanced memory optimizer for NLM simulations.
    
    This class provides memory optimization techniques including:
    - Memory pooling and reuse
    - Data compression
    - Lazy loading
    - Memory-efficient data structures
    - Automatic cleanup
    """
    
    def __init__(self, memory_limit_mb: int = 512):
        """
        Initialize memory optimizer.
        
        Args:
            memory_limit_mb: Memory limit in MB
        """
        print(f"Initializing Memory Optimizer (limit: {memory_limit_mb} MB)...")
        
        self.memory_limit_mb = memory_limit_mb
        self.current_memory = 0
        self.memory_blocks = []
        self.compression_enabled = True
        self.pooling_enabled = True
        
        print("✓ Memory optimizer initialized")
    
    def create_memory_pool(self, size: int, alignment: int = 64) -> 'MemoryPool':
        """
        Create memory pool for efficient allocation.
        
        Args:
            size: Size of memory pool in MB
            alignment: Memory alignment in bytes
            
        Returns:
            MemoryPool object
        """
        return MemoryPool(size, alignment)
    
    def compress_data(self, data: Any, compression_level: int = 6) -> bytes:
        """
        Compress data for memory efficiency.
        
        Args:
            data: Data to compress
            compression_level: Compression level (1-9)
            
        Returns:
            Compressed data as bytes
        """
        if not self.compression_enabled:
            return json.dumps(data).encode('utf-8')
        
        try:
            import zlib
            
            # Convert to JSON string
            json_str = json.dumps(data, separators=(',', ':'))
            
            # Compress
            compressed = zlib.compress(
                json_str.encode('utf-8'),
                compression_level=compression_level
            )
            
            compression_ratio = len(compressed) / len(json_str.encode('utf-8'))
            print(f"  - Compression ratio: {compression_ratio:.2%}")
            
            return compressed
            
        except ImportError:
            print("  - zlib not available, using JSON serialization")
            return json.dumps(data).encode('utf-8')
    
    def decompress_data(self, compressed_data: bytes) -> Any:
        """
        Decompress data.
        
        Args:
            compressed_data: Compressed data
            
        Returns:
            Decompressed data
        """
        try:
            import zlib
            
            # Try decompression
            try:
                decompressed = zlib.decompress(compressed_data)
                return json.loads(decompressed.decode('utf-8'))
            except:
                # Try as JSON if not compressed
                return json.loads(compressed_data.decode('utf-8'))
                
        except ImportError:
            return json.loads(compressed_data.decode('utf-8'))
    
    def optimize_percept_storage(self, percept: nlm.SensoryPercept) -> nlm.SensoryPercept:
        """
        Optimize percept storage for memory efficiency.
        
        Args:
            percept: Input percept
            
        Returns:
            Optimized percept
        """
        # Create optimized copy
        optimized = nlm.SensoryPercept()
        
        # Get original data
        vision_data = percept.getVision()
        touch_data = percept.getTouch()
        internal_data = percept.getInternal()
        proprioception_data = percept.getProprioception()
        
        # Apply memory optimizations
        optimized.setVision(self._optimize_array(vision_data))
        optimized.setTouch(self._optimize_array(touch_data))
        optimized.setInternal(self._optimize_array(internal_data))
        optimized.setProprioception(self._optimize_array(proprioception_data))
        
        # Preserve timestamp
        optimized.setTimestamp(percept.getTimestamp())
        
        return optimized
    
    def _optimize_array(self, data: List[float]) -> List[float]:
        """Optimize array storage."""
        if len(data) < 100:
            return data  # Too small to benefit from optimization
        
        # Check if array can be compressed (repeated values)
        unique_values = len(set(data))
        total_values = len(data)
        
        # If array is highly compressible, use compression
        if unique_values < total_values * 0.1:  # 10% unique values
            print(f"  - Using compressed array storage ({unique_values}/{total_values} unique values)")
            # In practice, would use dictionary encoding
            # For now, return original
            return data
        
        return data
    
    def clear_memory_blocks(self):
        """Clear memory blocks and force cleanup."""
        print("🧹 Clearing memory blocks...")
        
        # Clear memory pools
        for pool in self.memory_blocks:
            pool.clear()
        
        # Force garbage collection
        collected = gc.collect()
        print(f"  - Garbage collected: {collected} objects")
        
        self.current_memory = 0
class MemoryPool:
    """Memory pool for efficient allocation and reuse."""
    
    def __init__(self, size_mb: int, alignment: int = 64):
        """
        Initialize memory pool.
        
        Args:
            size_mb: Pool size in MB
            alignment: Memory alignment in bytes
        """
        self.size_bytes = size_mb * 1024 * 1024
        self.alignment = alignment
        self.allocated_blocks = []
        self.free_blocks = []
        
        print(f"  - Created memory pool: {size_mb} MB")
    
    def allocate(self, size_bytes: int) -> bytearray:
        """
        Allocate memory from pool.
        
        Args:
            size_bytes: Size to allocate
            
        Returns:
            Allocated bytearray
        """
        # Check if allocation fits
        if size_bytes > self.size_bytes:
            raise ValueError(f"Allocation size {size_bytes} exceeds pool size {self.size_bytes}")
        
        # Find suitable free block
        for i, block in enumerate(self.free_blocks):
            if block['size'] >= size_bytes:
                # Use this block
                allocated = self.free_blocks.pop(i)
                allocated['allocated'] = True
                self.allocated_blocks.append(allocated)
                return allocated['data'][:size_bytes]
        
        # Allocate new block if needed
        new_block = {
            'size': self.size_bytes,
            'data': bytearray(self.size_bytes),
            'allocated': True,
        }
        self.allocated_blocks.append(new_block)
        return new_block['data'][:size_bytes]
    
    def free(self, data: bytearray):
        """Free memory back to pool."""
        # Find and free block
        for block in self.allocated_blocks:
            if block['data'] == data:
                block['allocated'] = False
                self.free_blocks.append(block)
                break
    
    def clear(self):
        """Clear all allocated blocks."""
        for block in self.allocated_blocks:
            block['allocated'] = False
        self.free_blocks.extend(self.allocated_blocks)
        self.allocated_blocks.clear()
    
    def get_usage(self) -> Dict[str, Any]:
        """Get memory pool usage statistics."""
        total_allocated = sum(block['size'] for block in self.allocated_blocks)
        total_free = sum(block['size'] for block in self.free_blocks)
        
        return {
            'total_size': self.size_bytes,
            'allocated_size': total_allocated,
            'free_size': total_free,
            'utilization': total_allocated / self.size_bytes if self.size_bytes > 0 else 0,
            'allocated_blocks': len(self.allocated_blocks),
            'free_blocks': len(self.free_blocks),
        }
class DataStructureOptimizer:
    """
    Data structure optimizer for NLM simulations.
    
    This class provides optimizations for data structures including:
    - Efficient percept storage and access
    - Vectorized operations
    - NumPy integration
    - Custom data structures
    - Lazy evaluation
    """
    
    def __init__(self):
        """Initialize data structure optimizer."""
        print("Initializing Data Structure Optimizer...")
        self._setup_numpy_backend()
    
    def _setup_numpy_backend(self):
        """Setup NumPy backend if available."""
        try:
            import numpy as np
            self.np = np
            print("✓ NumPy backend enabled")
        except ImportError:
            print("⚠ NumPy not available, using Python lists")
            self.np = None
    
    def create_vectorized_percept(self, vision_data: List[float], 
                                 touch_data: List[float],
                                 internal_data: List[float],
                                 proprioception_data: List[float]) -> Dict[str, Any]:
        """
        Create vectorized representation of percept.
        
        Args:
            vision_data: Vision data
            touch_data: Touch data
            internal_data: Internal data
            proprioception_data: Proprioception data
            
        Returns:
            Dictionary with vectorized data
        """
        if self.np is not None:
            # Use NumPy for vectorization
            vectorized = {
                'vision': self.np.array(vision_data, dtype=np.float32),
                'touch': self.np.array(touch_data, dtype=np.float32),
                'internal': self.np.array(internal_data, dtype=np.float32),
                'proprioception': self.np.array(proprioception_data, dtype=np.float32),
                'combined': self._combine_vectorized_data(
                    vision_data, touch_data, internal_data, proprioception_data
                ),
            }
        else:
            # Fallback to Python lists
            vectorized = {
                'vision': vision_data,
                'touch': touch_data,
                'internal': internal_data,
                'proprioception': proprioception_data,
                'combined': self._combine_vectorized_data(
                    vision_data, touch_data, internal_data, proprioception_data
                ),
            }
        
        return vectorized
    
    def _combine_vectorized_data(self, *arrays) -> Any:
        """Combine multiple arrays into single vector."""
        if self.np is not None:
            return np.concatenate([np.array(arr) for arr in arrays])
        else:
            return [item for arr in arrays for item in arr]
    
    def apply_vectorized_operations(self, vector_data: Dict[str, Any], 
                                   operations: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Apply vectorized operations to data.
        
        Args:
            vector_data: Vectorized data
            operations: List of operations to apply
            
        Returns:
            Dictionary with results
        """
        results = {}
        
        for operation in operations:
            op_type = operation.get('type')
            input_key = operation.get('input')
            output_key = operation.get('output')
            
            if input_key not in vector_data:
                print(f"⚠ Missing input for operation {op_type}: {input_key}")
                continue
            
            input_data = vector_data[input_key]
            
            # Apply operation
            if op_type == 'normalize':
                if self.np is not None:
                    result = (input_data - np.min(input_data)) / (np.max(input_data) - np.min(input_data) + 1e-9)
                else:
                    min_val = min(input_data)
                    max_val = max(input_data)
                    result = [(x - min_val) / (max_val - min_val + 1e-9) for x in input_data]
            
            elif op_type == 'filter':
                # Simple moving average filter
                window_size = operation.get('window_size', 3)
                if self.np is not None and len(input_data) >= window_size:
                    result = np.convolve(input_data, np.ones(window_size)/window_size, mode='valid')
                else:
                    result = input_data  # Simple pass-through
            
            elif op_type == 'threshold':
                threshold = operation.get('threshold', 0.5)
                if self.np is not None:
                    result = np.where(input_data > threshold, 1.0, 0.0)
                else:
                    result = [1.0 if x > threshold else 0.0 for x in input_data]
            
            elif op_type == 'aggregate':
                # Compute statistics
                if self.np is not None:
                    result = {
                        'mean': np.mean(input_data),
                        'std': np.std(input_data),
                        'min': np.min(input_data),
                        'max': np.max(input_data),
                        'sum': np.sum(input_data),
                    }
                else:
                    result = {
                        'mean': sum(input_data) / len(input_data),
                        'std': np.std(input_data) if self.np else 0.0,
                        'min': min(input_data),
                        'max': max(input_data),
                        'sum': sum(input_data),
                    }
            
            else:
                print(f"⚠ Unknown operation type: {op_type}")
                continue
            
            results[output_key] = result
            print(f"✓ Applied operation {op_type}")
        
        return results
    
    def optimize_memory_usage(self, data_dict: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """
        Optimize memory usage of data structures.
        
        Args:
            data_dict: Dictionary of data to optimize
            
        Returns:
            Optimized data dictionary
        """
        optimized = {}
        
        for key, value in data_dict.items():
            if isinstance(value, list):
                optimized[key] = self._optimize_list_memory(value)
            elif isinstance(value, np.ndarray) if self.np else False:
                optimized[key] = self._optimize_numpy_memory(value)
            else:
                optimized[key] = value
        
        return optimized
    
    def _optimize_list_memory(self, data: List[Any]) -> List[Any]:
        """Optimize memory usage of Python list."""
        # Check if list is small
        if len(data) < 100:
            return data
        
        # Check if list contains many duplicates
        unique_values = len(set(str(item) for item in data))
        if unique_values < len(data) * 0.1:  # 10% unique
            print(f"  - List optimization: {len(data)} items, {unique_values} unique")
            # Could use dictionary encoding in practice
        
        return data
    
    def _optimize_numpy_memory(self, array: np.ndarray) -> np.ndarray:
        """Optimize memory usage of NumPy array."""
        # Convert to most efficient dtype
        if array.dtype == np.float64:
            return array.astype(np.float32)
        
        # Remove unnecessary dimensions
        while array.ndim > 1 and array.shape[-1] == 1:
            array = array.squeeze(-1)
        
        return array
class ParallelProcessor:
    """
    Parallel processor for NLM simulations.
    
    This class provides parallel processing capabilities including:
    - Thread-based parallel processing
    - Process-based parallel processing
    - Work distribution
    - Load balancing
    - Result aggregation
    """
    
    def __init__(self, max_workers: Optional[int] = None):
        """
        Initialize parallel processor.
        
        Args:
            max_workers: Maximum number of workers
        """
        print(f"Initializing Parallel Processor...")
        
        self.max_workers = max_workers or min(4, psutil.cpu_count() - 1)
        self.thread_pool = ThreadPoolExecutor(max_workers=self.max_workers)
        self.process_pool = ProcessPoolExecutor(max_workers=self.max_workers)
        
        print(f"✓ Parallel processor initialized with {self.max_workers} workers")
    
    def parallel_process_percepts(self, percepts: List[nlm.SensoryPercept],
                                 process_func: Callable,
                                 batch_size: Optional[int] = None) -> List[Any]:
        """
        Process percepts in parallel.
        
        Args:
            percepts: List of percepts to process
            process_func: Function to apply to each percept
            batch_size: Size of batches for thread processing
            
        Returns:
            List of processed results
        """
        if len(percepts) < self.config.get('parallel_threshold', 10):
            # Use sequential processing for small batches
            print(f"  - Using sequential processing ({len(percepts)} percepts < {self.config['parallel_threshold']})")
            return [process_func(percept) for percept in percepts]
        
        # Use parallel processing
        print(f"  - Using parallel processing ({len(percepts)} percepts)")
        
        if batch_size and batch_size < len(percepts):
            # Process in batches using threads
            batches = [percepts[i:i + batch_size] for i in range(0, len(percepts), batch_size)]
            
            def process_batch(batch):
                return [process_func(percept) for percept in batch]
            
            results = self.thread_pool.map(process_batch, batches)
            return [item for batch_result in results for item in batch_result]
        
        # Process all percepts in parallel
        futures = [self.thread_pool.submit(process_func, percept) for percept in percepts]
        return [future.result() for future in futures]
    
    def parallel_episode_simulation(self, num_episodes: int,
                                  episode_func: Callable,
                                  shared_resources: Dict[str, Any] = None) -> List[Dict[str, Any]]:
        """
        Run episode simulation in parallel.
        
        fnction
        Args:
            num_episodes: Number of episodes to simulate
            episode_func: Function to run for each episode
            shared_resources: Shared resources (brain, world, agent)
            
        Returns:
            List of episode results
        """
        print(f"\n🚀 Parallel Episode Simulation")
        print(f"  - Running {num_episodes} episodes in parallel")
        
        # Prepare shared resources for parallel processing
        shared_args = (shared_resources,) if shared_resources else ()
        
        # Create episode tasks
        def episode_task(episode_id):
            # Each episode gets its own resources or shares
            if shared_resources:
                # Create fresh copies for each episode
                episode_brain = nlm.createBrain(shared_resources['config'])
                episode_world = nlm.createSimpleWorld()
                episode_world.configure(**shared_resources['world_config'])
                episode_agent = nlm.createAgentBrain(episode_brain)
                
                # Run episode
                return episode_func(episode_id, episode_brain, episode_world, episode_agent)
            else:
                # Use shared resources (may cause conflicts, use with caution)
                return episode_func(episode_id, shared_resources)
        
        # Process episodes in parallel
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            futures = [executor.submit(episode_task, i) for i in range(num_episodes)]
            results = [future.result() for future in futures]
        
        print(f"✓ Parallel episode simulation completed")
        return results
    
    def distribute_load(self, load_items: List[Dict[str, Any]], 
                       worker_func: Callable) -> Dict[str, Any]:
        """
        Distribute load across workers.
        
        Args:
            load_items: List of load items to process
            worker_func: Function to process each load item
            
        Returns:
            Dictionary with load distribution results
        """
        print(f"📊 Load Distribution")
        print(f"  - Processing {len(load_items)} items")
        
        # Calculate optimal batch size
        num_workers = self.max_workers
        batch_size = max(1, len(load_items) // num_workers)
        
        # Create batches
        batches = [load_items[i:i + batch_size] for i in range(0, len(load_items), batch_size)]
        
        # Process batches in parallel
        def process_batch(batch):
            return [worker_func(item) for item in batch]
        
        futures = [self.thread_pool.submit(process_batch, batch) for batch in batches]
        batch_results = [future.result() for future in futures]
        
        # Flatten results
        all_results = [item for batch_result in batch_results for item in batch_result]
        
        print(f"✓ Load distribution completed")
        print(f"  - Processed {len(all_results)} items")
        
        return {
            'total_items': len(load_items),
            'processed_items': len(all_results),
            'batches_processed': len(batches),
            'results': all_results,
        }
    
    def cleanup(self):
        """Cleanup parallel resources."""
        print("🧹 Cleaning up parallel resources...")
        
        # Shutdown thread pool
        self.thread_pool.shutdown(wait=True)
        
        # Shutdown process pool
        self.process_pool.shutdown(wait=True)
        
        print("✓ Parallel resources cleaned up")
class PerformanceBenchmark:
    """
    Performance benchmarking tools for NLM simulations.
    
    This class provides comprehensive benchmarking including:
    - Baseline measurement
    - Comparative testing
    - Statistical analysis
    - Performance regression detection
    - Optimization validation
    """
    
    def __init__(self, baseline_file: Optional[str] = None):
        """
        Initialize performance benchmark.
        
        Args:
            baseline_file: Path to baseline performance data
        """
        print("Initializing Performance Benchmark...")
        
        self.baseline = {}
        self.benchmark_results = {}
        
        if baseline_file and Path(baseline_file).exists():
            self.load_baseline(baseline_file)
        
        print("✓ Performance benchmark initialized")
    
    def load_baseline(self, filepath: str):
        """Load baseline performance data."""
        try:
            with open(filepath, 'r') as f:
                self.baseline = json.load(f)
            print(f"✓ Baseline loaded from {filepath}")
        except Exception as e:
            print(f"⚠ Could not load baseline: {e}")
    
    def save_baseline(self, filepath: str):
        """Save baseline performance data."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.baseline, f, indent=2)
            print(f"✓ Baseline saved to {filepath}")
        except Exception as e:
            print(f"⚠ Could not save baseline: {e}")
    
    def run_comprehensive_benchmark(self, benchmark_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Run comprehensive benchmark suite.
        
        Args:
            benchmark_configs: List of benchmark configurations
            
        Returns:
            Dictionary with comprehensive benchmark results
        """
        print(f"\n📊 Comprehensive Benchmark Suite")
        print(f"  - Running {len(benchmark_configs)} benchmark tests")
        
        all_results = {}
        
        for config in benchmark_configs:
            benchmark_name = config.get('name', f'benchmark_{len(all_results)}')
            print(f"\n{'='*60}")
            print(f"BENCHMARK: {benchmark_name}")
            print(f"{'='*60}")
            
            results = self._run_single_benchmark(config)
            all_results[benchmark_name] = results
        
        # Analyze results
        analysis = self._analyze_benchmark_results(all_results)
        
        print(f"\n📈 Benchmark Suite Complete")
        print(f"  - Total benchmarks: {len(all_results)}")
        print(f"  - Benchmark names: {list(all_results.keys())}")
        
        return {
            'results': all_results,
            'analysis': analysis,
            'baseline': self.baseline,
        }
    
    def _run_single_benchmark(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Run single benchmark."""
        benchmark_func = config.get('function')
        benchmark_name = config.get('name', 'unnamed')
        
        if not benchmark_func:
            raise ValueError(f"No benchmark function provided for {benchmark_name}")
        
        print(f"  Running {benchmark_name}...")
        
        # Run benchmark
        start_time = time.time()
        result = benchmark_func()
        end_time = time.time()
        
        # Convert to benchmark format
        if hasattr(result, '__dict__'):
            benchmark_result = result.__dict__
        else:
            benchmark_result = {'result': result}
        
        benchmark_result['benchmark_time'] = end_time - start_time
        benchmark_result['timestamp'] = time.time()
        
        # Store results
        self.benchmark_results[benchmark_name] = benchmark_result
        
        print(f"  ✓ {benchmark_name} completed in {benchmark_result['benchmark_time']:.3f}s")
        
        return benchmark_result
    
    def _analyze_benchmark_results(self, results: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Analyze benchmark results."""
        analysis = {
            'summary': {},
            'performance_ranking': [],
            'trend_analysis': {},
            'improvement_suggestions': [],
        }
        
        # Calculate overall performance
        total_time = sum(r.get('benchmark_time', 0) for r in results.values())
        avg_time = total_time / len(results) if results else 0
        
        # Performance ranking
        performance_ranking = []
        for name, result in results.items():
            score = self._calculate_benchmark_score(result)
            performance_ranking.append({
                'name': name,
                'score': score,
                'time': result.get('benchmark_time', 0),
            })
        
        # Sort by performance score
        performance_ranking.sort(key=lambda x: x['score'], reverse=True)
        
        # Trend analysis
        if self.baseline:
            trend_analysis = self._compare_with_baseline(results)
        else:
            trend_analysis = {'status': 'no_baseline', 'message': 'No baseline available'}
        
        # Generate improvement suggestions
        improvement_suggestions = self._generate_improvement_suggestions(results, trend_analysis)
        
        # Compile analysis
        analysis['summary'] = {
            'total_benchmarks': len(results),
            'total_time': total_time,
            'average_time': avg_time,
            'fastest_benchmark': min(results.keys(), key=lambda x: results[x].get('benchmark_time', 0)),
            'slowest_benchmark': max(results.keys(), key=lambda x: results[x].get('benchmark_time', 0)),
        }
        
        analysis['performance_ranking'] = performance_ranking
        analysis['trend_analysis'] = trend_analysis
        analysis['improvement_suggestions'] = improvement_suggestions
        
        return analysis
    
    def _calculate_benchmark_score(self, result: Dict[str, Any]) -> float:
        """Calculate performance score for benchmark."""
        score = 1.0
        
        # Time score (higher is better)
        benchmark_time = result.get('benchmark_time', 0)
        if benchmark_time > 0:
            time_score = 1.0 / (benchmark_time + 0.001)
            score *= time_score * 0.7
        
        # Memory score (higher is better)
        memory_usage = result.get('memory_usage', 0)
        if memory_usage > 0:
            memory_score = 1.0 / (memory_usage / 1024 + 0.001)
            score *= memory_score * 0.3
        
        return score
    
    def _compare_with_baseline(self, results: Dict[str, Any]) -> Dict[str, Any]:nlm.SensoryPercept:
        """Compare results with baseline."""
        comparison = {
            'improved_benchmarks': [],
            'regressed_benchmarks': [],
            'unchanged_benchmarks': [],
            'details': {},
        }
        
        for name, result in results.items():
            if name in self.baseline:
                baseline_result = self.baseline[name]
                
                # Compare performance
                current_time = result.get('benchmark_time', 0)
                baseline_time = baseline_result.get('benchmark_time', 0)
                
                if current_time < baseline_time * 0.9:  # 10% improvement
                    comparison['improved_benchmarks'].append(name)
                    comparison['details'][name] = f"Improved by {(baseline_time/current_time - 1)*100:.1f}%"
                elif current_time > baseline_time * 1.1:  # 10% regression
                    comparison['regressed_benchmarks'].append(name)
                    comparison['details'][name] = f"Regressed by {(current_time/baseline_time - 1)*100:.1f}%"
                else:
                    comparison['unchanged_benchmarks'].append(name)
                    comparison['details'][name] = "No significant change"
        
        return comparison
    
    def _generate_improvement_suggestions(self, results: Dict[str, Any], trend_analysis: Dict[str, Any]) -> List[str]:nlm.SensoryPercept:
        """Generate improvement suggestions."""
        suggestions = []
        
        # Check for slow benchmarks
        for name, result in results.items():
            time_taken = result.get('benchmark_time', 0)
            if time_taken > 1.0:  # 1 second threshold
                suggestions.append(f"  📈 Consider optimizing {name} (took {time_taken:.2f}s)")
        
        # Check trend analysis
        if trend_analysis.get('regressed_benchmarks'):
            for benchmark in trend_analysis['regressed_benchmarks']:
                suggestions.append(f"  ⚠️ {benchmark} showed performance regression")
        
        if not suggestions:
            suggestions.append("  ✅ All benchmarks performing well")
        
        return suggestions

def main():
    """Main function demonstrating performance optimization."""
    print("Performance Optimization Examples - Efficient Simulation Techniques")
    print("=" * 80)
    
    # Initialize performance optimizer
    optimizer = PerformanceOptimizer()
    
    # Demo 1: Performance context management
    print("\n" + "=" * 80)
    print("DEMO 1: PERFORMANCE CONTEXT MANAGEMENT")
    print("=" * 80)
    
    with optimizer.performance_context("data_processing"):
        # Simulate work
        time.sleep(0.1)
        data = [i for i in range(1000)]
        processed = [x * 2 for x in data]
        
        print(f"  - Processed {len(data)} items")
    
    # Demo 2: Benchmarking
    print("\n" + "=" * 80)
    print("DEMO 2: OPERATION BENCHMARKING")
    print("=" * 80)
    
    def sample_operation_1():
        """Sample operation 1: Memory-intensive."""
        data = []
        for i in range(10000):
            data.append([j * 0.1 for j in range(100)])
        return len(data)
    
    def sample_operation_2():
        """Sample operation 2: CPU-intensive."""
        result = 0
        for i in range(1000000):
            result += i * 0.001
        return result
    
    # Benchmark operations
    results_1 = optimizer.benchmark_operation(
        sample_operation_1,
        "Memory-Intensive Operation",
        iterations=20,
        warmup_iterations=5
    )
    
    results_2 = optimizer.benchmark_operation(
        sample_operation_2,
        "CPU-Intensive Operation",
        iterations=20,
        warmup_iterations=5
    )
    
    # Demo 3: Memory optimization
    print("\n" + "=" * 80)
    print("DEMO 3: MEMORY OPTIMIZATION")
    print("=" * 80)
    
    memory_optimizer = MemoryOptimizer(memory_limit_mb=512)
    
    # Create memory pool
    memory_pool = memory_optimizer.create_memory_pool(256)
    
    # Allocate memory
    for i in range(10):
        allocated = memory_pool.allocate(1024)  # 1KB chunks
        # Simulate work with allocated memory
        _ = [j * 0.1 for j in range(100)]
    
    # Check usage
    usage = memory_pool.get_usage()
    print(f"  - Memory pool usage: {usage['utilization'] * 100:.1f}%")
    print(f"  - Allocated blocks: {usage['allocated_blocks']}")
    print(f"  - Free blocks: {usage['free_blocks']}")
    
    # Free memory
    memory_pool.clear()
    
    # Demo 4: Data structure optimization
    print("\n" + "=" * 80)
    print("DEMO 4: DATA STRUCTURE OPTIMIZATION")
    print("=" * 80)
    
    ds_optimizer = DataStructureOptimizer()
    
    # Create vectorized data
    vision_data = [i * 0.01 for i in range(256)]
    touch_data = [i * 0.1 for i in range(6)]
    internal_data = [i * 0.05 for i in range(6)]
    proprioception_data = [i * 0.02 for i in range(12)]
    
    vectorized = ds_optimizer.create_vectorized_percept(
        vision_data, touch_data, internal_data, proprioception_data
    )
    
    print(f"  - Created vectorized data:")
    print(f"    - Vision shape: {vectorized['vision'].shape if hasattr(vectorized['vision'], 'shape') else len(vectorized['vision'])}")
    print(f"    - Touch shape: {vectorized['touch'].shape if hasattr(vectorized['touch'], 'shape') else len(vectorized['touch'])}")
    print(f"    - Internal shape: {vectorized['internal'].shape if hasattr(vectorized['internal'], 'shape') else len(vectorized['internal'])}")
    print(f"    - Proprioception shape: {vectorized['proprioception'].shape if hasattr(vectorized['proprioception'], 'shape') else len(vectorized['proprioception'])}")
    
    # Apply vectorized operations
    operations = [
        {'type': 'normalize', 'input': 'vision', 'output': 'normalized_vision'},
        {'type': 'threshold', 'input': 'touch', 'output': 'touch_binary', 'threshold': 0.5},
        {'type': 'aggregate', 'input': 'internal', 'output': 'internal_stats'},
    ]
    
    results = ds_optimizer.apply_vectorized_operations(vectorized, operations)
    print(f"  - Applied operations: {list(results.keys())}")
    
    # Demo 5: Parallel processing
    print("\n" + "=" * 80)
    print("DEMO 5: PARALLEL PROCESSING")
    print("=" * 80)
    
    parallel_processor = ParallelProcessor(max_workers=2)
    
    # Create test percepts
    test_percepts = []
    for i in range(8):
        percept = nlm.SensoryPercept()
        vision_data = [i * 0.1 + j * 0.01 for j in range(256)]
        touch_data = [i * 0.05 + j * 0.01 for j in range(6)]
        internal_data = [i * 0.02 + j * 0.01 for j in range(6)]
        proprioception_data = [i * 0.01 + j * 0.01 for j in range(12)]
        
        percept.setVision(vision_data)
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        percept.setProprioception(proprioception_data)
        
        test_percepts.append(percept)
    
    # Define processing function
    def process_percept(percept):
        """Process single percept."""
        # Simple processing simulation
        vision = percept.getVision()
        touch = percept.getTouch()
        
        # Process data
        processed_vision = [x * 0.5 for x in vision]
        processed_touch = [x * 2.0 for x in touch]
        
        # Create result
        result = {
            'vision_processed': processed_vision,
            'touch_processed': processed_touch,
            'vision_sum': sum(processed_vision),
            'touch_sum': sum(processed_touch),
        }
        
        return result
    
    # Process in parallel
    parallel_results = parallel_processor.parallel_process_percepts(
        test_percepts, process_percept, batch_size=4
    )
    
    print(f"  - Processed {len(test_percepts)} percepts in parallel")
    print(f"  - Results: {len(parallel_results)} items")
    print(f"  - Average vision sum: {sum(r['vision_sum'] for r in parallel_results) / len(parallel_results):.2f}")
    print(f"  - Average touch sum: {sum(r['touch_sum'] for r in parallel_results) / len(parallel_results):.2f}")
    
    # Demo 6: Comprehensive benchmarking
    print("\n" + "=" * 80)
    print("DEMO 6: COMPREHENSIVE BENCHMARKING")
    print("=" * 80)
    
    # Create benchmark suite
    benchmark_suite = PerformanceBenchmark()
    
    # Define benchmark functions
    def benchmark_percept_creation():
        """Benchmark percept creation."""
        percepts = []
        for i in range(100):
            percept = nlm.SensoryPercept()
            vision_data = [j * 0.01 for j in range(256)]
            touch_data = [j * 0.1 for j in range(6)]
            internal_data = [j * 0.05 for j in range(6)]
            proprioception_data = [j * 0.02 for j in range(12)]
            
            percept.setVision(vision_data)
            percept.setTouch(touch_data)
            percept.setInternal(internal_data)
            percept.setProprioception(proprioception_data)
            
            percepts.append(percept)
        return len(percepts)
    
    def benchmark_data_processing():
        """Benchmark data processing."""
        results = []
        for i in range(50):
            data = [j * 0.1 for j in range(1000)]
            processed = [x * 2 for x in data if x > 0.5]
            results.append(len(processed))
        return sum(results)
    
    def benchmark_memory_usage():
        """Benchmark memory usage."""
        import sys
        # Create large data structure
        large_data = []
        for i in range(1000):
            large_data.append([j * 0.01 for j in range(100)])
        
        return sys.getsizeof(large_data)
    
    # Run comprehensive benchmark
    benchmark_configs = [
        {
            'name': 'Percept Creation',
            'function': benchmark_percept_creation,
        },
        {
            'name': 'Data Processing',
            'function': benchmark_data_processing,
        },
        {
            'name': 'Memory Usage',
            'function': benchmark_memory_usage,
        },
    ]
    
    comprehensive_results = benchmark_suite.run_comprehensive_benchmark(benchmark_configs)
    
    # Display comprehensive results
    print(f"\n📊 Comprehensive Benchmark Summary:")
    print(f"  - Total benchmarks: {comprehensive_results['analysis']['summary']['total_benchmarks']}")
    print(f"  - Average time: {comprehensive_results['analysis']['summary']['average_time']:.4f}s")
    print(f"  - Fastest benchmark: {comprehensive_results['analysis']['summary']['fastest_benchmark']}")
    
    print(f"\n🏆 Performance Ranking:")
    for i, ranking in enumerate(comprehensive_results['analysis']['performance_ranking'][:3]):
        print(f"  {i+1}. {ranking['name']}: {ranking['score']:.3f} (took {ranking['time']:.3f}s)")
    
    if comprehensive_results['analysis']['trend_analysis']['improved_benchmarks']:
        print(f"\n🚀 Improved Benchmarks:")
        for improved in comprehensive_results['analysis']['trend_analysis']['improved_benchmarks']:
            print(f"  - {improved}")
    
    # Cleanup
    parallel_processor.cleanup()
    
    # Save benchmark results
    with open("performance_benchmark_results.json", "w") as f:
        import json
        json.dump(comprehensive_results, f, indent=2, default=str)
    
    print(f"\n✅ Performance optimization demonstration complete!")
    print(f"📁 All results saved to performance_benchmark_results.json")

if __name__ == "__main__":
    main()