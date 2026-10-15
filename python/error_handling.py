#!/usr/bin/env python3
"""
NLM Error Handling and Memory Management

Comprehensive error handling and memory management utilities for NLM.
"""

import sys
import gc
import traceback
import weakref
import threading
import time
from pathlib import Path
from typing import Dict, List, Optional, Any, Callable, Union
from contextlib import contextmanager
import psutil
import resource
import warnings

# Import NLM modules
import pynlm

__all__ = [
    'ErrorHandler',
    'MemoryManager',
    'SafeExecutor',
    'ResourceMonitor',
    'createErrorHandler',
    'createMemoryManager',
    'createSafeExecutor',
    'createResourceMonitor'
]

class MemoryPool:
    """
    Memory pool for efficient allocation and deallocation.
    
    This class provides a memory pool to reduce memory allocation overhead
    and prevent memory fragmentation.
    """
    
    def __init__(self, initial_size: int = 1024, growth_increment: int = 512):
        """
        Initialize memory pool.
        
        Args:
            initial_size: Initial pool size
            growth_increment: Pool growth increment
        """
        self.pool = []
        self.lock = threading.Lock()
        self.initial_size = initial_size
        self.growth_increment = growth_increment
        self.size = initial_size
        
        # Pre-allocate initial pool
        for _ in range(initial_size):
            self.pool.append([None] * 10)  # Simulate object storage
    
    def allocate(self) -> List[Any]:
        """
        Allocate memory from pool.
        
        Returns:
            Allocated memory block
        """
        with self.lock:
            if not self.pool:
                # Grow pool
                for _ in range(self.growth_increment):
                    self.pool.append([None] * 10)
            
            return self.pool.pop()
    
    def deallocate(self, memory: List[Any]):
        """
        Deallocate memory to pool.
        
        Args:
            memory: Memory block to deallocate
        """
        with self.lock:
            if len(self.pool) < self.size:
                # Clear memory before returning to pool
                for i in range(len(memory)):
                    memory[i] = None
                self.pool.append(memory)
    
    def stats(self) -> Dict[str, Any]:
        """Get pool statistics."""
        with self.lock:
            return {
                'pool_size': self.size,
                'available_blocks': len(self.pool),
                'utilization': 1.0 - len(self.pool) / self.size if self.size > 0 else 0.0
            }

class ObjectRegistry:
    """
    Registry for tracking NLM objects and their lifetimes.
    
    This class provides tracking for NLM objects to:
    - Prevent memory leaks
    - Track object creation and destruction
    - Manage object dependencies
    """
    
    def __init__(self):
        self.objects = weakref.WeakValueDictionary()
        self.registry_lock = threading.Lock()
        self.creation_events = []
        self.destruction_events = []
    
    def register(self, obj_id: str, obj: Any, obj_type: str, metadata: Optional[Dict] = None):
        """
        Register object.
        
        Args:
            obj_id: Object ID
            obj: Object to register
            obj_type: Object type
            metadata: Object metadata
        """
        with self.registry_lock:
            self.objects[obj_id] = {
                'object': obj,
                'type': obj_type,
                'metadata': metadata or {},
                'created': time.time(),
                'last_access': time.time()
            }
            
            self.creation_events.append({
                'timestamp': time.time(),
                'object_id': obj_id,
                'type': obj_type,
                'action': 'created'
            })
    
    def unregister(self, obj_id: str):
        """
        Unregister object.
        
        Args:
            obj_id: Object ID
        """
        with self.registry_lock:
            if obj_id in self.objects:
                obj_info = self.objects.pop(obj_id)
                
                self.destruction_events.append({
                    'timestamp': time.time(),
                    'object_id': obj_id,
                    'type': obj_info['type'],
                    'action': 'destroyed'
                })
    
    def get(self, obj_id: str) -> Optional[Any]:
        """
        Get object by ID.
        
        Args:
            obj_id: Object ID
            
        Returns:
            Object or None if not found
        """
        with self.registry_lock:
            if obj_id in self.objects:
                self.objects[obj_id]['last_access'] = time.time()
                return self.objects[obj_id]['object']
            return None
    
    def list_objects(self, obj_type: Optional[str] = None) -> List[str]:
        """
        List registered objects.
        
        Args:
            obj_type: Optional object type filter
            
        Returns:
            List of object IDs
        """
        with self.registry_lock:
            if obj_type:
                return [obj_id for obj_id, obj_info in self.objects.items() 
                       if obj_info['type'] == obj_type]
            else:
                return list(self.objects.keys())
    
    def cleanup(self, min_age_seconds: float = 300.0):
        """
        Clean up old objects.
        
        Args:
            min_age_seconds: Minimum object age for cleanup
        """
        with self.registry_lock:
            current_time = time.time()
            to_remove = []
            
            for obj_id, obj_info in self.objects.items():
                if current_time - obj_info['last_access'] > min_age_seconds:
                    to_remove.append(obj_id)
            
            for obj_id in to_remove:
                self.unregister(obj_id)
            
            return len(to_remove)
    
    def stats(self) -> Dict[str, Any]:
        """Get registry statistics."""
        with self.registry_lock:
            current_time = time.time()
            total_objects = len(self.objects)
            
            # Calculate average age
            total_age = 0.0
            for obj_info in self.objects.values():
                total_age += current_time - obj_info['created']
            
            avg_age = total_age / total_objects if total_objects > 0 else 0.0
            
            return {
                'total_objects': total_objects,
                'average_age_seconds': avg_age,
                'creation_events': len(self.creation_events),
                'destruction_events': len(self.destruction_events)
            }

class ErrorHandler:
    """
    Comprehensive error handler for NLM.
    
    This class provides robust error handling including:
    - Error classification
    - Error recovery
    - Error reporting
    - Error prevention
    """
    
    def __init__(self, error_log_path: Optional[str] = None, 
                 max_errors: int = 1000, error_level: str = "warning"):
        """
        Initialize error handler.
        
        Args:
            error_log_path: Path to error log file
            max_errors: Maximum number of errors to store
            error_level: Error reporting level
        """
        self.error_log_path = error_log_path
        self.max_errors = max_errors
        self.error_level = error_level
        
        # Error storage
        self.errors = []
        self.error_lock = threading.Lock()
        
        # Error handlers
        self.error_handlers = {}
        self.register_default_handlers()
        
        # Statistics
        self.stats = {
            'total_errors': 0,
            'error_types': {},
            'recovery_attempts': 0,
            'successful_recoveries': 0
        }
    
    def register_default_handlers(self):
        """Register default error handlers."""
        self.error_handlers = {
            'memory_error': self._handle_memory_error,
            'type_error': self._handle_type_error,
            'value_error': self._handle_value_error,
            'runtime_error': self._handle_runtime_error,
            'import_error': self._handle_import_error,
            'attribute_error': self._handle_attribute_error,
            'key_error': self._handle_key_error
        }
    
    def register_handler(self, error_type: str, handler: Callable):
        """
        Register custom error handler.
        
        Args:
            error_type: Error type
            handler: Error handler function
        """
        self.error_handlers[error_type] = handler
    
    def handle_error(self, error: Exception, context: Optional[Dict] = None):
        """
        Handle error.
        
        Args:
            error: Error to handle
            context: Error context
        """
        if context is None:
            context = {}
        
        # Classify error
        error_type = self._classify_error(error)
        
        # Create error record
        error_record = {
            'timestamp': time.time(),
            'error_type': error_type,
            'error_message': str(error),
            'error_details': traceback.format_exc(),
            'context': context,
            'stack_trace': traceback.format_stack()
        }
        
        # Store error
        with self.error_lock:
            self.errors.append(error_record)
            
            # Trim errors if too many
            if len(self.errors) > self.max_errors:
                self.errors = self.errors[-self.max_errors:]
        
        # Update statistics
        self.stats['total_errors'] += 1
        self.stats['error_types'][error_type] = self.stats['error_types'].get(error_type, 0) + 1
        
        # Log error
        self._log_error(error_record)
        
        # Call error handler
        if error_type in self.error_handlers:
            try:
                recovery_result = self.error_handlers[error_type](error, context, error_record)
                self.stats['recovery_attempts'] += 1
                if recovery_result:
                    self.stats['successful_recoveries'] += 1
            except Exception as handler_error:
                self.handle_error(handler_error, {'handler_context': 'error_handler'})
    
    def _classify_error(self, error: Exception) -> str:
        """Classify error type."""
        error_class = error.__class__.__name__
        
        # Map common error types to standard categories
        error_mapping = {
            'MemoryError': 'memory_error',
            'TypeError': 'type_error',
            'ValueError': 'value_error',
            'RuntimeError': 'runtime_error',
            'ImportError': 'import_error',
            'AttributeError': 'attribute_error',
            'KeyError': 'key_error',
            'IndexError': 'index_error',
            'FileNotFoundError': 'file_error',
            'PermissionError': 'permission_error'
        }
        
        return error_mapping.get(error_class, 'unknown_error')
    
    def _handle_memory_error(self, error: Exception, context: Dict, 
                           error_record: Dict) -> bool:
        """Handle memory error."""
        # Try to free memory
        try:
            gc.collect()
            
            # Check if memory error persists
            import psutil
            process = psutil.Process()
            available_memory = process.memory_info().available
            
            if available_memory < 100 * 1024 * 1024:  # 100MB
                # Try to reduce memory usage
                return self._reduce_memory_usage()
            
            return True
        except:
            return False
    
    def _handle_type_error(self, error: Exception, context: Dict,
                          error_record: Dict) -> bool:
        """Handle type error."""
        # Log for debugging
        self._log_debug_info(f"Type error: {error}", context)
        return False  # Type errors usually cannot be recovered from
    
    def _handle_value_error(self, error: Exception, context: Dict,
                           error_record: Dict) -> bool:
        """Handle value error."""
        # Try to recover by using default values
        if 'default_value' in context:
            context['original_value'] = context.get('current_value')
            context['current_value'] = context['default_value']
            return True
        return False
    
    def _handle_runtime_error(self, error: Exception, context: Dict,
                            error_record: Dict) -> bool:
        """Handle runtime error."""
        # Try to recover by restarting component
        try:
            if 'component' in context:
                context['component'].reset()
                return True
        except:
            pass
        return False
    
    def _handle_import_error(self, error: Exception, context: Dict,
                            error_record: Dict) -> bool:
        """Handle import error."""
        # Try to install missing dependency
        try:
            if 'module_name' in context:
                import importlib
                importlib.import_module(context['module_name'])
                return True
        except:
            pass
        return False
    
    def _handle_attribute_error(self, error: Exception, context: Dict,
                               error_record: Dict) -> bool:
        """Handle attribute error."""
        # Try to use default attribute
        if 'default_attribute' in context:
            context['missing_attribute'] = context.get('attribute_name')
            return True
        return False
    
    def _handle_key_error(self, error: Exception, context: Dict,
                         error_record: Dict) -> bool:
        """Handle key error."""
        # Try to use default key
        if 'default_value' in context:
            context['missing_key'] = str(error)
            return True
        return False
    
    def _log_error(self, error_record: Dict):
        """Log error to file."""
        if self.error_log_path:
            try:
                with open(self.error_log_path, 'a') as f:
                    f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] ")
                    f.write(f"ERROR TYPE: {error_record['error_type']}\n")
                    f.write(f"MESSAGE: {error_record['error_message']}\n")
                    f.write(f"CONTEXT: {error_record['context']}\n")
                    f.write(f"DETAILS: {error_record['error_details']}\n")
                    f.write("-" * 80 + "\n")
            except Exception as log_error:
                pass  # Don't let logging errors interfere with error handling
    
    def _log_debug_info(self, message: str, context: Dict):
        """Log debug information."""
        if self.error_level in ['debug', 'warning', 'error']:
            import logging
            logging.debug(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {message}")
    
    def _reduce_memory_usage(self) -> bool:
        """Reduce memory usage."""
        # Try to reduce memory usage by:
        # 1. Collecting garbage
        # 2. Clearing caches
        # 3. Reducing object counts
        
        try:
            gc.collect()
            return True
        except:
            return False
    
    def _generate_recommended_actions(self, error_type: str) -> List[str]:
        """Generate recommended actions for error."""
        recommendations = {
            'memory_error': [
                "Check for memory leaks in your code",
                "Reduce data structures size",
                "Clear caches and delete unused objects",
                "Consider using memory pools for allocation"
            ],
            'type_error': [
                "Check variable types before operations",
                "Add type conversion or validation",
                "Use isinstance() checks"
            ],
            'value_error': [
                "Validate input values",
                "Use default values for invalid inputs",
                "Add input range validation"
            ],
            'runtime_error': [
                "Check for resource constraints",
                "Add timeout handling",
                "Implement proper error recovery"
            ]
        }
        
        return recommendations.get(error_type, ["Review error details and implementation"])
    
    def get_errors(self, error_type: Optional[str] = None, 
                   since_time: Optional[float] = None) -> List[Dict]:
        """
        Get errors.
        
        Args:
            error_type: Optional error type filter
            since_time: Optional timestamp filter
            
        Returns:
            List of errors
        """
        with self.error_lock:
            filtered_errors = self.errors.copy()
            
            if error_type:
                filtered_errors = [e for e in filtered_errors if e['error_type'] == error_type]
            
            if since_time:
                filtered_errors = [e for e in filtered_errors if e['timestamp'] >= since_time]
            
            return filtered_errors
    
    def clear_errors(self):
        """Clear all errors."""
        with self.error_lock:
            self.errors.clear()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get error handler statistics."""
        return self.stats.copy()

class MemoryManager:
    """
    Memory manager for NLM.
    
    This class provides memory management capabilities including:
    - Memory monitoring
    - Memory optimization
    - Memory cleanup
    - Memory profiling
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize memory manager.
        
        Args:
            brain: Brain to manage memory for
        """
        self.brain = brain
        self.memory_pools = {}
        self.memory_stats = {
            'total_allocated': 0,
            'peak_usage': 0,
            'current_usage': 0,
            'garbage_collections': 0
        }
        self.cleanup_thread = None
        self.monitoring_thread = None
        self.running = False
    
def start_monitoring(self, interval: float = 5.0):
        """
        Start memory monitoring.
        
        Args:
            interval: Monitoring interval in seconds
        """
        self.running = True
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            kwargs={'interval': interval},
            daemon=True
        )
        self.monitoring_thread.start()
    
    def stop_monitoring(self):
        """Stop memory monitoring."""
        self.running = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=10.0)
    
    def start_cleanup(self, interval: float = 60.0):
        """
        Start memory cleanup.
        
        Args:
            interval: Cleanup interval in seconds
        """
        self.cleanup_thread = threading.Thread(
            target=self._cleanup_loop,
            kwargs={'interval': interval},
            daemon=True
        )
        self.cleanup_thread.start()
    
    def stop_cleanup(self):
        """Stop memory cleanup."""
        if self.cleanup_thread:
            self.cleanup_thread.join(timeout=10.0)
    
    def _monitoring_loop(self, interval: float):
        """Monitoring loop."""
        while self.running:
            try:
                import psutil
                process = psutil.Process()
                memory_info = process.memory_info()
                
                self.memory_stats['current_usage'] = memory_info.rss
                
                # Log warnings if memory usage is high
                if memory_info.rss > 512 * 1024 * 1024:  # 512MB
                    warnings.warn(f"High memory usage: {memory_info.rss / 1024 / 1024:.1f} MB")
                
                time.sleep(interval)
            except:
                time.sleep(interval)
    
    def _cleanup_loop(self, interval: float):
        """Cleanup loop."""
        while self.running:
            try:
                # Gentle cleanup every cycle
                self._gentle_cleanup()
                
                time.sleep(interval)
            except:
                time.sleep(interval)
            except Exception:
                time.sleep(interval)
    
    def _cleanup_memory(self, interval: float):
        """Clean up memory periodically."""
        while self.running:
            try:
                # Gentle cleanup every cycle
                self._gentle_cleanup()
                
                time.sleep(interval)
            except Exception:
                time.sleep(interval)
    
    def _gentle_cleanup(self):
        """Perform gentle cleanup."""
        # Collect garbage
        gc.collect()
    
    def _aggressive_cleanup(self):
        """Perform aggressive cleanup."""
        # Clear caches
        gc.collect()
        
        # Force cleanup of non-essential objects
        # (This is a simplified implementation)
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics."""
        return self.memory_stats.copy()
    
    def get_pool_stats(self) -> Dict[str, Any]:
        """Get pool statistics."""
        stats = {}
        for pool_name, pool in self.memory_pools.items():
            stats[pool_name] = pool.stats()
        return stats

class SafeExecutor:
    """
    Safe executor for NLM operations.
    
    This class provides safe execution capabilities including:
    - Operation validation
    - Error recovery
    - Transaction management
    - Rollback capabilities
    """
    
    def __init__(self, error_handler: Optional[ErrorHandler] = None):
        """
        Initialize safe executor.
        
        Args:
            error_handler: Error handler to use
        """
        self.error_handler = error_handler or createErrorHandler()
        self.transaction_stack = []
        self.execution_lock = threading.Lock()
    
    @contextmanager
    def transaction(self, transaction_name: str):
        """
        Execute operation in transaction.
        
        Args:
            transaction_name: Transaction name
            
        Yields:
            Transaction context
        """
        transaction_context = {
            'name': transaction_name,
            'start_time': time.time(),
            'operations': []
        }
        
        try:
            self.transaction_stack.append(transaction_context)
            yield transaction_context
        except Exception as e:
            # Rollback
            self._rollback(transaction_context)
            raise
        finally:
            if self.transaction_stack and self.transaction_stack[-1] == transaction_context:
                self.transaction_stack.pop()
    
    def execute(self, operation: Callable, *args, **kwargs) -> Any:
        """
        Execute operation safely.
        
        Args:
            operation: Operation to execute
            *args: Operation arguments
            **kwargs: Operation keyword arguments
            
        Returns:
            Operation result
        """
        with self.execution_lock:
            try:
                return operation(*args, **kwargs)
            except Exception as e:
                self.error_handler.handle_error(e, {
                    'operation': operation.__name__,
                    'args': args,
                    'kwargs': kwargs,
                    'transaction': self._get_current_transaction()
                })
                raise
    
    def _get_current_transaction(self) -> Optional[Dict]:
        """Get current transaction."""
        return self.transaction_stack[-1] if self.transaction_stack else None
    
    def _rollback(self, transaction_context: Dict):
        """Rollback transaction."""
        for operation in reversed(transaction_context['operations']):
            try:
                operation['rollback']()
            except Exception as e:
                self.error_handler.handle_error(
                    e, {'action': 'rollback', 'operation': operation}
                )

class ResourceMonitor:
    """
    Resource monitor for NLM.
    
    This class provides resource monitoring capabilities including:
    - CPU monitoring
    - Memory monitoring
    - Disk monitoring
    - Network monitoring
    """
    
    def __init__(self):
        self.monitoring = False
        self.monitor_thread = None
        self.resource_data = []
        self.monitor_lock = threading.Lock()
    
    def start_monitoring(self, interval: float = 1.0):
        """
        Start resource monitoring.
        
        Args:
            interval: Monitoring interval in seconds
        """
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._resource_monitoring_loop,
            kwargs={'interval': interval},
            daemon=True
        )
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop resource monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=10.0)
    
    def _resource_monitoring_loop(self, interval: float):
        """Resource monitoring loop."""
        while self.monitoring:
            try:
                import psutil
                process = psutil.Process()
                
                resource_data = {
                    'timestamp': time.time(),
                    'cpu_percent': process.cpu_percent(),
                    'memory_percent': process.memory_percent(),
                    'memory_info': process.memory_info(),
                    'disk_io': process.io_counters(),
                    'network_io': process.net_io_counters()
                }
                
                with self.monitor_lock:
                    self.resource_data.append(resource_data)
                
                # Keep only recent data
                with self.monitor_lock:
                    if len(self.resource_data) > 1000:
                        self.resource_data = self.resource_data[-1000:]
                
                time.sleep(interval)
            except Exception:
                time.sleep(interval)
    
    def get_resource_data(self, since_time: Optional[float] = None) -> List[Dict]:
        """
        Get resource data.
        
        Args:
            since_time: Optional timestamp filter
            
        Returns:
            List of resource data
        """
        with self.monitor_lock:
            data = self.resource_data.copy()
        
        if since_time:
            data = [d for d in data if d['timestamp'] >= since_time]
        
        return data
    
    def get_current_resource_usage(self) -> Dict[str, Any]:
        """Get current resource usage."""
        with self.monitor_lock:
            if not self.resource_data:
                return {}
            
            latest_data = self.resource_data[-1]
            
            # Calculate statistics
            cpu_values = [d['cpu_percent'] for d in self.resource_data]
            memory_values = [d['memory_percent'] for d in self.resource_data]
            
            return {
                'cpu': {
                    'current': latest_data['cpu_percent'],
                    'average': sum(cpu_values) / len(cpu_values),
                    'max': max(cpu_values)
                },
                'memory': {
                    'current': latest_data['memory_percent'],
                    'average': sum(memory_values) / len(memory_values),
                    'max': max(memory_values)
                }
            }

# Convenience functions for error handling and memory management
def createErrorHandler(error_log_path: Optional[str] = None, 
                     max_errors: int = 1000, error_level: str = "warning") -> ErrorHandler:
    """Create error handler."""
    return ErrorHandler(error_log_path, max_errors, error_level)

def createMemoryManager(brain: pynlm.Brain) -> MemoryManager:
    """Create memory manager."""
    return MemoryManager(brain)

def createSafeExecutor(error_handler: Optional[ErrorHandler] = None) -> SafeExecutor:
    """Create safe executor."""
    return SafeExecutor(error_handler)

def createResourceMonitor() -> ResourceMonitor:
    """Create resource monitor."""
    return ResourceMonitor()