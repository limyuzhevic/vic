"""
Performance monitoring and benchmarking utilities for NLM simulations.
"""

from typing import Dict, Any, List, Optional, Tuple
import time
import numpy as np
import psutil
import os

from .nlm_exception import ValidationError


class PerformanceMonitor:
    """
    Performance monitoring and benchmarking for NLM simulations.
    
    Provides comprehensive performance monitoring including:
    - Step-by-step timing measurements
    - Memory usage tracking
    - CPU utilization monitoring
    - Performance statistics and analysis
    - Benchmarking utilities
    - Resource usage monitoring
    """
    
    def __init__(self, brain: 'Brain', world: Optional['SimpleWorld'] = None):
        """
        Initialize performance monitor for a brain and optional world.
        
        Args:
            brain: Brain instance to monitor
            world: World instance (optional)
        """
        if not brain:
            raise ValidationError("Brain instance cannot be None")
        
        self.brain = brain
        self.world = world
        
        # Timing data
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.cpu_usage: List[float] = []
        self.start_time: Optional[float] = None
        self.end_time: Optional[float] = None
        
        # Process tracking
        self.process = psutil.Process(os.getpid())
        
        # Configuration
        self.max_samples: int = 10000
        self.measure_memory: bool = True
        self.measure_cpu: bool = True
        self.measure_steps: bool = True
        
        # Statistics
        self._stats_computed: bool = False
        self._current_stats: Dict[str, Any] = {}
    
    def start(self) -> None:
        """
        Start performance monitoring.
        
        Begins recording timing and resource usage for subsequent simulation steps.
        """
        self.start_time = time.time()
        self.step_times.clear()
        self.memory_usage.clear()
        self.cpu_usage.clear()
        self._stats_computed = False
        self._current_stats.clear()
        
        # Record initial memory usage
        if self.measure_memory:
            self.memory_usage.append(self._get_memory_usage())
        
        # Record initial CPU usage
        if self.measure_cpu:
            self.cpu_usage.append(self._get_cpu_usage())
    
    def stop(self) -> Dict[str, Any]:
        """
        Stop performance monitoring and return final results.
        
        Returns:
            Dictionary containing performance statistics
        """
        self.end_time = time.time()
        
        # Record final measurements
        if self.measure_memory:
            self.memory_usage.append(self._get_memory_usage())
        
        if self.measure_cpu:
            self.cpu_usage.append(self._get_cpu_usage())
        
        # Compute statistics
        self._compute_statistics()
        
        return self._current_stats.copy()
    
    def record_step(self, step_time: float) -> None:
        """
        Record the time taken for a simulation step.
        
        Args:
            step_time: Time taken for the step (in seconds)
        """
        if not self.measure_steps:
            return
        
        self.step_times.append(step_time)
        
        # Record resource usage for this step
        if self.measure_memory:
            self.memory_usage.append(self._get_memory_usage())
        
        if self.measure_cpu:
            self.cpu_usage.append(self._get_cpu_usage())
        
        # Trim if too many samples
        if len(self.step_times) > self.max_samples:
            self._trim_data()
    
    def get_current_stats(self) -> Dict[str, Any]:
        """
        Get current performance statistics (if computed).
        
        Returns:
            Dictionary of current statistics, or empty dict if not computed
        """
        if not self._stats_computed:
            self._compute_statistics()
        
        return self._current_stats.copy()
    
    def get_step_times(self) -> List[float]:
        """Get recorded step times."""
        return self.step_times.copy()
    
    def get_memory_usage(self) -> List[float]:
        """Get recorded memory usage."""
        return self.memory_usage.copy()
    
    def get_cpu_usage(self) -> List[float]:
        """Get recorded CPU usage."""
        return self.cpu_usage.copy()
    
    def reset(self) -> None:
        """Reset all performance data."""
        self.step_times.clear()
        self.memory_usage.clear()
        self.cpu_usage.clear()
        self.start_time = None
        self.end_time = None
        self._stats_computed = False
        self._current_stats.clear()
    
    def configure(self, **kwargs) -> None:
        """
        Configure monitoring settings.
        
        Args:
            **kwargs: Configuration options
                - max_samples: Maximum number of samples to store
                - measure_memory: Whether to measure memory usage
                - measure_cpu: Whether to measure CPU usage
                - measure_steps: Whether to measure step times
        """
        for key, value in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, value)
    
    def _compute_statistics(self) -> None:
        """Compute performance statistics from collected data."""
        if self.start_time is None or self.end_time is None:
            return
        
        total_time = self.end_time - self.start_time
        
        # Basic timing statistics
        if self.step_times:
            step_stats = {
                'steps': len(self.step_times),
                'total_time': total_time,
                'avg_step_time': np.mean(self.step_times),
                'min_step_time': np.min(self.step_times),
                'max_step_time': np.max(self.step_times),
                'std_step_time': np.std(self.step_times) if len(self.step_times) > 1 else 0.0,
                'steps_per_second': len(self.step_times) / total_time if total_time > 0 else 0.0,
            }
        else:
            step_stats = {
                'steps': 0,
                'total_time': total_time,
                'avg_step_time': 0.0,
                'min_step_time': 0.0,
                'max_step_time': 0.0,
                'std_step_time': 0.0,
                'steps_per_second': 0.0,
            }
        
        # Memory statistics
        if self.memory_usage and len(self.memory_usage) > 1:
            memory_stats = {
                'memory_samples': len(self.memory_usage),
                'initial_memory': self.memory_usage[0],
                'final_memory': self.memory_usage[-1],
                'memory_change': self.memory_usage[-1] - self.memory_usage[0],
                'avg_memory': np.mean(self.memory_usage),
                'min_memory': np.min(self.memory_usage),
                'max_memory': np.max(self.memory_usage),
                'memory_std': np.std(self.memory_usage) if len(self.memory_usage) > 1 else 0.0,
            }
        else:
            memory_stats = {
                'memory_samples': 0,
                'initial_memory': 0.0,
                'final_memory': 0.0,
                'memory_change': 0.0,
                'avg_memory': 0.0,
                'min_memory': 0.0,
                'max_memory': 0.0,
                'memory_std': 0.0,
            }
        
        # CPU statistics
        if self.cpu_usage and len(self.cpu_usage) > 1:
            cpu_stats = {
                'cpu_samples': len(self.cpu_usage),
                'initial_cpu': self.cpu_usage[0],
                'final_cpu': self.cpu_usage[-1],
                'avg_cpu': np.mean(self.cpu_usage),
                'min_cpu': np.min(self.cpu_usage),
                'max_cpu': np.max(self.cpu_usage),
                'cpu_std': np.std(self.cpu_usage) if len(self.cpu_usage) > 1 else 0.0,
            }
        else:
            cpu_stats = {
                'cpu_samples': 0,
                'initial_cpu': 0.0,
                'final_cpu': 0.0,
                'avg_cpu': 0.0,
                'min_cpu': 0.0,
                'max_cpu': 0.0,
                'cpu_std': 0.0,
            }
        
        # Compilation efficiency
        if self.step_times:
            efficiency_stats = {
                'efficiency_score': 1.0 / (step_stats['avg_step_time'] + 0.001),  # Higher is better
                'bottleneck_ratio': step_stats['max_step_time'] / step_stats['avg_step_time'] if step_stats['avg_step_time'] > 0 else 1.0,
            }
        else:
            efficiency_stats = {
                'efficiency_score': 0.0,
                'bottleneck_ratio': 1.0,
            }
        
        # Memory efficiency
        if memory_stats['initial_memory'] > 0:
            memory_efficiency = memory_stats['memory_change'] / memory_stats['initial_memory']
        else:
            memory_efficiency = 0.0
        
        memory_efficiency_stats = {
            'memory_efficiency': memory_efficiency,
            'memory_efficiency_score': max(0, min(1, 1.0 - abs(memory_efficiency))),
        }
        
        # Combine all statistics
        self._current_stats = {
            'monitoring': {
                'start_time': self.start_time,
                'end_time': self.end_time,
                'total_time': total_time,
                'monitoring_active': (self.start_time is not None and self.end_time is None),
            },
            'steps': step_stats,
            'memory': memory_stats,
            'cpu': cpu_stats,
            'efficiency': efficiency_stats,
            'memory_efficiency': memory_efficiency_stats,
        }
        
        self._stats_computed = True
    
    def _trim_data(self) -> None:
        """Trim data to maximum sample size."""
        # Keep recent data
        keep_count = self.max_samples // 2
        
        self.step_times = self.step_times[-keep_count:]
        self.memory_usage = self.memory_usage[-keep_count:]
        self.cpu_usage = self.cpu_usage[-keep_count:]
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        try:
            memory_info = self.process.memory_info()
            return memory_info.rss / (1024 * 1024)  # Convert to MB
        except:
            return 0.0
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage."""
        try:
            return self.process.cpu_percent() / psutil.cpu_count()
        except:
            return 0.0
    
    def get_detailed_report(self) -> str:
        """
        Generate a detailed performance report.
        
        Returns:
            Formatted report string
        """
        if not self._stats_computed:
            self._compute_statistics()
        
        report_lines = [
            "NLM Performance Monitor Report",
            "=" * 50,
        ]
        
        if self.start_time:
            report_lines.append(f"Start Time: {time.ctime(self.start_time)}")
        if self.end_time:
            report_lines.append(f"End Time: {time.ctime(self.end_time)}")
        
        if self.start_time and self.end_time:
            total_time = self.end_time - self.start_time
            report_lines.append(f"Total Duration: {total_time:.2f} seconds")
        
        report_lines.append("")
        
        # Steps section
        if 'steps' in self._current_stats:
            steps = self._current_stats['steps']
            report_lines.append("Simulation Steps:")
            report_lines.append(f"  Total Steps: {steps['steps']}")
            report_lines.append(f"  Average Step Time: {steps['avg_step_time']*1000:.2f} ms")
            report_lines.append(f"  Min Step Time: {steps['min_step_time']*1000:.2f} ms")
            report_lines.append(f"  Max Step Time: {steps['max_step_time']*1000:.2f} ms")
            report_lines.append(f"  Step Time Standard Deviation: {steps['std_step_time']*1000:.2f} ms")
            report_lines.append(f"  Steps per Second: {steps['steps_per_second']:.2f}")
            report_lines.append("")
        
        # Memory section
        if 'memory' in self._current_stats:
            memory = self._current_stats['memory']
            report_lines.append("Memory Usage:")
            report_lines.append(f"  Initial Memory: {memory['initial_memory']:.1f} MB")
            report_lines.append(f"  Final Memory: {memory['final_memory']:.1f} MB")
            report_lines.append(f"  Memory Change: {memory['memory_change']:.1f} MB")
            report_lines.append(f"  Average Memory: {memory['avg_memory']:.1f} MB")
            report_lines.append(f"  Memory Standard Deviation: {memory['memory_std']:.1f} MB")
            report_lines.append("")
        
        # CPU section
        if 'cpu' in self._current_stats:
            cpu = self._current_stats['cpu']
            report_lines.append("CPU Usage:")
            report_lines.append(f"  Initial CPU: {cpu['initial_cpu']:.1f} cores")
            report_lines.append(f"  Final CPU: {cpu['final_cpu']:.1f} cores")
            report_lines.append(f"  Average CPU: {cpu['avg_cpu']:.1f} cores")
            report_lines.append(f"  CPU Standard Deviation: {cpu['cpu_std']:.1f} cores")
            report_lines.append("")
        
        # Efficiency section
        if 'efficiency' in self._current_stats:
            efficiency = self._current_stats['efficiency']
            report_lines.append("Performance Efficiency:")
            report_lines.append(f"  Efficiency Score: {efficiency['efficiency_score']:.2f}")
            report_lines.append(f"  Bottleneck Ratio: {efficiency['bottleneck_ratio']:.2f}")
            report_lines.append("")
        
        # Memory efficiency section
        if 'memory_efficiency' in self._current_stats:
            mem_eff = self._current_stats['memory_efficiency']
            report_lines.append("Memory Efficiency:")
            report_lines.append(f"  Memory Efficiency: {mem_eff['memory_efficiency']:.3f}")
            report_lines.append(f"  Memory Efficiency Score: {mem_eff['memory_efficiency_score']:.2f}")
            report_lines.append("")
        
        report_lines.append("=" * 50)
        
        return "\n".join(report_lines)


class BenchmarkHelper:
    """
    Helper class for benchmarking NLM performance.
    
    Provides static methods for common benchmarking scenarios.
    """
    
    @staticmethod
    def benchmark_brain_creation(num_brains: int = 10, **config_params) -> Dict[str, Any]:
        """
        Benchmark brain creation performance.
        
        Args:
            num_brains: Number of brains to create
            **config_params: Configuration parameters for brain creation
            
        Returns:
            Dictionary containing benchmark results
        """
        import pynlm
        
        config = pynlm.BrainConfig.with_parameters(**config_params)
        
        start_time = time.time()
        brains = []
        
        for _ in range(num_brains):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        creation_time = time.time() - start_time
        
        # Cleanup
        for brain in brains:
            brain.reset()
        
        return {
            'creation_time': creation_time,
            'brains_per_second': num_brains / creation_time,
            'avg_creation_time': creation_time / num_brains,
        }
    
    @staticmethod
    def benchmark_simulation(brain: 'Brain', world: 'SimpleWorld', num_steps: int) -> Dict[str, Any]:
        """
        Benchmark simulation performance.
        
        Args:
            brain: Brain instance
            world: World instance
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary containing benchmark results
        """
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(num_steps):
            world.update(0.1)
            brain.step(step)
        
        simulation_time = time.time() - start_time
        
        return {
            'simulation_time': simulation_time,
            'steps_per_second': num_steps / simulation_time,
            'avg_step_time': simulation_time / num_steps,
        }
    
    @staticmethod
    def benchmark_experiment(config: 'Config', world: 'SimpleWorld', 
                           num_steps: int, **enable_subsystems) -> Dict[str, Any]:
        """
        Benchmark complete agent experiment.
        
        Args:
            config: Configuration for the brain
            world: World instance
            num_steps: Number of simulation steps
            **enable_subsystems: Which subsystems to enable
            
        Returns:
            Dictionary containing benchmark results
        """
        import pynlm
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        brain.initialize()
        agent.initialize(world)
        
        # Enable subsystems
        for subsystem, enabled in enable_subsystems.items():
            if enabled:
                method_name = f"enable_{subsystem}"
                if hasattr(agent, method_name):
                    getattr(agent, method_name)(True)
        
        # Run experiment
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and apply it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward if learning is enabled
            if enable_subsystems.get('reward', False):
                reward = world.getAgentBody().energy / 100.0
                agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if enable_subsystems.get('development', False):
                agent.updateDevelopment(0.1)
        
        end_time = time.time()
        
        return {
            'simulation_time': end_time - start_time,
            'steps_per_second': num_steps / (end_time - start_time),
            'avg_step_time': (end_time - start_time) / num_steps,
        }
    
    @staticmethod
    def profile_memory_growth(brain: 'Brain', world: 'SimpleWorld', 
                            num_steps: int, steps_to_record: int = 10) -> List[float]:
        """
        Profile memory growth over simulation.
        
        Args:
            brain: Brain instance
            world: World instance
            num_steps: Number of simulation steps to run
            steps_to_record: Number of steps to record memory for
            
        Returns:
            List of memory usage values at recorded intervals
        """
        brain.initialize()
        
        memory_samples = []
        record_interval = max(1, num_steps // steps_to_record)
        
        for step in range(num_steps):
            # Record memory at intervals
            if step % record_interval == 0 or step == num_steps - 1:
                # Get memory usage (approximate)
                try:
                    process = psutil.Process()
                    memory_mb = process.memory_info().rss / (1024 * 1024)
                    memory_samples.append(memory_mb)
                except:
                    memory_samples.append(0.0)
            
            # Run step
            world.update(0.1)
            brain.step(step)
        
        return memory_samples
    
    @staticmethod
    def find_optimal_parameters(param_ranges: Dict[str, List[float]], 
                               config_template: 'Config', 
                               world: 'SimpleWorld', 
                               num_steps: int, 
                               objective_function: callable = None) -> Dict[str, Any]:
        """
        Find optimal parameters for NLM simulation using grid search.
        
        Args:
            param_ranges: Dictionary of parameter ranges to test
            config_template: Base configuration
            world: World instance
            num_steps: Number of simulation steps per test
            objective_function: Function that takes metrics and returns score
            
        Returns:
            Dictionary containing best parameters and performance
        """
        import pynlm
        
        if objective_function is None:
            # Default objective: maximize average firing rate while maintaining reasonable energy
            def objective_function(metrics):
                firing_rate = metrics.get('average_firing_rate', 0)
                energy = metrics.get('agent_energy', 0)
                return firing_rate * (1.0 - min(energy / 100.0, 1.0))
        
        best_result = None
        best_score = float('-inf')
        
        # Generate all parameter combinations
        param_combinations = BenchmarkHelper._generate_param_combinations(param_ranges)
        
        print(f"Testing {len(param_combinations)} parameter combinations...")
        
        for i, params in enumerate(param_combinations):
            try:
                # Create configuration with parameters
                config = pynlm.BrainConfig.with_parameters(**params, **config_template)
                
                # Create brain and agent
                brain = pynlm.createBrain(config)
                agent = pynlm.createAgentBrain(brain)
                
                brain.initialize()
                agent.initialize(world)
                
                # Enable subsystems based on parameters
                if params.get('enable_learning', False):
                    agent.enableRewardModulation(True)
                if params.get('enable_development', False):
                    agent.enableDevelopment(True)
                if params.get('enable_curiosity', False):
                    agent.enableCuriosity(True)
                
                # Run simulation
                for step in range(num_steps):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Collect metrics
                metrics = pynlm.SimulationMetrics.collect(brain, world, agent)
                
                # Calculate score
                score = objective_function(metrics)
                
                if score > best_score:
                    best_score = score
                    best_result = {
                        'parameters': params,
                        'metrics': metrics,
                        'score': score,
                    }
                
                # Print progress
                if (i + 1) % 10 == 0 or i == len(param_combinations) - 1:
                    print(f"Completed {i + 1}/{len(param_combinations)} tests, "
                          f"best score: {best_score:.3f}")
                
            except Exception as e:
                print(f"Test failed with parameters {params}: {e}")
                continue
        
        return best_result if best_result else {
            'parameters': {},
            'metrics': {},
            'score': float('-inf'),
            'error': 'No valid parameter combinations found'
        }
    
    @staticmethod
    def _generate_param_combinations(param_ranges: Dict[str, List[float]]) -> List[Dict[str, Any]]:
        """Generate all combinations of parameter values."""
        if not param_ranges:
            return [{}]
        
        # Get parameter names and values
        param_names = list(param_ranges.keys())
        param_values = list(param_ranges.values())
        
        # Generate combinations using recursion
        combinations = []
        
        def generate_combinations(current_combination, index):
            if index == len(param_names):
                combinations.append(current_combination.copy())
                return
            
            for value in param_values[index]:
                current_combination[param_names[index]] = value
                generate_combinations(current_combination, index + 1)
        
        generate_combinations({}, 0)
        return combinations


# Create module-level exports
__all__ = ['PerformanceMonitor', 'BenchmarkHelper']

# Set up psutil if available
try:
    import psutil
    _PSUTIL_AVAILABLE = True
except ImportError:
    _PSUTIL_AVAILABLE = False
    print("Warning: psutil not available. Performance monitoring will be limited.")