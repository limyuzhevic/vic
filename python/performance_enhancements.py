#!/usr/bin/env python3
"""
Performance enhancements for NLM Python bindings.

This module provides performance-optimized functions and utilities
to improve the performance of the NLM Python bindings.
"""

import sys
import os
import time
import numpy as np
import threading
from typing import List, Any, Optional, Union, Dict, Tuple, Callable
from functools import wraps, lru_cache
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import (
    EnhancedConfig,
    EnhancedBrain,
    EnhancedSimpleWorld,
    EnhancedAgentBrain,
    Vision,
    Audio,
    Action,
    SensoryPercept,
    NLMError,
    SimulationError,
    ConfigurationError,
    SensoryError,
)
class PerformanceMonitor:
    """Performance monitoring utility for NLM operations."""
    
    def __init__(self):
        self.timings: Dict[str, List[float]] = {}
        self.counters: Dict[str, int] = {}
        self._lock = threading.Lock()
    
    def start_timing(self, operation: str):
        """Start timing an operation."""
        with self._lock:
            if operation not in self.timings:
                self.timings[operation] = []
    
    def end_timing(self, operation: str) -> float:
        """End timing and return elapsed time."""
        with self._lock:
            if operation not in self.timings:
                self.timings[operation] = []
            
            elapsed = time.time()
            self.timings[operation].append(elapsed)
            self.counters[operation] = self.counters.get(operation, 0) + 1
            return elapsed
    
    def get_stats(self, operation: str) -> Dict[str, float]:
        """Get performance statistics for an operation."""
        with self._lock:
            if operation not in self.timings or not self.timings[operation]:
                return {"count": 0, "mean": 0.0, "std": 0.0, "min": 0.0, "max": 0.0}
            
            times = self.timings[operation]
            count = len(times)
            
            return {
                "count": count,
                "mean": np.mean(times),
                "std": np.std(times),
                "min": np.min(times),
                "max": np.max(times),
            }
    
    def get_all_stats(self) -> Dict[str, Dict[str, float]]:
        """Get performance statistics for all operations."""
        with self._lock:
            stats = {}
            for operation in self.timings:
                stats[operation] = self.get_stats(operation)
            return stats
    
    def reset(self):
        """Reset performance monitoring."""
        with self._lock:
            self.timings.clear()
            self.counters.clear()


# Global performance monitor
_perf_monitor = PerformanceMonitor()


def monitor_performance(operation: str):
    """Decorator to monitor performance of a function."""
    def decorator(func: Callable) -> Callable:
        @wraps(func)
        def wrapper(*args, **kwargs):
            _perf_monitor.start_timing(operation)
            start_time = time.time()
            
            try:
                result = func(*args, **kwargs)
                return result
            finally:
                _perf_monitor.end_timing(operation)
                elapsed = time.time() - start_time
        
        return wrapper
    return decorator


class OptimizedConfig:
    """Optimized configuration operations."""
    
    def __init__(self, config: EnhancedConfig):
        self._config = config
        self._cache = {}
        self._dirty = False
    
    @monitor_performance("config_set")
    def set(self, key: str, value: Any, source: str = "runtime") -> None:
        """Set configuration value with caching."""
        self._config.set(key, value, source)
        self._cache[key] = value
        self._dirty = True
    
    @monitor_performance("config_get")
    def get(self, key: str) -> Any:
        """Get configuration value with caching."""
        if key in self._cache:
            return self._cache[key]
        return self._config.get(key)
    
    @monitor_performance("config_validate")
    def validate(self) -> List[str]:
        """Validate configuration."""
        return self._config.validate()
    
    @monitor_performance("config_summary")
    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
    
    def batch_set(self, values: Dict[str, Any]) -> None:
        """Set multiple configuration values in batch."""
        for key, value in values.items():
            self.set(key, value)


class BatchedBrain:
    """Brain operations with batching for better performance."""
    
    def __init__(self, brain: EnhancedBrain):
        self._brain = brain
        self._step_queue = []
        self._batch_size = 10
    
    @monitor_performance("brain_batch_step")
    def batch_step(self, steps: List[int], times: Optional[List[float]] = None) -> None:
        """Perform multiple brain steps in batch."""
        if times is None:
            times = [float(step) for step in steps]
        
        for step, time_val in zip(steps, times):
            self._brain.step(step, time_val)
    
    @monitor_performance("brain_batch_inject")
    def batch_inject_current(self, injections: List[Tuple[Any, float]]) -> None:
        """Inject current into multiple neurons in batch."""
        for neuron_id, current in injections:
            self._brain.inject_current(neuron_id, current)
    
    @monitor_performance("brain_batch_initialize")
    def batch_initialize(self, configs: List[EnhancedConfig]) -> List[EnhancedBrain]:
        """Initialize multiple brains in batch."""
        brains = []
        for config in configs:
            brain = EnhancedBrain(config)
            if brain.initialize():
                brains.append(brain)
        return brains


class VectorizedSensory:
    """Vectorized sensory operations for better performance."""
    
    @staticmethod
    @monitor_performance("vision_batch_process")
    def batch_process_vision(vision_list: List[List[List[float]]]) -> List[np.ndarray]:
        """Process multiple vision inputs efficiently."""
        return [np.array(vision, dtype=np.float32) for vision in vision_list]
    
    @staticmethod
    @monitor_performance("audio_batch_process")
    def batch_process_audio(audio_list: List[List[float]]) -> List[np.ndarray]:
        """Process multiple audio inputs efficiently."""
        return [np.array(audio, dtype=np.float32) for audio in audio_list]
    
    @staticmethod
    @monitor_performance("percept_batch_process")
    def batch_process_percepts(percept_list: List[SensoryPercept]) -> List[SensoryPercept]:
        """Process multiple percepts efficiently."""
        return percept_list


class MemoryPool:
    """Memory pool for reusable objects."""
    
    def __init__(self):
        self._action_pool = []
        self._vision_pool = []
        self._audio_pool = []
        self._percept_pool = []
        self._lock = threading.Lock()
    
    def get_action(self) -> Action:
        """Get an action from the pool or create new one."""
        with self._lock:
            if self._action_pool:
                return self._action_pool.pop()
            return Action()
    
    def return_action(self, action: Action) -> None:
        """Return an action to the pool."""
        with self._lock:
            self._action_pool.append(action)
    
    def get_vision(self, width: int, height: int, channels: int) -> Vision:
        """Get a vision object from the pool or create new one."""
        with self._lock:
            if self._vision_pool:
                vision = self._vision_pool.pop()
                vision.__init__(width, height, channels)
                return vision
            return Vision(width, height, channels)
    
    def return_vision(self, vision: Vision) -> None:
        """Return a vision object to the pool."""
        with self._lock:
            self._vision_pool.append(vision)
    
    def clear(self):
        """Clear all pooled objects."""
        with self._lock:
            self._action_pool.clear()
            self._vision_pool.clear()
            self._audio_pool.clear()
            self._percept_pool.clear()


# Global memory pool
_memory_pool = MemoryPool()


def optimized_simulation(
    config: EnhancedConfig,
    num_steps: int,
    num_threads: int = 1,
    use_batching: bool = True
) -> Dict[str, Any]:
    """Run optimized simulation with multiple optimization strategies.
    
    Args:
        config: Configuration for the brain.
        num_steps: Number of simulation steps.
        num_threads: Number of threads to use for parallel processing.
        use_batching: Whether to use batching for operations.
        
    Returns:
        Dictionary containing simulation results and performance statistics.
    """
    start_time = time.time()
    
    # Create brain
    brain = EnhancedBrain(config)
    brain.initialize()
    
    # Prepare batched steps if enabled
    if use_batching and num_steps > 1:
        batched_brain = BatchedBrain(brain)
        
        # Split steps into batches
        batch_size = max(1, num_steps // num_threads)
        batches = []
        
        for i in range(0, num_steps, batch_size):
            batch_steps = list(range(i, min(i + batch_size, num_steps)))
            batches.append(batch_steps)
        
        # Process batches
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            list(executor.map(lambda batch: batched_brain.batch_step(batch), batches))
    else:
        # Process steps sequentially
        for step in range(num_steps):
            brain.step(step)
    
    end_time = time.time()
    
    # Collect results
    results = {
        "total_spikes": brain.total_spike_count,
        "final_firing_rate": brain.average_firing_rate,
        "final_ei_ratio": brain.excitation_inhibition_ratio,
        "final_stage": brain.developmental_stage,
        "simulation_time": end_time - start_time,
        "steps_per_second": num_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0,
    }
    
    # Get performance statistics
    results["performance_stats"] = _perf_monitor.get_all_stats()
    
    return results


@lru_cache(maxsize=128)
@monitor_performance("vision_create_cached")
def create_vision_cached(width: int, height: int, channels: int) -> Vision:
    """Create vision object with caching."""
    return Vision(width, height, channels)


@lru_cache(maxsize=64)
@monitor_performance("audio_create_cached")
def create_audio_cached(sample_rate: int, num_samples: int) -> Audio:
    """Create audio object with caching."""
    return Audio(sample_rate, num_samples)


@lru_cache(maxsize=256)
@monitor_performance("action_create_cached")
def create_action_cached(action_type: Any, parameters: Tuple[float, ...]) -> Action:
    """Create action object with caching."""
    return Action(action_type, list(parameters))


class OptimizedWorldSimulation:
    """Optimized world simulation with multiple strategies."""
    
    def __init__(self, world: EnhancedSimpleWorld):
        self._world = world
        self._observation_cache = {}
        self._action_cache = {}
    
    @monitor_performance("world_optimized_simulation")
    def run_optimized_simulation(
        self,
        agent: EnhancedAgentBrain,
        num_steps: int,
        use_caching: bool = True,
        observation_interval: int = 5
    ) -> Dict[str, Any]:
        """Run optimized world simulation.
        
        Args:
            agent: Agent brain for the simulation.
            num_steps: Number of simulation steps.
            use_caching: Whether to use caching for observations and actions.
            observation_interval: Interval for caching observations.
            
        Returns:
            Dictionary containing simulation results.
        """
        start_time = time.time()
        
        # Run simulation
        for step in range(num_steps):
            self._world.update(0.1)
            
            # Get sensory input with optional caching
            if use_caching and step % observation_interval == 0:
                percept_key = f"step_{step}"
                if percept_key not in self._observation_cache:
                    self._observation_cache[percept_key] = self._world.getSensoryPercept()
                percept = self._observation_cache[percept_key]
            else:
                percept = self._world.getSensoryPercept()
            
            # Process sensory input
            action = agent.processSensoryInput(percept)
            
            # Apply action with optional caching
            if use_caching and step % observation_interval == 0:
                action_key = f"step_{step}"
                if action_key not in self._action_cache:
                    self._action_cache[action_key] = action
                world_result = self._world.applyMotorCommand(self._action_cache[action_key], self._world.simulation_time)
            else:
                world_result = self._world.applyMotorCommand(action, self._world.simulation_time)
            
            # Agent applies reward modulation based on result
            if world_result.success:
                agent.applyRewardModulation(world_result.reward, agent.prediction_error)
        
        end_time = time.time()
        
        # Get final world state
        final_body = self._world.getAgentBody()
        
        return {
            "final_position": (final_body.x, final_body.y),
            "final_energy": final_body.energy,
            "final_health": final_body.health,
            "simulation_time": end_time - start_time,
            "steps_per_second": num_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0,
            "observations_cached": len(self._observation_cache) if use_caching else 0,
            "actions_cached": len(self._action_cache) if use_caching else 0,
        }


def get_performance_summary() -> Dict[str, Any]:
    """Get comprehensive performance summary."""
    return {
        "monitor_stats": _perf_monitor.get_all_stats(),
        "cache_stats": {
            "vision_cache_info": create_vision_cached.cache_info(),
            "audio_cache_info": create_audio_cached.cache_info(),
            "action_cache_info": create_action_cached.cache_info(),
        },
    }


def reset_performance_monitor():
    """Reset performance monitoring."""
    _perf_monitor.reset()
    create_vision_cached.cache_clear()
    create_audio_cached.cache_clear()
    create_action_cached.cache_clear()


def cleanup_resources():
    """Clean up all resources and reset pools."""
    _memory_pool.clear()
    reset_performance_monitor()


# Example usage functions

def example_optimized_simulation():
    """Example of optimized simulation."""
    print("=" * 60)
    print("Optimized Simulation Example")
    print("=" * 60)
    
    config = EnhancedConfig()
    config.set("brain.neuron_count", 5000)
    
    results = optimized_simulation(config, num_steps=1000, num_threads=2)
    
    print(f"Simulation completed in {results['simulation_time']:.3f}s")
    print(f"Steps per second: {results['steps_per_second']:.1f}")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Final firing rate: {results['final_firing_rate']:.3f} Hz")
    print(f"Final E/I ratio: {results['final_ei_ratio']:.3f}")
    
    # Show performance stats
    print("\nPerformance statistics:")
    for operation, stats in results['performance_stats'].items():
        print(f"  {operation}: {stats['count']} calls, "
              f"mean {stats['mean']:.6f}s, "
              f"std {stats['std']:.6f}s")
    
    print()


def example_optimized_world_simulation():
    """Example of optimized world simulation."""
    print("=" * 60)
    print("Optimized World Simulation Example")
    print("=" * 60)
    
    # Create world and agent
    world = EnhancedSimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    config = EnhancedConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = EnhancedBrain(config)
    brain.initialize()
    
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    # Run optimized simulation
    optimized_world = OptimizedWorldSimulation(world)
    results = optimized_world.run_optimized_simulation(
        agent, num_steps=50, use_caching=True, observation_interval=5
    )
    
    print(f"World simulation completed in {results['simulation_time']:.3f}s")
    print(f"Steps per second: {results['steps_per_second']:.1f}")
    print(f"Final position: {results['final_position']}")
    print(f"Final energy: {results['final_energy']:.1f}")
    print(f"Observations cached: {results['observations_cached']}")
    print(f"Actions cached: {results['actions_cached']}")
    
    print()


def example_memory_pool():
    """Example of memory pool usage."""
    print("=" * 60)
    print("Memory Pool Example")
    print("=" * 60)
    
    # Get objects from memory pool
    action1 = _memory_pool.get_action()
    print(f"Got action from pool: {action1.getName()}")
    
    vision1 = _memory_pool.get_vision(32, 32, 3)
    print(f"Got vision from pool: {vision1.width}x{vision1.height}x{vision1.channels}")
    
    # Use objects
    action1.setType(pynlm.ActionType.MoveForward)
    action1.setParameters([1.0, 0.5])
    
    vision1.setData([0.1, 0.2, 0.3, 0.4] * (32 * 32 * 3))
    
    # Return objects to pool
    _memory_pool.return_action(action1)
    _memory_pool.return_vision(vision1)
    
    print("Objects returned to memory pool")
    print(f"Pool size: Actions={len(_memory_pool._action_pool)}, "
          f"Vision={len(_memory_pool._vision_pool)}")
    
    print()


def run_performance_examples():
    """Run all performance optimization examples."""
    print("NLM Python Bindings - Performance Optimization Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_optimized_simulation,
        example_optimized_world_simulation,
        example_memory_pool,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"✗ Example {i} failed: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    # Show performance summary
    print("=" * 60)
    print("Performance Summary")
    print("=" * 60)
    
    summary = get_performance_summary()
    print(f"Monitor statistics: {len(summary['monitor_stats'])} operations tracked")
    print(f"Cache statistics:")
    for cache_name, cache_info in summary['cache_stats'].items():
        print(f"  {cache_name}: {cache_info}")
    
    print()
    print("✓ All performance optimization examples completed!")
    print("=" * 60)


if __name__ == "__main__":
    run_performance_examples()
