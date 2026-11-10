# NLM Python Advanced Features Interface
# This module provides advanced functionality for NLM Python bindings

import sys
import time
import json
import os
import threading
import tracemalloc
import psutil
from typing import Dict, Any, Optional, Callable, List
from enum import Enum
import traceback
import gc
from datetime import datetime

# Constants for resource monitoring
MAX_MEMORY_TRACKING = True
TRACK_SYSTEM_RESOURCES = True
TRACK_CALL_STACKS = False

class NLMLogLevel(Enum):
    DEBUG = 0
    INFO = 1
    WARNING = 2
    ERROR = 3

class NLMProfiler:
    """Simple performance profiler for NLM operations"""
    
    def __init__(self):
        self.metrics = {}
        self.start_times = {}
    
    def start(self, operation: str):
        """Start timing an operation"""
        self.start_times[operation] = time.perf_counter()
    
    def stop(self, operation: str) -> float:
        """Stop timing and return elapsed time in seconds"""
        if operation in self.start_times:
            elapsed = time.perf_counter() - self.start_times[operation]
            del self.start_times[operation]
            self.metrics[operation] = self.metrics.get(operation, 0.0) + elapsed
            return elapsed
        return 0.0
    
class NLMProfiler:
    """Enhanced performance profiler for NLM operations with memory tracking and system monitoring"""
    
    def __init__(self):
        self.metrics = {}
        self.start_times = {}
        self.call_stack_tracking = TRACK_CALL_STACKS
    
    def start(self, operation: str):
        """Start timing an operation"""
        self.start_times[operation] = time.perf_counter()
    
    def stop(self, operation: str) -> float:
        """Stop timing and return elapsed time in seconds"""
        if operation in self.start_times:
            elapsed = time.perf_counter() - self.start_times[operation]
            del self.start_times[operation]
            self.metrics[operation] = self.metrics.get(operation, 0.0) + elapsed
            return elapsed
        return 0.0
    
    def track_memory(self, operation: str):
        """Track memory usage for an operation"""
        if MAX_MEMORY_TRACKING:
            tracemalloc.start()
            self.start(operation)
    
    def stop_memory_tracking(self, operation: str) -> Dict[str, Any]:
        """Stop memory tracking and return memory statistics"""
        if operation in self.start_times:
            elapsed = time.perf_counter() - self.start_times[operation]
            del self.start_times[operation]
            
            if tracemalloc.is_tracing():
                current, peak = tracemalloc.get_traced_memory()
                tracemalloc.stop()
                
                memory_stats = {
                    'elapsed': elapsed,
                    'current_memory_mb': current / 1024 / 1024,
                    'peak_memory_mb': peak / 1024 / 1024,
                }
                
                if 'memory_stats' not in self.metrics:
                    self.metrics['memory_stats'] = []
                self.metrics['memory_stats'].append((operation, memory_stats))
                
                return memory_stats
        
        return {'elapsed': 0.0, 'current_memory_mb': 0.0, 'peak_memory_mb': 0.0}
    
    def get_system_metrics(self) -> Dict[str, Any]:
        """Get current system metrics"""
        if not TRACK_SYSTEM_RESOURCES:
            return {}
        
        process = psutil.Process()
        system_metrics = {
            'pid': process.pid,
            'memory_rss_mb': process.memory_info().rss / 1024 / 1024,
            'memory_vms_mb': process.memory_info().vms / 1024 / 1024,
            'cpu_percent': process.cpu_percent(),
            'num_threads': process.num_threads(),
            'create_time': process.create_time(),
            'num_fds': process.num_fds(),
            'status': process.status(),
        }
        
        return system_metrics
    
    def get_memory_usage(self) -> Dict[str, float]:
        """Get current memory usage information"""
        if not MAX_MEMORY_TRACKING:
            return {'current_mb': 0.0, 'peak_mb': 0.0}
        
        if tracemalloc.is_tracing():
            current, peak = tracemalloc.get_traced_memory()
            return {
                'current_mb': current / 1024 / 1024,
                'peak_mb': peak / 1024 / 1024,
            }
        
        process = psutil.Process()
        return {
            'current_mb': process.memory_info().rss / 1024 / 1024,
            'peak_mb': 0.0,
        }
    
    def get_gc_stats(self) -> Dict[str, Any]:
        """Get garbage collection statistics"""
        return {
            'collections': gc.collect(),
            'count': len(gc.get_objects()),
            'thresholds': gc.get_threshold(),
            'stats': gc.get_stats(),
        }
    
    def enable_call_stack_tracking(self):
        """Enable call stack tracking for profiling"""
        self.call_stack_tracking = True
        print("[NLM Profiler] Call stack tracking enabled")
    
    def disable_call_stack_tracking(self):
        """Disable call stack tracking"""
        self.call_stack_tracking = False
        print("[NLM Profiler] Call stack tracking disabled")
    
    def run_with_profiling(self, operation: str, func, *args, **kwargs):
        """Run a function with comprehensive profiling"""
        self.start(f"{operation}_start")
        
        if TRACK_CALL_STACKS:
            self.call_stack = traceback.extract_stack()
        
        if MAX_MEMORY_TRACKING:
            tracemalloc.start()
        
        start_time = time.perf_counter()
        
        try:
            result = func(*args, **kwargs)
            elapsed = time.perf_counter() - start_time
            
            # Collect metrics
            self.stop(f"{operation}_start")
            
            if MAX_MEMORY_TRACKING:
                current, peak = tracemalloc.get_traced_memory()
                tracemalloc.stop()
                
                self.metrics[f"{operation}_memory"] = {
                    'current_mb': current / 1024 / 1024,
                    'peak_mb': peak / 1024 / 1024,
                    'elapsed': elapsed,
                }
            
            self.metrics[f"{operation}_result"] = result
            return result
            
        except Exception as e:
            elapsed = time.perf_counter() - start_time
            self.metrics[f"{operation}_error"] = str(e)
            self.metrics[f"{operation}_elapsed"] = elapsed
            raise
    
    def benchmark(self, operation: str, func, *args, iterations: int = 10, 
                 warmup_iterations: int = 2):
        """Run a benchmark test with multiple iterations"""
        print(f"[NLM Profiler] Running benchmark for {operation} ({iterations} iterations)")
        
        # Warmup
        for i in range(warmup_iterations):
            func(*args)
        
        # Actual benchmark
        times = []
        for i in range(iterations):
            self.start(f"{operation}_iter_{i}")
            func(*args)
            elapsed = self.stop(f"{operation}_iter_{i}")
            times.append(elapsed)
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        std_time = (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
        
        benchmark_result = {
            'operation': operation,
            'iterations': iterations,
            'warmup_iterations': warmup_iterations,
            'times': times,
            'avg_time': avg_time,
            'min_time': min_time,
            'max_time': max_time,
            'std_time': std_time,
            'times_per_iteration': 1.0 / avg_time if avg_time > 0 else 0,
        }
        
        self.metrics[f"{operation}_benchmark"] = benchmark_result
        
        print(f"[NLM Profiler] Benchmark results for {operation}:")
        print(f"  Average time: {avg_time:.6f}s")
        print(f"  Min time: {min_time:.6f}s")
        print(f"  Max time: {max_time:.6f}s")
        print(f"  Std deviation: {std_time:.6f}s")
        print(f"  Operations per second: {1.0 / avg_time:.2f}")
        
        return benchmark_result
    
    def reset(self):
        """Reset all metrics"""
        self.metrics.clear()
        self.start_times.clear()
    
    def report(self):
        """Print performance report"""
        if not self.metrics:
            print("[NLM Profiler] No metrics collected")
            return
        
        print("[NLM Profiler] Performance Report:")
        for operation, elapsed in sorted(self.metrics.items(), 
                                        key=lambda x: x[1], reverse=True):
            print(f"  {operation}: {elapsed:.3f}s ({elapsed*1000:.1f}ms)")
    
    def get_metrics(self) -> Dict[str, float]:
        """Get all accumulated metrics"""
        return self.metrics.copy()

class NLMLogger:
    """Advanced logging system for NLM"""
    
    def __init__(self, level: NLMLogLevel = NLMLogLevel.INFO, 
                 log_file: Optional[str] = None, colors: bool = True):
        self.level = level
        self.log_file = log_file
        self.colors = colors and hasattr(sys.stdout, 'isatty') and sys.stdout.isatty()
        self.handlers = []
    
    def set_level(self, level: NLMLogLevel):
        """Set logging level"""
        self.level = level
    
    def debug(self, message: str, **kwargs):
        """Log debug message"""
        self._log(NLMLogLevel.DEBUG, message, **kwargs)
    
    def info(self, message: str, **kwargs):
        """Log info message"""
        self._log(NLMLogLevel.INFO, message, **kwargs)
    
    def warning(self, message: str, **kwargs):
        """Log warning message"""
        self._log(NLMLogLevel.WARNING, message, **kwargs)
    
    def error(self, message: str, **kwargs):
        """Log error message"""
        self._log(NLMLogLevel.ERROR, message, **kwargs)
    
    def _log(self, level: NLMLogLevel, message: str, **kwargs):
        """Internal logging method"""
        if level.value < self.level.value:
            return
        
        # Format message
        timestamp = time.strftime("%H:%M:%S")
        level_name = level.name
        
        if self.colors:
            color_map = {
                NLMLogLevel.DEBUG: '\033[36m',    # Cyan
                NLMLogLevel.INFO: '\033[32m',     # Green
                NLMLogLevel.WARNING: '\033[33m',  # Yellow
                NLMLogLevel.ERROR: '\033[31m',    # Red
            }
            reset = '\033[0m'
            prefix = f"{color_map[level]}[{timestamp}] {level_name}: {reset}"
        else:
            prefix = f"[{timestamp}] {level_name}: "
        
        full_message = prefix + message
        
        # Add context info
        if kwargs:
            context = " " + " ".join(f"{k}={v}" for k, v in kwargs.items())
            full_message += context
        
        # Print to console
        print(full_message)
        
        # Write to log file if specified
        if self.log_file:
            with open(self.log_file, 'a') as f:
                f.write(full_message + '\n')
    
    def add_handler(self, handler: Callable[[str], None]):
        """Add a custom log handler"""
        self.handlers.append(handler)
    
    def remove_handler(self, handler: Callable[[str], None]):
        """Remove a log handler"""
        if handler in self.handlers:
            self.handlers.remove(handler)

class NLMCommandLineInterface:
    """Command-line interface for NLM"""
    
    def __init__(self, name: str = "nlm"):
        self.name = name
        self.debug_mode = False
        self.profiler = NLMProfiler()
        self.logger = NLMLogger(NLMLogLevel.INFO)
        self.config = {}
    
    def set_debug_mode(self, enabled: bool):
        """Enable/disable debug mode"""
        self.debug_mode = enabled
        if enabled:
            self.logger.set_level(NLMLogLevel.DEBUG)
            print(f"[NLM CLI] Debug mode enabled")
        else:
            self.logger.set_level(NLMLogLevel.INFO)
            print(f"[NLM CLI] Debug mode disabled")
    
    def parse_args(self, args: Optional[list] = None) -> Dict[str, Any]:
        """Parse command-line arguments"""
        if args is None:
            args = sys.argv[1:]
        
        result = {
            'help': False,
            'version': False,
            'debug': False,
            'log_file': None,
            'config_file': None,
            'profile': False,
            'quiet': False,
        }
        
        i = 0
        while i < len(args):
            arg = args[i]
            
            if arg in ('--help', '-h'):
                result['help'] = True
            elif arg in ('--version', '-v'):
                result['version'] = True
            elif arg == '--debug':
                result['debug'] = True
            elif arg == '--log-file':
                if i + 1 < len(args):
                    result['log_file'] = args[i + 1]
                    i += 1
            elif arg == '--config-file':
                if i + 1 < len(args):
                    result['config_file'] = args[i + 1]
                    i += 1
            elif arg == '--profile':
                result['profile'] = True
            elif arg in ('--quiet', '-q'):
                result['quiet'] = True
            elif arg.startswith('--'):
                # Try to parse as --key=value
                if '=' in arg:
                    key, value = arg[2:].split('=', 1)
                    result[key] = value
                else:
                    # Unknown argument
                    pass
            elif arg.startswith('-'):
                # Single dash argument
                if arg == '-d':
                    result['debug'] = True
                elif arg == '-q':
                    result['quiet'] = True
                elif arg == '-v':
                    result['version'] = True
                elif arg == '-h':
                    result['help'] = True
                else:
                    # Unknown argument
                    pass
            else:
                # Positional argument - store as extra args
                if 'args' not in result:
                    result['args'] = []
                result['args'].append(arg)
            
            i += 1
        
        return result
    
    def run_command(self, command: str, *args, **kwargs):
        """Run a command with profiling"""
        if self.debug_mode:
            self.logger.debug(f"Running command: {command}")
        
        if self.profiler:
            self.profiler.start(f"command_{command}")
        
        try:
            # Execute command (placeholder - would interface with actual NLM)
            if self.debug_mode:
                self.logger.debug(f"Command {command} executed successfully")
            
            if self.profiler:
                self.profiler.stop(f"command_{command}")
            
            return True
        except Exception as e:
            if self.debug_mode:
                self.logger.error(f"Command {command} failed: {e}")
            if self.profiler:
                self.profiler.stop(f"command_{command}")
            raise
    
    def execute(self, args: Optional[list] = None) -> int:
        """Execute NLM with command-line arguments"""
        if args is None:
            args = sys.argv[1:]
        
        # Parse arguments
        parsed = self.parse_args(args)
        
        # Configure based on parsed args
        if parsed.get('debug'):
            self.set_debug_mode(True)
        
        if parsed.get('log_file'):
            self.logger = NLMLogger(NLMLogLevel.INFO, 
                                  log_file=parsed['log_file'], 
                                  colors=not parsed.get('quiet', False))
        
        if parsed.get('quiet'):
            self.logger.set_level(NLMLogLevel.WARNING)
        
        # Load config file if specified
        if parsed.get('config_file'):
            try:
                with open(parsed['config_file'], 'r') as f:
                    self.config = json.load(f)
                self.logger.info(f"Loaded config from {parsed['config_file']}")
            except Exception as e:
                self.logger.error(f"Failed to load config: {e}")
        
        # Handle special flags
        if parsed.get('help'):
            self.print_help()
            return 0
        
        if parsed.get('version'):
            print(f"NLM {self.name} v1.0.0")
            return 0
        
        # Process main command
        if parsed.get('args'):
            command = parsed['args'][0]
            command_args = parsed['args'][1:]
            
            # Run command with profiling if enabled
            if parsed.get('profile'):
                return self.run_profiled_command(command, command_args)
            else:
                return self.run_command(command, *command_args)
        else:
            print("No command specified. Use --help for usage.")
            return 1
    
    def run_profiled_command(self, command: str, args: list):
        """Run a command with detailed profiling"""
        print(f"[NLM CLI] Running command '{command}' with profiling...")
        
        try:
            # Run command
            result = self.run_command(command, *args)
            
            # Print profiling results
            print(f"[NLM CLI] Command completed with profiling:")
            for operation, elapsed in self.profiler.get_metrics().items():
                print(f"  {operation}: {elapsed:.3f}s")
            
            return 0 if result else 1
        except Exception as e:
            print(f"[NLM CLI] Command failed: {e}")
            return 1
    
    def print_help(self):
        """Print help information"""
        help_text = f"""
NLM (Neural Learning Machine) - Advanced Python Interface

Usage: {self.name} [options] <command> [args]

Options:
  --help, -h              Show this help message
  --version, -v           Show version information
  --debug                 Enable debug mode
  --log-file <file>       Write logs to specified file
  --config-file <file>     Load configuration from file
  --profile               Enable profiling for commands
  --quiet, -q             Suppress non-essential output

Commands:
  simulate              Run simulation
  explore              Explore model
  benchmark            Run benchmarks
  export               Export results
  import               Import data

Examples:
  {self.name} simulate --config my_config.json --profile
  {self.name} explore --debug --log-file nlm.log
  {self.name} benchmark --help

For more information about specific commands, use:
  {self.name} <command> --help
"""
        print(help_text)

# Create global instances for convenience
_default_profiler = NLMProfiler()
_default_logger = NLMLogger()
_default_cli = NLMCommandLineInterface()

# Expose main functions for backward compatibility
def nlm_run(args: Optional[list] = None) -> int:
    """Run NLM with command-line arguments (convenience function)"""
    return _default_cli.execute(args)

def nlm_profiler():
    """Get the default profiler instance"""
    return _default_profiler

def nlm_logger():
    """Get the default logger instance"""
    return _default_logger

def nlm_cli():
    """Get the default CLI instance"""
    return _default_cli

# Auto-execute if called as script
if __name__ == '__main__':
    sys.exit(nlm_run())