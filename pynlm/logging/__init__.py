"""
pynlm.logging - Advanced logging system for NLM project

This module provides comprehensive logging capabilities including:
- Multi-level logging (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- Context-aware logging (brain regions, simulation steps)
- Log file rotation and management
- Log level configuration and filtering
- Structured logging with JSON format
"""

import logging
import json
import os
import time
import threading
from datetime import datetime
from typing import Dict, Any, Optional, List, Union, Callable
from pathlib import Path
import logging.handlers
import traceback
from dataclasses import dataclass, asdict

logger = logging.getLogger(__name__)


@dataclass
class LogContext:
    """Context for structured logging."""
    brain_region: Optional[str] = None
    simulation_step: Optional[int] = None
    agent_id: Optional[str] = None
    timestamp: Optional[float] = None
    experiment_id: Optional[str] = None
    session_id: Optional[str] = None
    metadata: Dict[str, Any] = None

    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = time.time()
        if self.metadata is None:
            self.metadata = {}


class JSONFormatter(logging.Formatter):
    """JSON formatter for structured logging."""

    def __init__(self, include_context: bool = True):
        super().__init__()
        self.include_context = include_context
        self.context_stack = []

    def push_context(self, context: LogContext) -> None:
        """Push a context onto the stack."""
        self.context_stack.append(context)

    def pop_context(self) -> Optional[LogContext]:
        """Pop a context from the stack."""
        return self.context_stack.pop() if self.context_stack else None

    def get_current_context(self) -> Optional[LogContext]:
        """Get the current context."""
        return self.context_stack[-1] if self.context_stack else None

    def format(self, record: logging.LogRecord) -> str:
        """Format log record as JSON."""
        log_entry = {
            "timestamp": datetime.fromtimestamp(record.created).isoformat(),
            "level": record.levelname,
            "logger": record.name,
            "message": record.getMessage(),
        }

        if self.include_context:
            context = self.get_current_context()
            if context:
                log_entry["context"] = asdict(context)

        if hasattr(record, 'extra_data'):
            log_entry["extra_data"] = record.extra_data

        if hasattr(record, 'error_details'):
            log_entry["error_details"] = record.error_details

        return json.dumps(log_entry, default=str)


class NLMLogManager:
    """Manager for NLM logging system with file rotation and level management."""

    def __init__(self, log_dir: str = "./logs", max_file_size_mb: int = 100,
                 backup_count: int = 5, enable_json: bool = True):
        """
        Initialize NLM log manager.

        Parameters
        ----------
        log_dir : str
            Directory for log files
        max_file_size_mb : int
            Maximum log file size in MB before rotation
        backup_count : int
            Number of backup files to keep
        enable_json : bool
            Enable JSON formatting for structured logs
        """
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self.max_file_size = max_file_size_mb * 1024 * 1024
        self.backup_count = backup_count
        self.enable_json = enable_json
        self.formatter = JSONFormatter(include_context=True) if enable_json else None
        self.loggers: Dict[str, logging.Logger] = {}
        self.file_handlers: Dict[str, logging.handlers.RotatingFileHandler] = {}
        self.console_handlers: Dict[str, logging.StreamHandler] = {}
        self.context_managers: Dict[str, LogContext] = {}

    def get_logger(self, name: str, level: str = 'INFO', 
                   use_rotation: bool = True, log_to_file: bool = True) -> logging.Logger:
        """
        Get or create a logger with NLM-specific configuration.

        Parameters
        ----------
        name : str
            Logger name
        level : str
            Logging level
        use_rotation : bool
            Enable log file rotation
        log_to_file : bool
            Log to file in addition to console

        Returns
        -------
        logging.Logger
            Configured logger
        """
        if name in self.loggers:
            return self.loggers[name]

        logger = logging.getLogger(name)
        logger.setLevel(getattr(logging, level.upper()))

        # Remove existing handlers
        for handler in logger.handlers[:]:
            logger.removeHandler(handler)

        # Console handler
        console_handler = logging.StreamHandler()
        if self.formatter:
            console_handler.setFormatter(self.formatter)
        else:
            console_handler.setFormatter(
                logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            )
        logger.addHandler(console_handler)
        self.console_handlers[name] = console_handler

        # File handler with rotation
        if log_to_file:
            log_file = self.log_dir / f"{name.replace('.', '_')}.log"
            if use_rotation:
                file_handler = logging.handlers.RotatingFileHandler(
                    log_file, maxBytes=self.max_file_size,
                    backupCount=self.backup_count
                )
            else:
                file_handler = logging.FileHandler(log_file)

            if self.formatter:
                file_handler.setFormatter(self.formatter)
            else:
                file_handler.setFormatter(
                    logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
                )
            logger.addHandler(file_handler)
            self.file_handlers[name] = file_handler

        # Add NLM-specific filters and handlers
        logger.addFilter(self._create_context_filter(name))

        self.loggers[name] = logger
        logger.info(f"Logger initialized: {name}")

        return logger

    def _create_context_filter(self, logger_name: str):
        """Create a filter for NLM context."""
        def context_filter(record):
            # Add context information to log record
            context = self.context_managers.get(logger_name)
            if context:
                record.context = asdict(context)
            return True
        return context_filter

    def set_context(self, logger_name: str, context: LogContext) -> None:
        """Set the context for a specific logger."""
        self.context_managers[logger_name] = context
        if self.formatter:
            self.formatter.push_context(context)

    def clear_context(self, logger_name: str) -> None:
        """Clear the context for a specific logger."""
        if logger_name in self.context_managers:
            del self.context_managers[logger_name]
        if self.formatter:
            self.formatter.pop_context()

    def log_simulation_step(self, logger_name: str, step: int, message: str,
                           level: str = 'INFO', **kwargs) -> None:
        """Log a simulation step with context."""
        logger = self.get_logger(logger_name)
        context = LogContext(simulation_step=step, metadata=kwargs)
        self.set_context(logger_name, context)

        log_method = getattr(logger, level.lower())
        log_method(f"Step {step}: {message}")

        self.clear_context(logger_name)

    def log_brain_region(self, logger_name: str, region: str, message: str,
                        level: str = 'INFO', **kwargs) -> None:
        """Log a brain region activity with context."""
        logger = self.get_logger(logger_name)
        context = LogContext(brain_region=region, metadata=kwargs)
        self.set_context(logger_name, context)

        log_method = getattr(logger, level.lower())
        log_method(f"Region {region}: {message}")

        self.clear_context(logger_name)

    def log_agent(self, logger_name: str, agent_id: str, message: str,
                 level: str = 'INFO', **kwargs) -> None:
        """Log an agent-specific message with context."""
        logger = self.get_logger(logger_name)
        context = LogContext(agent_id=agent_id, metadata=kwargs)
        self.set_context(logger_name, context)

        log_method = getattr(logger, level.lower())
        log_method(f"Agent {agent_id}: {message}")

        self.clear_context(logger_name)

    def configure_levels(self, logger_configs: Dict[str, str]) -> None:
        """
        Configure logging levels for multiple loggers.

        Parameters
        ----------
        logger_configs : Dict[str, str]
            Dictionary mapping logger names to levels
        """
        for logger_name, level in logger_configs.items():
            if logger_name in self.loggers:
                self.loggers[logger_name].setLevel(getattr(logging, level.upper()))
                logger.info(f"Set logging level for {logger_name} to {level}")

    def get_log_stats(self) -> Dict[str, Any]:
        """Get statistics about log files."""
        stats = {}
        for name, handler in self.file_handlers.items():
            if isinstance(handler, logging.handlers.RotatingFileHandler):
                stats[name] = {
                    'current_size': handler.baseFilename and os.path.getsize(
                        handler.baseFilename) or 0,
                    'max_size': self.max_file_size,
                    'backup_count': self.backup_count
                }
        return stats


# Global log manager instance
_log_manager = None


def get_log_manager() -> NLMLogManager:
    """Get the global NLM log manager instance."""
    global _log_manager
    if _log_manager is None:
        _log_manager = NLMLogManager()
    return _log_manager


def setup_logging(log_file: Optional[str] = None, level: str = 'INFO',
                  format: Optional[str] = None, rotation: bool = True) -> logging.Logger:
    """
    Setup NLM logging configuration.

    Parameters
    ----------
    log_file : str, optional
        Path to log file, by default None (console only)
    level : str, optional
        Logging level, by default 'INFO'
    format : str, optional
        Log message format, by default None (JSON format)
    rotation : bool, optional
        Enable log file rotation, by default True

    Returns
    -------
    logging.Logger
        Configured logger
    """
    manager = get_log_manager()

    if log_file:
        # Extract logger name from file path
        log_name = Path(log_file).stem
        return manager.get_logger(log_name, level, rotation, True)
    else:
        # Default logger for the package
        return manager.get_logger('pynlm', level, rotation, False)


def log_simulation_step(logger_name: str, step: int, message: str,
                       level: str = 'INFO', **kwargs) -> None:
    """Convenience function to log a simulation step."""
    get_log_manager().log_simulation_step(logger_name, step, message, level, **kwargs)


def log_brain_region(logger_name: str, region: str, message: str,
                    level: str = 'INFO', **kwargs) -> None:
    """Convenience function to log a brain region activity."""
    get_log_manager().log_brain_region(logger_name, region, message, level, **kwargs)


def log_agent(logger_name: str, agent_id: str, message: str,
             level: str = 'INFO', **kwargs) -> None:
    """Convenience function to log an agent-specific message."""
    get_log_manager().log_agent(logger_name, agent_id, message, level, **kwargs)


# Specialized loggers for common NLM components
brain_logger = None
memory_logger = None
simulation_logger = None
plastic_logger = None
visualization_logger = None
checkpoint_logger = None


def get_brain_logger() -> logging.Logger:
    """Get brain region logger."""
    global brain_logger
    if brain_logger is None:
        manager = get_log_manager()
        brain_logger = manager.get_logger('brain', 'DEBUG')
    return brain_logger


def get_memory_logger() -> logging.Logger:
    """Get memory logger."""
    global memory_logger
    if memory_logger is None:
        manager = get_log_manager()
        memory_logger = manager.get_logger('memory', 'DEBUG')
    return memory_logger


def get_simulation_logger() -> logging.Logger:
    """Get simulation logger."""
    global simulation_logger
    if simulation_logger is None:
        manager = get_log_manager()
        simulation_logger = manager.get_logger('simulation', 'INFO')
    return simulation_logger


def get_plastic_logger() -> logging.Logger:
    """Get plasticity logger."""
    global plastic_logger
    if plastic_logger is None:
        manager = get_log_manager()
        plastic_logger = manager.get_logger('plasticity', 'DEBUG')
    return plastic_logger


def get_visualization_logger() -> logging.Logger:
    """Get visualization logger."""
    global visualization_logger
    if visualization_logger is None:
        manager = get_log_manager()
        visualization_logger = manager.get_logger('visualization', 'INFO')
    return visualization_logger


def get_checkpoint_logger() -> logging.Logger:
    """Get checkpoint logger."""
    global checkpoint_logger
    if checkpoint_logger is None:
        manager = get_log_manager()
        checkpoint_logger = manager.get_logger('checkpoint', 'INFO')
    return checkpoint_logger