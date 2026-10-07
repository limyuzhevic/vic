"""
Performance Profiling for NLM Python Bindings

This module provides comprehensive performance profiling tools for NLM neural simulations,
enabling detailed performance analysis, bottleneck identification, and optimization
recommendations.
"""

import time
import cProfile
import pstats
import io
import threading
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict
import numpy as np
import psutil
import json
from datetime import datetime

# Import performance monitoring classes
from .performance_optimizer import PerformanceMetrics

class ProfileEventType(Enum):
    """Types of profiling events"""
    FUNCTION_ENTRY = "function_entry"
    FUNCTION_EXIT = "function_exit"
    SIMULATION_STEP = "simulation_step"
    PERFORMANCE_ANOMALY = "performance_anomaly"
    OPTIMIZATION_SUGGESTION = "optimization_suggestion"

@dataclass
class ProfileEvent:
    """Profiling event data"""
    event_type: ProfileEventType
    timestamp: float
    function_name: str = ""
    function_args: Dict[str, Any] = field(default_factory=dict)
    step_id: int = 0
    duration_ms: float = 0.0
    data: Dict[str, Any] = field(default_factory=dict)

@dataclass
class PerformanceProfile:
    """Complete performance profile data"""
    timestamp: float
    step_id: int
    total_time_ms: float
    cpu_usage_percent: float
    memory_usage_mb: float
    function_profiles: Dict[str, Dict[str, Any]]
    bottleneck_functions: List[str]
    optimization_suggestions: List[str]
    recommendations: List[str]

class PerformanceProfiler:
    """
    Advanced performance profiler for NLM simulations.
    
    This profiler provides detailed function-level profiling, performance
    bottleneck analysis, and optimization recommendations for neural simulations.
    """
    
    def __init__(self, profile_interval: float = 0.1):
        """Initialize performance profiler"""
        self.profile_interval = profile_interval
        self.is_profiling = False
        self.profile_thread = None
        
        # Profiling data
        self.current_profile: Dict[str, Dict[str, Any]] = {}
        self.profile_history: List[PerformanceProfile] = []
        self.function_stats: Dict[str, Dict[str, Any]] = defaultdict(lambda: {
            'call_count': 0,
            'total_time': 0.0,
            'max_time': 0.0,
            'min_time': float('inf'),
            'avg_time': 0.0,
            'time_std': 0.0
        })
        
        # Performance thresholds
        self.thresholds = {
            'function_time_ms': 10.0,
            'step_time_ms': 100.0,
            'memory_usage_mb': 1024.0,
            'cpu_usage_percent': 80.0
        }
        
        # Anomaly detection
        self.anomaly_detector = AnomalyDetector()
        
        # Optimization engine
        self.optimization_engine = OptimizationEngine()
        
        # Performance metrics
        self.performance_metrics = {
            'steps_processed': 0,
            'total_time_ms': 0.0,
            'functions_profiled': 0,
            'anomalies_detected': 0,
            'optimizations_suggested': 0
        }
    
    def start_profiling(self):
        """Start performance profiling"""
        if self.is_profiling:
            return
        
        self.is_profiling = True
        self.profile_thread = threading.Thread(
            target=self._profiling_loop,
            daemon=True,
            name='performance_profiler'
        )
        self.profile_thread.start()
    
    def stop_profiling(self):
        """Stop performance profiling"""
        self.is_profiling = False
        if self.profile_thread:
            self.profile_thread.join(timeout=5.0)
    
    def _profiling_loop(self):
        """Main profiling loop"""
        last_sample_time = time.time()
        step_counter = 0
        
        while self.is_profiling:
            try:
                current_time = time.time()
                
                # Update profiling data
                self._update_profiling_data(current_time)
                
                # Check for performance anomalies
                self._check_performance_anomalies(current_time)
                
                # Generate optimization suggestions
                self._generate_optimization_suggestions(current_time)
                
                # Collect profile
                profile = self._collect_profile(current_time, step_counter)
                self.profile_history.append(profile)
                
                # Update performance metrics
                self.performance_metrics['steps_processed'] += 1
                self.performance_metrics['total_time_ms'] += profile.total_time_ms
                
                step_counter += 1
                
                # Control sampling rate
                elapsed = current_time - last_sample_time
                if elapsed < self.profile_interval:
                    time.sleep(self.profile_interval - elapsed)
                
                last_sample_time = time.time()
                
            except Exception as e:
                print(f"Profiling loop error: {e}")
                time.sleep(self.profile_interval)
    
    def _update_profiling_data(self, current_time: float):
        """Update profiling data with current metrics"""
        process = psutil.Process()
        
        # Get current system metrics
        memory_info = process.memory_info()
        cpu_percent = psutil.cpu_percent(interval=0.0)
        
        # Update function profiles
        for func_name, stats in self.function_stats.items():
            if stats['call_count'] > 0:
                stats['avg_time'] = stats['total_time'] / stats['call_count']
                stats['time_std'] = np.std([stats['total_time'] / stats['call_count']]) if stats['call_count'] > 1 else 0.0
    
    def profile_function(self, func_name: str, args: Dict[str, Any], 
                        func: Callable) -> Any:
        """Profile a function execution"""
        start_time = time.time()
        
        # Record function entry
        self._record_event(ProfileEventType.FUNCTION_ENTRY, {
            'function_name': func_name,
            'arguments': args,
            'timestamp': start_time
        })
        
        try:
            # Execute function
            result = func(*args.get('args', []), **args.get('kwargs', {}))
            
            # Record function exit
            end_time = time.time()
            duration_ms = (end_time - start_time) * 1000
            
            self._record_event(ProfileEventType.FUNCTION_EXIT, {
                'function_name': func_name,
                'duration_ms': duration_ms,
                'timestamp': end_time
            })
            
            # Update function statistics
            self._update_function_stats(func_name, duration_ms)
            
            return result
            
        except Exception as e:
            # Record error
            end_time = time.time()
            duration_ms = (end_time - start_time) * 1000
            
            self._record_event(ProfileEventType.FUNCTION_ENTRY, {
                'function_name': func_name,
                'error': str(e),
                'duration_ms': duration_ms,
                'timestamp': end_time
            })
            
            raise
    
    def _record_event(self, event_type: ProfileEventType, data: Dict[str, Any]):
        """Record profiling event"""
        event = ProfileEvent(
            event_type=event_type,
            timestamp=time.time(),
            **data
        )
        
        # Store event (simplified - would integrate with event system)
        pass
    
    def _update_function_stats(self, func_name: str, duration_ms: float):
        """Update function statistics"""
        stats = self.function_stats[func_name]
        
        stats['call_count'] += 1
        stats['total_time'] += duration_ms
        stats['max_time'] = max(stats['max_time'], duration_ms)
        stats['min_time'] = min(stats['min_time'], duration_ms)
        
        self.performance_metrics['functions_profiled'] += 1
    
    def _check_performance_anomalies(self, current_time: float):
        """Check for performance anomalies"""
        process = psutil.Process()
        memory_info = process.memory_info()
        cpu_percent = psutil.cpu_percent(interval=0.0)
        
        # Check for memory anomalies
        memory_mb = memory_info.rss / 1024 / 1024
        if memory_mb > self.thresholds['memory_usage_mb']:
            self.anomaly_detector.detect_anomaly(
                'memory_usage', memory_mb,
                self.thresholds['memory_usage_mb']
            )
        
        # Check for CPU anomalies
        if cpu_percent > self.thresholds['cpu_usage_percent']:
            self.anomaly_detector.detect_anomaly(
                'cpu_usage', cpu_percent,
                self.thresholds['cpu_usage_percent']
            )
    
    def _generate_optimization_suggestions(self, current_time: float):
        """Generate optimization suggestions based on profiling data"""
        suggestions = []
        
        # Analyze function performance
        for func_name, stats in self.function_stats.items():
            if stats['call_count'] > 0:
                avg_time_ms = stats['avg_time']
                
                if avg_time_ms > self.thresholds['function_time_ms']:
                    suggestions.append({
                        'type': 'function_optimization',
                        'function': func_name,
                        'current_time_ms': avg_time_ms,
                        'threshold_ms': self.thresholds['function_time_ms'],
                        'suggestion': f"Consider optimizing {func_name} for better performance"
                    })
        
        # Store optimization suggestions
        for suggestion in suggestions:
            self.optimization_engine.add_suggestion(suggestion)
    
    def _collect_profile(self, current_time: float, step_id: int) -> PerformanceProfile:
        """Collect current performance profile"""
        process = psutil.Process()
        memory_info = process.memory_info()
        cpu_percent = psutil.cpu_percent(interval=0.0)
        
        # Identify bottleneck functions
        bottleneck_functions = []
        for func_name, stats in self.function_stats.items():
            if stats['call_count'] > 0 and stats['avg_time'] > self.thresholds['function_time_ms']:
                bottleneck_functions.append(func_name)
        
        # Get optimization suggestions
        optimization_suggestions = self.optimization_engine.get_suggestions()
        
        # Get recommendations
        recommendations = self.optimization_engine.get_recommendations()
        
        return PerformanceProfile(
            timestamp=current_time,
            step_id=step_id,
            total_time_ms=sum(m['total_time'] for m in self.function_stats.values()) if self.function_stats else 0.0,
            cpu_usage_percent=cpu_percent,
            memory_usage_mb=memory_info.rss / 1024 / 1024,
            function_profiles={name: stats for name, stats in self.function_stats.items()},
            bottleneck_functions=bottleneck_functions,
            optimization_suggestions=optimization_suggestions,
            recommendations=recommendations
        )
    
    def get_current_profile(self) -> Dict[str, Any]:
        """Get current profiling data"""
        return {
            'function_stats': dict(self.function_stats),
            'performance_metrics': self.performance_metrics,
            'thresholds': self.thresholds
        }
    
    def get_profile_history(self, limit: Optional[int] = None) -> List[PerformanceProfile]:
        """Get profiling history"""
        if limit is None:
            return self.profile_history
        return self.profile_history[-limit:]
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report"""
        if not self.profile_history:
            return {'error': 'No profiling data available'}
        
        latest_profile = self.profile_history[-1] if self.profile_history else None
        
        report = {
            'report_timestamp': datetime.now().isoformat(),
            'performance_metrics': self.performance_metrics,
            'thresholds': self.thresholds,
            'current_profile': latest_profile.__dict__ if latest_profile else None,
            'optimization_summary': self.optimization_engine.get_summary(),
            'anomaly_summary': self.anomaly_detector.get_summary()
        }
        
        return report
    
    def export_profile(self, filepath: str):
        """Export profiling data to file"""
        export_data = {
            'export_timestamp': datetime.now().isoformat(),
            'profile_interval': self.profile_interval,
            'thresholds': self.thresholds,
            'performance_metrics': self.performance_metrics,
            'profile_history': [p.__dict__ for p in self.profile_history],
            'optimization_suggestions': self.optimization_engine.get_all_suggestions(),
            'anomaly_reports': self.anomaly_detector.get_all_reports()
        }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2)

class AnomalyDetector:
    """Detect performance anomalies and outliers"""
    
    def __init__(self):
        self.anomalies = []
        self.baseline_metrics = {}
        self.monitoring_window = 100
    
    def detect_anomaly(self, metric_name: str, current_value: float, threshold: float):
        """Detect performance anomaly"""
        anomaly = {
            'metric': metric_name,
            'current_value': current_value,
            'threshold': threshold,
            'timestamp': time.time(),
            'severity': 'high' if current_value > threshold * 1.5 else 'medium'
        }
        
        self.anomalies.append(anomaly)
        
        # Keep only recent anomalies
        if len(self.anomalies) > self.monitoring_window:
            self.anomalies = self.anomalies[-self.monitoring_window:]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get anomaly summary"""
        return {
            'total_anomalies': len(self.anomalies),
            'recent_anomalies': self.anomalies[-10:] if self.anomalies else [],
            'metric_breakdown': self._get_metric_breakdown()
        }
    
    def _get_metric_breakdown(self) -> Dict[str, int]:
        """Get anomaly count by metric"""
        breakdown = {}
        for anomaly in self.anomalies:
            metric = anomaly['metric']
            breakdown[metric] = breakdown.get(metric, 0) + 1
        return breakdown

class OptimizationEngine:
    """Generate optimization suggestions and recommendations"""
    
    def __init__(self):
        self.suggestions = []
        self.recommendations = []
        self.optimization_history = []
    
    def add_suggestion(self, suggestion: Dict[str, Any]):
        """Add optimization suggestion"""
        suggestion['timestamp'] = time.time()
        self.suggestions.append(suggestion)
        
        # Check if this is a new type of optimization
        if suggestion not in self.optimization_history:
            self.optimization_history.append(suggestion)
            self._generate_recommendation(suggestion)
    
    def get_suggestions(self) -> List[str]:
        """Get optimization suggestions as formatted strings"""
        return [s.get('suggestion', '') for s in self.suggestions]
    
    def get_recommendations(self) -> List[str]:
        """Get optimization recommendations"""
        return self.recommendations
    
    def get_all_suggestions(self) -> List[Dict[str, Any]]:
        """Get all optimization suggestions"""
        return self.suggestions
    
    def _generate_recommendation(self, suggestion: Dict[str, Any]):
        """Generate recommendation based on suggestion"""
        recommendation = f"Optimize {suggestion.get('function', 'unknown')} function"
        
        if suggestion.get('type') == 'function_optimization':
            recommendation += f" (currently {suggestion.get('current_time_ms', 0):.2f}ms, threshold {suggestion.get('threshold_ms', 0):.2f}ms)"
        
        self.recommendations.append(recommendation)
    
    def get_summary(self) -> Dict[str, Any]:
        """Get optimization summary"""
        return {
            'total_suggestions': len(self.suggestions),
            'recent_suggestions': self.suggestions[-5:] if self.suggestions else [],
            'recommendations': self.recommendations[-5:] if self.recommendations else []
        }

# Global performance profiling instances
_global_profiler = None

def get_profiler(profile_interval: float = 0.1) -> PerformanceProfiler:
    """Get or create global performance profiler"""
    global _global_profiler
    if _global_profiler is None:
        _global_profiler = PerformanceProfiler(profile_interval)
    return _global_profiler

# Convenience functions

def start_profiling(profile_interval: float = 0.1):
    """Start global performance profiling"""
    profiler = get_profiler(profile_interval)
    profiler.start_profiling()

def stop_profiling():
    """Stop global performance profiling"""
    profiler = get_profiler()
    profiler.stop_profiling()

def profile_function(func_name: str, args: Dict[str, Any], func: Callable) -> Any:
    """Profile function execution"""
    profiler = get_profiler()
    return profiler.profile_function(func_name, args, func)

def get_performance_report() -> Dict[str, Any]:
    """Get global performance report"""
    profiler = get_profiler()
    return profiler.get_performance_report()

def export_profile(filepath: str):
    """Export global profiling data"""
    profiler = get_profiler()
    profiler.export_profile(filepath)

# Context manager for profiling

import functools

@contextmanager
def profile_function_execution(func_name: str, **kwargs):
    """Context manager for profiling function execution"""
    profiler = get_profiler()
    
    def func_wrapper(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapped(*args, **inner_kwargs):
            all_args = {'args': args, 'kwargs': inner_kwargs, **kwargs}
            return profiler.profile_function(func_name, all_args, func)
        return wrapped
    
    return func_wrapper

print("Performance profiling module loaded successfully!")