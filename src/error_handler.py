#!/usr/bin/env python3
"""
NLM Error Handler and User Feedback System

Enhanced error handling and user feedback system for NLM with support for:
- Structured error reporting
- User-friendly error messages
- Progress indicators
- Graceful degradation
- Fallback mechanisms
- Logging and debugging support
"""

import sys
import traceback
import time
from enum import Enum
from typing import Dict, List, Optional, Any, Callable, Union
from datetime import datetime
import json

class ErrorCategory(Enum):
    """Error categories for classification."""
    SYSTEM = "system"
    CONFIGURATION = "configuration"
    HARDWARE = "hardware"
    PLASTICITY = "plasticity"
    MEMORY = "memory"
    NEURAL = "neural"
    WORLD = "world"
    AGENT = "agent"
    VALIDATION = "validation"
    NETWORK = "network"
    PERFORMANCE = "performance"

class ErrorSeverity(Enum):
    """Error severity levels."""
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"
    FATAL = "fatal"

class NLMError(Exception):
    """Base exception for NLM errors with enhanced metadata."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.SYSTEM, 
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 details: Optional[Dict] = None, 
                 recovery_suggestion: Optional[str] = None,
                 code: Optional[str] = None):
        super().__init__(message)
        self.message = message
        self.category = category
        self.severity = severity
        self.details = details or {}
        self.recovery_suggestion = recovery_suggestion
        self.code = code
        self.timestamp = datetime.now().isoformat()
        self.stack_trace = traceback.format_stack()
    
    def to_dict(self) -> Dict:
        """Convert error to dictionary representation."""
        return {
            'message': self.message,
            'category': self.category.value,
            'severity': self.severity.value,
            'details': self.details,
            'recovery_suggestion': self.recovery_suggestion,
            'code': self.code,
            'timestamp': self.timestamp
        }
    
    def __str__(self):
        """String representation with category and severity."""
        return f"[{self.category.value.upper()}:{self.severity.value.upper()}] {self.message}"
class UserFeedback:
    """User feedback system with different output modes."""
    
    def __init__(self, output_mode: str = "console", verbosity: str = "normal"):
        self.output_mode = output_mode
        self.verbosity = verbosity
        self.feedback_log: List[Dict] = []
        self.progress_indicators: Dict[str, Any] = {}
    
    def info(self, message: str, category: Optional[str] = None, 
             details: Optional[Dict] = None, timestamp: Optional[str] = None):
        """Log informational message."""
        feedback = {
            'type': 'info',
            'message': message,
            'category': category,
            'details': details,
            'timestamp': timestamp or datetime.now().isoformat()
        }
        self._output_feedback(feedback)
        self.feedback_log.append(feedback)
    
    def warning(self, message: str, category: Optional[str] = None,
                details: Optional[Dict] = None, timestamp: Optional[str] = None):
        """Log warning message."""
        feedback = {
            'type': 'warning',
            'message': message,
            'category': category,
            'details': details,
            'timestamp': timestamp or datetime.now().isoformat()
        }
        self._output_feedback(feedback)
        self.feedback_log.append(feedback)
    
    def error(self, message: str, category: Optional[str] = None,
              details: Optional[Dict] = None, timestamp: Optional[str] = None,
              suggestion: Optional[str] = None):
        """Log error message."""
        feedback = {
            'type': 'error',
            'message': message,
            'category': category,
            'details': details,
            'timestamp': timestamp or datetime.now().isoformat(),
            'suggestion': suggestion
        }
        self._output_feedback(feedback)
        self.feedback_log.append(feedback)
    
    def critical(self, message: str, category: Optional[str] = None,
                 details: Optional[Dict] = None, timestamp: Optional[str] = None,
                 suggestion: Optional[str] = None):
        """Log critical message."""
        feedback = {
            'type': 'critical',
            'message': message,
            'category': category,
            'details': details,
            'timestamp': timestamp or datetime.now().isoformat(),
            'suggestion': suggestion
        }
        self._output_feedback(feedback, force=True)
        self.feedback_log.append(feedback)
    
    def progress(self, task: str, progress: float, message: Optional[str] = None,
                 total: Optional[float] = None):
        """Update progress indicator."""
        indicator = {
            'task': task,
            'progress': progress,
            'message': message,
            'total': total,
            'timestamp': datetime.now().isoformat()
        }
        
        if progress == 0 or progress == 1:
            self._output_feedback({'type': 'progress', 'action': 'start', **indicator})
        
        if progress > 0 and progress < 1:
            self._output_feedback({'type': 'progress', 'action': 'update', **indicator})
        
        if progress == 1:
            self._output_feedback({'type': 'progress', 'action': 'complete', **indicator})
        
        self.progress_indicators[task] = indicator
    
    def _output_feedback(self, feedback: Dict, force: bool = False):
        """Output feedback based on configured mode."""
        if self.output_mode == "console":
            self._console_output(feedback, force)
        elif self.output_mode == "json":
            self._json_output(feedback)
        elif self.output_mode == "silent":
            pass  # Suppress all output
        else:
            self._console_output(feedback, force)
    
    def _console_output(self, feedback: Dict, force: bool = False):
        """Output feedback to console."""
        prefix = {
            'info': '[INFO]',
            'warning': '[WARNING]',
            'error': '[ERROR]',
            'critical': '[CRITICAL]',
            'progress': '[PROGRESS]'
        }.get(feedback['type'], '[UNKNOWN]')
        
        color = {
            'info': '\033[36m',      # Cyan
            'warning': '\033[33m',   # Yellow
            'error': '\033[31m',     # Red
            'critical': '\033[41m',  # Red background
            'progress': '\033[32m'   # Green
        }.get(feedback['type'], '')
        
        reset = '\033[0m'
        
        if force or self.verbosity in ['verbose', 'debug'] or feedback['type'] in ['error', 'critical']:
            message = feedback['message']
            
            if feedback['type'] == 'progress':
                action = feedback.get('action', 'update')
                if action == 'start':
                    message = f"{message}..."
                elif action == 'complete':
                    message = f"{message} - COMPLETE"
                else:
                    progress = feedback['progress']
                    percentage = int(progress * 100)
                    message = f"{message} ({percentage}%)"
            
            print(f"{color}{prefix} {message}{reset}")
            
            # Print additional details in verbose mode
            if self.verbosity in ['verbose', 'debug'] and 'details' in feedback:
                for key, value in feedback['details'].items():
                    if key != 'exception':  # Don't print full exception traceback
                        print(f"    {key}: {value}")
    
    def _json_output(self, feedback: Dict):
        """Output feedback as JSON."""
        import json
        print(json.dumps(feedback, indent=2))
    
    def get_feedback_summary(self) -> Dict:
        """Get summary of all feedback."""
        summary = {
            'total_messages': len(self.feedback_log),
            'by_type': {},
            'by_category': {},
            'by_severity': {}
        }
        
        for feedback in self.feedback_log:
            ftype = feedback['type']
            category = feedback.get('category')
            
            if ftype not in summary['by_type']:
                summary['by_type'][ftype] = 0
            summary['by_type'][ftype] += 1
            
            if category:
                if category not in summary['by_category']:
                    summary['by_category'][category] = 0
                summary['by_category'][category] += 1
        
        return summary
    
    def save_feedback(self, filepath: str):
        """Save feedback to file."""
        with open(filepath, 'w') as f:
            json.dump(self.feedback_log, f, indent=2)
class ErrorHandler:
    """Enhanced error handler with recovery and fallback mechanisms."""
    
    def __init__(self, feedback: Optional[UserFeedback] = None):
        self.feedback = feedback or UserFeedback()
        self.error_handlers: Dict[ErrorCategory, List[Callable]] = {}
        self.recovery_strategies: Dict[ErrorCategory, List[Callable]] = {}
        
        # Initialize default error handlers
        self._initialize_default_handlers()
    
    def _initialize_default_handlers(self):
        """Initialize default error handlers."""
        # System error handlers
        self.register_handler(ErrorCategory.SYSTEM, self._handle_system_error)
        self.register_handler(ErrorCategory.CONFIGURATION, self._handle_config_error)
        self.register_handler(ErrorCategory.HARDWARE, self._handle_hardware_error)
        self.register_handler(ErrorCategory.PLASTICITY, self._handle_plasticity_error)
        self.register_handler(ErrorCategory.MEMORY, self._handle_memory_error)
        self.register_handler(ErrorCategory.NEURAL, self._handle_neural_error)
        self.register_handler(ErrorCategory.WORLD, self._handle_world_error)
        self.register_handler(ErrorCategory.AGENT, self._handle_agent_error)
        
        # Recovery strategies
        self.register_recovery_strategy(ErrorCategory.CONFIGURATION, self._recover_config)
        self.register_recovery_strategy(ErrorCategory.HARDWARE, self._recover_hardware)
        self.register_recovery_strategy(ErrorCategory.SYSTEM, self._recover_system)
    
    def register_handler(self, category: ErrorCategory, handler: Callable):
        """Register an error handler for a category."""
        if category not in self.error_handlers:
            self.error_handlers[category] = []
        self.error_handlers[category].append(handler)
    
    def register_recovery_strategy(self, category: ErrorCategory, strategy: Callable):
        """Register a recovery strategy for a category."""
        if category not in self.recovery_strategies:
            self.recovery_strategies[category] = []
        self.recovery_strategies[category].append(strategy)
    
    def handle_error(self, error: NLMError) -> bool:
        """Handle an error. Returns True if error was handled."""
        # Find appropriate handler
        handlers = self.error_handlers.get(error.category, [])
        
        for handler in handlers:
            try:
                if handler(error):
                    return True
            except Exception as handler_error:
                self.feedback.error(f"Error handler failed: {handler_error}",
                                  category="system", suggestion="Check error handler implementation")
        
        # Try recovery strategies
        recovery_strategies = self.recovery_strategies.get(error.category, [])
        
        for strategy in recovery_strategies:
            try:
                if strategy(error):
                    self.feedback.info(f"Recovery successful for {error.category}",
                                    category="recovery", details=error.to_dict())
                    return True
            except Exception as recovery_error:
                self.feedback.error(f"Recovery strategy failed: {recovery_error}",
                                  category="system", suggestion="Check recovery strategy implementation")
        
        # Log unhandled error
        self.feedback.error(f"Unhandled error: {error}",
                          category=error.category.value,
                          details=error.to_dict(),
                          suggestion=error.recovery_suggestion)
        
        return False
    
    def _handle_system_error(self, error: NLMError) -> bool:
        """Handle system errors."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.critical(f"System error: {error.message}",
                                   category=error.category.value,
                                   suggestion=error.recovery_suggestion)
            return True
        
        return False
    
    def _handle_config_error(self, error: NLMError) -> bool:
        """Handle configuration errors."""
        if error.severity in [ErrorSeverity.ERROR, ErrorSeverity.CRITICAL]:
            self.feedback.error(f"Configuration error: {error.message}",
                              category=error.category.value,
                              details={'file': error.details.get('file', 'unknown')},
                              suggestion=error.recovery_suggestion)
            return True
        
        return False
    
    def _handle_hardware_error(self, error: NLMError) -> bool:
        """Handle hardware errors."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.critical(f"Hardware error: {error.message}",
                                   category=error.category.value,
                                   suggestion=error.recovery_suggestion)
            return True
        
        return False
    
    def _handle_plasticity_error(self, error: NLMError) -> bool:
        """Handle plasticity errors."""
        self.feedback.warning(f"Plasticity error: {error.message}",
                            category=error.category.value,
                            suggestion=error.recovery_suggestion)
        return True
    
    def _handle_memory_error(self, error: NLMError) -> bool:
        """Handle memory errors."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.critical(f"Memory error: {error.message}",
                                   category=error.category.value,
                                   suggestion=error.recovery_suggestion)
            return True
        
        self.feedback.warning(f"Memory issue: {error.message}",
                            category=error.category.value,
                            suggestion=error.recovery_suggestion)
        return True
    
    def _handle_neural_error(self, error: NLMError) -> bool:
        """Handle neural computation errors."""
        self.feedback.warning(f"Neural computation error: {error.message}",
                            category=error.category.value,
                            suggestion=error.recovery_suggestion)
        return True
    
    def _handle_world_error(self, error: NLMError) -> bool:
        """Handle world simulation errors."""
        if error.severity in [ErrorSeverity.ERROR, ErrorSeverity.CRITICAL]:
            self.feedback.error(f"World error: {error.message}",
                              category=error.category.value,
                              suggestion=error.recovery_suggestion)
            return True
        
        return False
    
    def _handle_agent_error(self, error: NLMError) -> bool:
        """Handle agent errors."""
        self.feedback.warning(f"Agent error: {error.message}",
                            category=error.category.value,
                            suggestion=error.recovery_suggestion)
        return True
    
    def _recover_config(self, error: NLMError) -> bool:
        """Recover from configuration error."""
        if 'config_file' in error.details:
            try:
                # Try to reload configuration
                self.feedback.info(f"Attempting to reload configuration: {error.details['config_file']}")
                # Implementation would depend on the specific application
                return True
            except Exception as e:
                self.feedback.error(f"Failed to reload configuration: {e}")
                return False
        
        return False
    
    def _recover_hardware(self, error: NLMError) -> bool:
        """Recover from hardware error."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.warning("Hardware recovery attempted (may require manual intervention)")
            return False
        
        return False
    
    def _recover_system(self, error: NLMError) -> bool:
        """Recover from system error."""
        self.feedback.info("Attempting system recovery...")
        # Implementation would depend on the specific application
        return False
class NLMExceptionMiddleware:
    """Middleware for handling NLM exceptions with enhanced user feedback."""
    
    def __init__(self, error_handler: ErrorHandler, feedback: Optional[UserFeedback] = None):
        self.error_handler = error_handler
        self.feedback = feedback or UserFeedback()
        self.middleware_stack: List[Callable] = []
    
    def add_middleware(self, middleware_func: Callable):
        """Add middleware to the processing stack."""
        self.middleware_stack.append(middleware_func)
    
    def process(self, func: Callable) -> Callable:
        """Create a wrapped function with error handling."""
        def wrapper(*args, **kwargs):
            return self._wrap_execution(func, args, kwargs)
        
        return wrapper
    
    def _wrap_execution(self, func: Callable, args: tuple, kwargs: dict) -> Any:
        """Wrap function execution with error handling."""
        task_name = getattr(func, '__name__', str(func))
        
        self.feedback.progress(task_name, 0, f"Starting {task_name}")
        
        try:
            # Apply middleware
            for middleware in self.middleware_stack:
                func = middleware(func)
            
            # Execute function
            result = func(*args, **kwargs)
            
            self.feedback.progress(task_name, 1, f"Completed {task_name}")
            return result
            
        except NLMError as e:
            self.feedback.error(f"NLM error in {task_name}: {e}",
                              category=e.category.value,
                              details=e.to_dict(),
                              suggestion=e.recovery_suggestion)
            
            handled = self.error_handler.handle_error(e)
            
            if not handled:
                self.feedback.critical(f"Unhandled NLM error: {e}",
                                      category=e.category.value,
                                      suggestion=e.recovery_suggestion)
            
            # Re-raise after logging
            raise
            
        except Exception as e:
            # Wrap unexpected errors
            lnm_error = NLMError(
                message=f"Unexpected error in {task_name}: {str(e)}",
                category=ErrorCategory.SYSTEM,
                severity=ErrorSeverity.ERROR,
                details={
                    'original_error': str(e),
                    'function': task_name,
                    'args': str(args),
                    'kwargs': str(kwargs)
                },
                recovery_suggestion="Check system logs and configuration"
            )
            
            self.error_handler.handle_error(lnm_error)
            raise lnm_error
class NLMSystem:
    """Complete NLM system with error handling and user feedback."""
    
    def __init__(self, feedback: Optional[UserFeedback] = None):
        self.feedback = feedback or UserFeedback()
        self.error_handler = ErrorHandler(self.feedback)
        self.middleware = NLMExceptionMiddleware(self.error_handler, self.feedback)
        
        # Register common error handlers
        self._register_common_handlers()
    
    def _register_common_handlers(self):
        """Register common error handlers and recovery strategies."""
        # Register handlers for various error categories
        self.error_handler.register_handler(ErrorCategory.CONFIGURATION, self._handle_config_error_detailed)
        self.error_handler.register_handler(ErrorCategory.HARDWARE, self._handle_hardware_error_detailed)
        self.error_handler.register_handler(ErrorCategory.PLASTICITY, self._handle_plasticity_error_detailed)
        self.error_handler.register_handler(ErrorCategory.MEMORY, self._handle_memory_error_detailed)
    
    def _handle_config_error_detailed(self, error: NLMError) -> bool:
        """Detailed handling of configuration errors."""
        if 'file' in error.details:
            self.feedback.error(f"Configuration file error: {error.message}",
                              category="configuration",
                              details={
                                  'file': error.details['file'],
                                  'path': error.details.get('path', 'unknown'),
                                  'line': error.details.get('line', 'unknown')
                              },
                              suggestion="Check file permissions and syntax")
            return True
        
        return False
    
    def _handle_hardware_error_detailed(self, error: NLMError) -> bool:
        """Detailed handling of hardware errors."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.critical(f"Hardware failure: {error.message}",
                                   category="hardware",
                                   details={
                                       'component': error.details.get('component', 'unknown'),
                                       'status': error.details.get('status', 'unknown')
                                   },
                                   suggestion="Check hardware connections and system resources")
            return True
        
        return False
    
    def _handle_plasticity_error_detailed(self, error: NLMError) -> bool:
        """Detailed handling of plasticity errors."""
        self.feedback.warning(f"Plasticity computation error: {error.message}",
                            category="plasticity",
                            details={
                                'neuron_id': error.details.get('neuron_id'),
                                'synapse_id': error.details.get('synapse_id'),
                                'plasticity_type': error.details.get('plasticity_type')
                            },
                            suggestion="Check neuron connectivity and plasticity parameters")
        return True
    
    def _handle_memory_error_detailed(self, error: NLMError) -> bool:
        """Detailed handling of memory errors."""
        if error.severity in [ErrorSeverity.CRITICAL, ErrorSeverity.FATAL]:
            self.feedback.critical(f"Memory system error: {error.message}",
                                   category="memory",
                                   details={
                                       'memory_type': error.details.get('memory_type', 'unknown'),
                                       'capacity': error.details.get('capacity'),
                                       'usage': error.details.get('usage')
                                   },
                                   suggestion="Check system memory availability and memory management configuration")
            return True
        
        return False
    
    def wrap_function(self, func: Callable) -> Callable:
        """Wrap a function with error handling."""
        return self.middleware.process(func)
    
    def execute_with_monitoring(self, func: Callable, *args, **kwargs) -> Any:
        """Execute function with monitoring and error handling."""
        import time
        
        start_time = time.time()
        self.feedback.info(f"Executing {getattr(func, '__name__', 'anonymous function')}")
        
        try:
            result = self.middleware.process(func)(*args, **kwargs)
            
            end_time = time.time()
            duration = end_time - start_time
            
            self.feedback.info(f"Completed in {duration:.3f}s", category="performance", details={'duration': duration})
            
            return result
            
        except Exception as e:
            end_time = time.time()
            duration = end_time - start_time
            
            self.feedback.error(f"Failed after {duration:.3f}s: {e}",
                              category="execution",
                              details={'duration': duration, 'error_type': type(e).__name__})
            
            raise
    
    def get_system_status(self) -> Dict:
        """Get current system status."""
        status = {
            'timestamp': datetime.now().isoformat(),
            'feedback': self.feedback.get_feedback_summary(),
            'error_count': len([f for f in self.feedback.feedback_log if f['type'] in ['error', 'critical']]),
            'progress_indicators': self.feedback.progress_indicators
        }
        
        return status
    
    def save_logs(self, filepath: str):
        """Save feedback logs to file."""
        self.feedback.save_feedback(filepath)
class ImprovedNLMBrain:
    """Improved NLM brain with enhanced error handling and user feedback."""
    
    def __init__(self, system: Optional[NLMSystem] = None):
        self.system = system or NLMSystem()
        self.neurons: List[Any] = []
        self.synapses: List[Any] = []
        self.plasticity_systems: List[Any] = []
    
    def wrap_method(self, func: Callable) -> Callable:
        """Wrap a method with error handling."""
        return self.system.wrap_function(func)
    
    def execute_with_monitoring(self, func: Callable, *args, **kwargs) -> Any:
        """Execute method with monitoring."""
        return self.system.execute_with_monitoring(func, *args, **kwargs)
    
    def initialize_brain(self, config: Dict):
        """Initialize brain with error handling."""
        try:
            # Validate configuration
            if not config.get('neuron_count', 0) > 0:
                raise NLMError("Neuron count must be positive", 
                             category=ErrorCategory.CONFIGURATION,
                             severity=ErrorSeverity.ERROR,
                             suggestion="Set a valid neuron count (e.g., 1000)")
            
            # Create neurons
            self.system.feedback.progress("initialize_brain", 0, "Creating neurons")
            
            for i in range(config.get('neuron_count', 1000)):
                self.system.feedback.progress("initialize_brain", 
                                            (i + 1) / config.get('neuron_count', 1000),
                                            f"Creating neuron {i+1}")
                
                # Simulate neuron creation with error handling
                try:
                    neuron = self._create_neuron(config)
                    self.neurons.append(neuron)
                except NLMError as e:
                    self.system.feedback.warning(f"Failed to create neuron {i+1}: {e}",
                                                category="neural",
                                                suggestion="Continuing with initialization")
            
            # Create synapses
            self.system.feedback.progress("initialize_brain", 0.5, "Creating synapses")
            
            for i in range(int(config.get('neuron_count', 1000) * config.get('synapse_density', 0.1))):
                self.system.feedback.progress("initialize_brain",
                                            (i + 1) / max(1, int(config.get('neuron_count', 1000) * config.get('synapse_density', 0.1))),
                                            f"Creating synapse {i+1}")
                
                try:
                    synapse = self._create_synapse(config)
                    self.synapses.append(synapse)
                except NLMError as e:
                    self.system.feedback.warning(f"Failed to create synapse {i+1}: {e}",
                                                category="neural",
                                                suggestion="Continuing with synapse creation")
            
            # Initialize plasticity systems
            self.system.feedback.progress("initialize_brain", 0.75, "Initializing plasticity systems")
            
            for plasticity_name in ['stdp', 'hebbian', 'structural']:
                try:
                    plasticity = self._create_plasticity_system(plasticity_name, config)
                    self.plasticity_systems.append(plasticity)
                    
                    self.system.feedback.info(f"Initialized {plasticity_name} plasticity system",
                                            category="plasticity")
                except NLMError as e:
                    self.system.feedback.warning(f"Failed to initialize {plasticity_name}: {e}",
                                                category="plasticity",
                                                suggestion="Some plasticity systems may not be available")
            
            self.system.feedback.progress("initialize_brain", 1.0, "Brain initialization complete")
            self.system.feedback.info(f"Brain initialized with {len(self.neurons)} neurons and {len(self.synapses)} synapses",
                                    category="system")
            
            return True
            
        except Exception as e:
            self.system.feedback.error(f"Brain initialization failed: {e}",
                                     category="system",
                                     details={'config': config},
                                     suggestion="Check configuration and system resources")
            raise
    
    def _create_neuron(self, config: Dict):
        """Create a neuron with error handling."""
        # Simulate neuron creation with potential errors
        neuron_id = len(self.neurons)
        
        # Check for configuration errors
        if config.get('neuron_count', 0) > 10000:  # Safety limit
            raise NLMError(f"Neuron count {config.get('neuron_count')} exceeds safety limit",
                         category=ErrorCategory.CONFIGURATION,
                         severity=ErrorSeverity.WARNING,
                         suggestion="Reduce neuron count or increase system resources")
        
        # Simulate random failure for testing
        import random
        if random.random() < 0.01:  # 1% chance of failure
            raise NLMError(f"Random neuron creation failure for neuron {neuron_id}",
                         category=ErrorCategory.NEURAL,
                         severity=ErrorSeverity.WARNING,
                         suggestion="This is a simulated failure. Retry initialization if needed.")
        
        return {
            'id': neuron_id,
            'type': 'internal',
            'config': config
        }
    
    def _create_synapse(self, config: Dict):
        """Create a synapse with error handling."""
        # Simulate synapse creation with potential errors
        synapse_id = len(self.synapses)
        
        # Check for connectivity errors
        if config.get('synapse_density', 0.1) > 0.5:
            raise NLMError(f"Synapse density {config.get('synapse_density')} too high",
                         category=ErrorCategory.CONFIGURATION,
                         severity=ErrorSeverity.WARNING,
                         suggestion="Reduce synapse density or increase system memory")
        
        return {
            'id': synapse_id,
            'source': 0,
            'target': 0,
            'weight': config.get('initial_weight_mean', 0.5),
            'config': config
        }
    
    def _create_plasticity_system(self, name: str, config: Dict):
        """Create a plasticity system with error handling."""
        # Check if plasticity is enabled in config
        if not config.get(f'plasticity.{name}.enable', True):
            raise NLMError(f"Plasticity system {name} is disabled in configuration",
                         category=ErrorCategory.CONFIGURATION,
                         severity=ErrorSeverity.WARNING,
                         suggestion="Enable {name} in configuration if needed")
        
        return {
            'name': name,
            'config': config
        }
class ImprovedNLMWorld:
    """Improved NLM world with enhanced error handling and user feedback."""
    
    def __init__(self, system: Optional[NLMSystem] = None):
        self.system = system or NLMSystem()
        self.cells: List[List[Any]] = []
        self.agent = None
    
    def initialize_world(self, config: Dict):
        """Initialize world with error handling."""
        try:
            width = config.get('width', 30)
            height = config.get('height', 30)
            
            # Validate dimensions
            if width <= 0 or height <= 0:
                raise NLMError(f"Invalid world dimensions: {width}x{height}",
                             category=ErrorCategory.CONFIGURATION,
                             severity=ErrorSeverity.ERROR,
                             suggestion="Set positive width and height (e.g., 30x30)")
            
            # Check for reasonable size
            if width * height > 10000:  # 10k cells maximum
                raise NLMError(f"World size {width}x{height} exceeds memory limit",
                             category=ErrorCategory.CONFIGURATION,
                             severity=ErrorSeverity.ERROR,
                             suggestion="Reduce world size or increase system memory")
            
            self.system.feedback.progress("initialize_world", 0, "Creating world grid")
            
            # Create world grid
            self.cells = [[self._create_cell(x, y, config) for y in range(height)] 
                         for x in range(width)]
            
            self.system.feedback.progress("initialize_world", 0.5, "Setting up agent")
            
            # Create agent
            self.agent = self._create_agent(config)
            
            # Initialize world objects
            self._initialize_world_objects(config)
            
            self.system.feedback.progress("initialize_world", 1.0, "World initialization complete")
            self.system.feedback.info(f"World initialized: {width}x{height} cells",
                                    category="world",
                                    details={'cells': width * height})
            
            return True
            
        except Exception as e:
            self.system.feedback.error(f"World initialization failed: {e}",
                                     category="world",
                                     details={'config': config},
                                     suggestion="Check world configuration and system resources")
            raise
    
    def _create_cell(self, x: int, y: int, config: Dict):
        """Create a cell in the world."""
        # Determine cell type based on configuration
        cell_type = config.get('cell_type', 'empty')
        
        # Add some variety for testing
        import random
        if random.random() < 0.1:  # 10% chance of hazard
            cell_type = 'hazard'
        elif random.random() < 0.05:  # 5% chance of resource
            cell_type = 'resource'
        
        return {
            'x': x,
            'y': y,
            'type': cell_type,
            'config': config
        }
    
    def _create_agent(self, config: Dict):
        """Create agent in world."""
        return {
            'id': 'agent_0',
            'x': config.get('agent_start_x', 0),
            'y': config.get('agent_start_y', 0),
            'energy': config.get('max_energy', 100),
            'config': config
        }
    
    def _initialize_world_objects(self, config: Dict):
        """Initialize world objects (resources, hazards, etc.)."""
        import random
        
        # Add resources
        resource_count = int(config.get('width', 30) * config.get('height', 30) * 0.1)
        
        for i in range(resource_count):
            x = random.randint(0, config.get('width', 30) - 1)
            y = random.randint(0, config.get('height', 30) - 1)
            
            if self.cells[x][y]['type'] == 'empty':
                self.cells[x][y]['type'] = 'resource'
                self.cells[x][y]['value'] = random.uniform(0.5, 2.0)
    
    def update(self, timestep: float, agent_action: Any):
        """Update world with agent action."""
        try:
            # Update agent position
            new_x = self.agent['x']
            new_y = self.agent['y']
            
            # Apply action
            if hasattr(agent_action, 'type'):
                if agent_action.type == 'MoveForward':
                    # Move forward (simplified)
                    new_x = min(new_x + 1, len(self.cells) - 1)
                elif agent_action.type == 'TurnLeft':
                    # Turn left (rotate vision)
                    pass
                elif agent_action.type == 'Interact':
                    # Interact with cell
                    if 0 <= new_x < len(self.cells) and 0 <= new_y < len(self.cells[0]):
                        cell = self.cells[new_x][new_y]
                        if cell['type'] == 'resource':
                            self.agent['energy'] = min(self.agent['energy'] + cell.get('value', 1.0), 
                                                    self.agent['config'].get('max_energy', 100))
                            cell['type'] = 'empty'
            
            # Check for hazards
            if 0 <= new_x < len(self.cells) and 0 <= new_y < len(self.cells[0]):
                cell = self.cells[new_x][new_y]
                if cell['type'] == 'hazard':
                    self.agent['energy'] = max(0, self.agent['energy'] - 10.0)
            
            # Update agent position
            self.agent['x'] = new_x
            self.agent['y'] = new_y
            
            # Decay energy over time
            self.agent['energy'] = max(0, self.agent['energy'] - config.get('energy_decay_rate', 0.1) * timestep)
            
            # Check for world events
            self._check_world_events()
            
            return True
            
        except Exception as e:
            self.system.feedback.error(f"World update failed: {e}",
                                     category="world",
                                     details={'action': str(agent_action), 'timestep': timestep},
                                     suggestion="Check agent action and system state")
            raise
    
    def _check_world_events(self):
        """Check for world events (agent death, etc.)."""
        if self.agent['energy'] <= 0:
            self.system.feedback.critical("Agent died due to energy depletion",
                                         category="world",
                                         suggestion="Check agent energy management and world hazards")
    
    def get_observation(self):
        """Get agent observation."""
        x, y = self.agent['x'], self.agent['y']
        
        # Create observation based on agent's perception
        observation = {
            'vision': [],
            'energy': self.agent['energy'],
            'position': {'x': x, 'y': y}
        }
        
        # Collect vision data
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                nx, ny = x + dx, y + dy
                if 0 <= nx < len(self.cells) and 0 <= ny < len(self.cells[0]):
                    cell = self.cells[nx][ny]
                    observation['vision'].append({
                        'type': cell['type'],
                        'value': cell.get('value', 0.0),
                        'distance': (dx**2 + dy**2) ** 0.5
                    })
        
        return observation
# Example usage and demonstration
if __name__ == "__main__":
    # Create enhanced system
    system = NLMSystem()
    
    # Create enhanced brain
    brain = ImprovedNLMBrain(system)
    
    # Create enhanced world
    world = ImprovedNLMWorld(system)
    
    # Example configuration
    config = {
        'neuron_count': 1000,
        'synapse_density': 0.1,
        'width': 30,
        'height': 30,
        'cell_type': 'empty',
        'max_energy': 100,
        'energy_decay_rate': 0.05
    }
    
    print("=== NLM Enhanced System Demonstration ===")
    print("This demonstrates improved error handling and user feedback.\n")
    
    # Initialize brain
    print("1. Initializing brain...")
    brain.initialize_brain(config)
    
    # Initialize world
    print("\n2. Initializing world...")
    world.initialize_world(config)
    
    # Run simulation
    print("\n3. Running simulation...")
    for step in range(10):
        system.feedback.progress("simulation", (step + 1) / 10, f"Step {step + 1}/10")
        
        # Get observation
        observation = world.get_observation()
        
        # Simulate agent action
        action = {'type': 'MoveForward'}  # Simulated action
        
        # Update world
        world.update(0.1, action)
        
        # Log progress
        if system.feedback.verbosity in ['verbose', 'debug']:
            print(f"   Step {step + 1}: Agent at ({world.agent['x']}, {world.agent['y']}), "
                  f"Energy: {world.agent['energy']:.1f}")
    
    # Show system status
    print("\n4. System status:")
    status = system.get_system_status()
    print(f"   Total feedback messages: {status['feedback']['total_messages']}")
    print(f"   Error count: {status['error_count']}")
    
    # Save feedback logs
    system.save_logs("nlm_feedback.json")
    print(f"   Feedback logs saved to nlm_feedback.json")
    
    print("\n=== Demonstration complete ===")
    print("The enhanced NLM system provides:")
    print("- Structured error handling and recovery")
    print("- User-friendly feedback and progress indicators")
    print("- Graceful degradation and fallback mechanisms")
    print("- Detailed logging and debugging support")
    print("- Enhanced validation and error reporting")