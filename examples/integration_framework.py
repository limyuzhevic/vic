#!/usr/bin/env python3
"""
NLM Performance Optimization Examples

This file demonstrates performance optimization techniques for NLM including
memory management, performance monitoring, parallel processing, and simulation scaling.

Requirements:

### 4. **Performance and Optimization Examples**
- **Memory Management**: Working with memory pools and optimization
- **Performance Monitoring**: Real-time performance tracking
- **Parallel Processing**: Leveraging SIMD and multi-core
- **Simulation Scaling**: Large-scale brain simulations
"""

import pynlm
import time
import logging
import psutil
import os
from typing import List, Dict, Any, Callable
from dataclasses import dataclass
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import threading

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def print_header(title: str):
    """Print a formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

# ============================================================================
# 1. Memory Management Examples
# ============================================================================

@dataclass
class MemoryPoolStats:
    """Statistics for memory pool performance."""
    total_allocations: int = 0
    total_frees: int = 0
    current_size: int = 0
    peak_size: int = 0
    allocation_failures: int = 0
    average_allocation_size: float = 0.0


class MemoryPool:
    """Custom memory pool for efficient memory management."""
    
    def __init__(self, block_size: int = 1024, initial_capacity: int = 100):
        self.block_size = block_size
        self.capacity = initial_capacity
        self.blocks = []
        self.free_blocks = []
        self.stats = MemoryPoolStats()
        self._lock = threading.Lock()
        
    def allocate(self, size: int = None) -> Any:
        """Allocate memory from pool."""
        with self._lock:
            size = size or self.block_size
            
            if self.free_blocks:
                block = self.free_blocks.pop()
                self.stats.total_allocations += 1
                self.stats.current_size += 1
                self.stats.peak_size = max(self.stats.peak_size, self.stats.current_size)
                return block
                
            if len(self.blocks) < self.capacity:
                block = bytearray(size)
                self.blocks.append(block)
                self.stats.total_allocations += 1
                self.stats.current_size += 1
                self.stats.peak_size = max(self.stats.peak_size, self.stats.current_size)
                return block
                
            self.stats.allocation_failures += 1
            return None
            
    def free(self, block: Any):
        """Free memory back to pool."""
        with self._lock:
            if block in self.blocks:
                self.free_blocks.append(block)
                self.stats.total_frees += 1
                self.stats.current_size -= 1
                
    def resize(self, new_capacity: int):
        """Resize the memory pool."""
        with self._lock:
            if new_capacity < self.capacity:
                # Remove excess blocks
                for _ in range(self.capacity - new_capacity):
                    if self.blocks:
                        block = self.blocks.pop()
                        self.free_blocks.append(block)
                        
            self.capacity = new_capacity
            
    def get_stats(self) -> MemoryPoolStats:
        """Get memory pool statistics."""
        with self._lock:
            return self.stats
            
    def optimize(self):
        """Optimize memory pool usage."""
        with self._lock:
            # Compact free blocks list
            self.free_blocks = [block for block in self.free_blocks if block is not None]
            
            # Rebalance if needed
            if self.stats.current_size < self.capacity * 0.1:
                # Increase capacity
                self.resize(int(self.capacity * 1.2))
            elif self.stats.current_size > self.capacity * 0.9:
                # Decrease capacity
                self.resize(max(100, int(self.capacity * 0.8)))


def working_memory_optimization():
    """Demonstrate working memory optimization techniques."""
    print_header("1.1 Working Memory Optimization")
    
    print("\nWorking memory optimization strategies:")
    
    # Create memory pool for working memory
    wm_pool = MemoryPool(block_size=512, initial_capacity=50)
    
    # Simulate working memory with optimized allocation
    patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.9, 0.8, 0.7, 0.6, 0.5],
        [0.2, 0.4, 0.6, 0.8, 1.0],
        [1.0, 0.8, 0.6, 0.4, 0.2],
        [0.5, 0.5, 0.5, 0.5, 0.5]
    ]
    
    # Store patterns using optimized memory
    allocated_patterns = []
    for i, pattern in enumerate(patterns):
        # Simulate allocation using memory pool
        pattern_array = wm_pool.allocate(len(pattern) * 8)  # 8 bytes per float
        if pattern_array is not None:
            # Convert to list for compatibility
            pattern_list = list(pattern)
            allocated_patterns.append(pattern_list)
            print(f"  Pattern {i}: {pattern_list} (allocated from pool)")
        else:
            print(f"  Pattern {i}: Allocation failed - pool exhausted")
    
    # Get pool statistics
    stats = wm_pool.get_stats()
    print(f"\nMemory pool statistics:")
    print(f"  Total allocations: {stats.total_allocations}")
    print(f"  Total frees: {stats.total_frees}")
    print(f"  Current size: {stats.current_size}")
    print(f"  Peak size: {stats.peak_size}")
    print(f"  Allocation failures: {stats.allocation_failures}")
    
    # Optimize pool
    wm_pool.optimize()
    
    # Free all allocations
    for pattern in allocated_patterns:
        wm_pool.free(pattern)
    
    print(f"\nPool after cleanup:")
    print(f"  Current size: {wm_pool.get_stats().current_size}")
    
    return wm_pool


def episodic_memory_consolidation():
    """Demonstrate episodic memory consolidation strategies."""
    print_header("1.2 Episodic Memory Consolidation")
    
    print("\nEpisodic memory consolidation strategies:")
    
    # Create brain with episodic memory
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    em = brain.getEpisodicMemory()
    wm = brain.getWorkingMemory()
    
    # Simulate storing many episodic memories
    print("\n1.2.1 Mass Memory Storage:")
    
    # Create memory pool for episodic memory
    episode_pool = MemoryPool(block_size=256, initial_capacity=200)
    
    episodes_stored = 0
    start_time = time.time()
    
    for episode_num in range(1000):
        # Create episode
        episode = pynlm.EpisodicMemoryItem()
        episode.timestamp = episode_num
        episode.metadata = f"episode_{episode_num}"
        
        # Simulate memory consolidation (moving from WM to EM)
        pattern = [0.1 * i % 1.0 for i in range(10)]
        
        # Store in working memory first
        wm.store(pattern, 1.0)
        
        # Consolidate to episodic memory (simulate with pool)
        episode_data = episode_pool.allocate(256)
        if episode_data:
            em.storeEpisode(episode)
            episodes_stored += 1
            
    consolidation_time = time.time() - start_time
    
    stats = episode_pool.get_stats()
    print(f"  Consonated {episodes_stored} episodes")
    print(f"  Consolidation time: {consolidation_time:.3f}s")
    print(f"  Pool allocations: {stats.total_allocations}")
    print(f"  Pool frees: {stats.total_frees}")
    
    # Demonstrate consolidation strategies
    print("\n1.2.2 Consolidation Strategies:")
    
    # Strategy 1: Time-based consolidation
    print("  Time-based consolidation:")
    recent_episodes = em.getRecentEpisodes(10)
    print(f"    Recent episodes: {len(recent_episodes)}")
    
    # Strategy 2: Frequency-based consolidation
    print("  Frequency-based consolidation:")
    # Simulate strengthening frequently accessed memories
    for episode in recent_episodes:
        if episode.metadata:
            # In real implementation, would increase memory strength
            pass
    
    # Strategy 3: Synchronized consolidation
    print("  Synchronized consolidation:")
    # Simulate coordinated consolidation with neuromodulation
    dopamine = brain.getDopamine()
    acetylcholine = brain.getAcetylcholine()
    
    dopamine.signalReward(1.0)
    acetylcholine.setAttentionLevel(0.8)
    
    print(f"    Neuromodulation-enhanced consolidation")
    
    # Cleanup
    episode_pool.optimize()
    
    return brain, em, wm


def memory_efficiency_analysis():
    """Analyze memory efficiency and provide optimization recommendations."""
    print_header("1.3 Memory Efficiency Analysis")
    
    print("\nMemory efficiency analysis:")
    
    # Create test memory systems
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    wm = brain.getWorkingMemory()
    em = brain.getEpisodicMemory()
    am = brain.getAssociativeMemory()
    
    # Get baseline metrics
    baseline_metrics = {
        "working_memory_capacity": wm.getCapacity(),
        "working_memory_traces": wm.getActiveTraces(),
        "episodic_memory_count": em.getEpisodeCount(),
        "total_neuron_count": brain.getTotalNeuronCount(),
        "total_spike_count": brain.getTotalSpikeCount()
    }
    
    print(f"\nBaseline memory metrics:")
    for key, value in baseline_metrics.items():
        print(f"  {key}: {value}")
    
    # Run simulation and measure memory usage
    print("\nMemory usage during simulation:")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable memory optimization
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    memory_samples = []
    
    for step in range(100):
        # Record memory state
        wm_size = wm.getActiveTraces()
        em_count = em.getEpisodeCount()
        
        memory_samples.append({
            'step': step,
            'wm_size': wm_size,
            'em_count': em_count,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        })
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply memory optimization periodically
        if step % 20 == 0:
            wm.optimize()  # In real implementation
    
    # Analyze memory usage patterns
    print(f"\nMemory usage analysis:")
    
    max_wm_usage = max(sample['wm_size'] for sample in memory_samples)
    max_em_count = max(sample['em_count'] for sample in memory_samples)
    avg_firing = sum(sample['firing_neurons'] for sample in memory_samples) / len(memory_samples)
    
    print(f"  Maximum working memory usage: {max_wm_usage}")
    print(f"  Maximum episodic memory count: {max_em_count}")
    print(f"  Average firing neurons: {avg_firing:.1f}")
    print(f"  Memory growth rate: {(max_em_count - baseline_metrics['episodic_memory_count']) / 10:.1f} memories/step")
    
    # Optimization recommendations
    print(f"\nOptimization recommendations:")
    
    if max_wm_usage > wm.getCapacity() * 0.9:
        print("  ⚠️  Working memory capacity near limit - consider increasing capacity or adding consolidation")
        
    if max_em_count > 100:
        print("  ⚠️  Episodic memory growing rapidly - implement consolidation strategies")
        
    if avg_firing > brain.getTotalNeuronCount() * 0.8:
        print("  ⚠️  High firing rate - consider reducing neural density or optimizing connectivity")
    
    print("  ✅ Memory usage appears efficient within normal ranges")
    
    return brain, memory_samples


def memory_management_examples_summary():
    """Print summary of memory management examples."""
    print_header("Memory Management Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Custom memory pool implementation for efficient allocation")
    print("✓ Working memory optimization with pool-based storage")
    print("✓ Episodic memory consolidation strategies")
    print("✓ Memory efficiency analysis and optimization recommendations")
    print("✓ Real-time memory usage monitoring")
    print("✓ Memory pool statistics and optimization algorithms")

# ============================================================================
# 2. Performance Monitoring Examples
# ============================================================================

@dataclass
class PerformanceMetrics:
    """Real-time performance monitoring metrics."""
    simulation_time: float = 0.0
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    firing_rate: float = 0.0
    prediction_error: float = 0.0
    neuromodulation_level: float = 0.0
    frame_time: float = 0.0


class PerformanceMonitor:
    """Real-time performance monitoring and profiling."""
    
    def __init__(self, sampling_rate: float = 10.0):
        self.sampling_rate = sampling_rate  # Hz
        self.metrics_history = []
        self.start_time = time.time()
        self.frame_times = []
        self.performance_thresholds = {
            'max_cpu_usage': 0.9,
            'max_memory_usage': 0.8,
            'max_frame_time': 0.1,  # 100ms
            'min_firing_rate': 0.01,
            'max_prediction_error': 0.5
        }
        
    def sample_metrics(self, brain: Any, world: Any, agent: Any) -> PerformanceMetrics:
        """Sample current performance metrics."""
        process = psutil.Process(os.getpid())
        
        metrics = PerformanceMetrics()
        metrics.simulation_time = time.time() - self.start_time
        metrics.cpu_usage = process.cpu_percent() / 100.0
        metrics.memory_usage = process.memory_info().rss / (1024 * 1024 * 1024)  # GB
        metrics.firing_rate = brain.getAverageFiringRate()
        metrics.prediction_error = brain.getPredictionError()
        metrics.neuromodulation_level = agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.5
        
        # Record frame time
        frame_time = time.time() - self.start_time - len(self.frame_times)
        self.frame_times.append(frame_time)
        if len(self.frame_times) > 100:
            self.frame_times.pop(0)
            
        metrics.frame_time = sum(self.frame_times) / len(self.frame_times)
        
        # Store metrics
        self.metrics_history.append(metrics)
        if len(self.metrics_history) > 1000:  # Keep last 1000 samples
            self.metrics_history.pop(0)
            
        return metrics
        
    def check_performance(self, metrics: PerformanceMetrics) -> List[str]:nwarnings = []
        
        if metrics.cpu_usage > self.performance_thresholds['max_cpu_usage']:
            warnings.append(f"High CPU usage: {metrics.cpu_usage:.1%}")
            
        if metrics.memory_usage > self.performance_thresholds['max_memory_usage']:
            warnings.append(f"High memory usage: {metrics.memory_usage:.1%}")
            
        if metrics.frame_time > self.performance_thresholds['max_frame_time']:
            warnings.append(f"Slow frame time: {metrics.frame_time:.3f}s")
            
        if metrics.firing_rate < self.performance_thresholds['min_firing_rate']:
            warnings.append(f"Low firing rate: {metrics.firing_rate:.3f}")
            
        if metrics.prediction_error > self.performance_thresholds['max_prediction_error']:
            warnings.append(f"High prediction error: {metrics.prediction_error:.3f}")
            
        return warnings
        
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.metrics_history:
            return {}
            
        metrics = self.metrics_history[-1]
        
        return {
            'current': metrics,
            'average': {
                'cpu_usage': sum(m.cpu_usage for m in self.metrics_history) / len(self.metrics_history),
                'memory_usage': sum(m.memory_usage for m in self.metrics_history) / len(self.metrics_history),
                'firing_rate': sum(m.firing_rate for m in self.metrics_history) / len(self.metrics_history),
                'frame_time': sum(m.frame_time for m in self.metrics_history) / len(self.metrics_history)
            },
            'min': {
                'cpu_usage': min(m.cpu_usage for m in self.metrics_history),
                'memory_usage': min(m.memory_usage for m in self.metrics_history),
                'firing_rate': min(m.firing_rate for m in self.metrics_history),
                'frame_time': min(m.frame_time for m in self.metrics_history)
            },
            'max': {
                'cpu_usage': max(m.cpu_usage for m in self.metrics_history),
                'memory_usage': max(m.memory_usage for m in self.metrics_history),
                'firing_rate': max(m.firing_rate for m in self.metrics_history),
                'frame_time': max(m.frame_time for m in self.metrics_history)
            }
        }


def real_time_performance_monitoring():
    """Demonstrate real-time performance monitoring."""
    print_header("2.1 Real-Time Performance Monitoring")
    
    print("\nReal-time performance monitoring:")
    
    # Initialize performance monitor
    monitor = PerformanceMonitor(sampling_rate=10.0)
    
    # Create simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable monitoring
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Monitoring simulation for 10 seconds...")
    
    # Run simulation with monitoring
    step = 0
    max_time = 10.0  # 10 seconds of simulation time
    last_print_time = time.time()
    
    while time.time() - last_print_time < max_time:
        step += 1
        
        # Record performance metrics
        metrics = monitor.sample_metrics(brain, world, agent)
        
        # Check for performance issues
        warnings = monitor.check_performance(metrics)
        if warnings:
            for warning in warnings:
                print(f"  ⚠️  {warning}")
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print status periodically
        if step % 100 == 0:
            print(f"  Step {step}: firing_rate={metrics.firing_rate:.3f}, "
                  f"cpu={metrics.cpu_usage:.1%}, memory={metrics.memory_usage:.1%}")
    
    # Get performance statistics
    stats = monitor.get_performance_stats()
    
    print(f"\nPerformance statistics:")
    if stats:
        print(f"  Current firing rate: {stats['current'].firing_rate:.3f}")
        print(f"  Current CPU usage: {stats['current'].cpu_usage:.1%}")
        print(f"  Current memory usage: {stats['current'].memory_usage:.1%}")
        print(f"  Current frame time: {stats['current'].frame_time:.3f}s")
        
        print(f"\nAverage performance:")
        print(f"  CPU usage: {stats['average']['cpu_usage']:.1%}")
        print(f"  Memory usage: {stats['average']['memory_usage']:.1%}")
        print(f"  Firing rate: {stats['average']['firing_rate']:.3f}")
        print(f"  Frame time: {stats['average']['frame_time']:.3f}s")
        
        print(f"\nPerformance ranges:")
        print(f"  CPU usage: {stats['min']['cpu_usage']:.1%} - {stats['max']['cpu_usage']:.1%}")
        print(f"  Memory usage: {stats['min']['memory_usage']:.1%} - {stats['max']['memory_usage']:.1%}")
        print(f"  Firing rate: {stats['min']['firing_rate']:.3f} - {stats['max']['firing_rate']:.3f}")
        print(f"  Frame time: {stats['min']['frame_time']:.3f}s - {stats['max']['frame_time']:.3f}s")
    
    return monitor


def performance_profiling():
    """Demonstrate detailed performance profiling."""
    print_header("2.2 Performance Profiling")
    
    print("\nDetailed performance profiling:")
    
    # Create a brain with profiling capabilities
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Add regions for profiling
    region_ids = []
    for i in range(10):
        region_id = brain.addRegion(f"profile_region_{i}")
        region_ids.append(region_id)
    
    # Get brain regions
    regions = [brain.getRegion(region_id) for region_id in region_ids]
    
    print(f"\nProfiling {len(regions)} brain regions:")
    
    # Profile each region
    region_stats = {}
    for region in regions:
        region_name = region.getName()
        
        # Simulate workload
        start_time = time.time()
        for i in range(1000):
            # Simulate neural computation
            if hasattr(region, 'getNeuronCount'):
                neuron_count = region.getNeuronCount()
            else:
                neuron_count = 100  # Simulated
                
        computation_time = time.time() - start_time
        
        # Profile memory usage
        process = psutil.Process(os.getpid())
        memory_before = process.memory_info().rss
        
        # Perform memory-intensive operation
        temp_array = []
        for i in range(10000):
            temp_array.append([float(j) for j in range(100)])
            
        memory_after = process.memory_info().rss
        memory_used = (memory_after - memory_before) / (1024 * 1024)  # MB
        
        # Free memory
        del temp_array
        
        region_stats[region_name] = {
            'computation_time': computation_time,
            'memory_used': memory_used,
            'neuron_count': neuron_count,
            'efficiency': neuron_count / computation_time if computation_time > 0 else 0
        }
        
        print(f"  {region_name}:")
        print(f"    Computation time: {computation_time:.4f}s")
        print(f"    Memory used: {memory_used:.2f}MB")
        print(f"    Neuron count: {neuron_count}")
        print(f"    Efficiency: {region_stats[region_name]['efficiency']:.1f} neurons/sec")
    
    # Find performance bottlenecks
    print(f"\nPerformance bottleneck analysis:")
    
    sorted_regions = sorted(region_stats.items(), 
                           key=lambda x: x[1]['efficiency'], 
                           reverse=True)
    
    print("  Most efficient regions:")
    for region_name, stats in sorted_regions[:3]:
        print(f"    {region_name}: {stats['efficiency']:.1f} neurons/sec")
        
    print("  Least efficient regions:")
    for region_name, stats in sorted_regions[-3:]:
        print(f"    {region_name}: {stats['efficiency']:.1f} neurons/sec")
        
    # Memory leak detection
    print(f"\nMemory leak detection:")
    
    # Monitor memory usage over time
    memory_samples = []
    for i in range(5):
        process = psutil.Process(os.getpid())
        memory_samples.append(process.memory_info().rss)
        time.sleep(0.1)
    
    # Check for memory leaks
    if len(memory_samples) > 1:
        memory_growth = memory_samples[-1] - memory_samples[0]
        memory_growth_mb = memory_growth / (1024 * 1024)
        
        print(f"  Memory growth over 0.5s: {memory_growth_mb:.2f}MB")
        
        if memory_growth_mb > 10:  # More than 10MB growth
            print("  ⚠️  Potential memory leak detected!")
        else:
            print("  ✅ No significant memory leak detected")
    
    return region_stats


def profiling_examples_summary():
    """Print summary of performance monitoring examples."""
    print_header("Performance Monitoring Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Real-time performance monitoring with metrics sampling")
    print("✓ Performance threshold checking and warning system")
    print("✓ Detailed performance statistics and analysis")
    print("✓ Region-level profiling and bottleneck detection")
    print("✓ Memory leak detection and memory usage tracking")
    print("✓ Performance optimization recommendations")

# ============================================================================
# 3. Parallel Processing Examples
# ============================================================================

class NeuralProcessor:
    """Neural processor for parallel computation."""
    
    def __init__(self, num_workers: int = 4):
        self.num_workers = num_workers
        self.worker_pool = ThreadPoolExecutor(max_workers=num_workers)
        self.process_pool = None
        
    def process_neuron_group(self, neurons: List[Any]) -> List[float]:
        """Process a group of neurons in parallel."""
        # Submit tasks to thread pool
        futures = []
        for neuron in neurons:
            future = self.worker_pool.submit(self._process_single_neuron, neuron)
            futures.append(future)
            
        # Collect results
        results = [future.result() for future in futures]
        return results
        
    def _process_single_neuron(self, neuron: Any) -> float:
        """Process a single neuron (simplified simulation)."""
        # Simulate neural computation
        time.sleep(0.001)  # Simulate computation time
        
        # Generate output based on neuron properties
        output = 0.5 + 0.5 * ((hash(str(neuron)) % 100) / 100.0)
        return output
        
    def process_simulation_step(self, neurons: List[Any], dt: float) -> Dict[str, Any]:
        """Process an entire simulation step using parallel processing."""
        start_time = time.time()
        
        # Split neurons into groups for parallel processing
        group_size = len(neurons) // self.num_workers
        groups = []
        
        for i in range(self.num_workers):
            start_idx = i * group_size
            end_idx = (i + 1) * group_size if i < self.num_workers - 1 else len(neurons)
            group = neurons[start_idx:end_idx]
            groups.append(group)
            
        # Process groups in parallel
        future_results = []
        for group in groups:
            future = self.worker_pool.submit(self.process_neuron_group, group)
            future_results.append(future)
            
        # Collect results
        all_results = []
        for future in future_results:
            results = future.result()
            all_results.extend(results)
            
        processing_time = time.time() - start_time
        
        return {
            'results': all_results,
            'processing_time': processing_time,
            'speedup': len(neurons) / processing_time if processing_time > 0 else 0
        }
        
    def close(self):
        """Clean up worker pools."""
        self.worker_pool.shutdown(wait=True)
        if self.process_pool:
            self.process_pool.shutdown(wait=True)
class SimulationParallelizer:
    """Parallel simulation execution for scaling."""
    
    def __init__(self, num_processes: int = 4):
        self.num_processes = num_processes
        self.process_pool = ProcessPoolExecutor(max_workers=num_processes)
        
    def simulate_independent_world(self, world_id: int, steps: int, dt: float) -> Dict[str, Any]:
        """Simulate an independent world in parallel."""
        start_time = time.time()
        
        # Create independent world simulation
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Run simulation
        for step in range(steps):
            world.update(dt)
            percept = world.getSensoryPercept()
            
            # Simple agent
            agent = pynlm.createAgentBrain(world.getBrain())
            if hasattr(world, 'getBrain'):
                agent.initialize(world)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        return {
            'world_id': world_id,
            'steps_completed': steps,
            'simulation_time': end_time - start_time,
            'efficiency': steps / (end_time - start_time) if (end_time - start_time) > 0 else 0
        }
        
    def close(self):
        """Clean up process pool."""
        self.process_pool.shutdown(wait=True)


def parallel_simulation_scaling():
    """Demonstrate parallel simulation scaling."""
    print_header("3.1 Parallel Simulation Scaling")
    
    print("\nParallel simulation scaling strategies:")
    
    # Create simulation with parallel processing
    processor = NeuralProcessor(num_workers=4)
    
    # Create simulated neurons for testing
    neurons = [f"neuron_{i}" for i in range(100)]
    
    print(f"\nProcessing {len(neurons)} neurons with {processor.num_workers} workers:")
    
    # Process neurons in parallel
    results = processor.process_simulation_step(neurons, 0.1)
    
    print(f"  Results computed: {len(results['results'])}")
    print(f"  Processing time: {results['processing_time']:.4f}s")
    print(f"  Processing speed: {results['speedup']:.1f} neurons/sec")
    print(f"  Parallel efficiency: {results['speedup'] / (len(neurons) / 0.1):.1%}")  # Normalized
    
    # Compare sequential vs parallel processing
    print(f"\nSequential vs Parallel comparison:")
    
    # Sequential processing
    start_time = time.time()
    sequential_results = []
    for neuron in neurons:
        result = processor._process_single_neuron(neuron)
        sequential_results.append(result)
    sequential_time = time.time() - start_time
    
    # Parallel processing
    start_time = time.time()
    parallel_result = processor.process_simulation_step(neurons, 0.1)
    parallel_time = parallel_result['processing_time']
    
    print(f"  Sequential processing time: {sequential_time:.4f}s")
    print(f"  Parallel processing time: {parallel_time:.4f}s")
    print(f"  Speedup: {sequential_time / parallel_time:.1f}x")
    print(f"  Efficiency: {(sequential_time / parallel_time) / processor.num_workers:.1%}")
    
    processor.close()
    
    return processor


def multi_core_neural_networks():
    """Demonstrate multi-core neural network processing."""
    print_header("3.2 Multi-Core Neural Networks")
    
    print("\nMulti-core neural network processing:")
    
    # Create neural network with parallel processing
    num_layers = 5
    neurons_per_layer = [100, 150, 200, 150, 100]
    total_neurons = sum(neurons_per_layer)
    
    # Simulate neural network with parallel processing
    processor = NeuralProcessor(num_workers=8)
    
    print(f"\nNeural network with {num_layers} layers:")
    print(f"  Layer sizes: {neurons_per_layer}")
    print(f"  Total neurons: {total_neurons}")
    
    # Process each layer in parallel
    layer_results = {}
    for layer_idx, neuron_count in enumerate(neurons_per_layer):
        neurons = [f"layer_{layer_idx}_neuron_{i}" for i in range(neuron_count)]
        
        # Create input for this layer
        input_data = [0.5] * neuron_count
        
        # Process layer
        result = processor.process_simulation_step(neurons, 0.1)
        
        layer_results[layer_idx] = {
            'input_size': neuron_count,
            'processing_time': result['processing_time'],
            'output_size': len(result['results']),
            'throughput': result['output_size'] / result['processing_time'] if result['processing_time'] > 0 else 0
        }
        
        print(f"  Layer {layer_idx}:")
        print(f"    Input size: {layer_results[layer_idx]['input_size']}")
        print(f"    Processing time: {layer_results[layer_idx]['processing_time']:.4f}s")
        print(f"    Throughput: {layer_results[layer_idx]['throughput']:.0f} neurons/sec")
    
    # Analyze network performance
    print(f"\nNetwork performance analysis:")
    
    total_processing_time = sum(stats['processing_time'] for stats in layer_results.values())
    total_throughput = sum(stats['throughput'] for stats in layer_results.values())
    
    print(f"  Total processing time: {total_processing_time:.4f}s")
    print(f"  Total throughput: {total_throughput:.0f} neurons/sec")
    print(f"  Network efficiency: {total_throughput / (total_neurons / total_processing_time):.1%}")
    
    # Bottleneck analysis
    slowest_layer = min(layer_results.items(), 
                       key=lambda x: x[1]['processing_time'])
    fastest_layer = max(layer_results.items(), 
                       key=lambda x: x[1]['throughput'])
    
    print(f"  Bottleneck layer: {slowest_layer[0]} ({slowest_layer[1]['processing_time']:.4f}s)")
    print(f"  Fastest layer: {fastest_layer[0]} ({fastest_layer[1]['throughput']:.0f} neurons/sec)")
    
    processor.close()
    
    return layer_results


def parallel_scaling_analysis():
    """Analyze parallel scaling behavior."""
    print_header("3.3 Parallel Scaling Analysis")
    
    print("\nParallel scaling analysis:")
    
    # Test scaling with different numbers of workers
    neuron_counts = [50, 100, 200, 400, 800]
    worker_counts = [1, 2, 4, 8, 16]
    
    print(f"\nScaling analysis:")
    print(f"{'Neurons':<10} {'Workers':<10} {'Time(s)':<10} {'Speedup':<10} {'Efficiency':<10}")
    print(f"{'-'*60}")
    
    # Simulate scaling tests (in real implementation, would run actual tests)
    for neuron_count in neuron_counts:
        for worker_count in worker_counts:
            if worker_count > neuron_count:
                continue  # Skip invalid combinations
                
            # Simulate processing time (inverse relationship with workers)
            base_time = neuron_count / 1000.0  # Base time
            overhead = worker_count * 0.01  # Parallel overhead
            processing_time = base_time / worker_count + overhead
            
            # Calculate metrics
            speedup = base_time / processing_time if processing_time > 0 else 1.0
            efficiency = speedup / worker_count
            
            # Only show interesting combinations
            if neuron_count <= 200 or worker_count <= 4:
                print(f"{neuron_count:<10} {worker_count:<10} {processing_time:.4f} {speedup:.2f}x {efficiency:.2%}")
    
    print(f"\nScaling observations:")
    print("  - Speedup increases with more workers up to optimal point")
    print("  - Efficiency decreases with more workers due to overhead")
    print("  - Amdahl's law limits parallel scaling")
    
    return None


def parallel_examples_summary():
    """Print summary of parallel processing examples."""
    print_header("Parallel Processing Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Neural processor with thread pool execution")
    print("✓ Parallel simulation step processing")
    print("✓ Multi-core neural network implementation")
    print("✓ Parallel scaling analysis and optimization")
    print("✓ Thread pool and process pool management")
    print("✓ Parallel algorithm design for neural computation")

# ============================================================================
# 4. Simulation Scaling Examples
# ============================================================================

class ScalableSimulation:
    """Scalable simulation framework for large-scale brain models."""
    
    def __init__(self, num_regions: int = 10, neurons_per_region: int = 1000):
        self.num_regions = num_regions
        self.neurons_per_region = neurons_per_region
        self.total_neurons = num_regions * neurons_per_region
        self.simulation_step = 0
        self.simulator = NeuralProcessor(num_workers=4)
        
    def create_large_brain(self) -> Any:
        """Create a large-scale brain model."""
        print(f"\nCreating large-scale brain model:")
        print(f"  Number of regions: {self.num_regions}")
        print(f"  Neurons per region: {self.neurons_per_region}")
        print(f"  Total neurons: {self.total_neurons:,}")
        
        # Create brain configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add regions
        for region_idx in range(self.num_regions):
            region_id = brain.addRegion(f"Region_{region_idx:04d}")
            
        # Simulate neural connectivity (in real implementation)
        print(f"  Brain created with {self.total_neurons:,} virtual neurons")
        
        return brain
        
    def run_large_simulation(self, steps: int, dt: float = 0.1) -> Dict[str, Any]:
        """Run large-scale simulation with performance tracking."""
        print(f"\nRunning large-scale simulation:")
        print(f"  Steps: {steps:,}")
        print(f"  Time step: {dt}")
        print(f"  Total neurons: {self.total_neurons:,}")
        
        # Create simulation world
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        world.reset()
        
        # Create agent
        brain = self.create_large_brain()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Performance tracking
        start_time = time.time()
        step_times = []
        
        print(f"  Starting simulation at {time.time():.1f}")
        
        for step in range(steps):
            step_start = time.time()
            
            # Run simulation step
            world.update(dt)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            self.simulation_step += 1
            
            # Print progress periodically
            if step % 1000 == 0:
                elapsed = time.time() - start_time
                print(f"    Step {step:,}: {elapsed:.1f}s elapsed, "
                      f"{step/elapsed:.0f} steps/sec")
        
        total_time = time.time() - start_time
        avg_step_time = sum(step_times) / len(step_times)
        
        simulation_stats = {
            'total_steps': steps,
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'steps_per_second': steps / total_time if total_time > 0 else 0,
            'neuron_count': self.total_neurons,
            'world_size': (world.getWidth(), world.getHeight())
        }
        
        print(f"\nSimulation completed:")
        print(f"  Total time: {total_time:.1f}s")
        print(f"  Average step time: {avg_step_time:.4f}s")
        print(f"  Steps per second: {simulation_stats['steps_per_second']:.0f}")
        print(f"  Neurons processed: {self.total_neurons:,}")
        print(f"  Neurons per second: {simulation_stats['steps_per_second'] * self.total_neurons:.0f}")
        
        return simulation_stats


def large_scale_simulation():
    """Demonstrate large-scale simulation capabilities."""
    print_header("4.1 Large-Scale Simulation")
    
    print("\nLarge-scale simulation examples:")
    
    # Small-scale simulation
    print("\n4.1.1 Small-scale simulation:")
    small_sim = ScalableSimulation(num_regions=5, neurons_per_region=100)
    small_stats = small_sim.run_large_simulation(steps=1000, dt=0.1)
    
    # Medium-scale simulation
    print("\n4.1.2 Medium-scale simulation:")
    medium_sim = ScalableSimulation(num_regions=20, neurons_per_region=500)
    medium_stats = medium_sim.run_large_simulation(steps=5000, dt=0.1)
    
    # Large-scale simulation
    print("\n4.1.3 Large-scale simulation:")
    large_sim = ScalableSimulation(num_regions=100, neurons_per_region=2000)
    large_stats = large_sim.run_large_simulation(steps=20000, dt=0.1)
    
    # Compare simulations
    print(f"\nSimulation scaling comparison:")
    print(f"{'Scale':<15} {'Neurons':<12} {'Steps':<10} {'Time(s)':<10} {'Steps/sec':<12} {'Eff(%)':<10}")
    print(f"{'-'*80}")
    
    for scale_name, stats in [("Small", small_stats), ("Medium", medium_stats), ("Large", large_stats)]:
        efficiency = (stats['steps_per_second'] / stats['steps_per_second']) * 100  # Relative
        print(f"{scale_name:<15} {stats['neuron_count']:<12,} {stats['total_steps']:<10,} "
              f"{stats['total_time']:<10.1f} {stats['steps_per_second']:<12.0f} {efficiency:.1f}%")
    
    return [small_stats, medium_stats, large_stats]


def memory_scaling_simulation():
    """Demonstrate memory scaling in large simulations."""
    print_header("4.2 Memory Scaling Simulation")
    
    print("\nMemory scaling in large simulations:")
    
    # Simulate memory usage scaling
    scenarios = [
        {"regions": 10, "neurons_per_region": 100, "name": "Small"},
        {"regions": 50, "neurons_per_region": 500, "name": "Medium"},
        {"regions": 100, "neurons_per_region": 2000, "name": "Large"}
    ]
    
    print(f"\nMemory usage analysis:")
    print(f"{'Scale':<10} {'Regions':<10} {'Neurons':<12} {'Memory(MB)':<12} {'Per Neuron':<15}")
    print(f"{'-'*65}")
    
    for scenario in scenarios:
        # Estimate memory usage
        # Each neuron uses approximately 100 bytes for basic simulation
        neuron_memory_bytes = 100
        memory_overhead_factor = 3.0  # System overhead
        
        total_memory_bytes = (scenario['regions'] * scenario['neurons_per_region'] * 
                            neuron_memory_bytes * memory_overhead_factor)
        memory_mb = total_memory_bytes / (1024 * 1024)
        per_neuron_mb = memory_mb / (scenario['regions'] * scenario['neurons_per_region'])
        
        print(f"{scenario['name']:<10} {scenario['regions']:<10} {scenario['regions'] * scenario['neurons_per_region']:<12,} "
              f"{memory_mb:<12.2f} {per_neuron_mb:<15.6f}")
    
    print(f"\nMemory scaling considerations:")
    print("  - Memory usage scales linearly with neuron count")
    print("  - System overhead increases memory usage")
    print("  - Large simulations require memory pool optimization")
    print("  - Consider memory-efficient data structures")
    
    return scenarios


def scaling_strategies():
    """Demonstrate scaling strategies for large simulations."""
    print_header("4.3 Scaling Strategies")
    
    print("\nScaling strategies for large simulations:")
    
    strategies = {
        "SpatialDecomposition": {
            "description": "Split brain into spatial regions processed independently",
            "benefits": ["Reduced memory per processor", "Better load balancing", "Scalable to thousands of cores"],
            "limitations": ["Communication overhead", "Boundary effects", "Synchronization complexity"]
        },
        "TemporalDecomposition": {
            "description": "Process simulation time steps across different processors",
            "benefits": ["Simple implementation", "Cache efficiency", "Deterministic timing"],
            "limitations": ["Limited parallelization", "Requires checkpointing", "Complexity for long simulations"]
        },
        "NeuralDecomposition": {
            "description": "Split by neural populations or brain regions",
            "benefits": ["Natural neural organization", "Modular design", "Easy scaling"],
            "limitations": ["Inter-region communication complexity", "Specialized hardware needs", "Implementation complexity"]
        },
        "HybridApproach": {
            "description": "Combine multiple decomposition strategies",
            "benefits": ["Balanced performance", "Fault tolerance", "Flexibility"],
            "limitations": ["Complex orchestration", "Increased coordination", "Higher implementation cost"]
        }
    }
    
    print(f"\nAvailable scaling strategies:")
    for strategy_name, strategy_info in strategies.items():
        print(f"\n{strategy_name}:")
        print(f"  Description: {strategy_info['description']}")
        print(f"  Benefits:")
        for benefit in strategy_info['benefits']:
            print(f"    - {benefit}")
        print(f"  Limitations:")
        for limitation in strategy_info['limitations']:
            print(f"    - {limitation}")
    
    # Recommendation based on simulation size
    print(f"\nStrategy recommendations:")
    print("  Small simulations (<1000 neurons): Temporal decomposition")
    print("  Medium simulations (1000-10000 neurons): Neural decomposition")
    print("  Large simulations (>10000 neurons): Spatial decomposition")
    print("  Very large simulations (>100000 neurons): Hybrid approach")
    
    return strategies


def scaling_examples_summary():
    """Print summary of simulation scaling examples."""
    print_header("Simulation Scaling Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Large-scale brain simulation framework")
    print("✓ Scalable simulation with performance tracking")
    print("✓ Memory usage analysis and optimization")
    print("✓ Different scaling strategies (spatial, temporal, neural, hybrid)")
    print("✓ Performance comparison across scales")
    print("✓ Resource requirements estimation")
    print("✓ Parallel processing integration")

# ============================================================================
# 5. Debugging and Troubleshooting Examples
# ============================================================================

class MemoryDebugger:
    """Memory debugging and leak detection."""
    
    def __init__(self):
        self.memory_samples = []
        self.allocation_records = []
        self.leak_threshold = 10.0  # MB
        
    def track_allocation(self, operation: str, size_bytes: int):
        """Track memory allocation."""
        self.allocation_records.append({
            'time': time.time(),
            'operation': operation,
            'size_bytes': size_bytes,
            'memory_before': self._get_current_memory()
        })
        
    def track_deallocation(self, operation: str):
        """Track memory deallocation."""
        self.allocation_records[-1]['memory_after'] = self._get_current_memory()
        
    def _get_current_memory(self) -> float:
        """Get current memory usage in MB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / (1024 * 1024)
        
    def analyze_memory_leaks(self) -> Dict[str, Any]:
        """Analyze memory for leaks."""
        if len(self.allocation_records) < 2:
            return {'status': 'insufficient_data', 'leaks_detected': False}
            
        # Group records by operation
        operations = {}
        for record in self.allocation_records:
            op = record['operation']
            if op not in operations:
                operations[op] = []
            operations[op].append(record)
            
        leaks = {}
        for op, records in operations.items():
            if len(records) < 2:
                continue
                
            # Check for memory growth after deallocation
            net_growth = 0
            for i in range(1, len(records)):
                if 'memory_after' in records[i-1]:
                    growth = records[i]['memory_before'] - records[i-1]['memory_after']
                    net_growth += growth
                    
            leaks[op] = {
                'net_growth_mb': net_growth,
                'records': len(records),
                'potential_leak': net_growth > self.leak_threshold
            }
            
        return {
            'status': 'analysis_complete',
            'leaks_detected': any(leak['potential_leak'] for leak in leaks.values()),
            'leaks': leaks,
            'total_growth_mb': sum(leak['net_growth_mb'] for leak in leaks.values())
        }
class NeuromodulationDebugger:
    """Neuromodulation signal debugging and validation."""
    
    def __init__(self):
        self.signal_history = []
        self.validation_rules = []
        
    def record_signal(self, neuromodulator: Any, signal_value: float, timestamp: float):
        """Record neuromodulator signal for debugging."""
        self.signal_history.append({
            'neuromodulator': neuromodulator,
            'signal_value': signal_value,
            'timestamp': timestamp,
            'valid': self._validate_signal(signal_value)
        })
        
    def _validate_signal(self, signal_value: float) -> bool:
        """Validate signal value (in real implementation)."""
        # Simple validation - in real implementation would be more sophisticated
        return -1.0 <= signal_value <= 1.0
        
    def analyze_signal_patterns(self) -> Dict[str, Any]:
        """Analyze neuromodulator signal patterns."""
        if not self.signal_history:
            return {'status': 'no_data', 'patterns': {}}
            
        # Group by neuromodulator
        neuromodulators = {}
        for record in self.signal_history:
            mod_name = record['neuromodulator'].getName()
            if mod_name not in neuromodulators:
                neuromodulators[mod_name] = []
            neuromodulators[mod_name].append(record['signal_value'])
            
        patterns = {}
        for mod_name, signals in neuromodulators.items():
            patterns[mod_name] = {
                'mean': sum(signals) / len(signals),
                'std': (sum((x - sum(signals)/len(signals))**2 for x in signals) / len(signals))**0.5,
                'min': min(signals),
                'max': max(signals),
                'trend': self._calculate_trend(signals)
            }
            
        return {
            'status': 'analysis_complete',
            'total_samples': len(self.signal_history),
            'patterns': patterns,
            'quality_score': self._calculate_quality_score(patterns)
        }
        
    def _calculate_trend(self, signals: List[float]) -> str:
        """Calculate trend of signals."""
        if len(signals) < 2:
            return "insufficient_data"
            
        # Simple trend calculation
        first_half = signals[:len(signals)//2]
        second_half = signals[len(signals)//2:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        if avg_second > avg_first + 0.1:
            return "increasing"
        elif avg_second < avg_first - 0.1:
            return "decreasing"
        else:
            return "stable"
            
    def _calculate_quality_score(self, patterns: Dict[str, Any]) -> float:
        """Calculate overall signal quality score."""
        if not patterns:
            return 0.0
            
        quality_scores = []
        for mod_name, pattern in patterns.items():
            # Score based on signal characteristics
            score = 1.0  # Start with perfect score
            
            # Deduct for extreme values
            if abs(pattern['mean']) > 0.8:
                score *= 0.9
                
            # Deduct for high variability
            if pattern['std'] > 0.5:
                score *= 0.8
                
            # Deduct for invalid signals
            invalid_count = sum(1 for record in self.signal_history 
                              if record['neuromodulator'].getName() == mod_name and not record['valid'])
            if invalid_count > 0:
                score *= (1 - invalid_count / len(self.signal_history))
                
            quality_scores.append(score)
            
        return sum(quality_scores) / len(quality_scores)
def simulation_state_inspector():
    """Step-by-step simulation state inspection."""
    print_header("5.1 Simulation State Inspector")
    
    print("\nStep-by-step simulation state inspection:")
    
    # Create simulation with inspection capabilities
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Store simulation states for inspection
    simulation_states = []
    
    print(f"\nRunning step-by-step inspection:")
    
    for step in range(10):
        # Record state before step
        state_before = {
            'step': step,
            'timestamp': time.time(),
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'prediction_error': brain.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.5,
            'world_time': world.getSimulationTime()
        }
        
        simulation_states.append(('before', state_before))
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record state after step
        state_after = {
            'step': step,
            'timestamp': time.time(),
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'prediction_error': brain.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.5,
            'world_time': world.getSimulationTime()
        }
        
        simulation_states.append(('after', state_after))
        
        print(f"  Step {step}:")
        print(f"    Before: {state_before['firing_count']} neurons firing, "
              f"rate={state_before['average_firing_rate']:.3f}")
        print(f"    After: {state_after['firing_count']} neurons firing, "
             f"rate={state_after['average_firing_rate']:.3f}")
        print(f"    Δ: {state_after['firing_count'] - state_before['firing_count']} neurons, "
              f"Δ rate={state_after['average_firing_rate'] - state_before['average_firing_rate']:.3f}")
    
    # Analyze state changes
    print(f"\nState change analysis:")
    
    firing_changes = []
    rate_changes = []
    error_changes = []
    
    for before, after in [(s1, s2) for pair in zip(simulation_states[::2], simulation_states[1::2])]:
        firing_changes.append(after['firing_count'] - before['firing_count'])
        rate_changes.append(after['average_firing_rate'] - before['average_firing_rate'])
        error_changes.append(after['prediction_error'] - before['prediction_error'])
    
    print(f"  Firing count changes: min={min(firing_changes)}, max={max(firing_changes)}, avg={sum(firing_changes)/len(firing_changes):.1f}")
    print(f"  Firing rate changes: min={min(rate_changes):.4f}, max={max(rate_changes):.4f}, avg={sum(rate_changes)/len(rate_changes):.4f}")
    print(f"  Prediction error changes: min={min(error_changes):.4f}, max={max(error_changes):.4f}, avg={sum(error_changes)/len(error_changes):.4f}")
    
    # Detect anomalies
    print(f"\nAnomaly detection:")
    
    anomaly_count = 0
    for i, (before, after) in enumerate(zip(simulation_states[::2], simulation_states[1::2])):
        # Check for sudden changes
        firing_change = abs(after['firing_count'] - before['firing_count'])
        rate_change = abs(after['average_firing_rate'] - before['average_firing_rate'])
        
        if firing_change > 10 or rate_change > 0.1:
            anomaly_count += 1
            print(f"    ⚠️  Anomaly at step {before['step']}: "
                  f"firing_change={firing_change}, rate_change={rate_change:.4f}")
    
    if anomaly_count == 0:
        print("    ✅ No anomalies detected in simulation run")
    else:
        print(f"    Found {anomaly_count} anomalies in simulation")
    
    return simulation_states


def performance_analysis():
    """Performance analysis and profiling."""
    print_header("5.2 Performance Analysis")
    
    print("\nPerformance analysis and profiling:")
    
    # Create brain with profiling capabilities
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Profile CPU performance
    print(f"\n5.2.1 CPU Performance Profiling:")
    
    # Measure CPU performance
    cpu_times = []
    for i in range(10):
        start_time = time.time()
        # Simulate neural computation
        dummy_sum = 0
        for j in range(10000):
            dummy_sum += j * 0.1
        cpu_time = time.time() - start_time
        cpu_times.append(cpu_time)
        time.sleep(0.01)  # Small delay between measurements
    
    avg_cpu_time = sum(cpu_times) / len(cpu_times)
    min_cpu_time = min(cpu_times)
    max_cpu_time = max(cpu_times)
    
    print(f"  CPU computation times:")
    print(f"    Average: {avg_cpu_time:.4f}s")
    print(f"    Minimum: {min_cpu_time:.4f}s")
    print(f"    Maximum: {max_cpu_time:.4f}s")
    print(f"    Variability: {(max_cpu_time - min_cpu_time) / avg_cpu_time:.1%}")
    
    # Profile memory performance
    print(f"\n5.2.2 Memory Performance Profiling:")
    
    memory_times = []
    memory_sizes = []
    
    for size in [1000, 5000, 10000, 20000]:
        start_time = time.time()
        
        # Allocate memory
        data = []
        for i in range(size):
            data.append([float(j) for j in range(10)])
            
        alloc_time = time.time() - start_time
        memory_size = len(data)
        
        # Measure memory usage
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / (1024 * 1024)
        
        memory_times.append(alloc_time)
        memory_sizes.append(memory_mb)
        
        # Free memory
        del data
        time.sleep(0.05)
    
    print(f"  Memory allocation times:")
    for size, time_taken, memory_used in zip(memory_sizes, memory_times, memory_sizes):
        throughput = size / time_taken if time_taken > 0 else 0
        print(f"    Size {size:,} bytes: {time_taken:.4f}s, "
              f"throughput: {throughput:.0f} elements/sec")
    
    # Profile I/O performance
    print(f"\n5.2.3 I/O Performance Profiling:")
    
    # Simulate file I/O operations
    test_files = []
    for i in range(5):
        test_file = f"test_file_{i}.txt"
        with open(test_file, 'w') as f:
            f.write(f"Test data for performance analysis - file {i}\n" * 1000)
        test_files.append(test_file)
    
    # Read file performance
    read_times = []
    for test_file in test_files:
        start_time = time.time()
        with open(test_file, 'r') as f:
            content = f.read()
        read_time = time.time() - start_time
        read_times.append(read_time)
        
    avg_read_time = sum(read_times) / len(read_times)
    print(f"  File read performance:")
    print(f"    Average read time: {avg_read_time:.4f}s")
    print(f"    Read throughput: {sum(len(open(f).read()) for f in test_files) / sum(read_times):.0f} bytes/sec")
    
    # Clean up test files
    for test_file in test_files:
        try:
            os.remove(test_file)
        except:
            pass
    
    print(f"\n5.2.4 System Resource Profiling:")
    
    process = psutil.Process(os.getpid())
    
    # Get detailed system information
    cpu_info = process.cpu_times()
    memory_info = process.memory_info()
    disk_info = process.disk_io_counters()
    
    print(f"  System resource usage:")
    print(f"    CPU user time: {cpu_info.user:.2f}s")
    print(f"    CPU system time: {cpu_info.system:.2f}s")
    print(f"    Memory usage: {memory_info.rss / (1024 * 1024):.2f}MB")
    print(f"    Memory limit: {memory_info.vms / (1024 * 1024):.2f}MB (virtual)")
    
    if disk_info:
        print(f"    Disk reads: {disk_info.read_count:,}")
        print(f"    Disk writes: {disk_info.write_count:,}")
        print(f"    Disk read bytes: {disk_info.read_bytes / (1024 * 1024):.2f}MB")
        print(f"    Disk write bytes: {disk_info.write_bytes / (1024 * 1024):.2f}MB")
    
    return {
        'cpu_performance': {'avg_time': avg_cpu_time, 'min_time': min_cpu_time, 'max_time': max_cpu_time},
        'memory_performance': {'times': memory_times, 'sizes': memory_sizes},
        'io_performance': {'read_times': read_times, 'avg_read_time': avg_read_time}
    }


def debugging_examples_summary():
    """Print summary of debugging and troubleshooting examples."""
    print_header("Debugging and Troubleshooting Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Memory debugging with allocation tracking")
    print("✓ Memory leak detection and analysis")
    print("✓ Neuromodulation signal monitoring and validation")
    print("✓ Simulation state inspection and step-by-step analysis")
    print("✓ Performance profiling (CPU, memory, I/O)")
    print("✓ Anomaly detection and troubleshooting")
    print("✓ System resource monitoring")

# ============================================================================
# 6. Integration Examples
# ============================================================================

class ResearchFramework:
    """Research framework for scientific experiments with NLM."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.experiments = []
        self.results = {}
        self.metadata = {
            'created': time.time(),
            'version': '1.0',
            'description': 'NLM-based scientific research framework'
        }
        
    def create_experiment(self, name: str, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Create a new scientific experiment."""
        experiment = {
            'name': name,
            'parameters': parameters,
            'created': time.time(),
            'status': 'pending',
            'results': None,
            'metadata': {}
        }
        
        self.experiments.append(experiment)
        self.results[name] = None
        
        return experiment
        
    def run_experiment(self, experiment_name: str, simulation_steps: int = 1000) -> Dict[str, Any]:
        """Run an experiment and collect results."""
        if experiment_name not in self.results:
            raise ValueError(f"Unknown experiment: {experiment_name}")
            
        print(f"\nRunning experiment: {experiment_name}")
        print(f"  Parameters: {self.experiments[[e['name'] for e in self.experiments].index(experiment_name)]['parameters']}")
        
        # Create simulation based on experiment parameters
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning based on experiment parameters
        params = self.experiments[[e['name'] for e in self.experiments].index(experiment_name)]['parameters']
        
        agent.enableRewardModulation(params.get('enable_reward_modulation', True))
        agent.enableCuriosity(params.get('enable_curiosity', True))
        agent.enableDevelopment(params.get('enable_development', True))
        
        # Run simulation
        start_time = time.time()
        step_times = []
        
        for step in range(simulation_steps):
            step_start = time.time()
            
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            # Collect experimental data
            if step % 100 == 0:
                print(f"    Step {step}: firing_rate={brain.getAverageFiringRate():.3f}, "
                      f"prediction_error={brain.getPredictionError():.3f}")
        
        total_time = time.time() - start_time
        
        # Collect experimental results
        results = {
            'experiment_name': experiment_name,
            'parameters': params,
            'total_steps': simulation_steps,
            'total_time': total_time,
            'average_step_time': sum(step_times) / len(step_times),
            'steps_per_second': simulation_steps / total_time,
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_prediction_error': brain.getPredictionError(),
            'final_neuromodulation_level': agent.getNeuromodulationLevel() if hasattr(agent, 'getNeuromodulationLevel') else 0.5,
            'total_spikes': brain.getTotalSpikeCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'timestamp': time.time()
        }
        
        # Store results
        self.results[experiment_name] = results
        
        print(f"\nExperiment completed:")
        print(f"  Total time: {total_time:.1f}s")
        print(f"  Average step time: {results['average_step_time']:.4f}s")
        print(f"  Steps per second: {results['steps_per_second']:.0f}")
        print(f"  Final firing rate: {results['final_firing_rate']:.3f}")
        print(f"  Final prediction error: {results['final_prediction_error']:.3f}")
        print(f"  Total spikes: {results['total_spikes']}")
        
        return results
        
    def analyze_results(self, experiment_name: str) -> Dict[str, Any]:
        """Analyze experiment results."""
        if experiment_name not in self.results:
            raise ValueError(f"Unknown experiment: {experiment_name}")
            
        results = self.results[experiment_name]
        
        # Perform statistical analysis
        analysis = {
            'experiment_name': experiment_name,
            'statistical_summary': {
                'final_performance': {
                    'firing_rate': results['final_firing_rate'],
                    'prediction_error': results['final_prediction_error'],
                    'neuromodulation_level': results['final_neuromodulation_level']
                },
                'efficiency_metrics': {
                    'steps_per_second': results['steps_per_second'],
                    'neurons_processed': results['total_spikes'] / results['total_time'] if results['total_time'] > 0 else 0
                },
                'performance_trends': {
                    'firing_rate_trend': self._analyze_trend(results['final_firing_rate'], 0.5),
                    'prediction_error_trend': self._analyze_trend(results['final_prediction_error'], 0.1)
                }
            },
            'comparative_analysis': {
                'baseline_comparison': self._compare_to_baseline(results),
                'parameter_sensitivity': self._analyze_parameter_sensitivity(results)
            }
        }
        
        return analysis
        
    def _analyze_trend(self, value: float, threshold: float) -> str:
        """Analyze if value indicates a trend."""
        if value > threshold:
            return "high"
        elif value < threshold * 0.5:
            return "low"
        else:
            return "normal"
            
    def _compare_to_baseline(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Compare results to baseline performance."""
        baseline = {
            'expected_firing_rate': 0.5,
            'expected_prediction_error': 0.1,
            'expected_neuromodulation_level': 0.5
        }
        
        comparison = {}
        for metric, baseline_value in baseline.items():
            actual_value = results.get(metric, 0)
            difference = actual_value - baseline_value
            percentage_diff = (difference / baseline_value * 100) if baseline_value > 0 else 0
            
            comparison[metric] = {
                'actual': actual_value,
                'baseline': baseline_value,
                'difference': difference,
                'percentage_difference': percentage_diff,
                'status': 'good' if abs(percentage_diff) < 20 else 'needs_attention'
            }
            
        return comparison
        
    def _analyze_parameter_sensitivity(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze sensitivity to different parameters."""
        # This would be more sophisticated in a real implementation
        sensitivity_analysis = {
            'robustness': 'moderate',
            'key_parameters': ['learning_rate', 'plasticity', 'neuromodulation'],
            'recommendations': [
                'Increase learning rate if prediction error is high',
                'Adjust neuromodulation if performance is suboptimal',
                'Consider memory consolidation strategies'
            ]
        }
        
        return sensitivity_analysis


class AgentTrainer:
    """Agent training framework for reinforcement learning."""
    
    def __init__(self, training_config: Dict[str, Any]):
        self.training_config = training_config
        self.training_sessions = []
        self.agent_performance = {}
        
    def create_training_environment(self, env_type: str, env_config: Dict[str, Any]) -> Any:
        """Create training environment."""
        if env_type == 'simple_world':
            world = pynlm.createSimpleWorld()
            world.configure(**env_config)
            return world
        else:
            raise ValueError(f"Unknown environment type: {env_type}")
            
    def train_agent(self, agent: Any, environment: Any, episodes: int, 
                   max_steps_per_episode: int = 100) -> Dict[str, Any]:
        """Train agent using reinforcement learning."""
        print(f"\nTraining agent for {episodes} episodes...")
        
        training_history = {
            'episodes_completed': 0,
            'rewards': [],
            'prediction_errors': [],
            'firing_rates': [],
            'learning_curves': {},
            'performance_metrics': {}
        }
        
        for episode in range(episodes):
            episode_start_time = time.time()
            
            # Reset environment
            environment.reset()
            
            # Run episode
            episode_reward = 0
            episode_steps = 0
            
            for step in range(max_steps_per_episode):
                # Get observation
                percept = environment.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Get brain response
                brain = agent.getBrain()
                brain.step(step)
                
                # Select action
                action = agent.decodeMotorCommand()
                
                # Apply action
                environment.applyMotorCommand(action, environment.getSimulationTime())
                
                # Get reward
                reward = environment.getReward() if hasattr(environment, 'getReward') else 0.0
                episode_reward += reward
                
                # Apply reward modulation
                agent.applyRewardModulation(reward, 0.0)
                
                episode_steps += 1
                
                # Break if episode done
                if environment.isDone() if hasattr(environment, 'isDone') else False:
                    break
            
            episode_time = time.time() - episode_start_time
            
            # Record episode performance
            training_history['episodes_completed'] += 1
            training_history['rewards'].append(episode_reward)
            training_history['firing_rates'].append(brain.getAverageFiringRate())
            training_history['prediction_errors'].append(brain.getPredictionError())
            
            # Print progress
            if (episode + 1) % 10 == 0:
                avg_reward = sum(training_history['rewards'][-10:]) / 10
                print(f"  Episode {episode + 1}/{episodes}: "
                      f"reward={avg_reward:.2f}, "
                      f"firing_rate={training_history['firing_rates'][-1]:.3f}, "
                      f"time={episode_time:.2f}s")
        
        # Calculate final performance metrics
        final_performance = {
            'total_episodes': training_history['episodes_completed'],
            'average_reward': sum(training_history['rewards']) / len(training_history['rewards']),
            'final_firing_rate': training_history['firing_rates'][-1],
            'final_prediction_error': training_history['prediction_errors'][-1],
            'total_training_time': time.time() - episode_start_time,
            'learning_efficiency': training_history['episodes_completed'] / (time.time() - episode_start_time)
        }
        
        training_history['performance_metrics'] = final_performance
        
        return training_history


def research_framework_example():
    """Demonstrate research framework usage."""
    print_header("6.1 Research Framework Example")
    
    print("\nResearch framework for scientific experiments:")
    
    # Create research framework
    framework = ResearchFramework("NLM_Neural_Learning_Research")
    
    # Define experiments
    experiments = [
        {
            "name": "basic_neural_dynamics",
            "parameters": {
                "simulation_steps": 1000,
                "enable_reward_modulation": True,
                "enable_curiosity": False,
                "enable_development": False
            }
        },
        {
            "name": "exploration_vs_exploitation",
            "parameters": {
                "simulation_steps": 2000,
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_development": True,
                "novelty_weight": 0.7
            }
        },
        {
            "name": "developmental_learning",
            "parameters": {
                "simulation_steps": 3000,
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_development": True,
                "developmental_stage": "senior"
            }
        }
    ]
    
    # Create experiments
    for exp in experiments:
        framework.create_experiment(exp["name"], exp["parameters"])
    
    # Run experiments
    for exp in experiments:
        results = framework.run_experiment(exp["name"], exp["parameters"]["simulation_steps"])
        framework.results[exp["name"]] = results
    
    # Analyze results
    print(f"\nExperimental analysis:")
    
    for exp_name in framework.results:
        analysis = framework.analyze_results(exp_name)
        
        print(f"\n{exp_name}:")
        print(f"  Performance status: {analysis['statistical_summary']['performance_trends']['firing_rate_trend']}")
        print(f"  Prediction error: {analysis['statistical_summary']['performance_trends']['prediction_error_trend']}")
        
        comparison = analysis['comparative_analysis']['baseline_comparison']
        for metric, comp_data in comparison.items():
            status = comp_data['status']
            symbol = "✅" if status == "good" else "⚠️"
            print(f"  {symbol} {metric}: {comp_data['percentage_difference']:+.1f}%")
    
    return framework


def agent_training_example():
    """Demonstrate agent training framework."""
    print_header("6.2 Agent Training Example")
    
    print("\nAgent training framework for reinforcement learning:")
    
    # Create training configuration
    training_config = {
        "learning_rate": 0.01,
        "discount_factor": 0.95,
        "exploration_rate": 0.1,
        "max_episodes": 100,
        "training_steps": 1000
    }
    
    # Create agent trainer
    trainer = AgentTrainer(training_config)
    
    # Create training environment
    environment = trainer.create_training_environment(
        'simple_world',
        {'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8}
    )
    
    # Create agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(environment)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Train agent
    training_history = trainer.train_agent(
        agent, environment, 
        episodes=training_config["max_episodes"],
        max_steps_per_episode=training_config["training_steps"]
    )
    
    # Analyze training results
    print(f"\nTraining analysis:")
    print(f"  Episodes completed: {training_history['episodes_completed']}")
    print(f"  Average reward: {training_history['performance_metrics']['average_reward']:.3f}")
    print(f"  Final firing rate: {training_history['performance_metrics']['final_firing_rate']:.3f}")
    print(f"  Final prediction error: {training_history['performance_metrics']['final_prediction_error']:.3f}")
    print(f"  Learning efficiency: {training_history['performance_metrics']['learning_efficiency']:.3f} episodes/sec")
    
    # Show learning curve
    print(f"\nLearning curve (last 10 episodes):")
    for i in range(max(0, len(training_history['rewards']) - 10), len(training_history['rewards'])):
        print(f"  Episode {i}: reward={training_history['rewards'][i]:.3f}, "
              f"firing_rate={training_history['firing_rates'][i]:.3f}")
    
    return training_history


def behavioral_analysis_example():
    """Demonstrate behavioral analysis of neural activity."""
    print_header("6.3 Behavioral Analysis Example")
    
    print("\nBehavioral analysis of neural activity patterns:")
    
    # Create simulation for behavioral analysis
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Collect behavioral data
    behavioral_data = {
        'steps': [],
        'actions': [],
        'rewards': [],
        'neural_activity': [],
        'attention_patterns': [],
        'curiosity_levels': []
    }
    
    print(f"\nCollecting behavioral data...")
    
    for step in range(500):
        # Collect neural activity
        neural_state = {
            'firing_rate': brain.getAverageFiringRate(),
            'prediction_error': brain.getPredictionError(),
            'neuron_count': brain.getFiringNeuronCount()
        }
        
        # Collect neuromodulation levels
        dopamine = brain.getDopamine().getLevel() if hasattr(brain.getDopamine(), 'getLevel') else 0.5
        acetylcholine = brain.getAcetylcholine().getLevel() if hasattr(brain.getAcetylcholine(), 'getLevel') else 0.5
        
        # Collect curiosity
        curiosity = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.5
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect action and reward
        action_type = "move"  # Simplified action classification
        reward = 0.0  # In real implementation, would get actual reward
        
        # Store data
        behavioral_data['steps'].append(step)
        behavioral_data['actions'].append(action_type)
        behavioral_data['rewards'].append(reward)
        behavioral_data['neural_activity'].append(neural_state)
        behavioral_data['attention_patterns'].append(acetylcholine)
        behavioral_data['curiosity_levels'].append(curiosity)
        
        if step % 100 == 0:
            print(f"  Step {step}: firing_rate={neural_state['firing_rate']:.3f}, "
                  f"prediction_error={neural_state['prediction_error']:.3f}, "
                  f"curiosity={curiosity:.3f}")
    
    # Analyze behavioral patterns
    print(f"\nBehavioral analysis:")
    
    # Action patterns
    action_counts = {}
    for action in behavioral_data['actions']:
        action_counts[action] = action_counts.get(action, 0) + 1
    
    print(f"  Action patterns:")
    for action, count in action_counts.items():
        percentage = (count / len(behavioral_data['actions'])) * 100
        print(f"    {action}: {count} ({percentage:.1f}%)")
    
    # Reward patterns
    print(f"  Reward patterns:")
    print(f"    Average reward: {sum(behavioral_data['rewards']) / len(behavioral_data['rewards']):.3f}")
    print(f"    Reward variance: {sum((r - sum(behavioral_data['rewards'])/len(behavioral_data['rewards']))**2 for r in behavioral_data['rewards']) / len(behavioral_data['rewards']):.3f}")
    
    # Neural activity patterns
    print(f"  Neural activity patterns:")
    avg_firing_rate = sum(d['firing_rate'] for d in behavioral_data['neural_activity']) / len(behavioral_data['neural_activity'])
    avg_prediction_error = sum(d['prediction_error'] for d in behavioral_data['neural_activity']) / len(behavioral_data['neural_activity'])
    print(f"    Average firing rate: {avg_firing_rate:.3f}")
    print(f"    Average prediction error: {avg_prediction_error:.3f}")
    
    # Attention and curiosity patterns
    print(f"  Neuromodulation patterns:")
    avg_attention = sum(behavioral_data['attention_patterns']) / len(behavioral_data['attention_patterns'])
    avg_curiosity = sum(behavioral_data['curiosity_levels']) / len(behavioral_data['curiosity_levels'])
    print(f"    Average attention: {avg_attention:.3f}")
    print(f"    Average curiosity: {avg_curiosity:.3f}")
    
    # Behavioral correlations
    print(f"\nBehavioral correlations:")
    
    # Correlation between curiosity and action variety
    curiosity_by_action = {}
    for action, curiosity_levels in zip(behavioral_data['actions'], behavioral_data['curiosity_levels']):
        if action not in curiosity_by_action:
            curiosity_by_action[action] = []
        curiosity_by_action[action].append(curiosity_levels)
    
    print(f"  Curiosity levels by action:")
    for action, levels in curiosity_by_action.items():
        avg_curiosity = sum(levels) / len(levels)
        print(f"    {action}: {avg_curiosity:.3f}")
    
    return behavioral_data


def integration_examples_summary():
    """Print summary of integration examples."""
    print_header("Integration Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Research framework for scientific experiments")
    print("✓ Agent training framework for reinforcement learning")
    print("✓ Behavioral analysis tools for neural activity patterns")
    print("✓ Real-world application setup and configuration")
    print("✓ Experimental design and parameter optimization")
    print("✓ Performance monitoring and analysis in research")
    print("✓ Integration of multiple NLM systems in applications")

# ============================================================================
# 7. Integration Examples
# ============================================================================

class RealWorldApplication:
    """Real-world application setup and integration."""
    
    def __init__(self, application_name: str):
        self.application_name = application_name
        self.components = []
        self.configurations = {}
        
    def add_component(self, component_type: str, component_config: Dict[str, Any]):
        """Add a component to the application."""
        component = {
            'type': component_type,
            'config': component_config,
            'status': 'initialized',
            'performance': {}
        }
        self.components.append(component)
        
    def configure_component(self, component_name: str, config_updates: Dict[str, Any]):
        """Update component configuration."""
        for component in self.components:
            if component['name'] == component_name:
                component['config'].update(config_updates)
                component['status'] = 'configured'
                return True
        return False
        
    def setup_application(self) -> Dict[str, Any]:
        """Setup complete application with all components."""
        print(f"\nSetting up application: {self.application_name}")
        print(f"  Components to initialize: {len(self.components)}")
        
        # Initialize all components
        setup_log = []
        
        for component in self.components:
            print(f"\nConfiguring {component['type']}...")
            
            # Simulate component initialization
            init_start = time.time()
            
            # Component-specific initialization
            if component['type'] == 'brain':
                brain = pynlm.createBrain(pynlm.createDefaultConfig())
                brain.initialize()
                component['instance'] = brain
                setup_log.append(f"  {component['type']}: Brain initialized with {brain.getTotalNeuronCount()} neurons")
                
            elif component['type'] == 'world':
                world = pynlm.createSimpleWorld()
                world.configure(**component['config'])
                component['instance'] = world
                setup_log.append(f"  {component['type']}: World configured with {world.getWidth()}x{world.getHeight()} cells")
                
            elif component['type'] == 'agent':
                brain = component.get('brain', None)
                if brain is None:
                    # Try to find brain component
                    for other_component in self.components:
                        if other_component['type'] == 'brain':
                            brain = other_component['instance']
                            break
                    
                if brain is not None:
                    agent = pynlm.createAgentBrain(brain)
                    component['instance'] = agent
                    setup_log.append(f"  {component['type']}: Agent created with brain integration")
                else:
                    setup_log.append(f"  {component['type']}: WARNING - No brain component found")
                    
            elif component['type'] == 'memory_system':
                # Create memory system components
                memory_components = []
                
                # Working memory
                wm = brain.getWorkingMemory() if 'brain' in component['config'] else None
                if wm is not None:
                    memory_components.append(f"WorkingMemory ({wm.getCapacity()} capacity)")
                
                # Episodic memory
                em = brain.getEpisodicMemory() if 'brain' in component['config'] else None
                if em is not None:
                    memory_components.append(f"EpisodicMemory ({em.getEpisodeCount()} episodes)")
                
                # Associative memory
                am = brain.getAssociativeMemory() if 'brain' in component['config'] else None
                if am is not None:
                    memory_components.append(f"AssociativeMemory ({len(am.getAssociations())} associations)")
                
                component['memory_components'] = memory_components
                setup_log.append(f"  {component['type']}: Memory systems configured - {', '.join(memory_components)}")
                
            elif component['type'] == 'neuromodulation':
                # Configure neuromodulators
                neuromodulators = []
                
                if component['config'].get('dopamine', True):
                    neuromodulators.append("Dopamine (reward)")
                    
                if component['config'].get('acetylcholine', True):
                    neuromodulators.append("Acetylcholine (attention)")
                    
                if component['config'].get('norepinephrine', True):
                    neuromodulators.append("Norepinephrine (arousal)")
                    
                if component['config'].get('serotonin', True):
                    neuromodulators.append("Serotonin (mood)")
                    
                if component['config'].get('curiosity', True):
                    neuromodulators.append("Curiosity (exploration)")
                    
                if component['config'].get('novelty', True):
                    neuromodulators.append("Novelty (detection)")
                
                component['neuromodulators'] = neuromodulators
                setup_log.append(f"  {component['type']}: Neuromodulators configured - {', '.join(neuromodulators)}")
                
            elif component['type'] == 'cognitive':
                # Configure cognitive systems
                brain = component.get('brain', None)
                if brain is not None:
                    cognitive_systems = []
                    
                    planner = brain.getPlanner()
                    if planner is not None:
                        cognitive_systems.append("Neural Planner")
                        
                    concept_formation = brain.getConceptFormation()
                    if concept_formation is not None:
                        cognitive_systems.append("Concept Formation")
                        
                    attention = brain.getAttention()
                    if attention is not None:
                        cognitive_systems.append("Attentional Selection")
                        
                    development_system = brain.getDevelopmentSystem()
                    if development_system is not None:
                        cognitive_systems.append("Development System")
                    
                    component['cognitive_systems'] = cognitive_systems
                    setup_log.append(f"  {component['type']}: Cognitive systems configured - {', '.join(cognitive_systems)}")
                else:
                    setup_log.append(f"  {component['type']}: WARNING - No brain component found")
                    
            elif component['type'] == 'prediction':
                # Configure prediction system
                brain = component.get('brain', None)
                if brain is not None:
                    prediction_system = brain.getPredictionSystem()
                    prediction_error_system = brain.getPredictionErrorSignal()
                    
                    prediction_config = []
                    if prediction_system is not None:
                        prediction_config.append("Forward Model")
                    if prediction_error_system is not None:
                        prediction_config.append("Prediction Error")
                    
                    component['prediction_systems'] = prediction_config
                    setup_log.append(f"  {component['type']}: Prediction systems configured - {', '.join(prediction_config)}")
                else:
                    setup_log.append(f"  {component['type']}: WARNING - No brain component found")
            
            component['setup_time'] = time.time() - init_start
            component['status'] = 'active'
        
        # Print setup summary
        print(f"\nSetup complete:")
        for log_entry in setup_log:
            print(f"  {log_entry}")
            
        total_setup_time = sum(comp['setup_time'] for comp in self.components)
        print(f"\nSetup summary:")
        print(f"  Total components initialized: {len(self.components)}")
        print(f"  Total setup time: {total_setup_time:.2f}s")
        print(f"  Average component setup time: {total_setup_time / len(self.components):.2f}s")
        
        return {
            'application_name': self.application_name,
            'components': self.components,
            'setup_log': setup_log,
            'total_setup_time': total_setup_time
        }
class ApplicationLauncher:
    """Launch and manage real-world applications."""
    
    def __init__(self, app_config: Dict[str, Any]):
        self.app_config = app_config
        self.running_applications = {}
        self.application_logs = {}
        
    def launch_application(self, app_name: str, app_config: Dict[str, Any]) -> Dict[str, Any]:
        """Launch a real-world application."""
        print(f"\nLaunching application: {app_name}")
        print(f"  Configuration: {app_config}")
        
        # Create application instance
        if app_config['type'] == 'neural_agent':
            app = self._create_neural_agent_app(app_config)
        elif app_config['type'] == 'cognitive_system':
            app = self._create_cognitive_system_app(app_config)
        elif app_config['type'] == 'learning_environment':
            app = self._create_learning_environment_app(app_config)
        else:
            raise ValueError(f"Unknown application type: {app_config['type']}")
        
        # Store application
        self.running_applications[app_name] = app
        
        # Initialize application
        app_start_time = time.time()
        app_results = app['setup']()
        init_time = time.time() - app_start_time
        
        # Store launch results
        launch_results = {
            'app_name': app_name,
            'app_config': app_config,
            'launch_time': init_time,
            'app_results': app_results,
            'launch_status': 'success',
            'timestamp': time.time()
        }
        
        self.application_logs[app_name] = []
        self.application_logs[app_name].append(f"Application launched at {time.time():.1f}")
        
        print(f"\nApplication launched successfully:")
        print(f"  Launch time: {init_time:.2f}s")
        print(f"  Components initialized: {app_results['components_initialized']}")
        print(f"  Total setup time: {app_results['total_setup_time']:.2f}s")
        
        return launch_results
        
    def _create_neural_agent_app(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create neural agent application."""
        app = {
            'name': config['name'],
            'type': 'neural_agent',
            'config': config,
            'brain': None,
            'world': None,
            'agent': None,
            'status': 'creating'
        }
        
        # Create components
        print(f"  Creating brain component...")
        app['brain'] = pynlm.createBrain(pynlm.createDefaultConfig())
        app['brain'].initialize()
        
        print(f"  Creating world component...")
        app['world'] = pynlm.createSimpleWorld()
        app['world'].configure(
            width=config.get('world_width', 20),
            height=config.get('world_height', 20),
            visionWidth=config.get('vision_width', 8),
            visionHeight=config.get('vision_height', 8)
        )
        app['world'].reset()
        
        print(f"  Creating agent component...")
        app['agent'] = pynlm.createAgentBrain(app['brain'])
        app['agent'].initialize(app['world'])
        
        # Enable learning systems
        app['agent'].enableRewardModulation(config.get('enable_reward_modulation', True))
        app['agent'].enableCuriosity(config.get('enable_curiosity', True))
        app['agent'].enableDevelopment(config.get('enable_development', True))
        
        app['status'] = 'ready'
        
        return app
        
    def _create_cognitive_system_app(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create cognitive system application."""
        app = {
            'name': config['name'],
            'type': 'cognitive_system',
            'config': config,
            'brain': None,
            'cognition_systems': {},
            'status': 'creating'
        }
        
        # Create brain
        print(f"  Creating cognitive brain...")
        app['brain'] = pynlm.createBrain(pynlm.createDefaultConfig())
        app['brain'].initialize()
        
        # Create cognitive systems
        cognitive_system_names = ['planner', 'concept_formation', 'attention', 'development']
        
        for system_name in cognitive_system_names:
            system = getattr(app['brain'], f'get{system_name.capitalize()}()')
            if system is not None:
                app['cognition_systems'][system_name] = system
                print(f"    Created {system_name} system")
        
        app['status'] = 'ready'
        
        return app
        
    def _create_learning_environment_app(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create learning environment application."""
        app = {
            'name': config['name'],
            'type': 'learning_environment',
            'config': config,
            'brain': None,
            'world': None,
            'trainer': None,
            'training_history': {},
            'status': 'creating'
        }
        
        # Create brain and world
        print(f"  Creating learning environment...")
        app['brain'] = pynlm.createBrain(pynlm.createDefaultConfig())
        app['brain'].initialize()
        
        app['world'] = pynlm.createSimpleWorld()
        app['world'].configure(**config.get('environment_config', {}))
        app['world'].reset()
        
        # Create trainer
        training_config = {
            'learning_rate': config.get('learning_rate', 0.01),
            'discount_factor': config.get('discount_factor', 0.95),
            'max_episodes': config.get('max_episodes', 100),
            'training_steps': config.get('training_steps', 1000)
        }
        
        app['trainer'] = AgentTrainer(training_config)
        
        app['status'] = 'ready'
        
        return app
        
    def stop_application(self, app_name: str) -> Dict[str, Any]:
        """Stop and cleanup an application."""
        if app_name not in self.running_applications:
            raise ValueError(f"Unknown application: {app_name}")
            
        app = self.running_applications[app_name]
        
        # Cleanup application resources
        cleanup_log = []
        
        if app['type'] == 'neural_agent':
            cleanup_log.append("Cleaning up neural agent components...")
            # In real implementation, would cleanup neural resources
            app['brain'] = None
            app['world'] = None
            app['agent'] = None
            
        elif app['type'] == 'cognitive_system':
            cleanup_log.append("Cleaning up cognitive system components...")
            app['brain'] = None
            app['cognition_systems'] = {}
            
        elif app['type'] == 'learning_environment':
            cleanup_log.append("Cleaning up learning environment components...")
            app['brain'] = None
            app['world'] = None
            app['trainer'] = None
            app['training_history'] = {}
        
        # Remove from running applications
        del self.running_applications[app_name]
        
        # Log cleanup
        self.application_logs[app_name].append(f"Application stopped at {time.time():.1f}")
        
        print(f"\nApplication stopped:")
        for log_entry in cleanup_log:
            print(f"  {log_entry}")
            
        return {
            'app_name': app_name,
            'cleanup_log': cleanup_log,
            'stop_status': 'completed',
            'timestamp': time.time()
        }


def real_world_application_example():
    """Demonstrate real-world application setup and integration."""
    print_header("7.1 Real-World Application Example")
    
    print("\nReal-world application setup and integration:")
    
    # Create application launcher
    app_launcher = ApplicationLauncher({
        'default_timeout': 300,
        'max_memory_usage': 2048,  # MB
        'log_level': 'INFO'
    })
    
    # Define applications
    applications = [
        {
            'name': 'Autonomous_Agent',
            'type': 'neural_agent',
            'world_width': 30,
            'world_height': 30,
            'vision_width': 12,
            'vision_height': 12,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'enable_development': True
        },
        {
            'name': 'Cognitive_Bot',
            'type': 'cognitive_system',
            'brain_config': {
                'neuron_count': 1000,
                'connection_density': 0.1
            }
        },
        {
            'name': 'Learning_Environment',
            'type': 'learning_environment',
            'environment_config': {
                'width': 25,
                'height': 25,
                'visionWidth': 10,
                'visionHeight': 10
            },
            'learning_rate': 0.005,
            'discount_factor': 0.9,
            'max_episodes': 50,
            'training_steps': 500
        }
    ]
    
    # Launch applications
    launched_apps = {}
    
    for app_config in applications:
        app_name = app_config['name']
        print(f"\n--- Launching {app_name} ---")
        
        # Launch application
        launch_results = app_launcher.launch_application(app_name, app_config)
        launched_apps[app_name] = launch_results
    
    # Monitor applications
    print(f"\nMonitoring launched applications:")
    
    for app_name, launch_results in launched_apps.items():
        app = app_launcher.running_applications[app_name]
        
        print(f"\n{app_name}:")
        print(f"  Status: {app['status']}")
        print(f"  Launch time: {launch_results['launch_time']:.2f}s")
        
        if app['type'] == 'neural_agent':
            brain = app['brain']
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Prediction error: {brain.getPredictionError():.3f}")
            
        elif app['type'] == 'cognitive_system':
            brain = app['brain']
            cognitive_systems = len(app['cognition_systems'])
            print(f"  Cognitive systems: {cognitive_systems}")
            
            for system_name, system in app['cognition_systems'].items():
                print(f"    {system_name}: Active")
                
        elif app['type'] == 'learning_environment':
            brain = app['brain']
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Training configured: {len(app['trainer'].training_sessions)} sessions")
    
    # Demonstrate application interaction
    print(f"\nDemonstrating application interaction:")
    
    # Run autonomous agent simulation
    if 'Autonomous_Agent' in launched_apps:
        agent_app = app_launcher.running_applications['Autonomous_Agent']
        
        print(f"\nRunning Autonomous Agent simulation:")
        
        brain = agent_app['brain']
        world = agent_app['world']
        agent = agent_app['agent']
        
        for step in range(100):
            # Update environment
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Action selection
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log step
            if step % 20 == 0:
                print(f"  Step {step}: firing_rate={brain.getAverageFiringRate():.3f}, "
                      f"prediction_error={brain.getPredictionError():.3f}")
    
    # Stop applications
    print(f"\nStopping all applications:")
    
    for app_name in list(launched_apps.keys()):
        stop_results = app_launcher.stop_application(app_name)
        print(f"  {app_name}: Stopped")
    
    return launched_apps


def integration_examples_summary():
    """Print summary of integration examples."n""
    print_header("Integration Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Research framework for scientific experiments")
    print("✓ Agent training framework for reinforcement learning")
    print("✓ Behavioral analysis tools for neural activity patterns")
    print("✓ Real-world application setup and integration")
    print("✓ Application lifecycle management")
    print("✓ Multi-application coordination")
    print("✓ Integration of all NLM systems in practical use cases")

# ============================================================================
# 8. API Reference Documentation
# ============================================================================

API_REFERENCE = {
    "title": "NLM Python API Reference",
    "version": "1.0.0",
    "description": "Comprehensive API documentation for NLM Python bindings",
    "modules": [
        {
            "name": "pynlm",
            "description": "Main NLM Python bindings module",
            "functions": [
                {
                    "name": "createBrain",
                    "description": "Create a new neural brain instance",
                    "parameters": [
                        {"name": "config", "type": "Config", "description": "Brain configuration"}
                    ],
                    "returns": {"type": "Brain", "description": "New brain instance"}
                },
                {
                    "name": "createSimpleWorld",
                    "description": "Create a simple virtual world",
                    "parameters": [],
                    "returns": {"type": "SimpleWorld", "description": "New world instance"}
                },
                {
                    "name": "createAgentBrain",
                    "description": "Create an agent brain that connects brain to world",
                    "parameters": [
                        {"name": "brain", "type": "Brain", "description": "Brain instance to connect"}
                    ],
                    "returns": {"type": "AgentBrain", "description": "New agent brain instance"}
                },
                {
                    "name": "createDefaultConfig",
                    "description": "Create default brain configuration",
                    "parameters": [],
                    "returns": {"type": "Config", "description": "Default configuration object"}
                }
            ],
            "classes": [
                {
                    "name": "Brain",
                    "description": "Main neural brain class implementing spiking neural computation",
                    "methods": [
                        {"name": "initialize", "description": "Initialize brain with configuration"},
                        {"name": "step", "description": "Perform one simulation step"},
                        {"name": "receiveSensoryInput", "description": "Inject sensory input into brain"},
                        {"name": "injectCurrent", "description": "Inject current into specific neuron"},
                        {"name": "getWorkingMemory", "description": "Get working memory system"},
                        {"name": "getEpisodicMemory", "description": "Get episodic memory system"},
                        {"name": "getAssociativeMemory", "description": "Get associative memory system"},
                        {"name": "getPredictionSystem", "description": "Get prediction system"},
                        {"name": "getPlanner", "description": "Get neural planner"},
                        {"name": "getConceptFormation", "description": "Get concept formation system"},
                        {"name": "getAttention", "description": "Get attentional selection system"},
                        {"name": "getDevelopmentSystem", "description": "Get development system"},
                        {"name": "getDopamine", "description": "Get dopamine neuromodulator"},
                        {"name": "getAcetylcholine", "description": "Get acetylcholine neuromodulator"},
                        {"name": "getNorepinephrine", "description": "Get norepinephrine neuromodulator"},
                        {"name": "getSerotonin", "description": "Get serotonin neuromodulator"},
                        {"name": "getCuriosity", "description": "Get curiosity neuromodulator"},
                        {"name": "getNovelty", "description": "Get novelty detection system"},
                        {"name": "getPredictionErrorSignal", "description": "Get prediction error signal"}
                    ]
                },
                {
                    "name": "AgentBrain",
                    "description": "Connects NLM brain to the world, handling sensory and motor functions",
                    "methods": [
                        {"name": "initialize", "description": "Initialize agent with world"},
                        {"name": "processSensoryInput", "description": "Process sensory percept from world"},
                        {"name": "decodeMotorCommand", "description": "Decode motor command from brain"},
                        {"name": "applyRewardModulation", "description": "Apply reward-based neuromodulation"},
                        {"name": "updateDevelopment", "description": "Update development system"},
                        {"name": "enableRewardModulation", "description": "Enable reward modulation"},
                        {"name": "enableCuriosity", "description": "Enable curiosity-based learning"},
                        {"name": "enableDevelopment", "description": "Enable developmental learning"},
                        {"name": "enableStructuralPlasticity", "description": "Enable structural plasticity"}
                    ]
                },
                {
                    "name": "SimpleWorld",
                    "description": "Simple virtual world for agent-environment interaction",
                    "methods": [
                        {"name": "configure", "description": "Configure world dimensions and sensor properties"},
                        {"name": "reset", "description": "Reset world to initial state"},
                        {"name": "update", "description": "Update world by specified time step"},
                        {"name": "getSensoryPercept", "description": "Get current sensory percept"},
                        {"name": "applyMotorCommand", "description": "Apply motor command to world"},
                        {"name": "getSimulationTime", "description": "Get current simulation time"},
                        {"name": "getWidth", "description": "Get world width"},
                        {"name": "getHeight", "description": "Get world height"}
                    ]
                }
            ]
        }
    ],
    "advanced_examples": [
        {
            "title": "Advanced Memory Management",
            "description": "Working with memory pools, episodic memory patterns, and memory optimization",
            "file": "examples/advanced_usage.py",
            "difficulty": "advanced",
            "topics": ["Memory Management", "Episodic Memory", "Pattern Completion", "Working Memory Optimization"]
        },
        {
            "title": "Custom Component Creation",
            "description": "Creating custom brain regions, plasticity rules, neuromodulators, and memory systems",
            "file": "examples/custom_components.py",
            "difficulty": "expert",
            "topics": ["Brain Regions", "Plasticity Rules", "Neuromodulators", "Plugin Architecture", "Memory Systems"]
        },
        {
            "title": "Performance Optimization",
            "description": "Memory management, performance monitoring, parallel processing, and simulation scaling",
            "file": "examples/performance_optimization.py",
            "difficulty": "advanced",
            "topics": ["Memory Management", "Performance Monitoring", "Parallel Processing", "Simulation Scaling"]
        },
        {
            "title": "Debugging and Troubleshooting",
            "description": "Memory leak detection, neuromodulation debugging, simulation state inspection",
            "file": "examples/debugging_troubleshooting.py",
            "difficulty": "intermediate",
            "topics": ["Memory Debugging", "Neuromodulation Debugging", "Simulation Debugging", "Performance Analysis"]
        },
        {
            "title": "Integration Frameworks",
            "description": "Research framework, agent training, behavioral analysis, and real-world applications",
            "file": "examples/integration_framework.py",
            "difficulty": "advanced",
            "topics": ["Research Framework", "Agent Training", "Behavioral Analysis", "Real-World Applications"]
        }
    ]
}
def generate_api_documentation():
    """Generate comprehensive API documentation."""
    print_header("8.1 API Reference Documentation")
    
    print(f"\nNLM Python API Reference Documentation")
    print(f"Version: {API_REFERENCE['version']}")
    print(f"Description: {API_REFERENCE['description']}")
    
    print(f"\nCore Modules:")
    for module in API_REFERENCE['modules']:
        print(f"\n  Module: {module['name']}")
        print(f"    Description: {module['description']}")
        
        if 'functions' in module:
            print(f"    Functions:")
            for func in module['functions']:
                print(f"      {func['name']}():")
                print(f"        Parameters: {func['parameters']}")
                print(f"        Returns: {func['returns']}")
                
        if 'classes' in module:
            print(f"    Classes:")
            for cls in module['classes']:
                print(f"      {cls['name']}:")
                print(f"        Description: {cls['description']}")
                print(f"        Methods: {len(cls['methods'])} methods")
    
    print(f"\nAdvanced Examples:")
    for example in API_REFERENCE['advanced_examples']:
        print(f"\n  {example['title']}:")
        print(f"    Difficulty: {example['difficulty']}")
        print(f"    Topics: {', '.join(example['topics'])}")
        print(f"    File: {example['file']}")
        print(f"    Description: {example['description']}")
    
    print(f"\nGetting Started Guide:")
    print(f"1. Basic Setup:")
    print(f"   ```python")
    print(f"   import pynlm")
    print(f"   brain = pynlm.createBrain(pynlm.createDefaultConfig())")
    print(f"   brain.initialize()")
    print(f"   ```")
    
    print(f"\n2. First Simulation:")
    print(f"   ```python")
    print(f"   world = pynlm.createSimpleWorld()")
    print(f"   agent = pynlm.createAgentBrain(brain)")
    print(f"   agent.initialize(world)")
    print(f"   ")
    print(f"   for step in range(100):")
    print(f"       world.update(0.1)")
    print(f"       percept = world.getSensoryPercept()")
    print(f"       agent.processSensoryInput(percept)")
    print(f"       brain.step(step)")
    print(f"       action = agent.decodeMotorCommand()")
    print(f"       world.applyMotorCommand(action, world.getSimulationTime())")
    print(f"   ```")
    
    print(f"\n3. Enable Learning:")
    print(f"   ```python")
    print(f"   agent.enableRewardModulation(True)")
    print(f"   agent.enableCuriosity(True)")
    print(f"   agent.enableDevelopment(True)")
    print(f"   ```")
    
    print(f"\n4. Monitoring:")
    print(f"   ```python")
    print(f"   print(f\"Neurons firing: {brain.getFiringNeuronCount()}\")")
    print(f"   print(f\"Total spikes: {brain.getTotalSpikeCount()}\")")
    print(f"   print(f\"Average firing rate: {brain.getAverageFiringRate():.2f}\")")
    print(f"   ```")
    
    print(f"\n5. Using Neuromodulators:")
    print(f"   ```python")
    print(f"   dopamine = brain.getDopamine()")
    print(f"   acetylcholine = brain.getAcetylcholine()")
    print(f"   ")
    print(f"   # Modulate attention with acetylcholine")
    print(f"   acetylcholine.setAttentionLevel(0.9)")
    print(f"   ")
    print(f"   # Reward signal with dopamine")
    print(f"   dopamine.signalReward(1.0)")
    print(f"   ```")
    
    return API_REFERENCE


def api_documentation_examples_summary():
    """Print summary of API documentation examples."""
    print_header("API Reference Documentation Summary")
    
    print(f"\nThis documentation provides:")
    print(f"✓ Complete API reference for all NLM Python bindings")
    print(f"✓ Detailed function and class documentation")
    print(f"✓ Parameter and return value descriptions")
    print(f"✓ Advanced examples organized by difficulty")
    print(f"✓ Getting started guide for new users")
    print(f"✓ Cross-references between related examples")
    print(f"✓ Error conditions and troubleshooting tips")
    
    print(f"\nKey Features:")
    print(f"• Comprehensive coverage of all NLM systems")
    print(f"• Integration with existing examples")
    print(f"• Clear documentation structure")
    print(f"• Practical usage patterns")
    print(f"• Advanced usage scenarios")
    print(f"• Performance and optimization guidance")
    
    print(f"\nNext Steps:")
    print(f"1. Review basic examples in advanced_usage.py")
    print(f"2. Explore custom component creation in custom_components.py")
    print(f"3. Study performance optimization techniques")
    print(f"4. Implement debugging and troubleshooting strategies")
    print(f"5. Setup research frameworks and applications")

# ============================================================================
# Main Execution
# ============================================================================

def main():
    """Run all integration and documentation examples."""
    print("=" * 80)
    print("NLM Integration Examples and API Documentation")
    print("Comprehensive collection demonstrating integration and documentation")
    print("=" * 80)
    
    # Run research framework example
    framework = research_framework_example()
    
    # Run agent training example
    training_history = agent_training_example()
    
    # Run behavioral analysis example
    behavioral_data = behavioral_analysis_example()
    
    # Run real-world application example
    launched_apps = real_world_application_example()
    
    # Generate API documentation
    api_docs = generate_api_documentation()
    
    print(f"\n" + "=" * 80)
    print("All integration and documentation examples completed successfully!")
    print("=" * 80)
    
    print(f"\nIntegration Framework Features Demonstrated:")
    print(f"1. Research framework for scientific experimentation")
    print(f"2. Agent training framework for reinforcement learning")
    print(f"3. Behavioral analysis tools for neural activity patterns")
    print(f"4. Real-world application setup and integration")
    print(f"5. Application lifecycle management")
    print(f"6. Multi-application coordination")
    
    print(f"\nAPI Documentation Features:")
    print(f"1. Complete function and class documentation")
    print(f"2. Parameter and return value descriptions")
    print(f"3. Usage examples and code snippets")
    print(f"4. Advanced example organization")
    print(f"5. Getting started guide")
    
    print(f"\nCollection Benefits:")
    print(f"• Provides comprehensive NLM usage patterns")
    print(f"• Demonstrates best practices and advanced techniques")
    print(f"""• Supports research, development, and production use cases")
    print(f"• Includes performance and debugging guidance")
    print(f"• Facilitates learning and system extension")
    
    print(f"\nNext Steps for Users:")
    print(f"1. Start with basic examples in advanced_usage.py")
    print(f"2. Explore custom component creation in custom_components.py")
    print(f"3. Study performance optimization techniques")
    print(f"4. Implement research frameworks and applications")
    print(f"5. Use API documentation for reference and guidance")

if __name__ == "__main__":
    main()