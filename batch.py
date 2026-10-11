"""
Batch processing system for NLM

This module provides batch processing capabilities for NLM including
parallel simulation execution, results aggregation, and batch analysis.
"""

import time
import threading
import queue
from typing import Dict, List, Any, Optional, Union, Callable, Tuple
from enum import Enum
from dataclasses import dataclass
from copy import deepcopy
from concurrent.futures import ThreadPoolExecutor, as_completed
import numpy as np

class BatchStatus(Enum):
    """Batch processing status."""
    QUEUED = "queued"
    RUNNING = "running"
    COMPLETED = "completed"
    CANCELLED = "cancelled"
    ERROR = "error"

class ProcessingMode(Enum):
    """Processing modes."""
    SEQUENTIAL = "sequential"
    PARALLEL = "parallel"
    ASYNC = "async"
    HYBRID = "hybrid"

@dataclass
class SimulationTask:
    """Simulation task for batch processing."""
    
    task_id: str
    brain: Any
    world: Any
    agent: Any
    steps: int
    timestep: float = 0.1
    priority: int = 1
    callback: Optional[Callable] = None
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class BatchResult:
    """Batch processing result."""
    
    task_id: str
    success: bool
    execution_time: float
    start_time: float
    end_time: float
    metrics: Dict[str, Any]
    error_message: Optional[str] = None
    
    def get_duration(self) -> float:
        """Get task duration."""
        return self.end_time - self.start_time

@dataclass
class BatchStatistics:
    """Batch processing statistics."""
    
    total_tasks: int = 0
    completed_tasks: int = 0
    failed_tasks: int = 0
    cancelled_tasks: int = 0
    total_execution_time: float = 0.0
    total_steps: int = 0
    avg_execution_time: float = 0.0
    avg_steps_per_second: float = 0.0
    success_rate: float = 0.0
    error_rate: float = 0.0
    
    def update(self, result: BatchResult):
        """Update statistics with result.
        
        Args:
            result: Batch result to update statistics with
        """
        if result.success:
            self.completed_tasks += 1
        else:
            self.failed_tasks += 1
        
        self.total_execution_time += result.execution_time
        self.total_steps += result.metrics.get('steps_completed', 0)
        
        self.avg_execution_time = self.total_execution_time / self.completed_tasks if self.completed_tasks > 0 else 0.0
        self.avg_steps_per_second = self.total_steps / self.total_execution_time if self.total_execution_time > 0 else 0.0
        self.success_rate = self.completed_tasks / self.total_tasks if self.total_tasks > 0 else 0.0
        self.error_rate = self.failed_tasks / self.total_tasks if self.total_tasks > 0 else 0.0
class BatchProcessor:
    """Batch processing system for NLM."""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """Initialize batch processor.
        
        Args:
            config: Configuration for batch processing
        """
        self.config = config or {}
        self.queue = queue.PriorityQueue()
        self.results: Dict[str, BatchResult] = {}
        self.tasks: Dict[str, SimulationTask] = {}
        self.status = BatchStatus.QUEUED
        self.workers: List[threading.Thread] = []
        self.executor: Optional[ThreadPoolExecutor] = None
        self.max_workers = self.config.get('max_workers', 4)
        self.processing_mode = ProcessingMode.SEQUENTIAL
        self.retry_count = self.config.get('retry_count', 3)
        self.retry_delay = self.config.get('retry_delay', 1.0)
        self.timeout = self.config.get('timeout', 300.0)
        self.progress_callback: Optional[Callable] = None
        self.start_time = 0.0
        self.end_time = 0.0
        
    def add_simulation(self, brain: Any, world: Any, agent: Any, steps: int, 
                      task_id: Optional[str] = None, **kwargs) -> str:
        """Add simulation to batch queue.
        
        Args:
            brain: Brain instance
            world: World instance
            agent: Agent instance
            steps: Number of simulation steps
            task_id: Optional task ID (generated if None)
            **kwargs: Additional task parameters
            
        Returns:
            Task ID
        """
        if task_id is None:
            task_id = f"task_{len(self.tasks) + 1}_{int(time.time())}"
        
        task = SimulationTask(
            task_id=task_id,
            brain=deepcopy(brain) if hasattr(brain, 'copy') else brain,
            world=deepcopy(world) if hasattr(world, 'copy') else world,
            agent=deepcopy(agent) if hasattr(agent, 'copy') else agent,
            steps=steps,
            **kwargs
        )
        
        self.tasks[task_id] = task
        self.queue.put((task.priority, task_id))
        
        if self.progress_callback:
            self.progress_callback('task_added', {'task_id': task_id, 'steps': steps})
        
        return task_id
    
    def process_batch(self, mode: Optional[ProcessingMode] = None, 
                     progress_callback: Optional[Callable] = None) -> List[BatchResult]:
        """Process all simulations in queue.
        
        Args:
            mode: Processing mode (uses default if None)
            progress_callback: Progress callback function
            
        Returns:
            List of batch results
        """
        if mode is None:
            mode = self.processing_mode
        
        if progress_callback:
            self.progress_callback = progress_callback
        
        self.status = BatchStatus.RUNNING
        self.start_time = time.time()
        
        # Clear previous results
        self.results.clear()
        
        results = []
        
        if mode == ProcessingMode.SEQUENTIAL:
            results = self._process_sequential()
        elif mode == ProcessingMode.PARALLEL:
            results = self._process_parallel()
        elif mode == ProcessingMode.ASYNC:
            results = self._process_async()
        elif mode == ProcessingMode.HYBRID:
            results = self._process_hybrid()
        
        self.end_time = time.time()
        self.status = BatchStatus.COMPLETED
        
        return results
    
    def _process_sequential(self) -> List[BatchResult]:
        """Process simulations sequentially.
        
        Returns:
            List of batch results
        """
        results = []
        
        while not self.queue.empty():
            priority, task_id = self.queue.get()
            task = self.tasks[task_id]
            
            result = self._process_task(task)
            results.append(result)
            self.results[task_id] = result
        
        return results
    
    def _process_parallel(self) -> List[BatchResult]:
        """Process simulations in parallel.
        
        Returns:
            List of batch results
        """
        results = []
        
        # Create thread pool
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_task = {}
            for task_id in list(self.tasks.keys()):
                future = executor.submit(self._process_task, self.tasks[task_id])
                future_to_task[future] = task_id
            
            # Collect results as they complete
            for future in as_completed(future_to_task):
                task_id = future_to_task[future]
                try:
                    result = future.result(timeout=self.timeout)
                    results.append(result)
                    self.results[task_id] = result
                except Exception as e:
                    result = BatchResult(
                        task_id=task_id,
                        success=False,
                        execution_time=0.0,
                        start_time=time.time(),
                        end_time=time.time(),
                        metrics={},
                        error_message=str(e)
                    )
                    results.append(result)
                    self.results[task_id] = result
        
        return results
    
    def _process_async(self) -> List[BatchResult]:
        """Process simulations asynchronously.
        
        Returns:
            List of batch results
        """
        results = []
        
        # Create async queue
        async_queue = queue.Queue()
        for task_id in list(self.tasks.keys()):
            async_queue.put(self.tasks[task_id])
        
        # Process in async manner
        while not async_queue.empty():
            task = async_queue.get()
            result = self._process_task(task)
            results.append(result)
            self.results[task_id] = result
        
        return results
    
    def _process_hybrid(self) -> List[BatchResult]:
        """Process simulations using hybrid approach.
        
        Returns:
            List of batch results
        """
        # Use parallel for CPU-intensive tasks
        cpu_tasks = [task for task in self.tasks.values() 
                    if task.steps > 100 or task.priority > 2]
        
        # Use sequential for quick tasks
        quick_tasks = [task for task in self.tasks.values() 
                      if task not in cpu_tasks]
        
        results = []
        
        # Process quick tasks sequentially
        for task in quick_tasks:
            result = self._process_task(task)
            results.append(result)
            self.results[task.task_id] = result
        
        # Process CPU-intensive tasks in parallel
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            future_to_task = {executor.submit(self._process_task, task): task 
                            for task in cpu_tasks}
            
            for future in as_completed(future_to_task):
                task = future_to_task[future]
                try:
                    result = future.result(timeout=self.timeout)
                    results.append(result)
                    self.results[task.task_id] = result
                except Exception as e:
                    result = BatchResult(
                        task_id=task.task_id,
                        success=False,
                        execution_time=0.0,
                        start_time=time.time(),
                        end_time=time.time(),
                        metrics={},
                        error_message=str(e)
                    )
                    results.append(result)
                    self.results[task.task_id] = result
        
        return results
    
    def _process_task(self, task: SimulationTask) -> BatchResult:
        """Process a single simulation task.
        
        Args:
            task: Simulation task to process
            
        Returns:
            Batch result
        """
        start_time = time.time()
        
        try:
            # Execute simulation
            metrics = self._execute_simulation(task)
            
            end_time = time.time()
            execution_time = end_time - start_time
            
            # Call task callback if provided
            if task.callback:
                task.callback({
                    'task_id': task.task_id,
                    'start_time': start_time,
                    'end_time': end_time,
                    'execution_time': execution_time,
                    'metrics': metrics
                })
            
            # Call progress callback
            if self.progress_callback:
                self.progress_callback('task_completed', {
                    'task_id': task.task_id,
                    'execution_time': execution_time,
                    'metrics': metrics
                })
            
            return BatchResult(
                task_id=task.task_id,
                success=True,
                execution_time=execution_time,
                start_time=start_time,
                end_time=end_time,
                metrics=metrics
            )
        
        except Exception as e:
            end_time = time.time()
            execution_time = end_time - start_time
            
            # Retry logic
            if task.task_id in self.tasks:
                task_retry = self.tasks[task.task_id]
                if task_retry.metadata.get('retry_count', 0) < self.retry_count:
                    # Increment retry count
                    task_retry.metadata['retry_count'] = task_retry.metadata.get('retry_count', 0) + 1
                    
                    # Wait before retry
                    time.sleep(self.retry_delay)
                    
                    # Retry the task
                    return self._process_task(task_retry)
            
            # Call progress callback for error
            if self.progress_callback:
                self.progress_callback('task_failed', {
                    'task_id': task.task_id,
                    'error': str(e),
                    'execution_time': execution_time
                })
            
            return BatchResult(
                task_id=task.task_id,
                success=False,
                execution_time=execution_time,
                start_time=start_time,
                end_time=end_time,
                metrics={},
                error_message=str(e)
            )
    
    def _execute_simulation(self, task: SimulationTask) -> Dict[str, Any]:
        """Execute a simulation task.
        
        Args:
            task: Simulation task
            
        Returns:
            Simulation metrics
        """
        # Reset brain, world, and agent
        if hasattr(task.brain, 'reset'):
            task.brain.reset()
        if hasattr(task.world, 'reset'):
            task.world.reset()
        if hasattr(task.agent, 'reset'):
            task.agent.reset()
        
        # Initialize brain, world, and agent
        if hasattr(task.brain, 'initialize'):
            task.brain.initialize()
        if hasattr(task.world, 'initialize'):
            task.world.initialize()
        
        # Run simulation
        start_step_time = time.time()
        
        for step in range(task.steps):
            # Get sensory input from world
            percept = task.world.getSensoryPercept()
            
            # Process sensory input with agent
            if hasattr(task.agent, 'processSensoryInput'):
                task.agent.processSensoryInput(percept)
            
            # Update world
            task.world.update(task.timestep)
            
            # Update brain
            if hasattr(task.brain, 'step'):
                task.brain.step(step, step * task.timestep)
            
            # Update agent
            if hasattr(task.agent, 'update'):
                task.agent.update(task.timestep)
            
            # Record progress
            if (step + 1) % 100 == 0:  # Log every 100 steps
                pass  # Can be extended with logging
        
        end_step_time = time.time()
        execution_time = end_step_time - start_step_time
        
        # Collect metrics
        metrics = {
            'steps_completed': task.steps,
            'execution_time': execution_time,
            'steps_per_second': task.steps / execution_time if execution_time > 0 else 0,
        }
        
        # Add brain metrics
        if hasattr(task.brain, 'getTotalSpikeCount'):
            metrics['total_spikes'] = task.brain.getTotalSpikeCount()
        
        if hasattr(task.brain, 'getFiringNeuronCount'):
            metrics['firing_neurons'] = task.brain.getFiringNeuronCount()
        
        # Add world metrics
        if hasattr(task.world, 'get_simulation_time'):
            metrics['world_time'] = task.world.get_simulation_time()
        
        return metrics
    
    def analyze_results(self) -> Dict[str, Any]:
        """Analyze batch processing results.
        
        Returns:
            Analysis results
        """
        if not self.results:
            return {}
        
        statistics = BatchStatistics()
        statistics.total_tasks = len(self.results)
        
        # Aggregate results
        for result in self.results.values():
            statistics.update(result)
        
        # Calculate additional statistics
        analysis = {
            'statistics': statistics,
            'performance': self._analyze_performance(),
            'bottlenecks': self._identify_bottlenecks(),
            'recommendations': self._generate_recommendations(),
        }
        
        return analysis
    
    def _analyze_performance(self) -> Dict[str, Any]:
        """Analyze batch performance.
        
        Returns:
            Performance analysis
        """
        performance = {
            'total_time': self.end_time - self.start_time,
            'throughput': 0.0,
            'efficiency': 0.0,
        }
        
        if self.end_time > self.start_time:
            performance['throughput'] = len(self.results) / (self.end_time - self.start_time)
            
            # Calculate efficiency based on successful tasks
            successful_tasks = sum(1 for r in self.results.values() if r.success)
            if successful_tasks > 0:
                total_time = sum(r.execution_time for r in self.results.values())
                efficiency = successful_tasks / self.max_workers if self.max_workers > 0 else 1.0
                performance['efficiency'] = efficiency
        
        return performance
    
    def _identify_bottlenecks(self) -> List[str]:
        """Identify performance bottlenecks.
        
        Returns:
            List of bottleneck descriptions
        """
        bottlenecks = []
        
        if not self.results:
            return bottlenecks
        
        # Find slowest tasks
        slowest_tasks = sorted(
            [(r.task_id, r.execution_time) for r in self.results.values() if r.success],
            key=lambda x: x[1],
            reverse=True
        )
        
        if slowest_tasks:
            top_slowest = slowest_tasks[:3]
            bottlenecks.append(f"Top {len(top_slowest)} slowest tasks: {[f'{tid} ({time:.2f}s)' for tid, time in top_slowest]}")
        
        # Find error-prone tasks
        error_tasks = [r for r in self.results.values() if not r.success]
        if error_tasks:
            bottlenecks.append(f"Error rate: {len(error_tasks)}/{len(self.results)} ({len(error_tasks)/len(self.results)*100:.1f}%)")
        
        return bottlenecks
    
    def _generate_recommendations(self) -> List[str]:
        """Generate processing recommendations.
        
        Returns:
            List of recommendations
        """
        recommendations = []
        
        analysis = self.analyze_results()
        statistics = analysis['statistics']
        
        # Performance recommendations
        if statistics.avg_execution_time > 10.0:  # 10 seconds
            recommendations.append("Average execution time is high. Consider increasing parallelism.")
        
        if statistics.success_rate < 0.9:  # 90% success rate
            recommendations.append("Success rate is low. Review error messages and task configuration.")
        
        if statistics.avg_steps_per_second < 10.0:
            recommendations.append("Steps per second is low. Consider optimizing simulation parameters.")
        
        return recommendations

# Create singleton instance for global use
_batch_processor_instance: Optional[BatchProcessor] = None

def get_batch_processor(config: Optional[Dict[str, Any]] = None) -> BatchProcessor:
    """Get global batch processor instance.
    
    Args:
        config: Configuration for batch processor
        
    Returns:
        BatchProcessor instance
    """
    global _batch_processor_instance
    if _batch_processor_instance is None:
        _batch_processor_instance = BatchProcessor(config)
    return _batch_processor_instance

def add_simulation(brain: Any, world: Any, agent: Any, steps: int, 
                  task_id: Optional[str] = None, **kwargs) -> str:
    """Add simulation using global batch processor.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Agent instance
        steps: Number of simulation steps
        task_id: Optional task ID
        **kwargs: Additional task parameters
        
    Returns:
        Task ID
    """
    processor = get_batch_processor()
    return processor.add_simulation(brain, world, agent, steps, task_id, **kwargs)

def process_batch(mode: Optional[ProcessingMode] = None, 
                 progress_callback: Optional[Callable] = None) -> List[BatchResult]:
    """Process batch using global batch processor.
    
    Args:
        mode: Processing mode
        progress_callback: Progress callback function
        
    Returns:
        List of batch results
    """
    processor = get_batch_processor()
    return processor.process_batch(mode, progress_callback)

# Export types and classes
__all__ = [
    'BatchStatus',
    'ProcessingMode',
    'SimulationTask',
    'BatchResult',
    'BatchStatistics',
    'BatchProcessor',
    'get_batch_processor',
    'add_simulation',
    'process_batch',
]