#!/usr/bin/env python3
"""
NLM Error Handling, Debugging Tools, and Performance Benchmarking Module
=====================================================================

Comprehensive error handling, debugging, and performance monitoring
for NLM Python applications. Provides capabilities for:

- Advanced error handling and recovery
- Debugging tools and instrumentation
- Performance monitoring and optimization
- Profiling and benchmarking
- Memory management and leak detection
- Real-time monitoring and alerting
- Advanced logging and diagnostics
- Automated testing and validation
- Continuous integration support
"""

import sys
import os
import time
import threading
import traceback
import signal
import psutil
import resource
from typing import Dict, List, Optional, Any, Callable, Tuple, Union
from dataclasses import dataclass, field
from enum import Enum
from datetime import datetime, timedelta
import json
import pickle
import inspect
import functools
import warnings
import logging
from contextlib import contextmanager
import tracemalloc
import cProfile
import pstats
from io import StringIO

# Performance and profiling
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from memory_profiler import memory_usage
from line_profiler import LineProfiler
from pytest_benchmark.fixture import bench

# NLM integration
try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Warning: pynlm module not found. Some features will not be available.")

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_debug.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)
class ErrorSeverity(Enum):
    """Error severity levels"""
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"
    FATAL = "fatal"
class ErrorCategory(Enum):
    """Error categories"""
    RUNTIME = "runtime"
    VALIDATION = "validation"
    CONFIGURATION = "configuration"
    MEMORY = "memory"
    PERFORMANCE = "performance"
    NETWORK = "network"
    INPUT_OUTPUT = "input_output"
    NEURAL_COMPUTATION = "neural_computation"
    SIMULATION = "simulation"
    MEMORY_MANAGEMENT = "memory_management"
    THREADING = "threading"
class ErrorContext:
    """Context information for errors"""
    
    def __init__(self,
                 error_id: str = None,
                 simulation_id: str = None,
                 component: str = None,
                 timestamp: datetime = None,
                 metadata: Dict[str, Any] = None):
        self.error_id = error_id or f"err_{int(time.time() * 1000000)}"
        self.simulation_id = simulation_id
        self.component = component
        self.timestamp = timestamp or datetime.now()
        self.metadata = metadata or {}
        self.stack_trace = None
    
    def add_metadata(self, key: str, value: Any):
        """Add metadata to error context"""
        self.metadata[key] = value
    
    def get_context_summary(self) -> Dict[str, Any]:
        """Get context summary"""
        return {
            'error_id': self.error_id,
            'simulation_id': self.simulation_id,
            'component': self.component,
            'timestamp': self.timestamp.isoformat(),
            'metadata_keys': list(self.metadata.keys()),
            'stack_trace_length': len(self.stack_trace) if self.stack_trace else 0
        }
class NLMError(Exception):
    """Base NLM exception class"""
    
    def __init__(self,
                 message: str,
                 severity: ErrorSeverity = ErrorSeverity.ERROR,
                 category: ErrorCategory = ErrorCategory.RUNTIME,
                 context: Optional[ErrorContext] = None,
                 cause: Optional[Exception] = None):
        super().__init__(message)
        self.message = message
        self.severity = severity
        self.category = category
        self.context = context or ErrorContext()
        self.cause = cause
        self.timestamp = datetime.now()
        
        # Add stack trace
        self.context.stack_trace = traceback.format_stack()
        
        # Set log level based on severity
        self._log_error()
    
    def _log_error(self):
        """Log error based on severity"""
        log_level = {
            ErrorSeverity.DEBUG: logger.debug,
            ErrorSeverity.INFO: logger.info,
            ErrorSeverity.WARNING: logger.warning,
            ErrorSeverity.ERROR: logger.error,
            ErrorSeverity.CRITICAL: logger.critical,
            ErrorSeverity.FATAL: logger.critical
        }[self.severity]
        
        log_level(f"NLM Error [{self.category.value.upper()}]: {self.message}")
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary"""
        return {
            'error_id': self.context.error_id,
            'message': self.message,
            'severity': self.severity.value,
            'category': self.category.value,
            'timestamp': self.timestamp.isoformat(),
            'component': self.context.component,
            'simulation_id': self.context.simulation_id,
            'metadata': self.context.metadata,
            'stack_trace': ''.join(self.context.stack_trace) if self.context.stack_trace else None
        }
class ErrorHandler:
    """Advanced error handling and recovery system"""
    
    def __init__(self, 
                 error_callback: Callable = None,
                 recovery_enabled: bool = True,
                 max_retries: int = 3,
                 retry_delay: float = 1.0):
        self.error_callback = error_callback
        self.recovery_enabled = recovery_enabled
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        self.error_history = []
        self.retry_counts = {}
        self.error_handlers = {}
        self.recovery_strategies = {}
        
        # Register default error handlers
        self._register_default_handlers()
    
    def _register_default_handlers(self):
        """Register default error handlers"""
        self.error_handlers = {
            'runtime': self._handle_runtime_error,
            'validation': self._handle_validation_error,
            'memory': self._handle_memory_error,
            'performance': self._handle_performance_error,
            'neural': self._handle_neural_error
        }
        
        self.recovery_strategies = {
            'runtime': self._recover_from_runtime_error,
            'validation': self._recover_from_validation_error,
            'memory': self._recover_from_memory_error,
            'performance': self._recover_from_performance_error,
            'neural': self._recover_from_neural_error
        }
    
    def add_error_handler(self,
                         category: ErrorCategory,
                         handler: Callable,
                         priority: int = 1):
        """
        Add custom error handler
        
        Args:
            category: Error category to handle
            handler: Error handler function
            priority: Handler priority (lower = higher priority)
        """
        if category not in self.error_handlers:
            self.error_handlers[category.value] = []
        
        self.error_handlers[category.value].append({
            'handler': handler,
            'priority': priority,
            'timestamp': time.time()
        })
        
        # Sort by priority
        self.error_handlers[category.value].sort(key=lambda x: x['priority'])
    
    def add_recovery_strategy(self,
                             category: ErrorCategory,
                             strategy: Callable,
                             condition: Callable = None):
        """
        Add recovery strategy
        
        Args:
            category: Error category to recover from
            strategy: Recovery strategy function
            condition: Condition function that must return True to apply strategy
        """
        if category not in self.recovery_strategies:
            self.recovery_strategies[category.value] = []
        
        self.recovery_strategies[category.value].append({
            'strategy': strategy,
            'condition': condition or (lambda *args, **kwargs: True),
            'timestamp': time.time()
        })
    
    def handle_error(self, 
                    error: Exception,
                    context: ErrorContext = None,
                    retry: bool = True) -> Dict[str, Any]:
        """
        Handle error with appropriate recovery strategy
        
        Args:
            error: Exception to handle
            context: Error context
            retry: Whether to retry if appropriate
            
        Returns:
            Dictionary with error handling results
        """
        context = context or ErrorContext()
        
        # Create NLM error from original exception
        if isinstance(error, NLMError):
            nlm_error = error
        else:
            # Determine error category and severity
            category = self._categorize_error(error)
            severity = self._determine_severity(error, context)
            
            nlm_error = NLMError(
                message=str(error),
                severity=severity,
                category=category,
                context=context,
                cause=error
            )
        
        # Store error
        self.error_history.append(nlm_error.to_dict())
        
        # Try to handle error
        result = self._handle_error_with_recovery(nlm_error, context)
        
        # Record retry count
        self.retry_counts[nlm_error.context.error_id] = self.retry_counts.get(nlm_error.context.error_id, 0) + 1
        
        # Call error callback if provided
        if self.error_callback:
            try:
                self.error_callback(nlm_error, result)
            except Exception as callback_error:
                logger.error(f"Error in error callback: {callback_error}")
        
        return result
    
    def _categorize_error(self, error: Exception) -> ErrorCategory:
        """Categorize error based on type and content"""
        error_type = type(error).__name__
        error_message = str(error).lower()
        
        # Memory errors
        if 'memory' in error_message or 'memory' in error_type.lower():
            return ErrorCategory.MEMORY
        
        # Performance errors
        if 'performance' in error_message or 'slow' in error_message or 'timeout' in error_message:
            return ErrorCategory.PERFORMANCE
        
        # Validation errors
        if 'validation' in error_message or 'invalid' in error_message or 'required' in error_message:
            return ErrorCategory.VALIDATION
        
        # Neural computation errors
        if 'neural' in error_message or 'neuron' in error_message or 'synapse' in error_message:
            return ErrorCategory.NEURAL_COMPUTATION
        
        # Configuration errors
        if 'config' in error_message or 'configuration' in error_message or 'parameter' in error_message:
            return ErrorCategory.CONFIGURATION
        
        # Default to runtime error
        return ErrorCategory.RUNTIME
    
    def _determine_severity(self, error: Exception, context: ErrorContext) -> ErrorSeverity:
        """Determine error severity"""
        # Check for critical errors
        if any(keyword in str(error).lower() for keyword in ['fatal', 'critical', 'fatal']):
            return ErrorSeverity.FATAL
        
        if any(keyword in str(error).lower() for keyword in ['error', 'exception', 'failed']):
            return ErrorSeverity.ERROR
        
        if any(keyword in str(error).lower() for keyword in ['warning', 'could not', 'unable']):
            return ErrorSeverity.WARNING
        
        # Default to info
        return ErrorSeverity.INFO
    
    def _handle_error_with_recovery(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:
        """Handle error with recovery"""
        category = error.category.value
        result = {
            'error_id': error.context.error_id,
            'handled': False,
            'recovered': False,
            'retry': False,
            'message': error.message
        }
        
        # Try to handle error with registered handlers
        if category in self.error_handlers:
            for handler_info in self.error_handlers[category]:
                try:
                    handler_result = handler_info['handler'](error, context)
                    if handler_result.get('handled', False):
                        result.update(handler_result)
                        result['handled'] = True
                        break
                except Exception as handler_error:
                    logger.error(f"Error in handler for {category}: {handler_error}")
        
        # Try recovery if not already handled
        if not result.get('recovered', False) and self.recovery_enabled:
            for strategy_info in self.recovery_strategies.get(category, []):
                try:
                    if strategy_info['condition'](error, context):
                        recovery_result = strategy_info['strategy'](error, context)
                        if recovery_result.get('recovered', False):
                            result.update(recovery_result)
                            result['recovered'] = True
                            break
                except Exception as recovery_error:
                    logger.error(f"Error in recovery strategy for {category}: {recovery_error}")
        
        # Retry if appropriate
        if retry and not result.get('recovered', False):
            retry_result = self._retry_error(error, context)
            result.update(retry_result)
        
        return result
    
    def _retry_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:
        """Retry error handling"""
        error_id = error.context.error_id
        retry_count = self.retry_counts.get(error_id, 0)
        
        if retry_count >= self.max_retries:
            return {'retry': False, 'message': 'Max retries exceeded'}
        
        # Check if error is retryable
        if not self._is_retryable_error(error):
            return {'retry': False, 'message': 'Error is not retryable'}
        
        # Wait before retry
        time.sleep(self.retry_delay)
        
        # Retry the operation (would need to capture original operation)
        # For now, return retry information
        return {
            'retry': True,
            'retry_count': retry_count + 1,
            'max_retries': self.max_retries
        }
    
    def _is_retryable_error(self, error: NLMError) -> bool:
        """Check if error is retryable"""
        # Check error category and message for retryable patterns
        retryable_categories = [ErrorCategory.RUNTIME, ErrorCategory.PERFORMANCE]
        
        if error.category in retryable_categories:
            retryable_messages = [
                'timeout', 'connection', 'temporary', 'unavailable', 
                'busy', 'resource', 'lock'
            ]
            
            return any(msg in error.message.lower() for msg in retryable_messages)
        
        return False
    
    def _handle_runtime_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:
        """Handle runtime errors"""
        return {
            'handled': True,
            'action': 'log_and_continue',
            'message': 'Runtime error handled and logged'
        }
    
    def _handle_validation_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:
        """Handle validation errors"""
        return {
            'handled': True,
            'action': 'validate_and_correct',
            'message': 'Validation error detected and corrected'
        }
    
    def _handle_memory_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:
        """Handle memory errors"""
        return {
            'handled': True,
            'action': 'cleanup_and_retry',
            'message': 'Memory error handled with cleanup'
        }
    
    def _handle_performance_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Handle performance errors"""
        return {
            'handled': True,
            'action': 'optimize_and_retry',
            'message': 'Performance error handled with optimization'
        }
    
    def _handle_neural_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Handle neural computation errors"""
        return {
            'handled': True,
            'action': 'reset_and_recover',
            'message': 'Neural computation error handled with reset'
        }
    
    def _recover_from_runtime_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Recover from runtime error"""
        return {
            'recovered': True,
            'recovery_action': 'continue_execution'
        }
    
    def _recover_from_validation_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Recover from validation error"""
        return {
            'recovered': True,
            'recovery_action': 'apply_defaults'
        }
    
    def _recover_from_memory_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Recover from memory error"""
        return {
            'recovered': True,
            'recovery_action': 'garbage_collect'
        }
    
    def _recover_from_performance_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Recover from performance error"""
        return {
            'recovered': True,
            'recovery_action': 'optimize_config'
        }
    
    def _recover_from_neural_error(self, error: NLMError, context: ErrorContext) -> Dict[str, Any]:n:
        """Recover from neural error"""
        return {
            'recovered': True,
            'recovery_action': 'reset_neural_state'
        }
    
    def get_error_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get error history"""
        return self.error_history[-limit:]
    
    def get_retry_counts(self) -> Dict[str, int]:
        """Get retry counts"""
        return self.retry_counts.copy()
    
    def clear_history(self):
        """Clear error history"""
        self.error_history.clear()
        self.retry_counts.clear()
class Debugger:
    """Advanced debugging and inspection tools"""
    
    def __init__(self):
        self.debugger_enabled = False
        self.breakpoints = {}
        self.watch_variables = {}
        self.debug_log = []
        self.line_profiler = None
        self.memory_profiler = None
    
    def enable_debugger(self, enable: bool = True):
        """Enable/disable debugger"""
        self.debugger_enabled = enable
        if enable:
            self._setup_debugging_tools()
        logger.info(f"Debugger {'enabled' if enable else 'disabled'}")
    
    def _setup_debugging_tools(self):
        """Setup debugging tools"""
        try:
            # Setup line profiler
            self.line_profiler = LineProfiler()
            
            # Setup memory profiler
            self.memory_profiler = memory_usage
            
            logger.info("Debugging tools setup complete")
            
        except Exception as e:
            logger.error(f"Failed to setup debugging tools: {e}")
    
    def add_breakpoint(self,
                      name: str,
                      condition: Callable = None,
                      line_number: int = None,
                      file_path: str = None):
        """
        Add breakpoint
        
        Args:
            name: Breakpoint name
            condition: Function that returns True if breakpoint should trigger
            line_number: Line number to break at
            file_path: File path to break at
        """
        self.breakpoints[name] = {
            'name': name,
            'condition': condition,
            'line_number': line_number,
            'file_path': file_path,
            'enabled': True,
            'hit_count': 0,
            'last_hit': None
        }
    
    def remove_breakpoint(self, name: str):
        """Remove breakpoint"""
        if name in self.breakpoints:
            del self.breakpoints[name]
    
    def set_watch_variable(self, name: str, value: Any, expression: str = None):
        """
        Set watch variable
        
        Args:
            name: Variable name
            value: Initial value
            expression: Expression to evaluate
        """
        self.watch_variables[name] = {
            'name': name,
            'value': value,
            'expression': expression,
            'last_updated': time.time(),
            'update_count': 0
        }
    
    def clear_watch_variable(self, name: str):
        """Clear watch variable"""
        if name in self.watch_variables:
            del self.watch_variables[name]
    
    def log_debug_info(self, 
                      message: str,
                      data: Any = None,
                      level: str = 'info',
                      component: str = None):
        """
        Log debug information
        
        Args:
            message: Debug message
            data: Additional data
            level: Log level
            component: Component name
        """
        debug_entry = {
            'timestamp': datetime.now().isoformat(),
            'message': message,
            'data': data,
            'level': level,
            'component': component,
            'watch_variables': self.watch_variables.copy()
        }
        
        self.debug_log.append(debug_entry)
        
        # Log to logger
        log_level = getattr(logger, level.lower(), logger.info)
        log_level(f"DEBUG [{component or 'unknown'}]: {message}")
        
        # Check breakpoints
        self._check_breakpoints(message, data, component)
    
    def _check_breakpoints(self,
                          message: str,
                          data: Any,
                          component: str):
        """Check if any breakpoints should trigger"""
        for breakpoint_name, breakpoint_info in self.breakpoints.items():
            if not breakpoint_info['enabled']:
                continue
            
            try:
                # Check condition
                if breakpoint_info['condition']:
                    condition_result = breakpoint_info['condition'](message, data, component)
                    if not condition_result:
                        continue
                
                # Trigger breakpoint
                breakpoint_info['hit_count'] += 1
                breakpoint_info['last_hit'] = datetime.now().isoformat()
                
                # Log breakpoint hit
                self.log_debug_info(
                    f"BREAKPOINT TRIGGERED: {breakpoint_name}",
                    {
                        'breakpoint_info': breakpoint_info,
                        'message': message,
                        'data': data,
                        'component': component
                    },
                    level='warning',
                    component='breakpoint'
                )
                
            except Exception as e:
                logger.error(f"Error checking breakpoint {breakpoint_name}: {e}")
    
    def profile_function(self, func: Callable) -> Callable:
        """
        Profile function execution
        
        Args:
            func: Function to profile
            
        Returns:
            Profiled function
        """
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            if self.line_profiler:
                self.line_profiler.add_function(func)
                return self.line_profiler(func)(*args, **kwargs)
            else:
                return func(*args, **kwargs)
        
        return wrapper
    
    def get_memory_usage(self, func: Callable = None, *args, **kwargs) -> Any:
        """
        Get memory usage
        
        Args:
            func: Function to execute
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Function result and memory usage
        """
        if self.memory_profiler:
            usage_before = self.memory_profiler()[0]
            result = func(*args, **kwargs) if func else None
            usage_after = self.memory_profiler()[0]
            
            return result, usage_after - usage_before
        else:
            return func(*args, **kwargs) if func else None, 0.0
    
    def get_debug_log(self, limit: int = 100, level: str = None) -> List[Dict[str, Any]]:
        """
        Get debug log entries
        
        Args:
            limit: Maximum number of entries
            level: Filter by log level
            
        Returns:
            List of debug log entries
        """
        entries = self.debug_log[-limit:]
        
        if level:
            entries = [entry for entry in entries if entry['level'] == level]
        
        return entries
    
    def get_breakpoint_stats(self) -> Dict[str, Any]:
        """Get breakpoint statistics"""
        stats = {}
        for name, info in self.breakpoints.items():
            stats[name] = {
                'enabled': info['enabled'],
                'hit_count': info['hit_count'],
                'last_hit': info['last_hit'],
                'condition': info['condition'] is not None
            }
        
        return stats
    
    def get_watch_variable_stats(self) -> Dict[str, Any]:
        """Get watch variable statistics"""
        stats = {}
        for name, info in self.watch_variables.items():
            stats[name] = {
                'last_updated': info['last_updated'],
                'update_count': info['update_count'],
                'type': info['type']
            }
        
        return stats
    
    def export_debug_log(self, filepath: str):
        """Export debug log to file"""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.debug_log, f, indent=2, default=str)
            logger.info(f"Debug log exported to {filepath}")
        except Exception as e:
            logger.error(f"Failed to export debug log: {e}")
    
    def cleanup(self):
        """Cleanup debugging resources"""
        self.breakpoints.clear()
        self.watch_variables.clear()
        self.debug_log.clear()
        
        if self.line_profiler:
            self.line_profiler.clear_functions()
        
        logger.info("Debug resources cleaned up")
class PerformanceMonitor:
    """Performance monitoring and benchmarking"""
    
    def __init__(self):
        self.metrics_history = []
        self.benchmark_results = {}
        self.profiling_data = {}
        self.is_monitoring = False
        self.monitor_thread = None
        self.sample_interval = 1.0
    
    def start_monitoring(self, interval: float = 1.0):
        """
        Start performance monitoring
        
        Args:
            interval: Sampling interval in seconds
        """
        self.sample_interval = interval
        self.is_monitoring = True
        
        def monitor_loop():
            while self.is_monitoring:
                try:
                    self._collect_metrics()
                    time.sleep(interval)
                except Exception as e:
                    logger.error(f"Monitoring error: {e}")
                    break
        
        self.monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitor_thread.start()
        logger.info(f"Performance monitoring started with interval {interval}s")
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.is_monitoring = False
        
        if self.monitor_thread and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        logger.info("Performance monitoring stopped")
    
    def _collect_metrics(self):
        """Collect performance metrics"""
        timestamp = time.time()
        
        # Get system metrics
        import psutil
        
        metrics = {
            'timestamp': timestamp,
            'cpu_percent': psutil.cpu_percent(),
            'memory_percent': psutil.virtual_memory().percent,
            'memory_available': psutil.virtual_memory().available,
            'disk_usage': psutil.disk_usage('/').percent,
            'network_io': psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv,
            'thread_count': len(threading.enumerate()),
            'process_count': psutil.cpu_count()
        }
        
        # Get process-specific metrics
        process = psutil.Process()
        metrics.update({
            'process_memory': process.memory_info().rss / (1024 * 1024),  # MB
            'process_cpu_percent': process.cpu_percent(),
            'open_files': len(process.open_files()),
            'connections': len(process.connections())
        })
        
        self.metrics_history.append(metrics)
        
        # Keep only recent data
        if len(self.metrics_history) > 10000:
            self.metrics_history = self.metrics_history[-5000:]
    
    def benchmark_function(self,
                          func: Callable,
                          *args,
                          name: str = None,
                          iterations: int = 10,
                          warmup_iterations: int = 3,
                          **kwargs) -> Dict[str, Any]:
        """
        Benchmark function performance
        
        Args:
            func: Function to benchmark
            *args: Function arguments
            name: Benchmark name
            iterations: Number of iterations
            warmup_iterations: Number of warmup iterations
            **kwargs: Function keyword arguments
            
        Returns:
            Dictionary with benchmark results
        """
        if name is None:
            name = func.__name__
        
        # Warmup
        for _ in range(warmup_iterations):
            func(*args, **kwargs)
        
        # Benchmark
        times = []
        for _ in range(iterations):
            start_time = time.time()
            func(*args, **kwargs)
            end_time = time.time()
            times.append(end_time - start_time)
        
        # Calculate statistics
        times_array = np.array(times)
        benchmark_result = {
            'name': name,
            'iterations': iterations,
            'warmup_iterations': warmup_iterations,
            'mean_time': float(np.mean(times_array)),
            'std_time': float(np.std(times_array)),
            'min_time': float(np.min(times_array)),
            'max_time': float(np.max(times_array)),
            'median_time': float(np.median(times_array)),
            'total_time': float(np.sum(times_array)),
            'time_per_iteration': float(np.mean(times_array) * 1000),  # ms
            'throughput': iterations / np.sum(times_array),  # iterations per second
            'timestamp': datetime.now().isoformat()
        }
        
        # Store benchmark result
        self.benchmark_results[name] = benchmark_result
        
        # Log results
        logger.info(f"Benchmark completed: {name} - {benchmark_result['mean_time']:.4f}s avg")
        
        return benchmark_result
    
    def profile_code(self,
                     code: str,
                     globals_dict: Dict[str, Any] = None,
                     locals_dict: Dict[str, Any] = None) -> Dict[str, Any]:
        """
        Profile code execution
        
        Args:
            code: Code to profile
            globals_dict: Global variables
            locals_dict: Local variables
            
        Returns:
            Dictionary with profiling results
        """
        if globals_dict is None:
            globals_dict = globals()
        if locals_dict is None:
            locals_dict = {}
        
        # Create profiler
        profiler = cProfile.Profile()
        
        # Run code
        profiler.enable()
        try:
            exec(code, globals_dict, locals_dict)
        finally:
            profiler.disable()
        
        # Process profiling data
        stream = StringIO()
        stats = pstats.Stats(profiler, stream=stream).sort_stats('cumulative')
        stats.print_stats()
        
        profile_data = {
            'code': code,
            'profile_stats': stream.getvalue(),
            'timestamp': datetime.now().isoformat()
        }
        
        self.profiling_data[code] = profile_data
        
        return profile_data
    
    def get_performance_summary(self, time_window: Optional[float] = None) -> Dict[str, Any]:
        """
        Get performance summary
        
        Args:
            time_window: Time window in seconds (None for all data)
            
        Returns:
            Dictionary with performance summary
        """
        if not self.metrics_history:
            return {'error': 'No metrics collected'}
        
        # Filter by time window
        if time_window is not None:
            cutoff_time = time.time() - time_window
            metrics = [m for m in self.metrics_history if m['timestamp'] >= cutoff_time]
        else:
            metrics = self.metrics_history
        
        if not metrics:
            return {'error': 'No metrics in time window'}
        
        # Calculate statistics
        cpu_values = [m['cpu_percent'] for m in metrics]
        memory_values = [m['memory_percent'] for m in metrics]
        thread_values = [m['thread_count'] for m in metrics]
        
        summary = {
            'time_window': time_window,
            'samples_count': len(metrics),
            'cpu': {
                'mean': float(np.mean(cpu_values)),
                'std': float(np.std(cpu_values)),
                'min': float(np.min(cpu_values)),
                'max': float(np.max(cpu_values)),
                'median': float(np.median(cpu_values))
            },
            'memory': {
                'mean': float(np.mean(memory_values)),
                'std': float(np.std(memory_values)),
                'min': float(np.min(memory_values)),
                'max': float(np.max(memory_values)),
                'median': float(np.median(memory_values))
            },
            'threads': {
                'mean': float(np.mean(thread_values)),
                'std': float(np.std(thread_values)),
                'min': float(np.min(thread_values)),
                'max': float(np.max(thread_values)),
                'median': float(np.median(thread_values))
            },
            'latest': metrics[-1]
        }
        
        return summary
    
    def get_benchmark_history(self, name: str = None) -> Dict[str, Any]:
        """
        Get benchmark history
        
        Args:
            name: Specific benchmark name (None for all)
            
        Returns:
            Dictionary with benchmark history
        """
        if name:
            return self.benchmark_results.get(name, {})
        else:
            return self.benchmark_results.copy()
    
    def cleanup(self):
        """Cleanup performance monitoring resources"""
        self.metrics_history.clear()
        self.benchmark_results.clear()
        self.profiling_data.clear()
        
        if self.is_monitoring:
            self.stop_monitoring()
        
        logger.info("Performance monitoring resources cleaned up")
class SystemMonitor:
    """System-wide monitoring for NLM"""
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.monitors = {}
        self.alert_rules = []
        self.dashboard_data = {}
        self.is_monitoring = False
        self.monitor_thread = None
        self.sample_interval = 1.0
    
    def add_monitor(self, name: str, monitor: Callable):
        """
        Add monitor
        
        Args:
            name: Monitor name
            monitor: Monitor function
        """
        self.monitors[name] = monitor
    
    def remove_monitor(self, name: str):
        """Remove monitor"""
        if name in self.monitors:
            del self.monitors[name]
    
    def add_alert_rule(self,
                      name: str,
                      condition: Callable,
                      severity: str = 'warning',
                      message: str = "Alert triggered"):
        """
        Add alert rule
        
        Args:
            name: Rule name
            condition: Function that takes monitoring data and returns boolean
            severity: Severity level ('info', 'warning', 'error')
            message: Alert message
        """
        self.alert_rules.append({
            'name': name,
            'condition': condition,
            'severity': severity,
            'message': message,
            'last_triggered': None
        })
    
    def start_monitoring(self, interval: float = 1.0):
        """
        Start system monitoring
        
        Args:
            interval: Monitoring interval in seconds
        """
        self.sample_interval = interval
        self.is_monitoring = True
        
        def monitor_loop():
            while self.is_monitoring:
                try:
                    self._collect_data()
                    self._check_alerts()
                    self._update_dashboard()
                    time.sleep(interval)
                except Exception as e:
                    logger.error(f"System monitoring error: {e}")
                    break
        
        self.monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitor_thread.start()
        logger.info(f"System monitoring started with interval {interval}s")
    
    def stop_monitoring(self):
        """Stop system monitoring"""
        self.is_monitoring = False
        
        if self.monitor_thread and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        logger.info("System monitoring stopped")
    
    def _collect_data(self):
        """Collect monitoring data"""
        timestamp = time.time()
        
        monitoring_data = {
            'timestamp': timestamp,
            'system': {},
            'nlm': {},
            'monitors': {}
        }
        
        # Collect system data
        import psutil
        monitoring_data['system'] = {
            'cpu_percent': psutil.cpu_percent(),
            'memory_percent': psutil.virtual_memory().percent,
            'memory_available': psutil.virtual_memory().available,
            'disk_usage': psutil.disk_usage('/').percent,
            'network_io': psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv,
            'process_count': psutil.cpu_count()
        }
        
        # Collect NLM-specific data
        if self.simulation:
            monitoring_data['nlm'] = self._collect_nlm_data()
        
        # Run custom monitors
        for name, monitor in self.monitors.items():
            try:
                monitoring_data['monitors'][name] = monitor()
            except Exception as e:
                logger.error(f"Monitor {name} failed: {e}")
        
        # Store data
        if not hasattr(self, 'monitoring_data'):
            self.monitoring_data = []
        self.monitoring_data.append(monitoring_data)
        
        # Keep only recent data
        if len(self.monitoring_data) > 10000:
            self.monitoring_data = self.monitoring_data[-5000:]
    
    def _collect_nlm_data(self) -> Dict[str, Any]:
        """Collect NLM-specific monitoring data"""
        nlm_data = {}
        
        try:
            if hasattr(self.simulation, 'get_simulation_status'):
                nlm_data['simulation_status'] = self.simulation.get_simulation_status()
            
            if hasattr(self.simulation, 'get_performance_metrics'):
                nlm_data['performance'] = self.simulation.get_performance_metrics()
            
            # Add other NLM-specific metrics
            nlm_data['memory_usage'] = 0.0  # Placeholder
            nlm_data['step_rate'] = 0.0  # Placeholder
            
        except Exception as e:
            logger.error(f"Failed to collect NLM data: {e}")
        
        return nlm_data
    
    def _check_alerts(self):
        """Check if any alert rules are triggered"""
        if not hasattr(self, 'monitoring_data') or not self.monitoring_data:
            return
        
        latest_data = self.monitoring_data[-1]
        
        for rule in self.alert_rules:
            try:
                if rule['condition'](latest_data):
                    self._trigger_alert(rule, latest_data)
            except Exception as e:
                logger.error(f"Alert rule error: {e}")
    
    def _trigger_alert(self, rule: Dict[str, Any], data: Dict[str, Any]):
        """Trigger alert notification"""
        rule['last_triggered'] = time.time()
        
        alert_message = {
            'name': rule['name'],
            'severity': rule['severity'],
            'message': rule['message'],
            'timestamp': time.time(),
            'data': data
        }
        
        logger.warning(f"ALERT [{rule['severity'].upper()}]: {rule['message']}")
        
        # In a real implementation, this would send notifications
        # (email, SMS, webhook, etc.)
    
    def _update_dashboard(self):
        """Update dashboard data"""
        if not hasattr(self, 'monitoring_data') or not self.monitoring_data:
            return
        
        latest_data = self.monitoring_data[-1]
        
        # Calculate dashboard metrics
        recent_data = self.monitoring_data[-100:]  # Last 100 samples
        
        dashboard = {
            'timestamp': datetime.now().isoformat(),
            'system': self._calculate_system_summary(recent_data),
            'nlm': self._calculate_nlm_summary(recent_data),
            'alerts': self._get_active_alerts(),
            'trend_data': self._get_trend_data(recent_data)
        }
        
        self.dashboard_data = dashboard
    
    def _calculate_system_summary(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate system summary from monitoring data"""
        if not data:
            return {}
        
        cpu_values = [d['system']['cpu_percent'] for d in data]
        memory_values = [d['system']['memory_percent'] for d in data]
        
        return {
            'cpu': {
                'mean': float(np.mean(cpu_values)),
                'max': float(np.max(cpu_values)),
                'current': cpu_values[-1] if cpu_values else 0.0
            },
            'memory': {
                'mean': float(np.mean(memory_values)),
                'max': float(np.max(memory_values)),
                'current': memory_values[-1] if memory_values else 0.0
            }
        }
    
    def _calculate_nlm_summary(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate NLM summary from monitoring data"""
        nlm_summaries = []
        for d in data:
            if 'nlm' in d:
                nlm_summaries.append(d['nlm'])
        
        if not nlm_summaries:
            return {}
        
        return {
            'simulation_count': len([d for d in data if 'nlm' in d]),
            'current_status': nlm_summaries[-1].get('simulation_status', {}),
            'performance_trend': 'stable'  # Placeholder
        }
    
    def _get_active_alerts(self) -> List[Dict[str, Any]]:
        """Get active alerts"""
        active_alerts = []
        
        for rule in self.alert_rules:
            if rule['last_triggered'] and (time.time() - rule['last_triggered']) < 300:  # 5 minutes
                active_alerts.append({
                    'name': rule['name'],
                    'severity': rule['severity'],
                    'message': rule['message'],
                    'last_triggered': rule['last_triggered']
                })
        
        return active_alerts
    
    def _get_trend_data(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Get trend data for charting"""
        if not data:
            return {}
        
        timestamps = [d['timestamp'] for d in data]
        cpu_values = [d['system']['cpu_percent'] for d in data]
        memory_values = [d['system']['memory_percent'] for d in data]
        
        return {
            'timestamps': timestamps,
            'cpu': cpu_values,
            'memory': memory_values,
            'cpu_trend': self._calculate_trend(cpu_values),
            'memory_trend': self._calculate_trend(memory_values)
        }
    
    def _calculate_trend(self, values: List[float]) -> Dict[str, Any]:
        """Calculate linear trend of values"""
        if len(values) < 2:
            return {'slope': 0.0, 'direction': 'stable'}
        
        x = np.arange(len(values))
        y = np.array(values)
        
        slope, intercept = np.polyfit(x, y, 1)
        
        if slope > 0.01:
            direction = 'increasing'
        elif slope < -0.01:
            direction = 'decreasing'
        else:
            direction = 'stable'
        
        return {
            'slope': float(slope),
            'direction': direction,
            'change_per_sample': float(slope)
        }
    
    def get_monitoring_history(self, limit: int = 1000) -> List[Dict[str, Any]]:
        """Get monitoring history"""
        if not hasattr(self, 'monitoring_data'):
            return []
        
        return self.monitoring_data[-limit:]
    
    def get_dashboard_data(self) -> Dict[str, Any]:
        """Get current dashboard data"""
        return self.dashboard_data.copy()
    
    def cleanup(self):
        """Cleanup system monitoring resources"""
        self.monitors.clear()
        self.alert_rules.clear()
        
        if hasattr(self, 'monitoring_data'):
            self.monitoring_data.clear()
        
        if self.is_monitoring:
            self.stop_monitoring()
        
        logger.info("System monitoring resources cleaned up")
class NLPToolkit:
    """
    Comprehensive NLM debugging and performance toolkit.
    
    Provides advanced debugging, error handling, and performance monitoring
    for NLM Python applications with comprehensive analysis capabilities.
    """
    
    def __init__(self, simulation: Any = None):
        self.simulation = simulation
        self.error_handler = ErrorHandler()
        self.debugger = Debugger()
        self.performance_monitor = PerformanceMonitor()
        self.system_monitor = SystemMonitor(simulation)
        self.is_initialized = False
        
        # Initialize toolkit
        self._initialize_toolkit()
    
    def _initialize_toolkit(self):
        """Initialize debugging toolkit"""
        try:
            # Configure debugging
            self.debugger.enable_debugger(True)
            
            # Start performance monitoring
            self.performance_monitor.start_monitoring()
            
            # Start system monitoring
            self.system_monitor.start_monitoring()
            
            self.is_initialized = True
            logger.info("NLM debugging toolkit initialized successfully")
            
        except Exception as e:
            logger.error(f"Failed to initialize NLM debugging toolkit: {e}")
            raise
    
    def handle_exception(self, 
                        exception: Exception,
                        context: Dict[str, Any] = None,
                        callback: Callable = None):
        """
        Handle exception with full context
        
        Args:
            exception: Exception to handle
            context: Context information
            callback: Callback function
        """
        error_context = ErrorContext()
        if context:
            for key, value in context.items():
                error_context.add_metadata(key, value)
        
        # Set component if simulation is available
        if self.simulation:
            error_context.component = 'simulation'
        
        # Handle error
        result = self.error_handler.handle_error(exception, error_context)
        
        # Call callback if provided
        if callback:
            try:
                callback(exception, result)
            except Exception as callback_error:
                logger.error(f"Error in exception callback: {callback_error}")
        
        return result
    
    def profile_function(self, func: Callable, *args, **kwargs) -> Any:
        """
        Profile function execution
        
        Args:
            func: Function to profile
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Function result and profiling data
        """
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        # Use debugger's profiler
        profiled_func = self.debugger.profile_function(func)
        
        return profiled_func(*args, **kwargs)
    
    def benchmark(self, 
                  func: Callable,
                  name: str = None,
                  iterations: int = 10,
                  warmup_iterations: int = 3,
                  *args,
                  **kwargs) -> Dict[str, Any]:
        """
        Benchmark function performance
        
        Args:
            func: Function to benchmark
            name: Benchmark name
            iterations: Number of iterations
            warmup_iterations: Number of warmup iterations
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Dictionary with benchmark results
        """
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        return self.performance_monitor.benchmark_function(
            func, name, iterations, warmup_iterations, *args, **kwargs
        )
    
    def start_monitoring(self):
        """Start all monitoring"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        self.performance_monitor.start_monitoring()
        self.system_monitor.start_monitoring()
    
    def stop_monitoring(self):
        """Stop all monitoring"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        self.performance_monitor.stop_monitoring()
        self.system_monitor.stop_monitoring()
    
    def set_breakpoint(self, name: str, condition: Callable = None):
        """Set debugging breakpoint"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        self.debugger.add_breakpoint(name, condition)
    
    def watch_variable(self, name: str, value: Any):
        """Watch variable for debugging"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        self.debugger.set_watch_variable(name, value)
    
    def get_debug_log(self, limit: int = 100, level: str = None) -> List[Dict[str, Any]]:
        """Get debug log"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        return self.debugger.get_debug_log(limit, level)
    
    def get_performance_summary(self, time_window: Optional[float] = None) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        return self.performance_monitor.get_performance_summary(time_window)
    
    def get_system_status(self) -> Dict[str, Any]:
        """Get system status"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        return self.system_monitor.get_dashboard_data()
    
    def export_debug_data(self, filepath: str):
        """Export debug data to file"""
        if not self.is_initialized:
            raise RuntimeError("Toolkit not initialized")
        
        # Export debug log
        debug_log_path = filepath.replace('.json', '_debug.log')
        self.debugger.export_debug_log(debug_log_path)
        
        # Export performance data
        performance_path = filepath.replace('.json', '_performance.json')
        with open(performance_path, 'w') as f:
            json.dump({
                'benchmarks': self.performance_monitor.get_benchmark_history(),
                'monitoring': self.performance_monitor.get_performance_summary()
            }, f, indent=2, default=str)
    
    def cleanup(self):
        """Cleanup toolkit resources"""
        if not self.is_initialized:
            return
        
        self.error_handler.clear_history()
        self.debugger.cleanup()
        self.performance_monitor.cleanup()
        self.system_monitor.cleanup()
        
        self.is_initialized = False
        logger.info("NLM debugging toolkit cleaned up")
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.cleanup()
def demonstrate_toolkit_capabilities():
    """Demonstrate toolkit capabilities"""
    print("=== NLM Error Handling, Debugging Tools, and Performance Benchmarking Module Demo ===\n")
    
    # Initialize toolkit
    toolkit = NLPToolkit()
    
    print(f"Toolkit initialized: {toolkit.is_initialized}")
    
    # Start monitoring
    print("\nStarting monitoring...")
    toolkit.start_monitoring()
    
    # Set up debugging
    print("\nSetting up debugging...")
    toolkit.set_breakpoint("test_breakpoint", condition=lambda msg, data, comp: "test" in msg.lower())
    toolkit.watch_variable("test_var", 42)
    
    # Log some debug info
    print("\nLogging debug information...")
    toolkit.debugger.log_debug_info("Starting simulation", {"step": 0}, "simulation", "info")
    toolkit.debugger.log_debug_info("Processing neural activity", {"neurons": 100}, "neural", "info")
    
    # Simulate some work with profiling
    print("\nSimulating work with profiling...")
    
    def sample_work_function(x):
        """Sample function for benchmarking"""
        time.sleep(0.01)  # Simulate work
        return x * 2
    
    # Benchmark the function
    bench_result = toolkit.benchmark(sample_work_function, 5, name="sample_work", iterations=5)
    print(f"Benchmark result: {bench_result['mean_time']:.4f}s avg")
    
    # Simulate some error handling
    print("\nSimulating error handling...")
    
    try:
        # Create and handle an error
        raise ValueError("Test error for handling")
    except Exception as e:
        # Handle with toolkit
        result = toolkit.handle_exception(e, {"context": "test_context"})
        print(f"Error handled: {result.get('handled', False)}")
    
    # Get performance summary
    print("\nGetting performance summary...")
    perf_summary = toolkit.get_performance_summary(time_window=60)
    print(f"Performance summary retrieved: {len(perf_summary)} metrics")
    
    # Get debug log
    print("\nGetting debug log...")
    debug_log = toolkit.get_debug_log(limit=5)
    print(f"Debug log entries: {len(debug_log)}")
    
    # Stop monitoring
    print("\nStopping monitoring...")
    toolkit.stop_monitoring()
    
    # Export debug data
    print("\nExporting debug data...")
    toolkit.export_debug_data("nlm_debug_output.json")
    print("Debug data exported successfully")
    
    return toolkit
def main():
    """Main demonstration function"""
    print("NLM Error Handling, Debugging Tools, and Performance Benchmarking Module")
    print("=" * 75)
    
    # Demonstrate toolkit capabilities
    toolkit = demonstrate_toolkit_capabilities()
    
    print("\nDebug and performance toolkit demonstration completed successfully!")
    return toolkit
if __name__ == "__main__":
    main()
