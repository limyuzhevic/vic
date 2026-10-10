"""Expert utility functions for NLM"""

from typing import Dict, List, Optional, Callable, Any, Union, Tuple
import time
import threading
import json
from pathlib import Path

class MemoryProfiler:
    """Memory profiler for expert users"""
    
    def __init__(self):
        self.memory_samples = []
        self.profiling_active = False
        self.sampling_rate = 100  # Hz
    
    def start_profiling(self, interval: float = 0.1) -> None:
        """Start memory profiling"""
        self.profiling_active = True
        self.sampling_rate = 1.0 / interval
        
        def profile_loop():
            while self.profiling_active:
                sample = self._get_memory_sample()
                self.memory_samples.append({
                    'timestamp': time.time(),
                    'sample': sample
                })
                time.sleep(1.0 / self.sampling_rate)
        
        self.profile_thread = threading.Thread(target=profile_loop, daemon=True)
        self.profile_thread.start()
    
    def stop_profiling(self) -> List[Dict]:
        """Stop memory profiling"""
        self.profiling_active = False
        return self.memory_samples.copy()
    
    def _get_memory_sample(self) -> Dict:
        """Get current memory sample"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            return {
                'rss': memory_info.rss,
                'vms': memory_info.vms,
                'shared': memory_info.shared,
                'text': memory_info.text,
                'data': memory_info.data,
                'dirty': getattr(memory_info, 'dirty', 0)
            }
        except ImportError:
            return {'error': 'psutil not installed'}
    
    def get_memory_trends(self, window: int = 100) -> Dict:
        """Get memory usage trends"""
        if not self.memory_samples:
            return {}
        
        recent_samples = self.memory_samples[-window:]
        
        rss_values = [s['sample'].get('rss', 0) for s in recent_samples if 'sample' in s and isinstance(s['sample'], dict)]
        if not rss_values:
            return {}
        
        return {
            'current_rss': rss_values[-1] if rss_values else 0,
            'peak_rss': max(rss_values) if rss_values else 0,
            'average_rss': sum(rss_values) / len(rss_values) if rss_values else 0,
            'rss_growth_rate': self._calculate_growth_rate(rss_values)
        }
    
    def _calculate_growth_rate(self, values: List[float]) -> float:
        """Calculate memory growth rate"""
        if len(values) < 2:
            return 0.0
        
        first_half = values[:len(values) // 2]
        second_half = values[len(values) // 2:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        return (avg_second - avg_first) / avg_first if avg_first > 0 else 0.0

class PerformanceProfiler:
    """Performance profiler for expert users"""
    
    def __init__(self):
        self.timings = {}
        self.profilers = {}
        self.global_stats = {
            'total_calls': 0,
            'total_time': 0.0,
            'start_time': time.time()
        }
    
    def profile_function(self, func_name: str, func: Callable, *args, **kwargs) -> Any:
        """Profile a function call"""
        start_time = time.time()
        
        try:
            result = func(*args, **kwargs)
            execution_time = time.time() - start_time
            
            if func_name not in self.timings:
                self.timings[func_name] = []
            
            self.timings[func_name].append(execution_time)
            
            if func_name not in self.profilers:
                self.profilers[func_name] = {
                    'call_count': 0,
                    'total_time': 0.0,
                    'min_time': float('inf'),
                    'max_time': 0.0
                }
            
            profiler = self.profilers[func_name]
            profiler['call_count'] += 1
            profiler['total_time'] += execution_time
            profiler['min_time'] = min(profiler['min_time'], execution_time)
            profiler['max_time'] = max(profiler['max_time'], execution_time)
            
            self.global_stats['total_calls'] += 1
            self.global_stats['total_time'] += execution_time
            
            return result
            
        except Exception as e:
            execution_time = time.time() - start_time
            if func_name not in self.timings:
                self.timings[func_name] = []
            self.timings[func_name].append(execution_time)
            raise
    
    def get_function_stats(self, func_name: str) -> Dict:
        """Get statistics for a function"""
        if func_name not in self.profilers:
            return {}
        
        profiler = self.profilers[func_name]
        timings = self.timings.get(func_name, [])
        
        return {
            'function_name': func_name,
            'call_count': profiler['call_count'],
            'total_time': profiler['total_time'],
            'average_time': profiler['total_time'] / profiler['call_count'] if profiler['call_count'] > 0 else 0,
            'min_time': profiler['min_time'],
            'max_time': profiler['max_time'],
            'standard_deviation': self._calculate_std_dev(timings) if timings else 0,
            'calls_per_second': profiler['call_count'] / (time.time() - self.global_stats['start_time'])
        }
    
    def get_global_stats(self) -> Dict:
        """Get global performance statistics"""
        uptime = time.time() - self.global_stats['start_time']
        
        return {
            'uptime': uptime,
            'total_calls': self.global_stats['total_calls'],
            'total_time': self.global_stats['total_time'],
            'average_call_time': self.global_stats['total_time'] / self.global_stats['total_calls'] if self.global_stats['total_calls'] > 0 else 0,
            'calls_per_second': self.global_stats['total_calls'] / uptime if uptime > 0 else 0,
            'profiled_functions': len(self.profilers)
        }
    
    def _calculate_std_dev(self, values: List[float]) -> float:
        """Calculate standard deviation"""
        if not values:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def clear_profiler(self, func_name: Optional[str] = None) -> None:
        """Clear profiler data"""
        if func_name:
            if func_name in self.timings:
                del self.timings[func_name]
            if func_name in self.profilers:
                del self.profilers[func_name]
        else:
            self.timings.clear()
            self.profilers.clear()

class BenchmarkRunner:
    """Benchmark runner for expert users"""
    
    def __init__(self, warmup_iterations: int = 10, benchmark_iterations: int = 100,
                 benchmark_timeout: float = 60.0):
        self.warmup_iterations = warmup_iterations
        self.benchmark_iterations = benchmark_iterations
        self.benchmark_timeout = benchmark_timeout
        self.benchmark_history = []
    
    def run_benchmark(self, benchmark_name: str, benchmark_func: Callable, 
                     *args, **kwargs) -> Dict:
        """Run benchmark for function"""
        benchmark_result = {
            'name': benchmark_name,
            'start_time': time.time(),
            'status': 'running'
        }
        
        try:
            # Warmup phase
            for i in range(self.warmup_iterations):
                if time.time() - benchmark_result['start_time'] > self.benchmark_timeout:
                    benchmark_result['status'] = 'timeout'
                    return benchmark_result
                
                try:
                    benchmark_func(*args, **kwargs)
                except Exception as e:
                    benchmark_result['status'] = 'warmup_error'
                    benchmark_result['error'] = str(e)
                    return benchmark_result
            
            # Benchmark phase
            times = []
            for i in range(self.benchmark_iterations):
                iteration_start = time.time()
                
                try:
                    result = benchmark_func(*args, **kwargs)
                    iteration_time = time.time() - iteration_start
                    times.append(iteration_time)
                    
                    if iteration_time > self.benchmark_timeout:
                        benchmark_result['status'] = 'iteration_timeout'
                        break
                        
                except Exception as e:
                    benchmark_result['status'] = 'error'
                    benchmark_result['error'] = str(e)
                    break
            
            if benchmark_result['status'] == 'running':
                benchmark_result['status'] = 'completed'
            
            # Calculate statistics
            if times:
                benchmark_result['results'] = {
                    'iterations': len(times),
                    'successful_iterations': len(times),
                    'failed_iterations': self.benchmark_iterations - len(times),
                    'mean_time': sum(times) / len(times),
                    'min_time': min(times),
                    'max_time': max(times),
                    'median_time': sorted(times)[len(times) // 2],
                    'std_deviation': self._calculate_std_dev(times),
                    'throughput': len(times) / sum(times) if sum(times) > 0 else 0
                }
            
            benchmark_result['end_time'] = time.time()
            benchmark_result['total_time'] = benchmark_result['end_time'] - benchmark_result['start_time']
            
        except Exception as e:
            benchmark_result['status'] = 'error'
            benchmark_result['error'] = str(e)
            benchmark_result['end_time'] = time.time()
        
        self.benchmark_history.append(benchmark_result)
        return benchmark_result
    
    def _calculate_std_dev(self, values: List[float]) -> float:
        """Calculate standard deviation"""
        if not values:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def get_benchmark_history(self, limit: int = 10) -> List[Dict]:
        """Get benchmark history"""
        return self.benchmark_history[-limit:]
    
    def get_benchmark_stats(self) -> Dict:
        """Get benchmark statistics"""
        completed_benchmarks = [b for b in self.benchmark_history if b['status'] == 'completed']
        
        return {
            'total_benchmarks': len(self.benchmark_history),
            'completed_benchmarks': len(completed_benchmarks),
            'failed_benchmarks': len(self.benchmark_history) - len(completed_benchmarks),
            'average_benchmark_time': (
                sum(b.get('total_time', 0) for b in completed_benchmarks) / len(completed_benchmarks)
                if completed_benchmarks else 0
            ),
            'last_benchmark': self.benchmark_history[-1] if self.benchmark_history else None
        }

class ProfilingSession:
    """Performance profiling session for expert users"""
    
    def __init__(self, session_name: str):
        self.session_name = session_name
        self.profiler = PerformanceProfiler()
        self.benchmark_runner = BenchmarkRunner()
        self.memory_profiler = MemoryProfiler()
        self.session_start_time = time.time()
        self.session_data = {
            'start_time': self.session_start_time,
            'profile_functions': {},
            'benchmarks': {},
            'memory_samples': []
        }
    
    def profile_function(self, func_name: str, func: Callable, *args, **kwargs) -> Any:
        """Profile function within session"""
        result = self.profiler.profile_function(func_name, func, *args, **kwargs)
        self.session_data['profile_functions'][func_name] = self.profiler.get_function_stats(func_name)
        return result
    
    def run_benchmark(self, benchmark_name: str, benchmark_func: Callable,
                     *args, **kwargs) -> Dict:
        """Run benchmark within session"""
        result = self.benchmark_runner.run_benchmark(benchmark_name, benchmark_func, *args, **kwargs)
        self.session_data['benchmarks'][benchmark_name] = result
        return result
    
    def start_memory_profiling(self, interval: float = 0.1) -> None:
        """Start memory profiling within session"""
        self.memory_profiler.start_profiling(interval)
    
    def stop_memory_profiling(self) -> List[Dict]:
        """Stop memory profiling within session"""
        samples = self.memory_profiler.stop_profiling()
        self.session_data['memory_samples'].extend(samples)
        return samples
    
    def get_session_report(self) -> Dict:
        """Get session report"""
        session_duration = time.time() - self.session_start_time
        
        return {
            'session_name': self.session_name,
            'start_time': self.session_start_time,
            'duration': session_duration,
            'profiling_stats': self.profiler.get_global_stats(),
            'benchmark_stats': self.benchmark_runner.get_benchmark_stats(),
            'memory_stats': self.memory_profiler.get_memory_trends(),
            'profile_functions': self.session_data['profile_functions'],
            'benchmarks': self.session_data['benchmarks']
        }
    
    def save_session(self, file_path: str) -> bool:
        """Save session data to file"""
        try:
            session_data = {
                'session_info': self.get_session_report(),
                'detailed_profile': self.profiler.timings,
                'detailed_benchmarks': self.benchmark_runner.benchmark_history,
                'detailed_memory': self.memory_profiler.memory_samples
            }
            
            with open(file_path, 'w') as f:
                json.dump(session_data, f, indent=2, default=str)
            
            return True
        except Exception as e:
            print(f"Error saving session: {e}")
            return False
    
    def load_session(self, file_path: str) -> bool:
        """Load session data from file"""
        try:
            with open(file_path, 'r') as f:
                session_data = json.load(f)
            
            # Restore profiler data
            if 'detailed_profile' in session_data:
                self.profiler.timings = session_data['detailed_profile']
            
            # Restore benchmark data
            if 'detailed_benchmarks' in session_data:
                self.benchmark_runner.benchmark_history = session_data['detailed_benchmarks']
            
            # Restore memory data
            if 'detailed_memory' in session_data:
                self.memory_profiler.memory_samples = session_data['detailed_memory']
            
            return True
        except Exception as e:
            print(f"Error loading session: {e}")
            return False

# Expert utility exports
__all__ = [
    'MemoryProfiler', 'PerformanceProfiler', 'BenchmarkRunner', 'ProfilingSession'
]
