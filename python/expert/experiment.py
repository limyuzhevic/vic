"""Experiment Control APIs for Expert Users"""

from typing import Dict, List, Optional, Callable, Any, Union, Tuple
import time
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from copy import deepcopy

# Optional numpy import
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

# Mock numpy functions for environments without numpy
if not NUMPY_AVAILABLE:
    class MockNumpy:
        def mean(self, data):
            return sum(data) / len(data) if data else 0
        
        def std(self, data):
            if not data or len(data) < 2:
                return 0.0
            mean = sum(data) / len(data)
            variance = sum((x - mean) ** 2 for x in data) / len(data)
            return variance ** 0.5
        
        def median(self, data):
            sorted_data = sorted(data)
            n = len(sorted_data)
            if n % 2 == 1:
                return sorted_data[n // 2]
            return (sorted_data[n // 2 - 1] + sorted_data[n // 2]) / 2
        
        def max(self, data):
            return max(data) if data else 0
        
        def min(self, data):
            return min(data) if data else 0
        
        def sum(self, data):
            return sum(data)
        
        def corrcoef(self, x, y):
            if not x or not y or len(x) != len(y):
                return [[1.0]]
            return [[1.0]]
    
    np = MockNumpy()

# For compatibility with mock numpy
if 'np' in globals() and NUMPY_AVAILABLE:
    pass
else:
    # Create a minimal numpy interface
    class MinimalNumpy:
        @staticmethod
        def mean(data):
            return sum(data) / len(data) if data else 0
        
        @staticmethod
        def std(data):
            if not data or len(data) < 2:
                return 0.0
            mean = sum(data) / len(data)
            variance = sum((x - mean) ** 2 for x in data) / len(data)
            return variance ** 0.5
        
        @staticmethod
        def median(data):
            sorted_data = sorted(data)
            n = len(sorted_data)
            if n % 2 == 1:
                return sorted_data[n // 2]
            return (sorted_data[n // 2 - 1] + sorted_data[n // 2]) / 2
        
        @staticmethod
        def max(data):
            return max(data) if data else 0
        
        @staticmethod
        def min(data):
            return min(data) if data else 0
        
        @staticmethod
        def sum(data):
            return sum(data)
        
        @staticmethod
        def abs(x):
            return abs(x)
        
        @staticmethod
        def sqrt(x):
            return x ** 0.5
        
        @staticmethod
        def percentile(data, p):
            sorted_data = sorted(data)
            n = len(sorted_data)
            if n == 0:
                return 0
            index = (p / 100) * (n - 1)
            if index == int(index):
                return sorted_data[int(index)]
            return sorted_data[int(index)] + (sorted_data[int(index) + 1] - sorted_data[int(index)]) * (index - int(index))
    
    np = MinimalNumpy()

class CustomExperimentLoop:
    """Custom experiment loops for expert control"""
    
    def __init__(self, duration: float = 100.0, time_step: float = 0.001,
                 loop_functions: Optional[List[Callable]] = None):
        self.duration = duration
        self.time_step = time_step
        self.loop_functions = loop_functions or []
        self.iterations = int(duration / time_step)
        self.current_iteration = 0
        self.experiment_start_time = 0.0
        self.experiment_state = {}
        self.loop_history = []
    
    def add_loop_function(self, function: Callable, name: str = "") -> None:
        """Add function to experiment loop"""
        self.loop_functions.append({
            'function': function,
            'name': name,
            'order': len(self.loop_functions)
        })
    
    def run(self, initial_state: Optional[Dict] = None) -> Dict:
        """Run experiment loop"""
        self.experiment_start_time = time.time()
        self.experiment_state = initial_state or {}
        self.current_iteration = 0
        
        loop_results = {
            'iterations': [],
            'total_time': 0.0,
            'start_time': self.experiment_start_time,
            'end_time': 0.0,
            'final_state': {},
            'performance_stats': {}
        }
        
        for iteration in range(self.iterations):
            iteration_start = time.time()
            iteration_data = {'iteration': iteration, 'timestamp': iteration_start}
            
            # Execute loop functions
            for loop_func in self.loop_functions:
                func_start = time.time()
                try:
                    result = loop_func['function'](self.experiment_state, iteration, self.time_step)
                    iteration_data[f"function_{loop_func['name']}"] = result
                except Exception as e:
                    iteration_data[f"error_{loop_func['name']}"] = str(e)
                
                func_time = time.time() - func_start
                iteration_data[f"function_{loop_func['name']}_time"] = func_time
            
            iteration_time = time.time() - iteration_start
            iteration_data['iteration_time'] = iteration_time
            
            loop_results['iterations'].append(iteration_data)
            self.current_iteration = iteration + 1
        
        loop_results['end_time'] = time.time()
        loop_results['total_time'] = loop_results['end_time'] - loop_results['start_time']
        loop_results['final_state'] = self.experiment_state
        loop_results['performance_stats'] = self._calculate_performance_stats(loop_results['iterations'])
        
        return loop_results
    
    def _calculate_performance_stats(self, iterations: List[Dict]) -> Dict:
        """Calculate performance statistics"""
        if not iterations:
            return {}
        
        iteration_times = [i.get('iteration_time', 0) for i in iterations]
        
        return {
            'total_iterations': len(iterations),
            'avg_iteration_time': sum(iteration_times) / len(iteration_times) if iteration_times else 0,
            'min_iteration_time': min(iteration_times) if iteration_times else 0,
            'max_iteration_time': max(iteration_times) if iteration_times else 0,
            'total_time': sum(iteration_times),
            'iterations_per_second': len(iterations) / sum(iteration_times) if sum(iteration_times) > 0 else 0
        }
    
    def pause(self) -> None:
        """Pause experiment loop"""
        self.experiment_state['paused'] = True
        self.experiment_state['pause_time'] = time.time()
    
    def resume(self) -> None:
        """Resume experiment loop"""
        if self.experiment_state.get('paused'):
            self.experiment_state['paused'] = False
            self.experiment_state['resume_time'] = time.time()
    
    def stop(self) -> None:
        """Stop experiment loop"""
        self.experiment_state['stopped'] = True
        self.experiment_state['stop_time'] = time.time()

class ParallelExperimentRunner:
    """Parallel experiment execution for expert control"""
    
    def __init__(self, max_workers: int = 4, execution_mode: str = "threads"):
        self.max_workers = max_workers
        self.execution_mode = execution_mode  # "threads" or "processes"
        self.executor = None
        self.active_experiments = {}
        self.experiment_results = {}
        self.execution_stats = {
            'total_experiments': 0,
            'completed_experiments': 0,
            'failed_experiments': 0,
            'average_execution_time': 0.0
        }
    
    def add_experiment(self, experiment_id: str, experiment_func: Callable, 
                      params: Optional[Dict] = None) -> None:
        """Add experiment to runner"""
        self.active_experiments[experiment_id] = {
            'function': experiment_func,
            'params': params or {},
            'status': 'pending',
            'submitted_time': time.time(),
            'start_time': None,
            'end_time': None
        }
    
    def run_experiments(self, experiment_ids: Optional[List[str]] = None) -> Dict:
        """Run experiments in parallel"""
        if self.execution_mode == "threads":
            self.executor = ThreadPoolExecutor(max_workers=self.max_workers)
        else:
            self.executor = ProcessPoolExecutor(max_workers=self.max_workers)
        
        target_ids = experiment_ids or list(self.active_experiments.keys())
        
        futures = {}
        for exp_id in target_ids:
            if exp_id in self.active_experiments:
                exp = self.active_experiments[exp_id]
                future = self.executor.submit(self._run_single_experiment, exp_id, exp)
                futures[exp_id] = future
        
        results = {}
        for exp_id, future in futures.items():
            try:
                result = future.result(timeout=300.0)  # 5 minute timeout
                results[exp_id] = result
                self.execution_stats['completed_experiments'] += 1
            except Exception as e:
                results[exp_id] = {'error': str(e), 'status': 'failed'}
                self.execution_stats['failed_experiments'] += 1
        
        self.experiment_results.update(results)
        self.execution_stats['total_experiments'] += len(target_ids)
        
        # Calculate average execution time
        if self.execution_stats['completed_experiments'] > 0:
            total_time = sum(r.get('execution_time', 0) for r in results.values() 
                           if 'execution_time' in r and 'error' not in r)
            self.execution_stats['average_execution_time'] = total_time / self.execution_stats['completed_experiments']
        
        return results
    
    def _run_single_experiment(self, experiment_id: str, experiment_config: Dict) -> Dict:
        """Run single experiment"""
        exp = experiment_config.copy()
        exp['status'] = 'running'
        exp['start_time'] = time.time()
        
        try:
            result = exp['function'](**exp['params'])
            execution_time = time.time() - exp['start_time']
            
            return {
                'status': 'completed',
                'result': result,
                'execution_time': execution_time,
                'start_time': exp['start_time'],
                'end_time': time.time()
            }
        except Exception as e:
            execution_time = time.time() - exp['start_time']
            return {
                'status': 'failed',
                'error': str(e),
                'execution_time': execution_time,
                'start_time': exp['start_time'],
                'end_time': time.time()
            }
    
    def get_experiment_status(self, experiment_id: str) -> Dict:
        """Get experiment status"""
        return self.active_experiments.get(experiment_id, {})
    
    def get_execution_stats(self) -> Dict:
        """Get execution statistics"""
        return self.execution_stats
    
    def wait_for_all(self, timeout: float = 300.0) -> Dict:
        """Wait for all experiments to complete"""
        if self.executor:
            self.executor.shutdown(wait=True)
        
        return {
            'status': 'completed',
            'stats': self.execution_stats,
            'results': self.experiment_results
        }

class CheckpointManager:
    """Experiment checkpointing for expert control"""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints", auto_checkpoint: bool = True,
                 checkpoint_interval: float = 60.0):
        self.checkpoint_dir = checkpoint_dir
        self.auto_checkpoint = auto_checkpoint
        self.checkpoint_interval = checkpoint_interval
        self.checkpoints = {}
        self.checkpoint_history = []
        self.auto_checkpoint_timestamps = {}
    
    def create_checkpoint(self, checkpoint_name: str, experiment_state: Dict, 
                         additional_data: Optional[Dict] = None) -> str:
        """Create experiment checkpoint"""
        if not checkpoint_name:
            checkpoint_name = f"checkpoint_{len(self.checkpoints)}_{int(time.time())}"
        
        checkpoint = {
            'name': checkpoint_name,
            'state': deepcopy(experiment_state),
            'additional_data': additional_data or {},
            'created_at': time.time(),
            'state_size': len(str(experiment_state)),
            'additional_data_size': len(str(additional_data) if additional_data else 0)
        }
        
        self.checkpoints[checkpoint_name] = checkpoint
        self.checkpoint_history.append(checkpoint)
        
        # Update auto-checkpoint timestamp
        self.auto_checkpoint_timestamps[checkpoint_name] = time.time()
        
        return checkpoint_name
    
    def restore_checkpoint(self, checkpoint_name: str, target_state: Dict) -> Dict:
        """Restore experiment state from checkpoint"""
        checkpoint = self.get_checkpoint(checkpoint_name)
        if not checkpoint:
            raise ValueError(f"Checkpoint {checkpoint_name} not found")
        
        # Restore state
        restored_state = deepcopy(checkpoint['state'])
        restored_state.update(target_state)
        
        # Merge additional data if provided
        if 'additional_data' in checkpoint:
            restored_state['checkpoint_data'] = checkpoint['additional_data']
        
        return restored_state
    
    def get_checkpoint(self, checkpoint_name: str) -> Optional[Dict]:
        """Get checkpoint by name"""
        return self.checkpoints.get(checkpoint_name)
    
    def delete_checkpoint(self, checkpoint_name: str) -> bool:
        """Delete checkpoint"""
        if checkpoint_name in self.checkpoints:
            del self.checkpoints[checkpoint_name]
            return True
        return False
    
    def cleanup_old_checkpoints(self, max_age: float = 3600.0) -> List[str]:
        """Clean up old checkpoints"""
        current_time = time.time()
        to_delete = []
        
        for name, checkpoint in list(self.checkpoints.items()):
            if current_time - checkpoint['created_at'] > max_age:
                to_delete.append(name)
        
        for name in to_delete:
            del self.checkpoints[name]
        
        return to_delete
    
    def get_checkpoint_info(self, checkpoint_name: str) -> Dict:
        """Get checkpoint information"""
        checkpoint = self.get_checkpoint(checkpoint_name)
        if not checkpoint:
            return {}
        
        return {
            'name': checkpoint['name'],
            'created_at': checkpoint['created_at'],
            'state_size': checkpoint['state_size'],
            'additional_data_size': checkpoint['additional_data_size'],
            'age': time.time() - checkpoint['created_at']
        }

class ResultAnalyzer:
    """Statistical analysis of experiment results for expert control"""
    
    def __init__(self, analysis_methods: Optional[List[str]] = None):
        self.analysis_methods = analysis_methods or ['mean', 'std', 'median', 'percentile', 'trend']
        self.analysis_history = []
    
    def analyze_results(self, results: List[Dict], analysis_type: str = "statistical") -> Dict:
        """Analyze experiment results"""
        analysis_result = {
            'analysis_type': analysis_type,
            'result_count': len(results),
            'analysis_timestamp': time.time(),
            'summary': {},
            'detailed_analysis': {}
        }
        
        if analysis_type == "statistical":
            analysis_result['detailed_analysis'] = self._statistical_analysis(results)
        
        elif analysis_type == "performance":
            analysis_result['detailed_analysis'] = self._performance_analysis(results)
        
        elif analysis_type == "convergence":
            analysis_result['detailed_analysis'] = self._convergence_analysis(results)
        
        elif analysis_type == "comparative":
            analysis_result['detailed_analysis'] = self._comparative_analysis(results)
        
        # Generate summary
        analysis_result['summary'] = self._generate_summary(analysis_result['detailed_analysis'])
        
        self.analysis_history.append({
            'results_count': len(results),
            'analysis_type': analysis_type,
            'timestamp': time.time()
        })
        
        return analysis_result
    
    def _statistical_analysis(self, results: List[Dict]) -> Dict:
        """Perform statistical analysis"""
        # Extract numeric values from results
        numeric_values = []
        for result in results:
            if isinstance(result, dict):
                for key, value in result.items():
                    if isinstance(value, (int, float)):
                        numeric_values.append(value)
            elif isinstance(result, (int, float)):
                numeric_values.append(result)
        
        if not numeric_values:
            return {'error': 'No numeric values found in results'}
        
        return {
            'mean': np.mean(numeric_values) if 'np' in globals() else sum(numeric_values) / len(numeric_values),
            'median': np.median(numeric_values) if 'np' in globals() else sorted(numeric_values)[len(numeric_values) // 2],
            'std': np.std(numeric_values) if 'np' in globals() else (sum((x - np.mean(numeric_values))**2 for x in numeric_values) / len(numeric_values))**0.5,
            'min': min(numeric_values),
            'max': max(numeric_values),
            'range': max(numeric_values) - min(numeric_values),
            'percentiles': {
                '25': np.percentile(numeric_values, 25) if 'np' in globals() else sorted(numeric_values)[len(numeric_values) // 4],
                '50': np.percentile(numeric_values, 50) if 'np' in globals() else sorted(numeric_values)[len(numeric_values) // 2],
                '75': np.percentile(numeric_values, 75) if 'np' in globals() else sorted(numeric_values)[3 * len(numeric_values) // 4],
                '95': np.percentile(numeric_values, 95) if 'np' in globals() else sorted(numeric_values)[-5]
            }
        }
    
    def _performance_analysis(self, results: List[Dict]) -> Dict:
        """Perform performance analysis"""
        performance_metrics = {}
        
        for result in results:
            if isinstance(result, dict):
                for key, value in result.items():
                    if 'time' in key.lower() or 'duration' in key.lower():
                        if isinstance(value, (int, float)):
                            if 'execution_time' not in performance_metrics:
                                performance_metrics['execution_time'] = []
                            performance_metrics['execution_time'].append(value)
        
        return {
            'performance_metrics': performance_metrics,
            'throughput': len(results) / sum(performance_metrics.get('execution_time', [1])) if performance_metrics.get('execution_time') else 0,
            'efficiency': self._calculate_efficiency(results)
        }
    
    def _convergence_analysis(self, results: List[Dict]) -> Dict:
        """Analyze convergence"""
        convergence_metrics = {}
        
        for result in results:
            if isinstance(result, dict):
                for key, value in result.items():
                    if isinstance(value, (int, float)):
                        if key not in convergence_metrics:
                            convergence_metrics[key] = []
                        convergence_metrics[key].append(value)
        
        convergence_analysis = {}
        for metric, values in convergence_metrics.items():
            if len(values) > 10:
                # Calculate convergence rate
                half_point = len(values) // 2
                first_half = values[:half_point]
                second_half = values[half_point:]
                
                first_half_mean = sum(first_half) / len(first_half)
                second_half_mean = sum(second_half) / len(second_half)
                
                convergence_rate = (first_half_mean - second_half_mean) / first_half_mean if first_half_mean != 0 else 0
                
                convergence_analysis[metric] = {
                    'convergence_rate': convergence_rate,
                    'stabilized': abs(convergence_rate) < 0.01,
                    'trend': 'decreasing' if convergence_rate > 0 else 'increasing'
                }
        
        return convergence_analysis
    
    def _comparative_analysis(self, results: List[Dict]) -> Dict:
        """Perform comparative analysis"""
        # Simple comparative analysis between different runs
        if len(results) < 2:
            return {'error': 'Need at least 2 results for comparative analysis'}
        
        comparison = {}
        
        for i, result in enumerate(results):
            if isinstance(result, dict):
                for key, value in result.items():
                    if isinstance(value, (int, float)):
                        if key not in comparison:
                            comparison[key] = []
                        comparison[key].append((i, value))
        
        comparative_results = {}
        for metric, values in comparison.items():
            if len(values) >= 2:
                values_sorted = sorted(values, key=lambda x: x[0])
                comparative_results[metric] = {
                    'values': [v for _, v in values_sorted],
                    'run_indices': [i for i, _ in values_sorted],
                    'mean': sum(v for _, v in values_sorted) / len(values_sorted),
                    'coefficient_of_variation': (
                        sum((v - sum(v)/len(v))**2 for _, v in values_sorted) / len(v) for _, v in values_sorted
                    )**0.5 if len(v) > 1 else 0
                }
        
        return comparative_results
    
    def _generate_summary(self, detailed_analysis: Dict) -> Dict:
        """Generate analysis summary"""
        summary = {
            'total_analyses': 1,
            'key_findings': [],
            'recommendations': []
        }
        
        # Generate findings based on analysis results
        if 'mean' in detailed_analysis:
            summary['key_findings'].append(f"Mean value: {detailed_analysis['mean']:.4f}")
        
        if 'std' in detailed_analysis:
            summary['key_findings'].append(f"Standard deviation: {detailed_analysis['std']:.4f}")
        
        if 'convergence_rate' in detailed_analysis:
            for metric, metrics in detailed_analysis.items():
                if 'convergence_rate' in metrics:
                    summary['key_findings'].append(
                        f"{metric} convergence rate: {metrics['convergence_rate']:.4f}"
                    )
        
        return summary
    
    def _calculate_efficiency(self, results: List[Dict]) -> float:
        """Calculate efficiency metric"""
        efficiency_scores = []
        
        for result in results:
            if isinstance(result, dict):
                score = 1.0
                # Penalize long execution times
                for key, value in result.items():
                    if 'time' in key.lower() and isinstance(value, (int, float)):
                        score /= (1 + value / 1000.0)  # Normalize to 1000s
                
                efficiency_scores.append(score)
        
        return sum(efficiency_scores) / len(efficiency_scores) if efficiency_scores else 0

class ExperimentMonitor:
    """Monitor experiment execution for expert control"""
    
    def __init__(self, monitoring_interval: float = 1.0):
        self.monitoring_interval = monitoring_interval
        self.monitoring_data = {}
        self.alert_rules = []
        self.alert_history = []
    
    def start_monitoring(self, experiment_id: str, monitoring_func: Callable) -> None:
        """Start monitoring experiment"""
        self.monitoring_data[experiment_id] = {
            'function': monitoring_func,
            'start_time': time.time(),
            'last_check': time.time(),
            'metrics': {},
            'alerts': []
        }
    
    def stop_monitoring(self, experiment_id: str) -> Dict:
        """Stop monitoring experiment"""
        if experiment_id not in self.monitoring_data:
            return {'error': 'Experiment not being monitored'}
        
        monitoring_data = self.monitoring_data.pop(experiment_id)
        return {
            'experiment_id': experiment_id,
            'monitoring_duration': time.time() - monitoring_data['start_time'],
            'final_metrics': monitoring_data['metrics'],
            'alerts_triggered': monitoring_data['alerts']
        }
    
    def add_alert_rule(self, rule_name: str, condition: Callable, message: str = "") -> None:
        """Add alert rule"""
        self.alert_rules.append({
            'name': rule_name,
            'condition': condition,
            'message': message
        })
    
    def check_alerts(self, experiment_id: str) -> List[Dict]:
        """Check alerts for experiment"""
        if experiment_id not in self.monitoring_data:
            return []
        
        experiment_data = self.monitoring_data[experiment_id]
        triggered_alerts = []
        
        for rule in self.alert_rules:
            try:
                if rule['condition'](experiment_data['metrics']):
                    alert = {
                        'rule_name': rule['name'],
                        'message': rule['message'],
                        'timestamp': time.time(),
                        'metrics': experiment_data['metrics']
                    }
                    triggered_alerts.append(alert)
                    experiment_data['alerts'].append(alert)
            except Exception as e:
                # Log error but don't break monitoring
                pass
        
        return triggered_alerts

# Expert experiment exports
__all__ = [
    'CustomExperimentLoop', 'ParallelExperimentRunner',
    'CheckpointManager', 'ResultAnalyzer', 'ExperimentMonitor'
]
