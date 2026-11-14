# NLM Logging and Error Handling System
# Comprehensive logging and error handling for NLM

# Logging configuration
LOG_LEVEL = "INFO"
LOG_FILE = "nlm.log"
MAX_LOG_SIZE = 10485760  # 10MB
LOG_RETENTION_DAYS = 30

# Error handling configuration
MAX_RETRIES = 3
RETRY_DELAY_MS = 100
ERROR_LOG_FILE = "nlm_errors.log"

# Colors for output
RED = '\033[91m'
GREEN = '\033[92m'
YELLOW = '\033[93m'
BLUE = '\033[94m'
PURPLE = '\033[95m'
CYAN = '\033[96m'
NC = '\033[0m'  # No color

# Log levels
DEBUG = 10
INFO = 20
WARNING = 30
ERROR = 40
CRITICAL = 50

# Global log level
_current_log_level = INFO

# Log entry structure
class LogEntry:
    def __init__(self, level, message, timestamp=None, context=None):
        self.level = level
        self.message = message
        self.timestamp = timestamp or self._get_timestamp()
        self.context = context or {}
    
    def _get_timestamp(self):
        import datetime
        return datetime.datetime.now().isoformat()
    
    def to_dict(self):
        return {
            'level': self.level,
            'level_name': self._get_level_name(self.level),
            'message': self.message,
            'timestamp': self.timestamp,
            'context': self.context
        }
    
    @staticmethod
    def _get_level_name(level):
        levels = {
            DEBUG: 'DEBUG',
            INFO: 'INFO',
            WARNING: 'WARNING',
            ERROR: 'ERROR',
            CRITICAL: 'CRITICAL'
        }
        return levels.get(level, 'UNKNOWN')

# Error entry structure
class ErrorEntry:
    def __init__(self, error_type, message, stack_trace=None, context=None, retry_count=0):
        self.error_type = error_type
        self.message = message
        self.stack_trace = stack_trace
        self.context = context or {}
        self.retry_count = retry_count
        self.timestamp = self._get_timestamp()
    
    def _get_timestamp(self):
        import datetime
        return datetime.datetime.now().isoformat()
    
    def to_dict(self):
        return {
            'error_type': self.error_type,
            'message': self.message,
            'stack_trace': self.stack_trace,
            'context': self.context,
            'retry_count': self.retry_count,
            'timestamp': self.timestamp
        }

# Logging functions
import os
import sys
import json
from datetime import datetime
from typing import Dict, List, Any, Optional

class NLMLogger:
    def __init__(self, log_file=None, log_level=None):
        self.log_file = log_file or LOG_FILE
        self.log_level = log_level or LOG_LEVEL
        self.error_log_file = ERROR_LOG_FILE
        self.log_entries = []
        self.error_entries = []
        
        # Ensure log directories exist
        os.makedirs(os.path.dirname(os.path.abspath(self.log_file)), exist_ok=True)
        os.makedirs(os.path.dirname(os.path.abspath(self.error_log_file)), exist_ok=True)
        
        # Set up log level
        global _current_log_level
        _current_log_level = self._get_log_level_int(self.log_level)
    
    def _get_log_level_int(self, level):
        levels = {
            'DEBUG': DEBUG,
            'INFO': INFO,
            'WARNING': WARNING,
            'ERROR': ERROR,
            'CRITICAL': CRITICAL
        }
        return levels.get(level.upper(), INFO)
    
    def _should_log(self, level):
        return level >= _current_log_level
    
    def debug(self, message, context=None):
        if self._should_log(DEBUG):
            entry = LogEntry(DEBUG, message, context=context)
            self._write_log(entry)
            self.log_entries.append(entry)
    
    def info(self, message, context=None):
        if self._should_log(INFO):
            entry = LogEntry(INFO, message, context=context)
            self._write_log(entry)
            self.log_entries.append(entry)
    
    def warning(self, message, context=None):
        if self._should_log(WARNING):
            entry = LogEntry(WARNING, message, context=context)
            self._write_log(entry)
            self.log_entries.append(entry)
    
    def error(self, message, error_type="Unknown", stack_trace=None, context=None, retry_count=0):
        if self._should_log(ERROR):
            entry = ErrorEntry(error_type, message, stack_trace, context, retry_count)
            self._write_error_log(entry)
            self.error_entries.append(entry)
    
    def critical(self, message, context=None):
        if self._should_log(CRITICAL):
            entry = LogEntry(CRITICAL, message, context=context)
            self._write_log(entry)
            self.log_entries.append(entry)
    
    def _write_log(self, entry):
        # Write to console with color
        color = self._get_level_color(entry.level)
        print(f"{color}[{entry._get_level_name(entry.level)}]{NC} {entry.message}")
        
        # Write to log file
        try:
            with open(self.log_file, 'a') as f:
                json.dump(entry.to_dict(), f)
                f.write('\n')
        except Exception as e:
            print(f"{RED}Error writing to log file: {e}{NC}")
    
    def _write_error_log(self, entry):
        # Write to console with color
        print(f"{RED}[ERROR]{NC} {entry.message} (Type: {entry.error_type})")
        
        # Write to error log file
        try:
            with open(self.error_log_file, 'a') as f:
                json.dump(entry.to_dict(), f)
                f.write('\n')
        except Exception as e:
            print(f"{RED}Error writing to error log file: {e}{NC}")
    
    def set_log_level(self, level):
        global _current_log_level
        _current_log_level = self._get_log_level_int(level)
        print(f"Log level set to: {level}")
    
    def get_log_level(self):
        return self._get_level_name(_current_log_level)
    
    def _get_level_color(self, level):
        colors = {
            DEBUG: CYAN,
            INFO: GREEN,
            WARNING: YELLOW,
            ERROR: RED,
            CRITICAL: PURPLE
        }
        return colors.get(level, NC)
    
    def _get_level_name(self, level):
        return LogEntry._get_level_name(level)
    
    def get_recent_logs(self, level=None, limit=10):
        filtered = self.log_entries
        if level:
            filtered = [e for e in filtered if e.level == level]
        
        return filtered[-limit:]
    
    def get_error_logs(self, error_type=None, limit=10):
        filtered = self.error_entries
        if error_type:
            filtered = [e for e in filtered if e.error_type == error_type]
        
        return filtered[-limit:]

class NLMErrorHandler:
    def __init__(self, logger=None):
        self.logger = logger or NLMLogger()
        self.retry_policies = {}
        self.error_handlers = {}
    
    def add_retry_policy(self, error_type, max_retries=MAX_RETRIES, delay_ms=RETRY_DELAY_MS):
        self.retry_policies[error_type] = {
            'max_retries': max_retries,
            'delay_ms': delay_ms
        }
    
    def add_error_handler(self, error_type, handler):
        self.error_handlers[error_type] = handler
    
    def handle_error(self, error, context=None):
        error_type = self._get_error_type(error)
        
        # Check if we have a handler for this error type
        if error_type in self.error_handlers:
            try:
                result = self.error_handlers[error_type](error, context)
                if result is True:
                    return True  # Error handled
            except Exception as e:
                self.logger.error(f"Error handler failed: {e}", error_type="HandlerError")
        
        # Log the error
        retry_count = self._get_retry_count(error_type)
        self.logger.error(
            str(error),
            error_type=error_type,
            stack_trace=self._get_stack_trace(error),
            context=context,
            retry_count=retry_count
        )
        
        # Check if we should retry
        if error_type in self.retry_policies:
            policy = self.retry_policies[error_type]
            if retry_count < policy['max_retries']:
                self._retry_error(error, error_type, policy, context)
                return True  # Error was retried
        
        return False  # Error not handled
    
    def _get_error_type(self, error):
        # Try to extract error type from the error
        if isinstance(error, str):
            return "GenericError"
        elif hasattr(error, '__class__'):
            return error.__class__.__name__
        else:
            return "UnknownError"
    
    def _get_retry_count(self, error_type):
        # In a real implementation, this would be stored in a more sophisticated way
        # For now, we'll use a simple approach
        return 0
    
    def _get_stack_trace(self, error):
        import traceback
        return traceback.format_exc()
    
    def _retry_error(self, error, error_type, policy, context):
        import time
        retry_count = self._get_retry_count(error_type)
        
        self.logger.info(f"Retrying {error_type} (attempt {retry_count + 1}/{policy['max_retries']})")
        
        # Wait before retry
        time.sleep(policy['delay_ms'] / 1000.0)
        
        # In a real implementation, this would actually retry the operation
        # For now, we'll just log the retry

class NLMSystemMonitor:
    def __init__(self, logger=None):
        self.logger = logger or NLMLogger()
        self.performance_metrics = {}
        self.start_time = time.time()
    
    def record_metric(self, name, value, context=None):
        timestamp = time.time()
        self.performance_metrics[name] = {
            'value': value,
            'timestamp': timestamp,
            'context': context or {}
        }
        
        self.logger.debug(f"Performance metric recorded: {name} = {value}", context=context)
    
    def get_metric(self, name):
        if name in self.performance_metrics:
            return self.performance_metrics[name]['value']
        return None
    
    def get_all_metrics(self):
        return self.performance_metrics
    
    def print_system_status(self):
        print(f"{BLUE}=== NLM System Status ==={NC}")
        print(f"Uptime: {time.time() - self.start_time:.2f} seconds")
        
        if self.performance_metrics:
            print(f"{CYAN}Performance Metrics:{NC}")
            for name, data in self.performance_metrics.items():
                print(f"  {name}: {data['value']}")
        
        print(f"{CYAN}Log Level: {self.logger.get_log_level()}{NC}")
        print(f"{CYAN}Log File: {self.logger.log_file}{NC}")
        print(f"{CYAN}Error Log File: {self.logger.error_log_file}{NC}")
        
        # Get recent error count
        recent_errors = len(self.logger.get_error_logs(limit=5))
        print(f"{YELLOW}Recent errors: {recent_errors}{NC}")

# Global instances
logger = NLMLogger()
error_handler = NLMErrorHandler(logger)
system_monitor = NLMSystemMonitor(logger)

# Convenience functions

def debug(message, context=None):
    logger.debug(message, context)

def info(message, context=None):
    logger.info(message, context)

def warning(message, context=None):
    logger.warning(message, context)

def error(message, error_type="Unknown", stack_trace=None, context=None, retry_count=0):
    logger.error(message, error_type, stack_trace, context, retry_count)

def critical(message, context=None):
    logger.critical(message, context)

def set_log_level(level):
    logger.set_log_level(level)

def handle_error(error, context=None):
    return error_handler.handle_error(error, context)

def record_metric(name, value, context=None):
    system_monitor.record_metric(name, value, context)

def print_system_status():
    system_monitor.print_system_status()

# Exception handling

class NLMException(Exception):
    def __init__(self, message, error_type="GenericError", context=None):
        super().__init__(message)
        self.error_type = error_type
        self.context = context or {}
    
    def __str__(self):
        return f"[{self.error_type}] {super().__str__()}"

# Example usage
if __name__ == "__main__":
    print("NLM Logging and Error Handling System")
    print("==================================")
    
    # Test logging
    info("System initialized")
    debug("Debug message")
    warning("Warning message")
    error("Error message", error_type="ConfigurationError")
    critical("Critical error")
    
    # Test error handling
    try:
        handle_error(ValueError("Test error"), {"test": "context"})
    except Exception as e:
        print(f"Error: {e}")
    
    # Test system monitor
    record_metric("neuron_count", 500)
    record_metric("synapse_count", 10000)
    print_system_status()
    
    print("\nDemo completed successfully!")
