"""
Logger module with enhanced Python API for NLM

This module provides structured logging support with log level filtering,
file output with rotation, and log level hierarchy.
"""

import os
import time
import threading
from typing import Optional, Dict, Any, List, Union
from enum import Enum
from datetime import datetime
import json

# Import the C++ bindings
from .pynlm import (
    LogLevel as CppLogLevel,
    LogEntry as CppLogEntry,
    ILogger as CppILogger,
    ConsoleLogger as CppConsoleLogger,
    Logger as CppLogger,
)

class LogLevel(Enum):
    """Log level hierarchy."""
    
    DEBUG = CppLogLevel.Debug
    INFO = CppLogLevel.Info
    WARNING = CppLogLevel.Warning
    ERROR = CppLogLevel.Error
    CRITICAL = CppLogLevel.Critical
    
    @classmethod
    def from_string(cls, level_str: str) -> 'LogLevel':
        """Create LogLevel from string.
        
        Args:
            level_str: Log level string (debug, info, warning, error, critical)
            
        Returns:
            LogLevel enum
        """
        level_map = {
            'debug': cls.DEBUG,
            'info': cls.INFO,
            'warning': cls.WARNING,
            'error': cls.ERROR,
            'critical': cls.CRITICAL,
        }
        return level_map.get(level_str.lower(), cls.INFO)
    
    def to_string(self) -> str:
        """Convert to string.
        
        Returns:
            Log level string
        """
        return self.name.lower()
class StructuredLogEntry:
    """Structured log entry with key-value pairs."""
    
    def __init__(self, level: LogLevel, message: str, 
                 file: Optional[str] = None, line: Optional[int] = None,
                 function: Optional[str] = None, extra: Optional[Dict[str, Any]] = None):
        """Initialize structured log entry.
        
        Args:
            level: Log level
            message: Log message
            file: Source file
            line: Line number
            function: Function name
            extra: Additional key-value data
        """
        self.level = level
        self.message = message
        self.file = file
        self.line = line
        self.function = function
        self.extra = extra or {}
        self.timestamp = datetime.now()
        self.thread_id = threading.current_thread().ident
        
        # Add to global stats
        StructuredLogger.add_to_stats(level, message)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary.
        
        Returns:
            Dictionary representation
        """
        entry = {
            'timestamp': self.timestamp.isoformat(),
            'level': self.level.to_string(),
            'message': self.message,
            'thread_id': self.thread_id,
            'file': self.file,
            'line': self.line,
            'function': self.function,
            'extra': self.extra,
        }
        return entry
    
    def to_json(self) -> str:
        """Convert to JSON string.
        
        Returns:
            JSON string
        """
        return json.dumps(self.to_dict())
class LogFileHandler:
    """Log file handler with rotation support."""
    
    def __init__(self, filepath: str, max_size_mb: int = 10, 
                 backup_count: int = 5, encoding: str = 'utf-8'):
        """Initialize log file handler.
        
        Args:
            filepath: Path to log file
            max_size_mb: Maximum file size in MB before rotation
            backup_count: Number of backup files to keep
            encoding: File encoding
        """
        self.filepath = filepath
        self.max_size = max_size_mb * 1024 * 1024
        self.backup_count = backup_count
        self.encoding = encoding
        self._file = None
        self._file_size = 0
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(os.path.abspath(filepath)), exist_ok=True)
    
    def write(self, entry: StructuredLogEntry):
        """Write log entry to file.
        
        Args:
            entry: Log entry to write
        """
        # Check if rotation is needed
        if self._file_size >= self.max_size:
            self._rotate()
        
        if self._file is None:
            self._file = open(self.filepath, 'a', encoding=self.encoding)
        
        # Write entry
        json_line = entry.to_json() + '\n'
        self._file.write(json_line)
        self._file.flush()
        self._file_size += len(json_line.encode(self.encoding))
    
    def _rotate(self):
        """Rotate log file."""
        if self._file:
            self._file.close()
        
        # Close current file and create backup
        for i in range(self.backup_count - 1, 0, -1):
            if os.path.exists(f"{self.filepath}.{i}"):
                os.rename(f"{self.filepath}.{i}", f"{self.filepath}.{i+1}")
        
        if os.path.exists(self.filepath):
            os.rename(self.filepath, f"{self.filepath}.1")
        
        # Reopen file for new log
        self._file = open(self.filepath, 'a', encoding=self.encoding)
        self._file_size = 0
    
    def close(self):
        """Close log file."""
        if self._file:
            self._file.close()
            self._file = None
class ConsoleHandler:
    """Console log handler with formatting."""
    
    def __init__(self, use_colors: bool = True, 
                 format_string: Optional[str] = None):
        """Initialize console handler.
        
        Args:
            use_colors: Whether to use ANSI color codes
            format_string: Custom format string
        """
        self.use_colors = use_colors
        self.format_string = format_string or "%(timestamp)s - %(name)s - %(level)s - %(message)s"
    
    def write(self, entry: StructuredLogEntry):
        """Write log entry to console.
        
        Args:
            entry: Log entry to write
        """
        # Format message
        level_str = entry.level.to_string().upper()
        timestamp_str = entry.timestamp.strftime('%Y-%m-%d %H:%M:%S')
        
        if self.use_colors:
            color_code = {
                'DEBUG': '\033[36m',    # Cyan
                'INFO': '\033[32m',     # Green
                'WARNING': '\033[33m',  # Yellow
                'ERROR': '\033[31m',    # Red
                'CRITICAL': '\033[41m', # Red background
            }
            reset_code = '\033[0m'
            level_str = f"{color_code.get(level_str, '')}{level_str}{reset_code}"
        
        # Build formatted message
        message_parts = [f"[{timestamp_str}] [{level_str}] [{entry.function or 'unknown'}:{(entry.line or 0)}]"]
        message_parts.append(entry.message)
        
        if entry.extra:
            extra_str = ', '.join(f"{k}={v}" for k, v in entry.extra.items())
            message_parts.append(f"[{extra_str}]")
        
        formatted_message = ' '.join(message_parts)
        print(formatted_message)
class StructuredLogger:
    """Structured logger with key-value pairs and level hierarchy."""
    
    def __init__(self, name: str = "NLM", level: LogLevel = LogLevel.INFO,
                 console_handler: Optional[ConsoleHandler] = None,
                 file_handler: Optional[LogFileHandler] = None):
        """Initialize structured logger.
        
        Args:
            name: Logger name
            level: Minimum log level
            console_handler: Console handler (creates default if None)
            file_handler: File handler (creates default if None)
        """
        self.name = name
        self.level = level
        self.console_handler = console_handler or ConsoleHandler()
        self.file_handler = file_handler
        self._cpp_logger = CppLogger()
        self._log_queue = []
        self._flush_thread = threading.Thread(target=self._flush_worker, daemon=True)
        self._flush_thread.start()
        
        # Add to global stats
        add_to_global_stats('logger_created', {'name': name, 'level': level.to_string()})
    
    def debug(self, message: str, **kwargs):
        """Log debug message.
        
        Args:
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= LogLevel.DEBUG.value:
            entry = StructuredLogEntry(LogLevel.DEBUG, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.debug(message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def info(self, message: str, **kwargs):
        """Log info message.
        
        Args:
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= LogLevel.INFO.value:
            entry = StructuredLogEntry(LogLevel.INFO, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.info(message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def warning(self, message: str, **kwargs):
        """Log warning message.
        
        Args:
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= LogLevel.WARNING.value:
            entry = StructuredLogEntry(LogLevel.WARNING, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.warning(message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def error(self, message: str, **kwargs):
        """Log error message.
        
        Args:
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= LogLevel.ERROR.value:
            entry = StructuredLogEntry(LogLevel.ERROR, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.error(message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def critical(self, message: str, **kwargs):
        """Log critical message.
        
        Args:
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= LogLevel.CRITICAL.value:
            entry = StructuredLogEntry(LogLevel.CRITICAL, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.critical(message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def log(self, level: LogLevel, message: str, **kwargs):
        """Log message at specified level.
        
        Args:
            level: Log level
            message: Log message
            **kwargs: Additional context
        """
        if self.level.value <= level.value:
            entry = StructuredLogEntry(level, message, **kwargs)
            self._log_async(entry)
            self._cpp_logger.log(level.value, message, kwargs.get('file'), kwargs.get('line'), kwargs.get('function'))
    
    def _log_async(self, entry: StructuredLogEntry):
        """Log entry asynchronously.
        
        Args:
            entry: Log entry to log
        """
        with threading.Lock():
            self._log_queue.append(entry)
    
    def _flush_worker(self):
        """Background worker to flush log queue."""
        while True:
            time.sleep(0.1)  # Flush every 100ms
            self._flush_queue()
    
    def _flush_queue(self):
        """Flush log queue."""
        entries = []
        with threading.Lock():
            if self._log_queue:
                entries = self._log_queue.copy()
                self._log_queue.clear()
        
        for entry in entries:
            # Write to file handler if present
            if self.file_handler:
                try:
                    self.file_handler.write(entry)
                except Exception as e:
                    print(f"Error writing to log file: {e}")
            
            # Write to console handler
            try:
                self.console_handler.write(entry)
            except Exception as e:
                print(f"Error writing to console: {e}")
    
    def set_level(self, level: LogLevel):
        """Set minimum log level.
        
        Args:
            level: New minimum log level
        """
        self.level = level
        self._cpp_logger.setLevel(level.value)
    
    def get_level(self) -> LogLevel:
        """Get current log level.
        
        Returns:
            Current log level
        """
        return self.level
    
    @classmethod
    def get_logger(cls, name: str = "NLM") -> 'StructuredLogger':
        """Get a logger instance.
        
        Args:
            name: Logger name
            
        Returns:
            Logger instance
        """
        return cls(name)
class LoggerManager:
    """Global logger manager for NLM."""
    
    def __init__(self):
        self._loggers: Dict[str, StructuredLogger] = {}
        self._file_handlers: Dict[str, LogFileHandler] = {}
        self._console_handlers: Dict[str, ConsoleHandler] = {}
        self._global_stats: Dict[str, Any] = {}
    
    def get_logger(self, name: str = "NLM", level: LogLevel = LogLevel.INFO,
                   console: bool = True, file: Optional[str] = None) -> StructuredLogger:
        """Get or create a logger instance.
        
        Args:
            name: Logger name
            level: Minimum log level
            console: Whether to enable console logging
            file: Optional log file path
            
        Returns:
            Logger instance
        """
        if name not in self._loggers:
            console_handler = ConsoleHandler() if console else None
            file_handler = None
            
            if file:
                file_handler = LogFileHandler(file)
                self._file_handlers[name] = file_handler
            
            if console_handler:
                self._console_handlers[name] = console_handler
            
            self._loggers[name] = StructuredLogger(
                name=name,
                level=level,
                console_handler=console_handler,
                file_handler=file_handler
            )
        
        return self._loggers[name]
    
    def get_global_stats(self) -> Dict[str, Any]:
        """Get global logging statistics.
        
        Returns:
            Global statistics
        """
        return self._global_stats.copy()
    
    def add_to_stats(self, key: str, value: Any):
        """Add to global statistics.
        
        Args:
            key: Statistic key
            value: Statistic value
        """
        if key not in self._global_stats:
            self._global_stats[key] = 0
        
        if isinstance(self._global_stats[key], (int, float)) and isinstance(value, (int, float)):
            self._global_stats[key] += value
        else:
            self._global_stats[key] = value
    
    def reset_stats(self):
        """Reset global statistics."""
        self._global_stats.clear()
    
    def close_all(self):
        """Close all log files."""
        for file_handler in self._file_handlers.values():
            file_handler.close()
        
        self._file_handlers.clear()
        self._console_handlers.clear()
# Global logger manager instance
_logger_manager = LoggerManager()

# Convenience functions
def get_logger(name: str = "NLM", level: LogLevel = LogLevel.INFO,
              console: bool = True, file: Optional[str] = None) -> StructuredLogger:
    """Get a logger instance.
    
    Args:
        name: Logger name
        level: Minimum log level
        console: Whether to enable console logging
        file: Optional log file path
        
    Returns:
        Logger instance
    """
    return _logger_manager.get_logger(name, level, console, file)

def log_to_file(logger_name: str, filepath: str, level: LogLevel = LogLevel.INFO,
                max_size_mb: int = 10, backup_count: int = 5):
    """Configure logging to file for a logger.
    
    Args:
        logger_name: Logger name
        filepath: Path to log file
        level: Minimum log level
        max_size_mb: Maximum file size in MB
        backup_count: Number of backup files
        
    Returns:
        LogFileHandler instance
    """
    logger_instance = _logger_manager.get_logger(logger_name, level, console=False)
    file_handler = LogFileHandler(filepath, max_size_mb, backup_count)
    logger_instance.file_handler = file_handler
    _logger_manager._file_handlers[logger_name] = file_handler
    return file_handler

def log_to_console(logger_name: str, level: LogLevel = LogLevel.INFO, 
                  use_colors: bool = True, format_string: Optional[str] = None):
    """Configure logging to console for a logger.
    
    Args:
        logger_name: Logger name
        level: Minimum log level
        use_colors: Whether to use colors
        format_string: Custom format string
        
    Returns:
        ConsoleHandler instance
    """
    logger_instance = _logger_manager.get_logger(logger_name, level, console=True)
    console_handler = ConsoleHandler(use_colors, format_string)
    logger_instance.console_handler = console_handler
    _logger_manager._console_handlers[logger_name] = console_handler
    return console_handler

def add_to_global_stats(key: str, value: Any):
    """Add to global statistics.
    
    Args:
        key: Statistic key
        value: Statistic value
    """
    _logger_manager.add_to_stats(key, value)

def get_global_stats() -> Dict[str, Any]:
    """Get global logging statistics.
    
    Returns:
        Global statistics
    """
    return _logger_manager.get_global_stats()

def close_all_loggers():
    """Close all log files and handlers."""
    _logger_manager.close_all()

# Export types and classes
__all__ = [
    'LogLevel',
    'StructuredLogEntry',
    'LogFileHandler',
    'ConsoleHandler',
    'StructuredLogger',
    'LoggerManager',
    'get_logger',
    'log_to_file',
    'log_to_console',
    'add_to_global_stats',
    'get_global_stats',
    'close_all_loggers',
]