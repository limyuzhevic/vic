# Utility functions and helpers

import os
import sys
import json
import time
import threading
from typing import Dict, Any, List, Optional, Union, Callable
from dataclasses import dataclass, field
from enum import Enum

from .exceptions import NLMErrors
from .config import Config

class Environment:
    """Environment variables and configuration."""
    
    # NLM environment variables
    NLM_HOME = "NLM_HOME"
    NLM_CONFIG_DIR = "NLM_CONFIG_DIR"
    NLM_CHECKPOINT_DIR = "NLM_CHECKPOINT_DIR"
    NLM_LOG_LEVEL = "NLM_LOG_LEVEL"
    NLM_VERBOSE = "NLM_VERBOSE"
    
    # Default paths
    DEFAULT_HOME = os.path.expanduser("~/.nlm")
    DEFAULT_CONFIG_DIR = os.path.join(DEFAULT_HOME, "configs")
    DEFAULT_CHECKPOINT_DIR = os.path.join(DEFAULT_HOME, "checkpoints")
    
    @classmethod
    def get_nlm_home(cls) -> str:
        """Get NLM home directory."""
        return os.environ.get(cls.NLM_HOME, cls.DEFAULT_HOME)
        
    @classmethod
    def get_config_dir(cls) -> str:
        """Get configuration directory."""
        return os.environ.get(cls.NLM_CONFIG_DIR, cls.DEFAULT_CONFIG_DIR)
        
    @classmethod
    def get_checkpoint_dir(cls) -> str:
        """Get checkpoint directory."""
        return os.environ.get(cls.NLM_CHECKPOINT_DIR, cls.DEFAULT_CHECKPOINT_DIR)
        
    @classmethod
    def ensure_directories(cls):
        """Ensure all necessary directories exist."""
        os.makedirs(cls.get_config_dir(), exist_ok=True)
        os.makedirs(cls.get_checkpoint_dir(), exist_ok=True)

class MemoryUsage:
    """Memory usage monitoring."""
    
    @staticmethod
    def get_memory_usage() -> Dict[str, float]:
        """Get current memory usage."""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            
            return {
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'percent': process.memory_percent(),
            }
        except ImportError:
            return {'error': 'psutil not available'}
        except Exception as e:
            return {'error': f'Failed to get memory usage: {e}'}

class PerformanceMonitor:
    """Performance monitoring utilities."""
    
    def __init__(self):
        self.timings: List[float] = []
        self.memory_samples: List[Dict[str, float]] = []
        self.start_time: Optional[float] = None
        self.step_count = 0
        
    def start(self):
        """Start monitoring."""
        self.start_time = time.time()
        self.step_count = 0
        
    def record_step(self):
        """Record a simulation step."""
        if self.start_time:
            step_time = time.time() - self.start_time
            self.timings.append(step_time)
            self.memory_samples.append(MemoryUsage.get_memory_usage())
            self.step_count += 1
            self.start_time = time.time()
            
    def get_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.timings:
            return {}
            
        return {
            'total_steps': self.step_count,
            'avg_step_time': sum(self.timings) / len(self.timings),
            'min_step_time': min(self.timings),
            'max_step_time': max(self.timings),
            'steps_per_second': self.step_count / sum(self.timings) if sum(self.timings) > 0 else 0,
            'current_memory': self.memory_samples[-1] if self.memory_samples else {},
            'avg_memory': self._average_memory_usage(),
        }
        
    def _average_memory_usage(self) -> Dict[str, float]:
        """Calculate average memory usage."""
        if not self.memory_samples:
            return {}
            
        avg = {}
        for key in self.memory_samples[0].keys():
            if key == 'error':
                avg[key] = self.memory_samples[0][key]
            else:
                values = [sample[key] for sample in self.memory_samples if key in sample]
                if values:
                    avg[key] = sum(values) / len(values)
        
        return avg
        
    def reset(self):
        """Reset monitoring."""
        self.timings.clear()
        self.memory_samples.clear()
        self.start_time = None
        self.step_count = 0

class ProgressBar:
    """Progress bar for long-running simulations."""
    
    def __init__(self, total: int, width: int = 50, fill_char: str = '█', 
                 empty_char: str = '░', show_time: bool = True):
        self.total = total
        self.width = width
        self.fill_char = fill_char
        self.empty_char = empty_char
        self.show_time = show_time
        self.start_time = time.time()
        self.last_update = 0
        self.last_percent = -1
        
    def update(self, current: int, message: str = ""):
        """Update progress bar."""
        current_time = time.time()
        
        # Update every 100ms or at completion
        if current_time - self.last_update < 0.1 and current < self.total:
            return
            
        percent = current / self.total
        filled = int(self.width * percent)
        
        # Only redraw if progress changed
        if percent == self.last_percent:
            return
            
        # Clear line and print new progress
        sys.stdout.write('\r')
        sys.stdout.write(f"[{self.fill_char * filled}{self.empty_char * (self.width - filled)}] {percent:.1%}")
        
        if message:
            sys.stdout.write(f" {message}")
            
        if self.show_time:
            elapsed = current_time - self.start_time
            rate = current / elapsed if elapsed > 0 else 0
            remaining = (self.total - current) / rate if rate > 0 else 0
            sys.stdout.write(f" | {elapsed:.1f}s elapsed, {rate:.1f} steps/s, {remaining:.1f}s remaining")
            
        sys.stdout.flush()
        self.last_update = current_time
        self.last_percent = percent
        
        if current >= self.total:
            sys.stdout.write('\n')
            
    def close(self):
        """Close progress bar."""
        if self.last_percent >= 0 and self.last_percent < 1.0:
            sys.stdout.write('\n')

class SimulationManager:
    """Manager for complex simulation workflows."""
    
    def __init__(self):
        self.sessions: List['BrainSession'] = []
        self.checkpoints: Dict[str, str] = {}
        
    def add_session(self, session: 'BrainSession'):
        """Add a simulation session."""
        self.sessions.append(session)
        
    def remove_session(self, session: 'BrainSession'):
        """Remove a simulation session."""
        if session in self.sessions:
            self.sessions.remove(session)
            
    def run_parallel(self, num_workers: int = 4):
        """Run sessions in parallel."""
        if num_workers <= 1:
            for session in self.sessions:
                session.run()
        else:
            # Use thread pool for parallel execution
            import concurrent.futures
            
            with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
                futures = [executor.submit(session.run) for session in self.sessions]
                concurrent.futures.wait(futures)
                
    def save_all_checkpoints(self):
        """Save checkpoints for all sessions."""
        Environment.ensure_directories()
        
        for i, session in enumerate(self.sessions):
            checkpoint_path = f"{Environment.get_checkpoint_dir()}/session_{i}.bin"
            session.save_state(checkpoint_path)
            self.checkpoints[f"session_{i}"] = checkpoint_path
            
    def load_checkpoint(self, session_id: str):
        """Load a specific checkpoint."""
        if session_id in self.checkpoints:
            for session in self.sessions:
                session.load_state(self.checkpoints[session_id])
                
    def cleanup(self):
        """Clean up all sessions."""
        for session in self.sessions:
            session.cleanup()
        self.sessions.clear()
        self.checkpoints.clear()

class ExperimentRunner:
    """Runner for structured experiments."""
    
    def __init__(self, name: str):
        self.name = name
        self.results: List[Dict[str, Any]] = []
        self.config: Optional[Config] = None
        
    def set_config(self, config: Config):
        """Set experiment configuration."""
        self.config = config
        
    def run_trial(self, trial_id: int, setup_func: Callable, run_func: Callable,
                  teardown_func: Callable) -> Dict[str, Any]:
        """Run a single experiment trial."""
        if not self.config:
            raise NLMErrors("Experiment config not set")
            
        trial_result = {
            'trial_id': trial_id,
            'config': self.config.to_dict(),
            'start_time': time.time(),
        }
        
        try:
            # Setup
            setup_func()
            
            # Run experiment
            trial_result.update(run_func())
            
            # Success
            trial_result.update({
                'status': 'completed',
                'end_time': time.time(),
            })
            
        except Exception as e:
            trial_result.update({
                'status': 'failed',
                'error': str(e),
                'end_time': time.time(),
            })
            
        finally:
            # Teardown
            teardown_func()
            
        self.results.append(trial_result)
        return trial_result
        
    def get_summary(self) -> Dict[str, Any]:
        """Get experiment summary."""
        if not self.results:
            return {}
            
        completed = [r for r in self.results if r['status'] == 'completed']
        failed = [r for r in self.results if r['status'] == 'failed']
        
        return {
            'name': self.name,
            'total_trials': len(self.results),
            'completed_trials': len(completed),
            'failed_trials': len(failed),
            'success_rate': len(completed) / len(self.results) if self.results else 0,
            'avg_duration': sum(r['end_time'] - r['start_time'] for r in completed) / len(completed) if completed else 0,
        }
        
    def save_results(self, filepath: str):
        """Save experiment results to file."""
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
            
    def load_results(self, filepath: str):
        """Load experiment results from file."""
        with open(filepath, 'r') as f:
            self.results = json.load(f)