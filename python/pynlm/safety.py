pynlm/safety.py
Name: Safety module
Summary: Safety and security features for NLM brain control

from pynlm.interface import ScriptingInterface
import time
import threading
import inspect
import sys
from typing import Dict, List, Any, Optional, Callable, Set, Union
from dataclasses import dataclass
from enum import Enum
import os


class AccessLevel(Enum):
    """Access level enumeration"""
    NONE = 0
    READ = 1
    WRITE = 2
    EXECUTE = 3
    ADMIN = 4


class SecurityLevel(Enum):
    """Security level enumeration"""
    PUBLIC = "public"
    RESTRICTED = "restricted"
    CONFIDENTIAL = "confidential"
    SECRET = "secret"


@dataclass
class SecurityPolicy:
    """Security policy for NLM operations"""
    name: str
    description: str
    access_levels: Dict[str, AccessLevel]
    allowed_operations: List[str]
    denied_operations: List[str]
    time_restrictions: Optional[Dict[str, Any]] = None
    resource_limits: Optional[Dict[str, Any]] = None
    audit_required: bool = False
    encryption_required: bool = False
    data_classification: SecurityLevel = SecurityLevel.PUBLIC
    
    def check_access(self, operation: str, context: Dict[str, Any] = None) -> bool:
        """Check if operation is allowed by policy"""
        # Check if operation is in allowed operations
        if self.allowed_operations and operation not in self.allowed_operations:
            return False
        
        # Check if operation is in denied operations
        if self.denied_operations and operation in self.denied_operations:
            return False
        
        # Check access level
        if context and 'user_id' in context:
            user_id = context['user_id']
            if user_id in self.access_levels:
                required_level = self.access_levels[user_id]
                # Check if operation requires certain access level
                if operation in ['brain_reset', 'brain_save', 'config_modify']:
                    if required_level < AccessLevel.WRITE:
                        return False
                elif operation in ['brain_load', 'config_read']:
                    if required_level < AccessLevel.READ:
                        return False
        
        return True
    
    def check_resource_limits(self, context: Dict[str, Any]) -> bool:
        """Check if resource limits are respected"""
        if not self.resource_limits:
            return True
        
        # Check memory limits
        if 'max_memory_mb' in self.resource_limits:
            current_memory = context.get('memory_usage_mb', 0)
            max_memory = self.resource_limits['max_memory_mb']
            if current_memory > max_memory:
                return False
        
        # Check CPU limits
        if 'max_cpu_percent' in self.resource_limits:
            current_cpu = context.get('cpu_usage_percent', 0)
            max_cpu = self.resource_limits['max_cpu_percent']
            if current_cpu > max_cpu:
                return False
        
        # Check time limits
        if 'max_execution_time_ms' in self.resource_limits:
            execution_time = context.get('execution_time_ms', 0)
            max_time = self.resource_limits['max_execution_time_ms']
            if execution_time > max_time:
                return False
        
        return True
    
    def check_time_restrictions(self, context: Dict[str, Any]) -> bool:
        """Check if time restrictions are respected"""
        if not self.time_restrictions:
            return True
        
        current_time = time.time()
        current_hour = time.localtime(current_time).tm_hour
        
        # Check allowed hours
        if 'allowed_hours' in self.time_restrictions:
            allowed_hours = self.time_restrictions['allowed_hours']
            if current_hour not in allowed_hours:
                return False
        
        # Check maintenance windows
        if 'maintenance_windows' in self.time_restrictions:
            for window in self.time_restrictions['maintenance_windows']:
                if window['start_hour'] <= current_hour < window['end_hour']:
                    return False
        
        return True


class Sandbox:
    """Sandbox environment for safe code execution"""
    
    def __init__(self, name: str, security_policy: SecurityPolicy):
        self.name = name
        self.security_policy = security_policy
        self.allowed_modules = security_policy.allowed_operations
        self.denied_modules = security_policy.denied_operations
        self.resource_limits = security_policy.resource_limits
        self.time_restrictions = security_policy.time_restrictions
        self.executed_code = []
        self.code_history = []
        self.max_code_history = 1000
        self.sandbox_id = f"sandbox_{int(time.time() * 1000)}"
        self.context = {}
    
    def execute_code(self, code: str, context: Dict[str, Any] = None, 
                    timeout: Optional[float] = None) -> Any:
        """Execute code in sandbox"""
        if context is None:
            context = {}
        
        # Add sandbox to context
        context['sandbox'] = self
        context['sandbox_id'] = self.sandbox_id
        
        # Check time restrictions
        if not self.security_policy.check_time_restrictions(context):
            raise SecurityError(f"Time restrictions violated in sandbox {self.name}")
        
        # Check resource limits
        if not self.security_policy.check_resource_limits(context):
            raise SecurityError(f"Resource limits violated in sandbox {self.name}")
        
        # Parse and validate code
        self._validate_code(code)
        
        # Set up execution environment
        execution_context = self._create_execution_context(context)
        
        # Execute code with timeout
        start_time = time.time()
        
        try:
            if timeout is not None:
                # Use threading for timeout
                result_container = {'result': None, 'exception': None}
                exception_container = {'exception': None}
                
                def target():
                    try:
                        result_container['result'] = self._execute(code, execution_context)
                    except Exception as e:
                        exception_container['exception'] = e
                
                thread = threading.Thread(target=target)
                thread.start()
                thread.join(timeout=timeout)
                
                if thread.is_alive():
                    raise TimeoutError(f"Code execution timed out after {timeout}s")
                
                if exception_container['exception']:
                    raise exception_container['exception']
                
                result = result_container['result']
            else:
                result = self._execute(code, execution_context)
            
            execution_time = time.time() - start_time
            
            # Record execution
            self.executed_code.append({
                'code': code,
                'context': context,
                'result': result,
                'execution_time': execution_time,
                'timestamp': time.time(),
                'sandbox_id': self.sandbox_id
            })
            
            # Update context with execution results
            if isinstance(result, dict):
                execution_context.update(result)
            
            # Update security context
            context.update({
                'last_execution_time': execution_time,
                'total_executions': len(self.executed_code)
            })
            
            return result
            
        except Exception as e:
            execution_time = time.time() - start_time
            
            # Record failed execution
            self.executed_code.append({
                'code': code,
                'context': context,
                'error': str(e),
                'execution_time': execution_time,
                'timestamp': time.time(),
                'sandbox_id': self.sandbox_id
            })
            
            raise SecurityError(f"Code execution failed in sandbox {self.name}: {e}")
    
    def _validate_code(self, code: str):
        """Validate code for security"""
        # Check for dangerous imports
        dangerous_imports = [
            'os.system', 'os.popen', 'os.exec', 'os.fork',
            'subprocess.run', 'subprocess.call', 'subprocess.Popen',
            'eval', 'exec', 'compile',
            '__import__', 'importlib',
            'socket', 'urllib', 'http',
            'threading', 'multiprocessing', 'concurrent.futures',
            'logging', 'traceback', 'inspect'
        ]
        
        for dangerous in dangerous_imports:
            if dangerous in code:
                raise SecurityError(f"Dangerous operation detected: {dangerous}")
        
        # Check for file operations
        dangerous_file_ops = [
            'open(', 'file(', 'with open(',
            'read(', 'write(', 'delete(', 'remove(',
            'unlink(', 'rename(', 'move('
        ]
        
        for op in dangerous_file_ops:
            if op in code:
                raise SecurityError(f"File operation detected: {op}")
        
        # Check for network operations
        dangerous_network_ops = [
            'requests.get', 'requests.post', 'urllib.request',
            'socket.connect', 'socket.send', 'socket.recv',
            'ftp', 'http', 'https'
        ]
        
        for op in dangerous_network_ops:
            if op in code:
                raise SecurityError(f"Network operation detected: {op}")
    
    def _create_execution_context(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Create execution context for code execution"""
        # Create safe execution environment
        execution_context = {
            'print': print,
            'len': len,
            'range': range,
            'dict': dict,
            'list': list,
            'tuple': tuple,
            'float': float,
            'int': int,
            'str': str,
            'abs': abs,
            'max': max,
            'min': min,
            'sum': sum,
            'time': time,
            'json': json,
            'numpy': np if NUMPY_AVAILABLE else None,
            'matplotlib': plt if MATPLOTLIB_AVAILABLE else None,
            'pandas': pd if PANDAS_AVAILABLE else None,
        }
        
        # Add interface objects
        execution_context.update(context)
        
        # Add NLM objects
        execution_context.update({
            'ScriptingInterface': ScriptingInterface,
            'ScriptingMode': ScriptingMode,
            'EventType': EventType,
            'Brain': Brain,
            'Config': Config,
            'NeuronType': NeuronType,
            'SynapseType': SynapseType,
            'ActionType': ActionType,
            'Vision': Vision,
            'Audio': Audio,
            'InternalSignals': InternalSignals
        })
        
        return execution_context
    
    def _execute(self, code: str, context: Dict[str, Any]) -> Any:
        """Execute code in sandbox"""
        try:
            # Use compile and exec for safety
            compiled_code = compile(code, f'<{self.name}>', 'exec')
            exec(compiled_code, context)
            
            # Return result if any
            if '_result' in context:
                return context['_result']
            
            return {'status': 'success', 'message': 'Code executed'}
            
        except Exception as e:
            raise SecurityError(f"Execution error: {e}")
    
    def get_execution_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get execution history"""
        if limit is not None:
            return self.executed_code[-limit:]
        return self.executed_code.copy()
    
    def clear_execution_history(self):
        """Clear execution history"""
        self.executed_code.clear()
    
    def get_sandbox_info(self) -> Dict[str, Any]:
        """Get sandbox information"""
        return {
            'name': self.name,
            'sandbox_id': self.sandbox_id,
            'security_policy': self.security_policy.name,
            'allowed_modules': self.allowed_modules,
            'denied_modules': self.denied_modules,
            'execution_count': len(self.executed_code),
            'resource_limits': self.resource_limits,
            'time_restrictions': self.time_restrictions
        }


class AccessControl:
    """Access control system for NLM"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.users: Dict[str, Dict[str, Any]] = {}
        self.roles: Dict[str, List[str]] = {}
        self.permissions: Dict[str, Set[str]] = {}
        self.sessions: Dict[str, Dict[str, Any]] = {}
        self.current_session_id: Optional[str] = None
        self.session_timeout = 3600  # 1 hour
    
    def create_user(self, user_id: str, password_hash: str, role: str,
                   email: str, permissions: Optional[List[str]] = None):
        """Create a new user"""
        if user_id in self.users:
            raise ValueError(f"User already exists: {user_id}")
        
        self.users[user_id] = {
            'password_hash': password_hash,
            'role': role,
            'email': email,
            'created_at': time.time(),
            'last_login': None,
            'status': 'active'
        }
        
        if role not in self.roles:
            self.roles[role] = []
        
        if permissions is None:
            permissions = self._get_default_permissions_for_role(role)
        
        self.permissions[user_id] = set(permissions)
    
    def authenticate_user(self, user_id: str, password_hash: str) -> Optional[str]:
        """Authenticate user and create session"""
        if user_id not in self.users:
            raise ValueError(f"User not found: {user_id}")
        
        user = self.users[user_id]
        
        if user['password_hash'] != password_hash:
            raise ValueError("Invalid password")
        
        if user['status'] != 'active':
            raise ValueError(f"User account is {user['status']}")
        
        # Create session
        session_id = f"session_{int(time.time() * 1000)}_{user_id}"
        self.sessions[session_id] = {
            'user_id': user_id,
            'created_at': time.time(),
            'last_activity': time.time(),
            'ip_address': 'unknown',
            'user_agent': 'unknown'
        }
        
        user['last_login'] = time.time()
        self.current_session_id = session_id
        
        return session_id
    
    def authorize_operation(self, operation: str, session_id: str = None,
                           resource: str = None) -> bool:
        """Authorize operation for session"""
        if session_id is None:
            session_id = self.current_session_id
        
        if not session_id or session_id not in self.sessions:
            return False
        
        session = self.sessions[session_id]
        user_id = session['user_id']
        
        # Update last activity
        session['last_activity'] = time.time()
        
        # Check if user has permission
        if user_id not in self.permissions:
            return False
        
        if operation not in self.permissions[user_id]:
            return False
        
        # Check role-based permissions
        role = self.users[user_id]['role']
        role_permissions = self._get_default_permissions_for_role(role)
        
        if operation in role_permissions:
            return True
        
        return True
    
    def add_role_permission(self, role: str, permission: str):
        """Add permission to role"""
        if role not in self.roles:
            self.roles[role] = []
        
        if permission not in self.roles[role]:
            self.roles[role].append(permission)
        
        # Add to all users with this role
        for user_id, user in self.users.items():
            if user['role'] == role:
                self.permissions[user_id].add(permission)
    
    def remove_role_permission(self, role: str, permission: str):
        """Remove permission from role"""
        if role in self.roles and permission in self.roles[role]:
            self.roles[role].remove(permission)
        
        # Remove from all users with this role
        for user_id, user in self.users.items():
            if user['role'] == role:
                self.permissions[user_id].discard(permission)
    
    def get_user_permissions(self, user_id: str) -> Set[str]:
        """Get user permissions"""
        return self.permissions.get(user_id, set())
    
    def get_role_permissions(self, role: str) -> List[str]:
        """Get role permissions"""
        return self.roles.get(role, [])
    
    def get_user_info(self, user_id: str) -> Dict[str, Any]:
        """Get user information"""
        return self.users.get(user_id, {}).copy()
    
    def list_users(self) -> List[str]:
        """List all users"""
        return list(self.users.keys())
    
    def list_roles(self) -> List[str]:
        """List all roles"""
        return list(self.roles.keys())
    
    def cleanup_expired_sessions(self, timeout: Optional[float] = None):
        """Clean up expired sessions"""
        if timeout is None:
            timeout = self.session_timeout
        
        current_time = time.time()
        expired_sessions = []
        
        for session_id, session in self.sessions.items():
            if current_time - session['last_activity'] > timeout:
                expired_sessions.append(session_id)
        
        for session_id in expired_sessions:
            del self.sessions[session_id]
        
        return len(expired_sessions)
    
    def _get_default_permissions_for_role(self, role: str) -> List[str]:
        """Get default permissions for role"""
        default_permissions = {
            'admin': ['brain_read', 'brain_write', 'brain_execute', 'config_read', 
                     'config_write', 'simulation_start', 'simulation_stop', 
                     'simulation_step', 'plugin_load', 'plugin_unload', 
                     'user_manage', 'security_manage'],
            'operator': ['brain_read', 'brain_write', 'brain_execute', 
                        'config_read', 'simulation_start', 'simulation_stop',
                        'simulation_step', 'plugin_load'],
            'user': ['brain_read', 'simulation_start'],
            'guest': ['brain_read']
        }
        
        return default_permissions.get(role, [])


class InputValidator:
    """Input validation for NLM operations"""
    
    def __init__(self):
        self.validation_rules: Dict[str, Callable] = {}
        self.allowed_values: Dict[str, Set[Any]] = {}
        self.pattern_validators: Dict[str, str] = {}
        self.range_validators: Dict[str, Dict[str, float]] = {}
    
    def add_validation_rule(self, field: str, validator: Callable, 
                           error_message: str = None):
        """Add validation rule"""
        self.validation_rules[field] = validator
        
        if error_message:
            # Store error message separately if needed
            pass
    
    def add_allowed_values(self, field: str, values: List[Any],
                          error_message: str = None):
        """Add allowed values for field"""
        self.allowed_values[field] = set(values)
    
    def add_pattern_validator(self, field: str, pattern: str,
                             error_message: str = None):
        """Add pattern validator for field"""
        self.pattern_validators[field] = pattern
    
    def add_range_validator(self, field: str, min_value: float,
                           max_value: float, error_message: str = None):
        """Add range validator for field"""
        self.range_validators[field] = {
            'min': min_value,
            'max': max_value
        }
    
    def validate_input(self, input_data: Dict[str, Any]) -> List[str]:
        """Validate input data"""
        errors = []
        
        for field, value in input_data.items():
            # Check allowed values
            if field in self.allowed_values and value not in self.allowed_values[field]:
                errors.append(f"Invalid value for {field}: {value}. Allowed: {list(self.allowed_values[field])}")
            
            # Check pattern validators
            if field in self.pattern_validators:
                import re
                pattern = self.pattern_validators[field]
                if not re.match(pattern, str(value)):
                    errors.append(f"Invalid pattern for {field}: {value}")
            
            # Check range validators
            if field in self.range_validators:
                range_info = self.range_validators[field]
                if value < range_info['min'] or value > range_info['max']:
                    errors.append(f"Value for {field} out of range: {value}. Must be between {range_info['min']} and {range_info['max']}")
            
            # Check custom validators
            if field in self.validation_rules:
                try:
                    if not self.validation_rules[field](value):
                        errors.append(f"Validation failed for {field}: {value}")
                except Exception as e:
                    errors.append(f"Validation error for {field}: {e}")
        
        return errors
    
    def validate_brain_config(self, config: Dict[str, Any]) -> List[str]:
        """Validate brain configuration"""
        errors = []
        
        # Validate brain parameters
        if 'neuron_count' in config:
            if not isinstance(config['neuron_count'], int) or config['neuron_count'] <= 0:
                errors.append("Invalid neuron count")
        
        if 'synapse_density' in config:
            if not isinstance(config['synapse_density'], (int, float)) or config['synapse_density'] < 0:
                errors.append("Invalid synapse density")
        
        if 'learning_rate' in config:
            if not isinstance(config['learning_rate'], (int, float)) or config['learning_rate'] < 0 or config['learning_rate'] > 1:
                errors.append("Learning rate must be between 0 and 1")
        
        # Validate simulation parameters
        if 'dt' in config:
            if not isinstance(config['dt'], (int, float)) or config['dt'] <= 0:
                errors.append("Invalid time step")
        
        if 'duration' in config:
            if not isinstance(config['duration'], (int, float)) or config['duration'] <= 0:
                errors.append("Invalid simulation duration")
        
        return errors
    
    def validate_stimulus(self, stimulus: Dict[str, Any]) -> List[str]:
        """Validate stimulus input"""
        errors = []
        
        if 'type' not in stimulus:
            errors.append("Stimulus type is required")
            return errors
        
        stimulus_type = stimulus['type']
        
        if stimulus_type == 'current':
            if 'neuron_id' not in stimulus:
                errors.append("Neuron ID is required for current stimulus")
            elif not isinstance(stimulus['neuron_id'], int) or stimulus['neuron_id'] < 0:
                errors.append("Invalid neuron ID")
        
        elif stimulus_type == 'type':
            if 'neuron_type' not in stimulus:
                errors.append("Neuron type is required for type stimulus")
            elif not isinstance(stimulus['neuron_type'], str):
                errors.append("Neuron type must be a string")
        
        elif stimulus_type == 'sensory':
            if 'sensory_type' not in stimulus:
                errors.append("Sensory type is required for sensory stimulus")
            elif stimulus['sensory_type'] not in ['vision', 'audio', 'internal']:
                errors.append("Invalid sensory type")
        
        if 'current' in stimulus:
            if not isinstance(stimulus['current'], (int, float)) or stimulus['current'] < 0:
                errors.append("Current must be a positive number")
        
        return errors


class ResourceMonitor:
    """Resource monitor for NLM system"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.monitoring = False
        self.monitor_thread = None
        self.resource_history = []
        self.max_history = 1000
        self.resource_limits: Dict[str, Any] = {}
        self.alert_thresholds: Dict[str, float] = {}
        self.event_handlers: Dict[str, List[Callable]] = {}
        self.alert_handlers: Dict[str, List[Callable]] = {}
    
    def start_monitoring(self, interval: float = 1.0):
        """Start resource monitoring"""
        if self.monitoring:
            return
        
        self.monitoring = True
        
        self.monitor_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True,
            args=(interval,)
        )
        self.monitor_thread.start()
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Resource monitoring started with {interval}s interval")
    
    def stop_monitoring(self):
        """Stop resource monitoring"""
        self.monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        logger = __import__('logging').getLogger(__name__)
        logger.info("Resource monitoring stopped")
    
    def set_resource_limits(self, **kwargs):
        """Set resource usage limits"""
        self.resource_limits.update(kwargs)
    
    def set_alert_thresholds(self, **kwargs):
        """Set alert thresholds"""
        self.alert_thresholds.update(kwargs)
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """Add event handler"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
    
    def add_alert_handler(self, alert_type: str, handler: Callable):
        """Add alert handler"""
        if alert_type not in self.alert_handlers:
            self.alert_handlers[alert_type] = []
        self.alert_handlers[alert_type].append(handler)
    
    def get_resource_usage(self) -> Dict[str, Any]:
        """Get current resource usage"""
        # In a real implementation, this would use system monitoring tools
        # For this example, we'll return mock data
        return {
            'memory_mb': 512.0,
            'cpu_percent': 45.0,
            'thread_count': 2,
            'execution_time_ms': 10.0,
            'active_connections': 1,
            'file_handles': 5
        }
    
    def is_resource_available(self) -> bool:
        """Check if resources are available"""
        usage = self.get_resource_usage()
        
        # Check memory limits
        if 'max_memory_mb' in self.resource_limits:
            if usage['memory_mb'] > self.resource_limits['max_memory_mb']:
                return False
        
        # Check CPU limits
        if 'max_cpu_percent' in self.resource_limits:
            if usage['cpu_percent'] > self.resource_limits['max_cpu_percent']:
                return False
        
        return True
    
    def get_resource_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get resource usage history"""
        if limit is not None:
            return self.resource_history[-limit:]
        return self.resource_history.copy()
    
    def clear_resource_history(self):
        """Clear resource history"""
        self.resource_history.clear()
    
    def _monitoring_loop(self, interval: float):
        """Internal monitoring loop"""
        while self.monitoring:
            try:
                # Get resource usage
                usage = self.get_resource_usage()
                
                # Add to history
                self.resource_history.append({
                    'timestamp': time.time(),
                    **usage
                })
                
                # Trim history
                if len(self.resource_history) > self.max_history:
                    self.resource_history = self.resource_history[-self.max_history:]
                
                # Check alert thresholds
                self._check_alerts(usage)
                
                # Notify event handlers
                self._notify_event_handlers(usage)
                
                time.sleep(interval)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Resource monitoring loop error: {e}")
                time.sleep(1.0)
    
    def _check_alerts(self, usage: Dict[str, Any]):
        """Check if any alerts need to be triggered"""
        for alert_type, threshold in self.alert_thresholds.items():
            if alert_type == 'memory_mb' and usage.get('memory_mb', 0) > threshold:
                self._trigger_alert('resource_exceeded', {
                    'resource': 'memory',
                    'current': usage['memory_mb'],
                    'threshold': threshold
                })
            
            elif alert_type == 'cpu_percent' and usage.get('cpu_percent', 0) > threshold:
                self._trigger_alert('resource_exceeded', {
                    'resource': 'cpu',
                    'current': usage['cpu_percent'],
                    'threshold': threshold
                })
            
            elif alert_type == 'execution_time_ms' and usage.get('execution_time_ms', 0) > threshold:
                self._trigger_alert('resource_exceeded', {
                    'resource': 'execution_time',
                    'current': usage['execution_time_ms'],
                    'threshold': threshold
                })
    
    def _trigger_alert(self, alert_type: str, data: Dict[str, Any]):
        """Trigger an alert"""
        for handler in self.alert_handlers.get(alert_type, []):
            try:
                handler(alert_type, data)
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Alert handler error: {e}")
        
        logger = __import__('logging').getLogger(__name__)
        logger.warning(f"Alert triggered: {alert_type} - {data}")
    
    def _notify_event_handlers(self, usage: Dict[str, Any]):
        """Notify event handlers"""
        for event_type, handlers in self.event_handlers.items():
            for handler in handlers:
                try:
                    handler(event_type, usage)
                except Exception as e:
                    logger = __import__('logging').getLogger(__name__)
                    logger.error(f"Event handler error: {e}")


# Import statements for missing dependencies
import time
import json
import os

try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

try:
    import pandas as pd
    PANDAS_AVAILABLE = True
except ImportError:
    PANDAS_AVAILABLE = False
    pd = None

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

# Import NLM classes
from pynlm.interface import (
    ScriptingInterface, ScriptingEvent, EventType, ScriptingMode,
    Brain, Config, NeuronType, SynapseType, ActionType,
    Vision, Audio, InternalSignals
)

# Custom exception
class SecurityError(Exception):
    """Security error for NLM"""
    pass

# Export key classes and functions
__all__ = [
    'AccessLevel',
    'SecurityLevel',
    'SecurityPolicy',
    'Sandbox',
    'AccessControl',
    'InputValidator',
    'ResourceMonitor',
    'SecurityError'
]
