"""
Logging utilities for NLM.
"""

from typing import List
import time
import sys
from datetime import datetime

class LogLevel:
    """Log levels for NLM."""
    
    DEBUG = 0
    INFO = 1
    WARNING = 2
    ERROR = 3
    CRITICAL = 4

class LogMessage:
    """Individual log message."""
    
    def __init__(self, level: int, message: str, timestamp: float):
        self.level = level
        self.message = message
        self.timestamp = timestamp
    
    def __str__(self) -> str:
        level_names = {
            LogLevel.DEBUG: "DEBUG",
            LogLevel.INFO: "INFO",
            LogLevel.WARNING: "WARNING",
            LogLevel.ERROR: "ERROR",
            LogLevel.CRITICAL: "CRITICAL"
        }
        level_name = level_names.get(self.level, "UNKNOWN")
        time_str = datetime.fromtimestamp(self.timestamp).strftime("%H:%M:%S")
        return f"[{time_str}] {level_name}: {self.message}"

class Logger:
    """
    Logger for NLM neural simulation.
    
    Provides logging with different levels and handlers.
    """
    
    _global_logger: Optional['Logger'] = None
    
    def __init__(self):
        self.handlers: List = []
        self.messages: List[LogMessage] = []
    
    def add_handler(self, handler) -> None:
        """
        Add a log handler.
        
        Args:
            handler: Log handler (function or object with write method)
        """
        self.handlers.append(handler)
    
    def remove_handler(self, handler) -> None:
        """
        Remove a log handler.
        
        Args:
            handler: Log handler to remove
        """
        if handler in self.handlers:
            self.handlers.remove(handler)
    
    def log(self, level: int, message: str) -> None:
        """
        Log a message.
        
        Args:
            level: Log level
            message: Log message
        """
        timestamp = time.time()
        msg = LogMessage(level, message, timestamp)
        self.messages.append(msg)
        
        for handler in self.handlers:
            if callable(handler):
                handler(level, message)
            else:
                handler.write(f"{msg}\n")
    
    def debug(self, message: str) -> None:
        """Log debug message."""
        self.log(LogLevel.DEBUG, message)
    
    def info(self, message: str) -> None:
        """Log info message."""
        self.log(LogLevel.INFO, message)
    
    def warning(self, message: str) -> None:
        """Log warning message."""
        self.log(LogLevel.WARNING, message)
    
    def error(self, message: str) -> None:
        """Log error message."""
        self.log(LogLevel.ERROR, message)
    
    def critical(self, message: str) -> None:
        """Log critical message."""
        self.log(LogLevel.CRITICAL, message)
    
    def get_messages(self, level: Optional[int] = None) -> List[LogMessage]:
        """
        Get log messages.
        
        Args:
            level: Filter by log level
            
        Returns:
            List of log messages
        """
        if level is None:
            return self.messages
        return [msg for msg in self.messages if msg.level == level]
    
    def clear(self) -> None:
        """Clear all log messages."""
        self.messages.clear()
    
    @classmethod
    def set_global(cls, logger: 'Logger') -> None:
        """
        Set the global logger.
        
        Args:
            logger: Logger instance
        """
        cls._global_logger = logger
    
    @classmethod
    def get_global(cls) -> 'Logger':
        """
        Get the global logger.
        
        Returns:
            Global logger instance
        """
        if cls._global_logger is None:
            cls._global_logger = cls()
        return cls._global_logger

__all__ = ['LogLevel', 'LogMessage', 'Logger']
