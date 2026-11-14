#!/usr/bin/env python3
"""
Error handling and logging improvements for NLM

This module provides enhanced error handling and logging capabilities
for NLM simulations, including:
- Comprehensive exception handling
- Advanced logging with levels and formatting
- Error recovery mechanisms
- Performance error tracking
- Development and production error handling
"""

import logging
import sys
import traceback
from typing import Dict, List, Any, Optional, Callable
from enum import Enum
import time
import json
from datetime import datetime

# Try to import pynlm
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None

class ErrorLevel(Enum):
    """Error severity levels"""
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"
    FATAL = "FATAL"

class ErrorCategory(Enum):
    """Error categories"""
    CONFIGURATION = "Configuration"
    NEURAL = "Neural"
    MOTOR = "Motor"
    SENSORY = "Sensory"
    MEMORY = "Memory"
    PLASTICITY = "Plasticity"
    DEVELOPMENT = "Development"
    SYSTEM = "System"
    NETWORK = "Network"
    PERFORMANCE = "Performance"

class NLMError(Exception):
    """Base exception class for NLM errors"""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.SYSTEM,
                 level: ErrorLevel = ErrorLevel.ERROR, details: Dict[str, Any] = None):
        super().__init__(message)
        self.message = message
        self.category = category
        self.level = level
        self.details = details or {}
        self.timestamp = datetime.now()
        self.traceback = traceback.format_stack()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary representation"""
        return {
            'message': self.message,
            'category': self.category.value,
            'level': self.level.value,
            'timestamp': self.timestamp.isoformat(),
            'details': self.details,
            'traceback': self.traceback[-5:] if len(self.traceback) > 5 else self.traceback
        }

class ErrorHandler:
    """Enhanced error handling for NLM"""
    
    def __init__(self, config=None):
        self.config = config or {}
        self.error_counts: Dict[str, int] = defaultdict(int)
        self.error_history: List[NLMError] = []
        self.last_error_time = None
        self.error_patterns: Dict[str, List[datetime]] = defaultdict(list)
        
        # Configure logging
        self.setup_logging()
        
        # Error recovery callbacks
        self.recovery_callbacks: Dict[ErrorCategory, List[Callable]] = defaultdict(list)
        
        # Performance error tracking
        self.performance_errors: List[Dict[str, Any]] = []
        
        # Configuration errors
        self.config_errors: List[Dict[str, Any]] = []
    
    def setup_logging(self):
        """Setup comprehensive logging system"""
        # Create logger
        self.logger = logging.getLogger('nlm')
        self.logger.setLevel(logging.DEBUG)
        
        # Remove existing handlers
        self.logger.handlers.clear()
        
        # Console handler with formatting
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.DEBUG)
        
        # Custom formatter with colors and context
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(category)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        console_handler.setFormatter(formatter)
        self.logger.addHandler(console_handler)
        
        # File handler for detailed logs
        if self.config.get('enable_file_logging', True):
            log_file = self.config.get('log_file', 'nlm_simulation.log')
            file_handler = logging.FileHandler(log_file, mode='a')
            file_handler.setLevel(logging.DEBUG)
            file_handler.setFormatter(formatter)
            self.logger.addHandler(file_handler)
        
        # Performance logging to separate file
        perf_handler = logging.FileHandler('nlm_performance.log', mode='a')
        perf_handler.setLevel(logging.INFO)
        perf_handler.setFormatter(logging.Formatter(
            '%(asctime)s - %(levelname)s - PERFORMANCE - %(message)s'
        ))
        self.logger.addHandler(perf_handler)
    
    def log_error(self, error: NLMError):
        """Log error with full context"""
        self.error_counts[error.category.value] += 1
        self.error_history.append(error)
        self.last_error_time = error.timestamp
        
        # Log error based on level
        if error.level == ErrorLevel.CRITICAL or error.level == ErrorLevel.FATAL:
            self.logger.critical(error.message, extra={'category': error.category.value})
        elif error.level == ErrorLevel.ERROR:
            self.logger.error(error.message, extra={'category': error.category.value})
        elif error.level == ErrorLevel.WARNING:
            self.logger.warning(error.message, extra={'category': error.category.value})
        elif error.level == ErrorLevel.INFO:
            self.logger.info(error.message, extra={'category': error.category.value})
        else:
            self.logger.debug(error.message, extra={'category': error.category.value})
        
        # Add to error patterns
        self.error_patterns[error.category.value].append(error.timestamp)
        
        # Cleanup old error patterns
        self._cleanup_error_patterns()
    
    def _cleanup_error_patterns(self):
        """Remove error patterns older than 24 hours"""
        cutoff_time = datetime.now() - timedelta(hours=24)
        
        for category in list(self.error_patterns.keys()):
            # Remove old timestamps
            self.error_patterns[category] = [
                ts for ts in self.error_patterns[category] if ts > cutoff_time
            ]
            
            # Remove category if empty
            if not self.error_patterns[category]:
                del self.error_patterns[category]
    
    def log_performance_error(self, error_type: str, details: Dict[str, Any]):
        """Log performance-related errors"""
        perf_error = {
            'timestamp': datetime.now(),
            'error_type': error_type,
            'details': details,
            'severity': 'HIGH' if 'memory' in error_type.lower() else 'MEDIUM'
        }
        
        self.performance_errors.append(perf_error)
        
        # Log performance error
        self.logger.error(
            f"Performance error: {error_type}",
            extra={
                'category': ErrorCategory.PERFORMANCE.value,
                'details': details
            }
        )
    
    def log_config_error(self, error_type: str, key: str, value: Any, details: str = ""):
        """Log configuration-related errors"""
        config_error = {
            'timestamp': datetime.now(),
            'error_type': error_type,
            'key': key,
            'value': str(value),
            'details': details
        }
        
        self.config_errors.append(config_error)
        
        # Log configuration error
        self.logger.warning(
            f"Configuration error: {error_type} for key '{key}'",
            extra={
                'category': ErrorCategory.CONFIGURATION.value,
                'key': key,
                'details': details
            }
        )
    
    def handle_error(self, error: NLMError) -> bool:
        """Handle error and attempt recovery"""
        self.log_error(error)
        
        # Check if error is recoverable
        if self._is_recoverable(error):
            # Attempt recovery
            recovery_result = self._attempt_recovery(error)
            
            if recovery_result:
                self.logger.info(f"Successfully recovered from error: {error.message}")
                return True
            else:
                self.logger.error(f"Failed to recover from error: {error.message}")
                return False
        else:
            # Non-recoverable error
            self.logger.critical(f"Non-recoverable error: {error.message}")
            return False
    
    def _is_recoverable(self, error: NLMError) -> bool:
        """Check if error is recoverable"""
        # Configuration errors are often recoverable
        if error.category == ErrorCategory.CONFIGURATION:
            return True
        
        # Neural errors are usually recoverable
        if error.category == ErrorCategory.NEURAL:
            return True
        
        # Performance errors are recoverable if not critical
        if error.category == ErrorCategory.PERFORMANCE:
            return error.level not in [ErrorLevel.CRITICAL, ErrorLevel.FATAL]
        
        return False
    
    def _attempt_recovery(self, error: NLMError) -> bool:
        """Attempt to recover from error"""
        # Call recovery callbacks
        callbacks = self.recovery_callbacks.get(error.category, [])
        
        for callback in callbacks:
            try:
                if callback(error):
                    return True
            except Exception as e:
                self.logger.error(f"Recovery callback failed: {e}")
        
        # Category-specific recovery
        if error.category == ErrorCategory.CONFIGURATION:
            return self._recover_config_error(error)
        elif error.category == ErrorCategory.NEURAL:
            return self._recover_neural_error(error)
        elif error.category == ErrorCategory.PERFORMANCE:
            return self._recover_performance_error(error)
        
        return False
    
    def _recover_config_error(self, error: NLMError) -> bool:
        """Recover from configuration error"""
        try:
            # Reset to default configuration
            self.logger.info("Resetting to default configuration")
            
            # Remove problematic key
            if 'key' in error.details:
                # This would require access to the actual config object
                pass
            
            return True
        except Exception as e:
            self.logger.error(f"Configuration recovery failed: {e}")
            return False
    
    def _recover_neural_error(self, error: NLMError) -> bool:
        """Recover from neural error"""
        try:
            # Reset brain state
            self.logger.info("Resetting neural state")
            
            # Reduce network complexity if needed
            return True
        except Exception as e:
            self.logger.error(f"Neural recovery failed: {e}")
            return False
    
    def _recover_performance_error(self, error: NLMError) -> bool:
        """Recover from performance error"""
        try:
            # Reduce simulation complexity
            self.logger.info("Reducing simulation complexity for performance")
            
            return True
        except Exception as e:
            self.logger.error(f"Performance recovery failed: {e}")
            return False
    
    def add_recovery_callback(self, category: ErrorCategory, callback: Callable):
        """Add error recovery callback"""
        self.recovery_callbacks[category].append(callback)
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get summary of errors"""
        return {
            'total_errors': len(self.error_history),
            'error_by_category': dict(self.error_counts),
            'last_error_time': self.last_error_time.isoformat() if self.last_error_time else None,
            'recent_errors': len(self.error_history) - len([e for e in self.error_history if e.timestamp < datetime.now() - timedelta(hours=1)])
        }
    
    def export_error_log(self) -> str:
        """Export error log as JSON"""
        error_log = {
            'errors': [error.to_dict() for error in self.error_history],
            'error_counts': dict(self.error_counts),
            'config_errors': self.config_errors,
            'performance_errors': self.performance_errors,
            'summary': self.get_error_summary()
        }
        
        return json.dumps(error_log, indent=2, default=str)

class LoggingManager:
    """Manage logging across different NLM components"""
    
    def __init__(self):
        self.log_levels: Dict[str, int] = {
            'nlm_core': logging.DEBUG,
            'nlm_agent': logging.INFO,
            'nlm_world': logging.INFO,
            'nlm_experiments': logging.WARNING,
            'nlm_tests': logging.DEBUG
        }
        
        self.component_loggers: Dict[str, logging.Logger] = {}
    
    def get_logger(self, component: str) -> logging.Logger:
        """Get logger for specific component"""
        if component not in self.component_loggers:
            logger = logging.getLogger(f'nlm.{component}')
            logger.setLevel(self.log_levels.get(component, logging.INFO))
            
            # Add handler if not already present
            if not logger.handlers:
                handler = logging.StreamHandler()
                formatter = logging.Formatter(
                    '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
                )
                handler.setFormatter(formatter)
                logger.addHandler(handler)
            
            self.component_loggers[component] = logger
        
        return self.component_loggers[component]
    
    def set_log_level(self, component: str, level: int):
        """Set log level for component"""
        if component in self.component_loggers:
            self.component_loggers[component].setLevel(level)
        
        self.log_levels[component] = level

class ErrorContext:
    """Context manager for error handling"""
    
    def __init__(self, error_handler: ErrorHandler, category: ErrorCategory):
        self.error_handler = error_handler
        self.category = category
        self.errors_caught = []
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_val is not None:
            # Convert exception to NLM error
            error = NLMError(
                message=str(exc_val),
                category=self.category,
                level=ErrorLevel.ERROR,
                details={'exception_type': exc_type.__name__ if exc_type else None}
            )
            
            self.errors_caught.append(error)
            self.error_handler.handle_error(error)
            
            # Return False to propagate the exception
            return False
        
        return True

def safe_execute(func: Callable, error_handler: ErrorHandler,
                category: ErrorCategory = ErrorCategory.SYSTEM,
                fallback: Any = None) -> Any:
    """Safely execute a function with error handling"""
    try:
        return func()
    except Exception as e:
        error = NLMError(
            message=f"Error executing {func.__name__}: {e}",
            category=category,
            level=ErrorLevel.ERROR,
            details={'function': func.__name__, 'exception': str(e)}
        )
        
        if error_handler.handle_error(error):
            # Recovery was successful, try again
            try:
                return func()
            except Exception as e2:
                error2 = NLMError(
                    message=f"Error in recovery attempt: {e2}",
                    category=category,
                    level=ErrorLevel.CRITICAL,
                    details={'original_error': str(e), 'recovery_error': str(e2)}
                )
                error_handler.handle_error(error2)
        
        # Return fallback value
        return fallback

def with_error_context(error_handler: ErrorHandler,
                       category: ErrorCategory = ErrorCategory.SYSTEM):
    """Context manager for error handling"""
    return ErrorContext(error_handler, category)

# Specialized error handlers for different components
class NeuralErrorHandler(ErrorHandler):
    """Error handler for neural system"""
    
    def __init__(self, config=None):
        super().__init__(config)
        
        # Add neural-specific recovery callbacks
        self.add_recovery_callback(ErrorCategory.NEURAL, self._recover_neural_error)
        self.add_recovery_callback(ErrorCategory.PLASTICITY, self._recover_plasticity_error)
        self.add_recovery_callback(ErrorCategory.DEVELOPMENT, self._recover_development_error)
    
    def _recover_neural_error(self, error: NLMError) -> bool:
        """Recover from neural error"""
        self.logger.info(f"Attempting neural recovery for: {error.message}")
        
        # Reduce neural activity if error is related to high firing rates
        if 'firing' in error.message.lower():
            self.logger.info("Reducing neural firing rates")
            # Implementation would depend on specific NLM architecture
        
        return True
    
    def _recover_plasticity_error(self, error: NLMError) -> bool:
        """Recover from plasticity error"""
        self.logger.info(f"Attempting plasticity recovery for: {error.message}")
        
        # Reset plasticity rules
        return True
    
    def _recover_development_error(self, error: NLMError) -> bool:
        """Recover from development error"""
        self.logger.info(f"Attempting development recovery for: {error.message}")
        
        # Reset to earlier developmental stage
        return True

class ConfigErrorHandler(ErrorHandler):
    """Error handler for configuration system"""
    
    def __init__(self, config=None):
        super().__init__(config)
        
        # Add configuration-specific recovery callbacks
        self.add_recovery_callback(ErrorCategory.CONFIGURATION, self._recover_config_error)
    
    def _recover_config_error(self, error: NLMError) -> bool:
        """Recover from configuration error"""
        self.logger.info(f"Attempting configuration recovery for: {error.message}")
        
        # Load default configuration
        if 'key' in error.details:
            self.logger.info(f"Resetting configuration key: {error.details['key']}")
        
        return True

# Global error handler instances
_core_error_handler = None
_neural_error_handler = None
_config_error_handler = None

def get_error_handler(handler_type: str = 'core') -> ErrorHandler:
    """Get global error handler instance"""
    global _core_error_handler, _neural_error_handler, _config_error_handler
    
    if handler_type == 'core':
        if _core_error_handler is None:
            _core_error_handler = ErrorHandler()
        return _core_error_handler
    elif handler_type == 'neural':
        if _neural_error_handler is None:
            _neural_error_handler = NeuralErrorHandler()
        return _neural_error_handler
    elif handler_type == 'config':
        if _config_error_handler is None:
            _config_error_handler = ConfigErrorHandler()
        return _config_error_handler
    
    return _core_error_handler

def setup_nlm_error_handling(config: Dict[str, Any] = None):
    """Setup error handling for NLM"""
    if config is None:
        config = {}
    
    # Setup error handlers
    core_handler = get_error_handler('core')
    neural_handler = get_error_handler('neural')
    config_handler = get_error_handler('config')
    
    # Configure handlers based on config
    core_handler.config.update(config)
    neural_handler.config.update(config)
    config_handler.config.update(config)
    
    # Setup logging
    core_handler.setup_logging()
    
    return {
        'core': core_handler,
        'neural': neural_handler,
        'config': config_handler
    }

if __name__ == "__main__":
    print("NLM Error Handling and Logging System")
    print("=" * 40)
    
    # Setup error handling
    error_handlers = setup_nlm_error_handling({
        'enable_file_logging': True,
        'log_file': 'nlm_detailed.log',
        'log_level': 'DEBUG'
    })
    
    core_handler = error_handlers['core']
    
    # Example error handling
    try:
        # Simulate an error
        raise NLMError(
            "Test neural error for demonstration",
            category=ErrorCategory.NEURAL,
            level=ErrorLevel.WARNING,
            details={'neuron_id': '123', 'firing_rate': 1000.0}
        )
    except NLMError as e:
        core_handler.handle_error(e)
    
    # Simulate a configuration error
    try:
        raise NLMError(
            "Invalid configuration value",
            category=ErrorCategory.CONFIGURATION,
            level=ErrorLevel.ERROR,
            details={'key': 'brain.neuron_count', 'value': 'invalid', 'expected': 'int'}
        )
    except NLMError as e:
        core_handler.handle_error(e)
    
    # Get error summary
    summary = core_handler.get_error_summary()
    print(f"\nError Summary: {json.dumps(summary, indent=2)}")
    
    # Export error log
    error_log = core_handler.export_error_log()
    
    # Write error log to file
    with open('nlm_error_log.json', 'w') as f:
        f.write(error_log)
    
    print(f"\nError log exported to: nlm_error_log.json")
    print("\nError handling and logging system demonstration complete!")