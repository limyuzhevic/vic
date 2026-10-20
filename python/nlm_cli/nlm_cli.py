#!/usr/bin/env python3
"""
Enhanced NLM Error Handling System

Comprehensive error handling for NLM (Neural Learning Machine) CLI and related tools.
Implements centralized error management, custom exception hierarchy, validation,
error recovery, and diagnostic reporting.
"""

import sys
import logging
import time
import traceback
from typing import Dict, List, Any, Optional, Union, Callable, Type
from enum import Enum, auto
from datetime import datetime
from dataclasses import dataclass, field
from contextlib import contextmanager
import json
import yaml
from pathlib import Path
import threading
import asyncio

# Version
__version__ = "2.0.0"

# Error codes enum for systematic error identification
class ErrorCode(Enum):
    # Configuration errors
    CONFIG_FILE_NOT_FOUND = auto()
    CONFIG_INVALID_FORMAT = auto()
    CONFIG_VALIDATION_FAILED = auto()
    CONFIG_KEY_NOT_FOUND = auto()
    CONFIG_TYPE_MISMATCH = auto()
    CONFIG_LOAD_FAILED = auto()
    CONFIG_SAVE_FAILED = auto()
    
    # Neural network errors
    BRAIN_INITIALIZATION_FAILED = auto()
    BRAIN_NOT_INITIALIZED = auto()
    NEURON_NOT_FOUND = auto()
    SYNAPSE_CREATION_FAILED = auto()
    POPULATION_OVERFLOW = auto()
    REGION_NOT_FOUND = auto()
    INVALID_NEURON_TYPE = auto()
    BRAIN_STEP_FAILED = auto()
    
    # Simulation errors
    SIMULATION_NOT_STARTED = auto()
    SIMULATION_STEP_FAILED = auto()
    INVALID_TIME_STEP = auto()
    CLOCK_SYNC_FAILED = auto()
    TICK_FAILED = auto()
    SIMULATION_TIMEOUT = auto()
    
    # Memory errors
    MEMORY_ALLOCATION_FAILED = auto()
    MEMORY_CORRUPTION = auto()
    NULL_POINTER_DEREFERENCE = auto()
    INVALID_MEMORY_ACCESS = auto()
    MEMORY_POOL_EXHAUSTED = auto()
    
    # Validation errors
    INPUT_VALIDATION_FAILED = auto()
    RANGE_VALIDATION_FAILED = auto()
    TYPE_VALIDATION_FAILED = auto()
    CONSTRAINT_VALIDATION_FAILED = auto()
    SCHEMA_VALIDATION_FAILED = auto()
    DATA_INTEGRITY_FAILED = auto()
    
    # Sensory system errors
    SENSOR_NOT_INITIALIZED = auto()
    SENSOR_DATA_CORRUPTED = auto()
    SENSOR_OVERFLOW = auto()
    SENSOR_TIMEOUT = auto()
    AUDIO_PROCESSING_FAILED = auto()
    VISION_PROCESSING_FAILED = auto()
    
    # Motor system errors
    MOTOR_COMMAND_FAILED = auto()
    ACTION_QUEUE_FULL = auto()
    INVALID_ACTION_TYPE = auto()
    MOTOR_SYSTEM_BUSY = auto()
    ACTUATOR_FAILURE = auto()
    
    # Plasticity errors
    PLASTICITY_RULE_FAILED = auto()
    INVALID_PLACING_METHOD = auto()
    STDP_CALCULATION_FAILED = auto()
    HEBBIAN_UPDATE_FAILED = auto()
    
    # Neuromodulation errors
    MODULATOR_NOT_INITIALIZED = auto()
    NOVELTY_DETECTION_FAILED = auto()
    REWARD_CALCULATION_FAILED = auto()
    PREDICTION_ERROR_CALCULATION_FAILED = auto()
    CURIOUSY_TOO_LOW = auto()
    
    # Prediction errors
    PREDICTION_FAILED = auto()
    MODEL_NOT_TRAINED = auto()
    INVALID_PREDICTION_TYPE = auto()
    PREDICTION_TIMEOUT = auto()
    
    # Cognition errors
    CONCEPT_FORMATION_FAILED = auto()
    PLANNING_FAILED = auto()
    INVALID_CONCEPT_TYPE = auto()
    
    # Memory system errors
    WORKING_MEMORY_FULL = auto()
    EPISODIC_MEMORY_WRITE_FAILED = auto()
    SEMANTIC_MEMORY_ACCESS_FAILED = auto()
    PROCEEDURAL_MEMORY_RECALL_FAILED = auto()
    
    # Performance errors
    PERFORMANCE_LIMIT_EXCEEDED = auto()
    OPTIMIZATION_FAILED = auto()
    SIMD_NOT_SUPPORTED = auto()
    
    # General errors
    UNKNOWN_ERROR = auto()
    NOT_IMPLEMENTED = auto()
    OUT_OF_MEMORY = auto()
    INVALID_ARGUMENT = auto()
    TIMEOUT = auto()
    NETWORK_ERROR = auto()
    IO_ERROR = auto()
    PERMISSION_DENIED = auto()
    SYSTEM_ERROR = auto()

# Exception hierarchy
class NLMException(Exception):
    """Base exception for all NLM errors"""
    
    def __init__(self, 
                 error_code: ErrorCode,
                 message: str,
                 context: Optional[Dict[str, Any]] = None,
                 cause: Optional[Exception] = None,
                 file: Optional[str] = None,
                 line: Optional[int] = None,
                 function: Optional[str] = None):
        self.error_code = error_code
        self.message = message
        self.context = context or {}
        self.cause = cause
        self.file = file or ""
        self.line = line or 0
        self.function = function or ""
        self.timestamp = datetime.now()
        self.stack_trace = traceback.format_stack()
        
        # Add file/line info from __init__ if not provided
        if not file and "__init__" in self.__class__.__name__:
            import inspect
            frame = inspect.currentframe()
            if frame and frame.f_back:
                self.file = frame.f_back.f_code.co_filename
                self.line = frame.f_back.f_lineno
                self.function = frame.f_back.f_code.co_name
        
        super().__init__(self._format_message())
    
    def _format_message(self) -> str:
        """Format the error message with error code and context"""
        msg = f"[{self.error_code.name}] {self.message}"
        if self.file:
            msg += f" (at {self.file}:{self.line})"
        if self.context:
            context_str = ", ".join(f"{k}={v}" for k, v in self.context.items())
            msg += f" Context: {context_str}"
        return msg
    
    def __str__(self) -> str:
        return self._format_message()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert exception to dictionary for serialization"""
        return {
            "error_code": self.error_code.name,
            "message": self.message,
            "context": self.context,
            "file": self.file,
            "line": self.line,
            "function": self.function,
            "timestamp": self.timestamp.isoformat(),
            "stack_trace": self.stack_trace,
            "cause": str(self.cause) if self.cause else None
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'NLMException':
        """Create exception from dictionary"""
        error_code = ErrorCode[data.get("error_code", "UNKNOWN_ERROR")]
        message = data.get("message", "Unknown error")
        context = data.get("context", {})
        
        # Try to reconstruct cause if available
        cause = None
        if data.get("cause"):
            cause = Exception(data["cause"])
        
        return cls(
            error_code=error_code,
            message=message,
            context=context,
            cause=cause,
            file=data.get("file"),
            line=data.get("line"),
            function=data.get("function")
        )
    
    def log(self, logger: logging.Logger, level=logging.ERROR) -> None:
        """Log the exception with detailed information"""
        logger.log(level, str(self))
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug(f"Stack trace: {''.join(self.stack_trace)}")
            if self.cause:
                logger.debug(f"Caused by: {self.cause}")
            if self.context:
                logger.debug(f"Context: {self.context}")

# Specific exception classes
class ConfigurationException(NLMException):
    """Exception for configuration-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.CONFIG_VALIDATION_FAILED, message, **kwargs)

class BrainException(NLMException):
    """Exception for brain-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.BRAIN_INITIALIZATION_FAILED, message, **kwargs)

class SimulationException(NLMException):
    """Exception for simulation-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.SIMULATION_STEP_FAILED, message, **kwargs)

class MemoryException(NLMException):
    """Exception for memory-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MEMORY_ALLOCATION_FAILED, message, **kwargs)

class ValidationException(NLMException):
    """Exception for validation-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.INPUT_VALIDATION_FAILED, message, **kwargs)

class SensoryException(NLMException):
    """Exception for sensory system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.SENSOR_NOT_INITIALIZED, message, **kwargs)

class MotorException(NLMException):
    """Exception for motor system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MOTOR_COMMAND_FAILED, message, **kwargs)

class PlasticityException(NLMException):
    """Exception for plasticity system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.PLASTICITY_RULE_FAILED, message, **kwargs)

class NeuromodulationException(NLMException):
    """Exception for neuromodulation system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MODULATOR_NOT_INITIALIZED, message, **kwargs)

class PredictionException(NLMException):
    """Exception for prediction system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.PREDICTION_FAILED, message, **kwargs)

class CognitionException(NLMException):
    """Exception for cognition system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.CONCEPT_FORMATION_FAILED, message, **kwargs)

# Error recovery and resilience
class ErrorRecovery:
    """Error recovery mechanisms"""
    
    class RecoveryStrategy(Enum):
        FAIL_FAST = "fail_fast"
        GRACEFUL_DEGRADATION = "graceful_degradation"
        RETRY = "retry"
        FALLBACK = "fallback"
        CIRCUIT_BREAKER = "circuit_breaker"
        AUTO_RECOVER = "auto_recover"
    
    @dataclass
    class RecoveryConfig:
        strategy: RecoveryStrategy = RecoveryStrategy.FAIL_FAST
        max_retries: int = 3
        timeout_ms: int = 5000
        retry_delay_ms: int = 1000
        circuit_breaker_threshold: int = 5
        circuit_breaker_timeout_ms: int = 60000
        fallback_condition: Optional[Callable[[], bool]] = None
        recovery_action: Optional[Callable[[], None]] = None
        
        # Performance tracking
        total_operations: int = field(default=0, init=False)
        failed_operations: int = field(default=0, init=False)
        last_failure_time: Optional[float] = field(default=None, init=False)
        
        def record_operation(self, success: bool):
            self.total_operations += 1
            if not success:
                self.failed_operations += 1
                self.last_failure_time = time.time()
        
        def is_circuit_open(self) -> bool:
            """Check if circuit breaker is open"""
            if self.last_failure_time is None:
                return False
            
            if self.failed_operations >= self.circuit_breaker_threshold:
                if (time.time() - self.last_failure_time) * 1000 >= self.circuit_breaker_timeout_ms:
                    # Reset circuit breaker after timeout
                    self.failed_operations = 0
                    self.last_failure_time = None
                    return False
                return True
            return False
        
        def can_proceed(self) -> bool:
            """Check if operation can proceed"""
            if self.strategy == self.RecoveryStrategy.FAIL_FAST:
                return True
            
            if self.strategy == self.RecoveryStrategy.CIRCUIT_BREAKER:
                return not self.is_circuit_open()
            
            return True
    
    @staticmethod
    def with_retry(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with retry mechanism"""
        if not config.can_proceed():
            raise ConfigurationException("Circuit breaker is open, cannot retry operation")
        
        for attempt in range(config.max_retries + 1):
            try:
                result = func(*args, **kwargs)
                config.record_operation(True)
                return result
            except Exception as e:
                config.record_operation(False)
                if attempt == config.max_retries:
                    raise
                
                # Exponential backoff
                delay = config.retry_delay_ms * (2 ** attempt)
                time.sleep(delay / 1000.0)
    
    @staticmethod
    def with_graceful_degradation(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with graceful degradation"""
        try:
            result = func(*args, **kwargs)
            config.record_operation(True)
            return result
        except Exception as e:
            config.record_operation(False)
            
            if config.fallback_condition and config.fallback_condition():
                # Use fallback
                if config.recovery_action:
                    config.recovery_action()
                raise ValidationException(f"Operation failed, using fallback: {e}")
            
            raise
    
    @staticmethod
    def with_circuit_breaker(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with circuit breaker"""
        if not config.can_proceed():
            raise ConfigurationException("Circuit breaker is open")
        
        try:
            result = func(*args, **kwargs)
            config.record_operation(True)
            return result
        except Exception as e:
            config.record_operation(False)
            raise
    
    @staticmethod
    def auto_recover(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with automatic recovery"""
        if not config.can_proceed():
            if config.recovery_action:
                config.recovery_action()
        
        return ErrorRecovery.with_retry(config, func, *args, **kwargs)

# Validation system
class Validation:
    """Validation utilities for NLM"""
    
    @staticmethod
    def validate_range(value: Union[int, float], min_val: Union[int, float], max_val: Union[int, float], 
                      name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is within specified range"""
        if value < min_val or value > max_val:
            error_msg = f"Value {name} ({value}) is out of range [{min_val}, {max_val}]"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_positive(value: Union[int, float], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is positive"""
        if value <= 0:
            error_msg = f"Value {name} ({value}) must be positive"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_non_negative(value: Union[int, float], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is non-negative"""
        if value < 0:
            error_msg = f"Value {name} ({value}) must be non-negative"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_type(value: Any, expected_type: Type, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is of expected type"""
        if not isinstance(value, expected_type):
            error_msg = f"Value {name} ({value}) is not of type {expected_type.__name__}"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_not_null(value: Any, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is not None"""
        if value is None:
            error_msg = f"Value {name} cannot be None"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_string_not_empty(value: str, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that string is not empty"""
        if not value or not value.strip():
            error_msg = f"Value {name} cannot be empty"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_list_not_empty(value: List, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that list is not empty"""
        if not value:
            error_msg = f"Value {name} cannot be empty list"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_max_size(value: Union[List, Dict, str], max_size: int, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value size does not exceed maximum"""
        actual_size = len(value)
        if actual_size > max_size:
            error_msg = f"Value {name} size ({actual_size}) exceeds maximum ({max_size})"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_dict_keys(value: Dict, required_keys: List[str], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that dictionary contains required keys"""
        missing_keys = [key for key in required_keys if key not in value]
        if missing_keys:
            error_msg = f"Value {name} is missing required keys: {', '.join(missing_keys)}"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True

class PerformanceMonitor:
    """Performance monitoring for NLM operations"""
    
    def __init__(self):
        self.metrics = {}
        self.start_times = {}
        self.end_times = {}
    
    def start_operation(self, operation: str):
        """Start monitoring an operation"""
        self.start_times[operation] = time.perf_counter()
    
    def end_operation(self, operation: str) -> float:
        """End monitoring and return duration in seconds"""
        if operation not in self.start_times:
            raise ValueError(f"Operation '{operation}' was not started")
        
        end_time = time.perf_counter()
        duration = end_time - self.start_times[operation]
        
        # Store metrics
        if operation not in self.metrics:
            self.metrics[operation] = {
                'total_time': 0.0,
                'count': 0,
                'min_time': float('inf'),
                'max_time': 0.0,
                'times': []
            }
        
        self.metrics[operation]['total_time'] += duration
        self.metrics[operation]['count'] += 1
        self.metrics[operation]['min_time'] = min(self.metrics[operation]['min_time'], duration)
        self.metrics[operation]['max_time'] = max(self.metrics[operation]['max_time'], duration)
        self.metrics[operation]['times'].append(duration)
        
        # Remove start time
        del self.start_times[operation]
        
        return duration
    
    def get_operation_stats(self, operation: str) -> Dict[str, Any]:
        """Get statistics for an operation"""
        if operation not in self.metrics:
            return {}
        
        metrics = self.metrics[operation]
        if metrics['count'] == 0:
            return {}
        
        avg_time = metrics['total_time'] / metrics['count']
        recent_times = metrics['times'][-10:] if len(metrics['times']) > 10 else metrics['times']
        recent_avg = sum(recent_times) / len(recent_times) if recent_times else 0.0
        
        return {
            'total_time': metrics['total_time'],
            'count': metrics['count'],
            'average_time': avg_time,
            'recent_average_time': recent_avg,
            'min_time': metrics['min_time'],
            'max_time': metrics['max_time'],
            'recent_times': recent_times,
            'operations_per_second': metrics['count'] / metrics['total_time'] if metrics['total_time'] > 0 else 0.0
        }
    
    def log_performance(self, operation: str, duration: float):
        """Log performance metrics"""
        stats = self.get_operation_stats(operation)
        if stats:
            self.logger.info(
                f"Performance: {operation} - "
                f"Duration: {duration:.3f}s, "
                f"Average: {stats['average_time']:.3f}s, "
                f"Count: {stats['count']}"
            )
            
            # Performance warning if duration is too long
            if duration > 60.0:  # 60 seconds threshold
                self.logger.warning(f"Operation '{operation}' took {duration:.2f}s, consider optimizing")
    
    def get_all_stats(self) -> Dict[str, Dict[str, Any]]:
        """Get statistics for all operations"""
        return {
            operation: self.get_operation_stats(operation)
            for operation in self.metrics
        }
    
    def reset(self):
        """Reset all metrics"""
        self.metrics.clear()
        self.start_times.clear()
        self.end_times.clear()


class PerformanceProfiler:
    """Performance profiler for NLM operations"""
    
    def __init__(self):
        self.profiles = {}
        self.current_profile = None
        self.profiling_enabled = False
    
    def start_profile(self, profile_name: str):
        """Start profiling a specific operation"""
        self.current_profile = profile_name
        self.profiles[profile_name] = {
            'start_time': time.perf_counter(),
            'end_time': None,
            'duration': 0.0,
            'samples': [],
            'memory_samples': []
        }
    
    def end_profile(self, profile_name: str) -> float:
        """End profiling and return duration"""
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' was not started")
        
        profile = self.profiles[profile_name]
        end_time = time.perf_counter()
        profile['end_time'] = end_time
        profile['duration'] = end_time - profile['start_time']
        
        self.current_profile = None
        return profile['duration']
    
    def add_sample(self, operation: str, memory_usage: float = None):
        """Add a sample to the current profile"""
        if not self.current_profile:
            return
        
        profile = self.profiles[self.current_profile]
        sample_time = time.perf_counter() - profile['start_time']
        profile['samples'].append((sample_time, operation))
        
        if memory_usage is not None:
            profile['memory_samples'].append((sample_time, memory_usage))
    
    def get_profile_stats(self, profile_name: str) -> Dict[str, Any]:
        """Get statistics for a profile"""
        if profile_name not in self.profiles:
            return {}
        
        profile = self.profiles[profile_name]
        if not profile['samples']:
            return {}
        
        # Analyze samples by operation
        operations = {}
        for sample_time, operation in profile['samples']:
            if operation not in operations:
                operations[operation] = {
                    'count': 0,
                    'total_time': 0.0,
                    'times': []
                }
            
            operations[operation]['count'] += 1
            operations[operation]['total_time'] += sample_time
            operations[operation]['times'].append(sample_time)
        
        # Calculate statistics
        for op_stats in operations.values():
            if op_stats['count'] > 0:
                op_stats['average_time'] = op_stats['total_time'] / op_stats['count']
                op_stats['recent_times'] = op_stats['times'][-10:]
        
        return {
            'total_duration': profile['duration'],
            'total_samples': len(profile['samples']),
            'operations': operations,
            'start_time': profile['start_time'],
            'end_time': profile['end_time']
        }
    
    def compare_profiles(self, profile1: str, profile2: str) -> Dict[str, Any]:
        """Compare two profiles"""
        stats1 = self.get_profile_stats(profile1)
        stats2 = self.get_profile_stats(profile2)
        
        if not stats1 or not stats2:
            return {}
        
        # Compare by operation
        operations = set()
        operations.update(stats1['operations'].keys())
        operations.update(stats2['operations'].keys())
        
        comparison = {}
        for op in operations:
            op_stats1 = stats1['operations'].get(op, {})
            op_stats2 = stats2['operations'].get(op, {})
            
            if op_stats1['count'] > 0 and op_stats2['count'] > 0:
                ratio = op_stats2['average_time'] / op_stats1['average_time']
                comparison[op] = {
                    'profile1_average': op_stats1['average_time'],
                    'profile2_average': op_stats2['average_time'],
                    'improvement_ratio': ratio,
                    'improvement_percent': (ratio - 1) * 100
                }
        
        return {
            'duration_ratio': stats2['total_duration'] / stats1['total_duration'] if stats1['total_duration'] > 0 else 0,
            'sample_count_ratio': stats2['total_samples'] / stats1['total_samples'] if stats1['total_samples'] > 0 else 0,
            'operation_comparison': comparison
        }


class BenchmarkRunner:
    """Benchmark runner for NLM operations"""
    
    def __init__(self):
        self.benchmark_results = []
    
    def run_benchmark(self, name: str, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Run a benchmark"""
        # Warm-up
        for _ in range(3):
            func(*args, **kwargs)
        
        # Actual benchmark
        times = []
        for _ in range(10):
            start_time = time.perf_counter()
            result = func(*args, **kwargs)
            end_time = time.perf_counter()
            times.append(end_time - start_time)
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        std_dev = (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
        
        result = {
            'name': name,
            'result': result,
            'times': times,
            'average_time': avg_time,
            'min_time': min_time,
            'max_time': max_time,
            'std_dev': std_dev,
            'operations_per_second': 1.0 / avg_time if avg_time > 0 else 0.0
        }
        
        self.benchmark_results.append(result)
        return result
    
    def get_benchmark_summary(self) -> Dict[str, Any]:
        """Get benchmark summary"""
        if not self.benchmark_results:
            return {}
        
        summary = {
            'total_benchmarks': len(self.benchmark_results),
            'benchmarks': []
        }
        
        for result in self.benchmark_results:
            summary['benchmarks'].append({
                'name': result['name'],
                'average_time': result['average_time'],
                'min_time': result['min_time'],
                'max_time': result['max_time'],
                'std_dev': result['std_dev'],
                'operations_per_second': result['operations_per_second']
            })
        
        return summary

    def compare_with_baseline(self, baseline_name: str) -> Dict[str, Any]:
        """Compare current benchmarks with baseline"""
        baseline = None
        current = []
        
        for result in self.benchmark_results:
            if result['name'] == baseline_name:
                baseline = result
            else:
                current.append(result)
        
        if not baseline:
            return {'error': f"Baseline benchmark '{baseline_name}' not found"}
        
        comparison = {
            'baseline': baseline['name'],
            'baseline_stats': {
                'average_time': baseline['average_time'],
                'operations_per_second': baseline['operations_per_second']
            },
            'comparisons': []
        }
        
        for result in current:
            ratio = result['average_time'] / baseline['average_time']
            comparison['comparisons'].append({
                'name': result['name'],
                'average_time': result['average_time'],
                'improvement_ratio': ratio,
                'improvement_percent': (ratio - 1) * 100
            })
        
        return comparison


# Add performance monitoring to error handling
    
    @staticmethod
    def collect_diagnostics(exception: Exception) -> DiagnosticInfo:
        """Collect diagnostics information for an exception"""
        info = ErrorReporter.DiagnosticInfo(
            error_type=type(exception).__name__,
            error_message=str(exception),
            stack_trace=traceback.format_exc(),
            system_state=ErrorReporter._get_system_state(),
            performance_impact=ErrorReporter._analyze_performance_impact(exception)
        )
        return info
    
    @staticmethod
    def _get_system_state() -> str:
        """Get current system state information"""
        import platform
        import psutil
        
        state = []
        state.append(f"Platform: {platform.system()} {platform.release()}")
        state.append(f"Python: {platform.python_version()}")
        
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            state.append(f"Memory: RSS={memory_info.rss / 1024 / 1024:.1f}MB, VMS={memory_info.vms / 1024 / 1024:.1f}MB")
            state.append(f"CPU: {process.cpu_percent()}%")
        except ImportError:
            state.append("psutil: not available")
        except Exception as e:
            state.append(f"Memory check failed: {e}")
        
        return "; ".join(state)
    
    @staticmethod
    def _analyze_performance_impact(exception: Exception) -> str:
        """Analyze performance impact of the error"""
        # This is a simplified implementation
        if isinstance(exception, (ValidationException, MemoryException)):
            return "Low impact - error is recoverable"
        elif isinstance(exception, (BrainException, SimulationException)):
            return "High impact - may require restart"
        elif isinstance(exception, ConfigurationException):
            return "Medium impact - affects system configuration"
        else:
            return "Unknown impact"
    
    @staticmethod
    def generate_report(diagnostic: DiagnosticInfo, logger: logging.Logger) -> str:
        """Generate a human-readable error report"""
        report = f"Error Report\n"
        report += f"Timestamp: {datetime.now().isoformat()}\n"
        report += f"Error Type: {diagnostic.error_type}\n"
        report += f"Error Message: {diagnostic.error_message}\n"
        report += f"System State: {diagnostic.system_state}\n"
        report += f"Performance Impact: {diagnostic.performance_impact}\n"
        
        if diagnostic.context:
            report += "Context:\n"
            for key, value in diagnostic.context.items():
                report += f"  {key}: {value}\n"
        
        if diagnostic.recovery_suggestions:
            report += "Recovery Suggestions:\n"
            for suggestion in diagnostic.recovery_suggestions:
                report += f"  - {suggestion}\n"
        
        # Log the report
        logger.error(report)
        
        return report

# Error handling infrastructure
class ErrorHandling:
    """Centralized error handling infrastructure"""
    
    class ErrorHandlerConfig:
        def __init__(self):
            self.default_log_level = logging.WARNING
            self.error_reporting_enabled = True
            self.error_serialization_enabled = True
            self.error_recovery_enabled = True
            self.circuit_breaker_enabled = True
            self.graceful_degradation_enabled = True
            self.retry_enabled = True
            self.max_error_history = 100
            self.error_persistence_file = "error_history.json"
    
    def __init__(self, config: Optional[ErrorHandlerConfig] = None):
        self.config = config or self.ErrorHandlerConfig()
        self.error_history: List[Dict[str, Any]] = []
        self.error_handlers: List[Callable[[NLMException], None]] = []
        self.logger = logging.getLogger(__name__)
        
        # Setup logging
        logging.basicConfig(
            level=self.config.default_log_level,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get comprehensive performance statistics"""
        return {
            'brain_stats': self.performance_monitor.get_all_stats(),
            'profiler_stats': {name: self.performance_profiler.get_profile_stats(name)
                             for name in self.performance_profiler.profiles},
            'benchmark_summary': self.benchmark_runner.get_benchmark_summary(),
            'system_stats': self._get_system_stats()
        }

    def _get_system_stats(self) -> Dict[str, Any]:
        """Get system performance statistics"""
        try:
            import psutil
            process = psutil.Process()
            return {
                'memory_usage_mb': process.memory_info().rss / 1024 / 1024,
                'cpu_percent': process.cpu_percent(),
                'threads': process.num_threads(),
                'open_files': len(process.open_files()),
                'connections': len(process.connections())
            }
        except ImportError:
            return {'psutil': 'not available'}
        except Exception as e:
            return {'error': f"Failed to get system stats: {e}"}

    def export_performance_report(self, format: str = "json") -> str:
        """Export performance report in specified format"""
        stats = self.get_performance_stats()
        
        if format == "json":
            return json.dumps(stats, indent=2, default=str)
        elif format == "yaml":
            return yaml.dump(stats, default_flow_style=False)
        elif format == "text":
            report = "Performance Report\n"
            report += f"Generated: {datetime.now().isoformat()}\n\n"
            
            report += "Brain Operations:\n"
            for op, op_stats in stats['brain_stats'].items():
                report += f"  {op}: {op_stats}\n"
            
            report += "\nProfiler Statistics:\n"
            for profile, profile_stats in stats['profiler_stats'].items():
                report += f"  {profile}: {profile_stats}\n"
            
            report += f"\nBenchmark Results:\n{stats['benchmark_summary']}\n"
            
            return report
        else:
            raise ValueError(f"Unsupported format: {format}")

    def generate_performance_alerts(self) -> List[str]:
        """Generate performance alerts based on metrics"""
        alerts = []
        stats = self.get_performance_stats()
        
        # Check for slow operations
        for op, op_stats in stats['brain_stats'].items():
            if op_stats.get('average_time', 0) > 10.0:  # 10 seconds threshold
                alerts.append(f"Operation '{op}' has high average time: {op_stats['average_time']:.2f}s")
        
        # Check for memory issues
        system_stats = stats.get('system_stats', {})
        if isinstance(system_stats, dict) and 'memory_usage_mb' in system_stats:
            memory_mb = system_stats['memory_usage_mb']
            if memory_mb > 1024:  # 1GB threshold
                alerts.append(f"High memory usage: {memory_mb:.1f}MB")
        
        # Check for CPU issues
        if isinstance(system_stats, dict) and 'cpu_percent' in system_stats:
            cpu_percent = system_stats['cpu_percent']
            if cpu_percent > 90:  # 90% threshold
                alerts.append(f"High CPU usage: {cpu_percent}%")
        
        return alerts

    def optimize_configuration(self) -> Dict[str, Any]:
        """Optimize configuration based on performance analysis"""
        stats = self.get_performance_stats()
        optimizations = {
            'suggested_optimizations': [],
            'recommended_changes': [],
            'performance_improvements': {}
        }
        
        # Analyze slow operations
        for op, op_stats in stats['brain_stats'].items():
            avg_time = op_stats.get('average_time', 0)
            if avg_time > 5.0:  # 5 seconds threshold
                optimizations['suggested_optimizations'].append(
                    f"Optimize operation '{op}': average time {avg_time:.2f}s"
                )
                optimizations['recommended_changes'].append(
                    f"Consider batch processing for '{op}' or increase system resources"
                )
        
        # Analyze memory usage
        system_stats = stats.get('system_stats', {})
        if isinstance(system_stats, dict) and 'memory_usage_mb' in system_stats:
            memory_mb = system_stats['memory_usage_mb']
            if memory_mb > 512:  # 512MB threshold
                optimizations['suggested_optimizations'].append(
                    f"Reduce memory usage: {memory_mb:.1f}MB"
                )
                optimizations['recommended_changes'].append(
                    "Consider using memory-efficient data structures or enable garbage collection"
                )
        
        return optimizations

    def monitor_simulation_performance(self, brain, world, max_steps: int = 1000) -> Dict[str, Any]:
        """Monitor performance during simulation"""
        start_time = time.perf_counter()
        self.performance_profiler.start_profile("simulation")
        
        step_times = []
        memory_samples = []
        
        try:
            for step in range(max_steps):
                step_start = time.perf_counter()
                
                # Perform simulation step
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                step_time = time.perf_counter() - step_start
                step_times.append(step_time)
                
                # Sample memory every 100 steps
                if step % 100 == 0:
                    try:
                        import psutil
                        process = psutil.Process()
                        memory_mb = process.memory_info().rss / 1024 / 1024
                        memory_samples.append(memory_mb)
                    except ImportError:
                        pass
                
                # Add sample to profiler
                self.performance_profiler.add_sample(f"step_{step}", 
                                                   memory_samples[-1] if memory_samples else None)
                
                # Performance alert if step takes too long
                if step_time > 1.0:  # 1 second threshold
                    self.logger.warning(f"Step {step} took {step_time:.3f}s")
            
            total_time = time.perf_counter() - start_time
            self.performance_profiler.end_profile("simulation")
            
            return {
                'total_steps': max_steps,
                'total_time': total_time,
                'average_step_time': sum(step_times) / len(step_times) if step_times else 0,
                'min_step_time': min(step_times) if step_times else 0,
                'max_step_time': max(step_times) if step_times else 0,
                'memory_samples': memory_samples,
                'steps_per_second': max_steps / total_time if total_time > 0 else 0,
                'profiler_stats': self.performance_profiler.get_profile_stats("simulation")
            }
            
        except Exception as e:
            self.performance_profiler.end_profile("simulation")
            raise ErrorHandling.handle_error_with_recovery(
                SimulationException(f"Performance monitoring failed: {e}"),
                self.error_config
            )

    def compare_performance(self, brain1: Any, brain2: Any, world: Any, steps: int = 100) -> Dict[str, Any]:n        """Compare performance of two brain configurations"""
        start_time = time.perf_counter()
        self.performance_profiler.start_profile("performance_comparison")
        
        results = {
            'brain1_stats': {},
            'brain2_stats': {},
            'performance_comparison': {}
        }
        
        try:
            # Test brain 1
            world.reset()
            brain1.initialize()
            self.performance_profiler.start_profile("brain1_test")
            
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent1.processSensoryInput(percept)
                brain1.step(step)
                action1 = agent1.decodeMotorCommand()
                world.applyMotorCommand(action1, world.getSimulationTime())
            
            brain1_duration = self.performance_profiler.end_profile("brain1_test")
            results['brain1_stats'] = self.performance_profiler.get_profile_stats("brain1_test")
            
            # Test brain 2
            world.reset()
            brain2.initialize()
            self.performance_profiler.start_profile("brain2_test")
            
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent2.processSensoryInput(percept)
                brain2.step(step)
                action2 = agent2.decodeMotorCommand()
                world.applyMotorCommand(action2, world.getSimulationTime())
            
            brain2_duration = self.performance_profiler.end_profile("brain2_test")
            results['brain2_stats'] = self.performance_profiler.get_profile_stats("brain2_test")
            
            # Compare results
            comparison = self.performance_profiler.compare_profiles("brain1_test", "brain2_test")
            results['performance_comparison'] = comparison
            
            total_time = time.perf_counter() - start_time
            self.performance_profiler.end_profile("performance_comparison")
            
            return results
            
        except Exception as e:
            self.performance_profiler.end_profile("performance_comparison")
            raise ErrorHandling.handle_error_with_recovery(
                BrainException(f"Performance comparison failed: {e}"),
                self.error_config
            )

    def generate_performance_report(self, format: str = "json", include_benchmarks: bool = True,
                                   include_comparisons: bool = True) -> str:
        """Generate comprehensive performance report"""
        report = {
            'generated_at': datetime.now().isoformat(),
            'system_info': self._get_system_info(),
            'brain_performance': self.performance_monitor.get_all_stats(),
            'profiler_results': {},
            'benchmark_results': self.benchmark_runner.get_benchmark_summary() if include_benchmarks else {},
            'performance_alerts': self.generate_performance_alerts(),
            'optimization_suggestions': self.optimize_configuration()
        }
        
        if include_comparisons:
            report['performance_comparisons'] = {}
        
        if format == "json":
            return json.dumps(report, indent=2, default=str)
        elif format == "yaml":
            return yaml.dump(report, default_flow_style=False)
        elif format == "text":
            return self._generate_text_report(report)
        else:
            raise ValueError(f"Unsupported format: {format}")

    def _generate_text_report(self, report: Dict[str, Any]) -> str:
        """Generate text format performance report"""
        text = "NLM Performance Report\n"
        text += f"Generated: {report['generated_at']}\n\n"
        
        text += "System Information:\n"
        for key, value in report['system_info'].items():
            text += f"  {key}: {value}\n"
        
        text += "\nBrain Performance Statistics:\n"
        for op, stats in report['brain_performance'].items():
            text += f"  {op}: {stats}\n"
        
        text += "\nPerformance Alerts:\n"
        for alert in report['performance_alerts']:
            text += f"  WARNING: {alert}\n"
        
        text += "\nOptimization Suggestions:\n"
        for suggestion in report['optimization_suggestions']['suggested_optimizations']:
            text += f"  {suggestion}\n"
        
        if report['benchmark_results']:
            text += "\nBenchmark Results:\n"
            text += json.dumps(report['benchmark_results'], indent=2)
        
        return text

    def export_performance_data(self, file_path: str):
        """Export performance data to file"""
        try:
            with open(file_path, 'w') as f:
                f.write(self.generate_performance_report("json"))
        except Exception as e:
            self.logger.error(f"Failed to export performance data: {e}")
            raise ErrorHandling.handle_error_with_recovery(
                IOError(f"Failed to export performance data: {e}"),
                self.error_config
            )

    def import_performance_data(self, file_path: str):
        """Import performance data from file"""
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
            
            # Import performance data
            self.performance_monitor.metrics = data.get('brain_performance', {})
            self.benchmark_runner.benchmark_results = data.get('benchmark_results', [])
            
        except Exception as e:
            self.logger.error(f"Failed to import performance data: {e}")
            raise ErrorHandling.handle_error_with_recovery(
                IOError(f"Failed to import performance data: {e}"),
                self.error_config
            )

    def run_performance_test_suite(self, test_name: str) -> Dict[str, Any]:
        """Run comprehensive performance test suite"""
        test_start_time = time.perf_counter()
        
        try:
            # Run benchmark tests
            benchmark_results = {}
            for brain_type in ['simple', 'advanced', 'optimized']:
                benchmark_name = f"{test_name}_{brain_type}_benchmark"
                benchmark_result = self.benchmark_runner.run_benchmark(
                    benchmark_name,
                    self._run_brain_test,
                    brain_type
                )
                benchmark_results[f"{brain_type}_benchmark"] = benchmark_result
            
            # Run simulation performance tests
            simulation_results = {}
            for world_size in ['small', 'medium', 'large']:
                sim_name = f"{test_name}_{world_size}_simulation"
                # This would need to be implemented based on specific test requirements
                simulation_results[f"{world_size}_simulation"] = {
                    'status': 'pending',
                    'test_name': sim_name
                }
            
            # Compare results
            comparison = {}
            if len(benchmark_results) > 1:
                benchmarks = list(benchmark_results.values())
                baseline = benchmarks[0]
                comparison = self.benchmark_runner.compare_with_baseline(baseline['name'])
            
            total_time = time.perf_counter() - test_start_time
            
            return {
                'test_name': test_name,
                'total_time': total_time,
                'benchmark_results': benchmark_results,
                'simulation_results': simulation_results,
                'comparison': comparison,
                'performance_stats': self.get_performance_stats()
            }
            
        except Exception as e:
            raise ErrorHandling.handle_error_with_recovery(
                BrainException(f"Performance test suite failed: {e}"),
                self.error_config
            )

    def _run_brain_test(self, brain_type: str) -> Dict[str, Any]:
        """Internal method to run brain test"""
        # This would implement the actual brain test based on brain_type
        return {
            'brain_type': brain_type,
            'test_result': 'passed',
            'metrics': {}
        }

    def cleanup_performance_data(self):
        """Clean up performance data"""
        self.performance_monitor.reset()
        self.benchmark_runner.benchmark_results.clear()
        self.performance_profiler.profiles.clear()
        
        # Save performance data if configured
        if self.config.get('performance', {}).get('save_data', True):
            try:
                data_file = self.config.get('performance', {}).get('data_file', 'performance_data.json')
                self.export_performance_data(data_file)
            except Exception as e:
                self.logger.warning(f"Failed to save performance data: {e}")

# Global error handler
_global_error_handler: Optional[ErrorHandling] = None

def get_global_error_handler() -> ErrorHandling:
    """Get the global error handler instance"""
    global _global_error_handler
    if _global_error_handler is None:
        _global_error_handler = ErrorHandling()
    return _global_error_handler

def set_global_error_handler(handler: ErrorHandling):
    """Set the global error handler instance"""
    global _global_error_handler
    _global_error_handler = handler

def initialize_error_handling(config: Optional[ErrorHandling.ErrorHandlerConfig] = None):
    """Initialize the error handling system"""
    handler = ErrorHandling(config)
    set_global_error_handler(handler)
    return handler

def handle_exception(exception: Exception):
    """Handle exception with global error handler"""
    global_error_handler = get_global_error_handler()
    
    if isinstance(exception, NLMException):
        global_error_handler.handle_error(exception)
    else:
        # Convert non-NLM exceptions to NLM exceptions
        nlm_error = NLMException(
            ErrorCode.SYSTEM_ERROR,
            str(exception),
            cause=exception
        )
        global_error_handler.handle_error(nlm_error)

# Convenience macros for error handling
import functools

def safe_call(func: Callable, *args, **kwargs) -> Any:
    """Safely call a function with error handling"""
    try:
        return func(*args, **kwargs)
    except NLMException:
        raise
    except Exception as e:
        raise NLMException(ErrorCode.SYSTEM_ERROR, f"Error calling {func.__name__}: {str(e)}") from e

def validate_call(func: Callable) -> Callable:
    """Decorator to validate function calls"""
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # Validate arguments (example implementation)
        for arg in args:
            Validation.validate_not_null(arg)
        
        return func(*args, **kwargs)
    return wrapper

def with_error_handler(config: ErrorRecovery.RecoveryConfig):
    """Decorator to add error handling to a function"""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            try:
                return ErrorRecovery.with_retry(config, func, *args, **kwargs)
            except NLMException:
                raise
            except Exception as e:
                raise NLMException(ErrorCode.SYSTEM_ERROR, f"Error in {func.__name__}: {str(e)}") from e
        return wrapper
    return decorator

# Initialize default error handling
def _init_default_error_handling():
    """Initialize default error handling configuration"""
    config = ErrorHandling.ErrorHandlerConfig()
    config.default_log_level = logging.WARNING
    config.error_reporting_enabled = True
    config.error_recovery_enabled = True
    config.circuit_breaker_enabled = True
    config.graceful_degradation_enabled = True
    config.retry_enabled = True
    
    initialize_error_handling(config)

# Auto-initialize
_init_default_error_handling()

# Global exception handler
import sys

class GlobalExceptionHandler:
    """Global exception handler for the entire application"""
    
    def __init__(self):
        self.original_excepthook = sys.excepthook
        self.error_handler = get_global_error_handler()
    
    def __enter__(self):
        sys.excepthook = self._handle_exception
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            self._handle_exception(exc_type, exc_val, exc_tb)
    
    def _handle_exception(self, exc_type, exc_value, exc_traceback):
        """Handle unhandled exceptions"""
        if issubclass(exc_type, NLMException):
            nlm_exception = exc_value
            self.error_handler.handle_error(nlm_exception)
        else:
            # Convert to NLM exception
            nlm_exception = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Unhandled exception: {str(exc_value)}",
                cause=exc_value,
                file=exc_traceback.tb_frame.f_code.co_filename,
                line=exc_traceback.tb_lineno,
                function=exc_traceback.tb_frame.f_code.co_name
            )
            self.error_handler.handle_error(nlm_exception)
        
        # Call original handler
        self.original_excepthook(exc_type, exc_value, exc_traceback)

# Usage example
if __name__ == "__main__":
    # Example usage
    try:
        # Test error handling
        Validation.validate_range(15, 0, 10)
    except ValidationException as e:
        print(f"Caught validation error: {e}")
    
    try:
        # Test error recovery
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.RETRY,
            max_retries=2
        )
        
        def failing_function():
            raise ValidationException("This function fails")
        
        # This will retry and then fail
        ErrorRecovery.with_retry(config, failing_function)
    except Exception as e:
        print(f"Caught expected error: {e}")
    
    print("Error handling system initialized successfully!")