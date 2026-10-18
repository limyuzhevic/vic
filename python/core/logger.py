"""
Logging system for NLM

This module provides logging functionality for the NLM system.
"""

import time
import sys
from enum import Enum
from typing import List

class LogLevel(Enum):
    Debug = 0
    Info = 1
    Warning = 2
    Error = 3
    Critical = 4

    @classmethod
    def from_string(cls, level_str: str):
        mapping = {
            "debug": cls.Debug,
            "info": cls.Info,
            "warning": cls.Warning,
            "error": cls.Error,
            "critical": cls.Critical
        }
        return mapping.get(level_str.lower(), cls.Info)

    def __str__(self):
        return self.name

class LogEntry:
    def __init__(self, level: LogLevel, message: str, timestamp: float = None):
        self.level = level
        self.message = message
        self.timestamp = timestamp or time.time()

    def __repr__(self):
        return f"[{self.level}] {self.timestamp}: {self.message}"

class Logger:
    def __init__(self):
        self._entries: List[LogEntry] = []
        self._level = LogLevel.Info
        self._console_output = True

    def setLevel(self, level: LogLevel):
        self._level = level

    def debug(self, message: str):
        if self._level <= LogLevel.Debug:
            entry = LogEntry(LogLevel.Debug, message)
            self._entries.append(entry)
            if self._console_output:
                print(f"[DEBUG] {entry.timestamp}: {message}")

    def info(self, message: str):
        if self._level <= LogLevel.Info:
            entry = LogEntry(LogLevel.Info, message)
            self._entries.append(entry)
            if self._console_output:
                print(f"[INFO] {entry.timestamp}: {message}")

    def warning(self, message: str):
        if self._level <= LogLevel.Warning:
            entry = LogEntry(LogLevel.Warning, message)
            self._entries.append(entry)
            if self._console_output:
                print(f"[WARNING] {entry.timestamp}: {message}")

    def error(self, message: str):
        if self._level <= LogLevel.Error:
            entry = LogEntry(LogLevel.Error, message)
            self._entries.append(entry)
            if self._console_output:
                print(f"[ERROR] {entry.timestamp}: {message}")

    def critical(self, message: str):
        if self._level <= LogLevel.Critical:
            entry = LogEntry(LogLevel.Critical, message)
            self._entries.append(entry)
            if self._console_output:
                print(f"[CRITICAL] {entry.timestamp}: {message}")

    def getEntries(self) -> List[LogEntry]:
        return self._entries.copy()

    def getLevel(self) -> LogLevel:
        return self._level

    def clear(self):
        self._entries.clear()

    def __repr__(self):
        return f"Logger(level={self._level}, entries={len(self._entries)})"

# Global logger instance
_logger_instance = Logger()

class ConsoleLogger:
    def __init__(self, level: LogLevel = LogLevel.Info):
        self._level = level

    def log(self, level: LogLevel, message: str):
        if self._level <= level:
            print(f"[{level}] {time.time()}: {message}")

# Global functions
_logger_instance = Logger()

logger_instance = Logger()

# NLM_LOG macros for compatibility with C++ code
_get_logger = getGlobal

def NLM_LOG_DEBUG(message: str):
    getGlobal().debug(message)

def NLM_LOG_INFO(message: str):
    getGlobal().info(message)

def NLM_LOG_WARNING(message: str):
    getGlobal().warning(message)

def NLM_LOG_ERROR(message: str):
    getGlobal().error(message)

def NLM_LOG_CRITICAL(message: str):
    getGlobal().critical(message)

# Global logger setter
_logger_instance = None

def setGlobal(logger):
    global _logger_instance
    _logger_instance = logger

def getGlobal():
    return _logger_instance

# Initialize with default logger
setGlobal(Logger())

# Convenience functions

def createLogger():
    return Logger()

__all__ = [
    'LogLevel',
    'LogEntry',
    'Logger',
    'ConsoleLogger',
    'createLogger',
    'NLM_LOG_DEBUG',
    'NLM_LOG_INFO',
    'NLM_LOG_WARNING',
    'NLM_LOG_ERROR',
    'NLM_LOG_CRITICAL',
    'getGlobal',
    'setGlobal',
]