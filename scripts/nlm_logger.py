#!/usr/bin/env python3
"""
NLM Advanced Logger
Enhanced logging system for Neural Learning Machine

Features:
- Multi-level logging with filtering
- Structured logging (JSON, YAML)
- Performance monitoring integration
- Log rotation and backup
- Color-coded console output
- Log level configuration
- Custom log formatters
"""

import os
import sys
import time
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional, Union
from datetime import datetime
from enum import Enum
import threading
import logging
from logging.handlers import RotatingFileHandler

class LogLevel(Enum):
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

class LogFormat(Enum):
    TEXT = "text"
    JSON = "json"
    STRUCTURED = "structured"

class ColorFormatter(logging.Formatter):
    """Color-coded log formatter for console output."""
    
    COLORS = {
        'DEBUG': '\033[36m',    # Cyan
        'INFO': '\033[32m',     # Green
        'WARNING': '\033[33m',  # Yellow
        'ERROR': '\033[31m',    # Red
        'CRITICAL': '\033[41m', # Red background
    }
    
    RESET = '\033[0m'
    
    def format(self, record):
        levelname = record.levelname
        message = super().format(record)
        
        if levelname in self.COLORS:
            color = self.COLORS[levelname]
            return f"{color}{levelname}{self.RESET}: {message}"
        
        return f"{levelname}: {message}"

class StructuredFormatter(logging.Formatter):
    """Structured log formatter for JSON/YAML output."""
    
    def __init__(self, format_type: LogFormat = LogFormat.JSON):
        self.format_type = format_type
        super().__init__()
    
    def format(self, record):
        log_entry = {
            'timestamp': datetime.now().isoformat(),
            'level': record.levelname,
            'message': record.getMessage(),
            'module': record.name,
            'thread': threading.current_thread().name,
        }
        
        # Add extra fields if present
        if hasattr(record, 'extra_data'):
            log_entry['extra'] = record.extra_data
        
        if self.format_type == LogFormat.JSON:
            return json.dumps(log_entry)
        elif self.format_type == LogFormat.STRUCTURED:
            return yaml.dump(log_entry, default_flow_style=False)
        else:
            return super().format(record)

class PerformanceLogger:
    """Performance monitoring and logging."""
    
    def __init__(self):
        self.performance_metrics = {}
        self.start_times = {}
    
    def start_operation(self, operation_name: str):
        """Start timing an operation."""
        self.start_times[operation_name] = time.perf_counter()
    
    def end_operation(self, operation_name: str) -> float:
        """End timing an operation and return duration."""
        if operation_name not in self.start_times:
            return 0.0
        
        end_time = time.perf_counter()
        duration = end_time - self.start_times[operation_name]
        del self.start_times[operation_name]
        
        # Store metric
        if operation_name not in self.performance_metrics:
            self.performance_metrics[operation_name] = []
        self.performance_metrics[operation_name].append(duration)
        
        return duration
    
    def end_performance(self, operation_name: str) -> float:
        """Alias for end_operation."""
        return self.end_operation(operation_name)
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get performance metrics summary."""
        summary = {}
        
        for operation, times in self.performance_metrics.items():
            if times:
                summary[operation] = {
                    'count': len(times),
                    'min': min(times) * 1000,
                    'max': max(times) * 1000,
                    'mean': statistics.mean(times) * 1000,
                    'median': statistics.median(times) * 1000
                }
        
        return summary

class NLMLogger:
    """Advanced logger for NLM."""
    
    def __init__(self, config: Optional[Dict] = None):
        self.config = config or {}
        self.performance_logger = PerformanceLogger()
        self.root_logger = logging.getLogger("nlm")
        self.root_logger.setLevel(logging.DEBUG)
        
        # Remove existing handlers
        for handler in self.root_logger.handlers[:]:
            self.root_logger.removeHandler(handler)
        
        # Setup handlers based on configuration
        self.setup_handlers()
    
    def setup_handlers(self):
        """Setup logging handlers based on configuration."""
        log_level = self.config.get('log_level', 'INFO').upper()
        log_format = self.config.get('log_format', 'text').lower()
        log_to_file = self.config.get('log_to_file', False)
        log_filename = self.config.get('log_filename', 'nlm.log')
        log_rotation_kb = self.config.get('log_rotation_kb', 1024)
        
        # Set level
        level = getattr(logging, log_level)
        self.root_logger.setLevel(level)
        
        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(level)
        
        if log_format == 'text':
            console_handler.setFormatter(ColorFormatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            ))
        elif log_format == 'structured':
            format_type = LogFormat.JSON if 'json' in log_format else LogFormat.STRUCTURED
            console_handler.setFormatter(StructuredFormatter(format_type))
        
        self.root_logger.addHandler(console_handler)
        
        # File handler (if enabled)
        if log_to_file:
            os.makedirs(os.path.dirname(log_filename), exist_ok=True)
            
            file_handler = RotatingFileHandler(
                log_filename, maxBytes=log_rotation_kb * 1024, backupCount=5
            )
            file_handler.setLevel(level)
            
            if log_format == 'text':
                file_handler.setFormatter(logging.Formatter(
                    '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
                ))
            elif log_format == 'structured':
                format_type = LogFormat.JSON if 'json' in log_format else LogFormat.STRUCTURED
                file_handler.setFormatter(StructuredFormatter(format_type))
            
            self.root_logger.addHandler(file_handler)
    
    def get_logger(self, name: str = None) -> logging.Logger:
        """Get a logger instance."""
        if name:
            return logging.getLogger(f"nlm.{name}")
        return self.root_logger
    
    def log_performance(self, operation_name: str, duration: float):
        """Log performance metric."""
        self.performance_logger.end_operation(operation_name)
        
        logger = self.get_logger("performance")
        logger.info(f"Performance: {operation_name} took {duration*1000:.2f}ms")
    
    def start_performance(self, operation_name: str):
        """Start timing performance."""
        self.performance_logger.start_operation(operation_name)
    
    def log_memory_usage(self):
        """Log current memory usage."""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            
            logger = self.get_logger("memory")
            logger.info(f"Memory usage: RSS={memory_info.rss / 1024 / 1024:.2f}MB, "
                       f"VMS={memory_info.vms / 1024 / 1024:.2f}MB")
        except ImportError:
            pass  # psutil not available
    
    def log_experiment(self, experiment_name: str, parameters: Dict, results: Dict):
        """Log experiment details."""
        logger = self.get_logger("experiment")
        
        experiment_log = {
            'experiment': experiment_name,
            'parameters': parameters,
            'results': results,
            'timestamp': datetime.now().isoformat()
        }
        
        if self.config.get('log_format') == 'json':
            logger.info(json.dumps(experiment_log))
        else:
            logger.info(f"Experiment {experiment_name}: {results}")

class LoggerManager:
    """Manager for NLM logging system."""
    
    def __init__(self, config_file: str = None):
        self.config = {}
        if config_file and Path(config_file).exists():
            self.load_config(config_file)
        
        self.logger = NLMLogger(self.config)

    def load_config(self, config_file: str):
        """Load logger configuration."""
        try:
            if config_file.endswith('.json'):
                with open(config_file, 'r') as f:
                    self.config = json.load(f)
            elif config_file.endswith(('.yaml', '.yml')):
                with open(config_file, 'r') as f:
                    self.config = yaml.safe_load(f)
            else:
                print(f"Warning: Unknown config file format: {config_file}")
        except Exception as e:
            print(f"Error loading config: {e}")

    def configure(self, **kwargs):
        """Configure logger with given options."""
        self.config.update(kwargs)
        self.logger = NLMLogger(self.config)

    def get_logger(self, name: str = None) -> logging.Logger:
        """Get a logger instance."""
        return self.logger.get_logger(name)

    def setup_performance_logging(self, iterations: int = 10):
        """Setup periodic performance logging."""
        logger = self.get_logger("system")
        
        def log_performance():
            for i in range(iterations):
                logger.info(f"Performance check {i+1}/{iterations}")
                self.logger.log_memory_usage()
                time.sleep(1)
        
        # Start in background thread
        thread = threading.Thread(target=log_performance, daemon=True)
        thread.start()

def main():
    logger_manager = LoggerManager()

    parser = argparse.ArgumentParser(
        description="NLM Advanced Logger",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Configure logger
    config_parser = subparsers.add_parser("configure", help="Configure logger")
    config_parser.add_argument("--log-level", choices=["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
                              help="Log level")
    config_parser.add_argument("--log-format", choices=["text", "json", "structured"],
                              help="Log format")
    config_parser.add_argument("--log-file", help="Log file path")
    config_parser.add_argument("--log-rotation", type=int, help="Log rotation in KB")
    config_parser.add_argument("--config-file", help="Load configuration from file")

    # Get logger
    subparsers.add_parser("get-logger", help="Get logger instance")

    # Log example messages
    examples_parser = subparsers.add_parser("examples", help="Show logging examples")

    # Performance monitoring
    perf_parser = subparsers.add_parser("performance", help="Performance monitoring")
    perf_parser.add_argument("--start", help="Start performance monitoring")
    perf_parser.add_argument("--end", help="End performance monitoring")

    # Memory usage
    subparsers.add_parser("memory", help="Log memory usage")

    # Experiment logging
    exp_parser = subparsers.add_parser("experiment", help="Log experiment")
    exp_parser.add_argument("--name", required=True, help="Experiment name")
    exp_parser.add_argument("--parameters", help="Experiment parameters (JSON)")
    exp_parser.add_argument("--results", help="Experiment results (JSON)")

    args = parser.parse_args()

    if args.command == "configure":
        config = {}
        if args.log_level:
            config['log_level'] = args.log_level
        if args.log_format:
            config['log_format'] = args.log_format
        if args.log_file:
            config['log_to_file'] = True
            config['log_filename'] = args.log_file
        if args.log_rotation:
            config['log_rotation_kb'] = args.log_rotation
        
        if args.config_file:
            logger_manager.load_config(args.config_file)
        
        if config:
            logger_manager.configure(**config)
            print("✅ Logger configured successfully!")

    elif args.command == "get-logger":
        logger = logger_manager.get_logger()
        logger.info("Logger instance retrieved successfully!")
        print("✅ Logger instance created!")

    elif args.command == "examples":
        print("\n📝 Logging Examples:")
        print("=" * 40)
        print("""
# Example 1: Basic logging
logger = logging.getLogger("nlm.test")
logger.debug("Debug message")
logger.info("Info message")
logger.warning("Warning message")
logger.error("Error message")

# Example 2: Performance logging
logger_manager = LoggerManager()
logger_manager.logger.start_performance("brain_step")
# ... do work ...
logger_manager.logger.log_performance("brain_step", duration)

# Example 3: Memory logging
logger_manager.logger.log_memory_usage()

# Example 4: Experiment logging
logger_manager.logger.log_experiment(
    "neural_plasticity_test",
    {"learning_rate": 0.01, "neurons": 1000},
    {"accuracy": 0.95, "time": 123.4}
)
        """)

    elif args.command == "performance":
        if args.start:
            logger_manager.logger.start_performance(args.start)
            print(f"✅ Started performance monitoring: {args.start}")
        elif args.end:
            duration = logger_manager.logger.end_performance(args.end)
            print(f"✅ Ended performance monitoring: {args.end} ({duration*1000:.2f}ms)")
        else:
            parser.print_help()

    elif args.command == "memory":
        logger_manager.logger.log_memory_usage()
        print("✅ Memory usage logged!")

    elif args.command == "experiment":
        parameters = json.loads(args.parameters) if args.parameters else {}
        results = json.loads(args.results) if args.results else {}
        logger_manager.logger.log_experiment(args.name, parameters, results)
        print(f"✅ Experiment logged: {args.name}")

    else:
        parser.print_help()

if __name__ == "__main__":
    main()
