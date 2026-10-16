#!/usr/bin/env python3
"""
Logging and monitoring utilities for NLM

This module provides comprehensive logging and monitoring capabilities for NLM simulations,
including file logging, console logging, statistics collection, and real-time monitoring.
"""

import logging
import time
import threading
import json
import sys
from typing import Callable, Optional, Dict, Any, List
from datetime import datetime
from pathlib import Path

class SimulationLogger:
    """Comprehensive logging system for NLM simulations"""
    
    def __init__(self, name: str = "nlm_simulation"):
        self.name = name
        self.logger = logging.getLogger(name)
        self.logger.setLevel(logging.DEBUG)
        self.handlers: List[logging.Handler] = []
        self.file_handlers: List[logging.FileHandler] = []
        self.console_handler = None
        self.custom_handlers: List[Callable] = []
        self.log_file = None
        
    def add_console_handler(self, level: str = "INFO", format_string: Optional[str] = None):
        """Add console handler"""
        if format_string is None:
            format_string = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        
        self.console_handler = logging.StreamHandler(sys.stdout)
        self.console_handler.setLevel(getattr(logging, level.upper()))
        formatter = logging.Formatter(format_string)
        self.console_handler.setFormatter(formatter)
        self.logger.addHandler(self.console_handler)
        self.handlers.append(self.console_handler)
        
    def add_file_handler(self, filepath: str, level: str = "INFO", max_size: int = 10 * 1024 * 1024, backup_count: int = 5):
        """Add file handler with rotation"""
        from logging.handlers import RotatingFileHandler
        
        self.log_file = Path(filepath)
        self.log_file.parent.mkdir(parents=True, exist_ok=True)
        
        handler = RotatingFileHandler(
            self.log_file, maxBytes=max_size, backupCount=backup_count
        )
        handler.setLevel(getattr(logging, level.upper()))
        
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        
        self.logger.addHandler(handler)
        self.file_handlers.append(handler)
        self.handlers.append(handler)
        
    def add_custom_handler(self, handler: Callable[[str, str, str], None]):
        """Add custom handler function"""
        self.custom_handlers.append(handler)
    
    def log_event(self, event_type: str, message: str, level: str = "INFO", **kwargs):
        """Log an event with metadata"""
        log_data = {
            'timestamp': datetime.now().isoformat(),
            'event_type': event_type,
            'message': message,
            'level': level,
            **kwargs
        }
        
        # Log to standard logger
        log_level = getattr(logging, level.upper())
        self.logger.log(log_level, json.dumps(log_data))
        
        # Call custom handlers
        for handler in self.custom_handlers:
            try:
                handler(event_type, message, level, **kwargs)
            except Exception as e:
                print(f"Custom handler error: {type(e).__name__}: {e}")
    
    def log_simulation_start(self, config: Dict[str, Any]):
        """Log simulation start with configuration"""
        self.log_event(
            event_type="simulation_start",
            message="Simulation started",
            level="INFO",
            configuration=config,
            timestamp=datetime.now().isoformat()
        )
    
    def log_simulation_end(self, stats: Dict[str, Any]):
        """Log simulation end with statistics"""
        self.log_event(
            event_type="simulation_end",
            message="Simulation ended",
            level="INFO",
            statistics=stats,
            timestamp=datetime.now().isoformat()
        )
    
    def log_error(self, error_type: str, message: str, exception: Exception, **kwargs):
        """Log an error"""
        self.log_event(
            event_type="error",
            message=f"{error_type}: {message}",
            level="ERROR",
            error_type=error_type,
            message=message,
            exception_type=type(exception).__name__,
            exception_message=str(exception),
            **kwargs
        )
    
    def log_checkpoint(self, filepath: str, brain_stats: Dict[str, Any]):
        """Log checkpoint save/load operation"""
        self.log_event(
            event_type="checkpoint",
            message="Checkpoint operation",
            level="INFO",
            filepath=filepath,
            brain_stats=brain_stats,
            timestamp=datetime.now().isoformat()
        )
    
    def clear_handlers(self):
        """Remove all handlers"""
        for handler in self.handlers:
            self.logger.removeHandler(handler)
        self.handlers.clear()
        self.file_handlers.clear()
        self.custom_handlers.clear()

class MetricsCollector:
    """Collect and analyze simulation metrics"""
    
    def __init__(self, name: str = "nlm_metrics"):
        self.name = name
        self.metrics = {}
        self.callbacks: List[Callable] = []
        self.lock = threading.Lock()
    
    def record(self, metric_name: str, value: Any, timestamp: Optional[float] = None, **kwargs):
        """Record a metric"""
        if timestamp is None:
            timestamp = time.time()
        
        with self.lock:
            if metric_name not in self.metrics:
                self.metrics[metric_name] = []
            
            metric_data = {
                'value': value,
                'timestamp': timestamp,
                'data': kwargs
            }
            
            self.metrics[metric_name].append(metric_data)
            
            # Call callbacks
            for callback in self.callbacks:
                try:
                    callback(metric_name, value, timestamp, **kwargs)
                except Exception as e:
                    print(f"Metrics callback error: {type(e).__name__}: {e}")
    
    def record_step_metrics(self, step: int, brain: nlm.BrainWrapper, world: nlm.SimpleWorldWrapper,
                            agent: nlm.AgentBrainWrapper, action, reward: float, 
                            novelty: float, curiosity: float):
        """Record step-level metrics"""
        body = world.getAgentBody()
        
        self.record('steps', step)
        self.record('position_x', body.x)
        self.record('position_y', body.y)
        self.record('energy', body.energy)
        self.record('health', body.health)
        self.record('age', body.age)
        self.record('total_neurons', brain.getBrain().getTotalNeuronCount())
        self.record('active_neurons', brain.getBrain().getActiveNeuronCount())
        self.record('firing_neurons', brain.getBrain().getFiringNeuronCount())
        self.record('total_spikes', brain.getBrain().getTotalSpikeCount())
        self.record('average_firing_rate', brain.getBrain().getAverageFiringRate())
        self.record('excitation_inhibition_ratio', brain.getBrain().getExcitationInhibitionRatio())
        self.record('developmental_stage', agent.getDevelopmentalStage())
        self.record('neuromodulation_level', agent.getNeuromodulationLevel())
        self.record('curiosity_level', agent.getCuriosityLevel())
        self.record('novelty_level', agent.getNoveltyLevel())
        self.record('prediction_error', agent.getPredictionError())
        self.record('action', str(action))
        self.record('reward', reward)
        self.record('novelty', novelty)
        self.record('curiosity', curiosity)
    
    def get_metric_summary(self, metric_name: str, last_n: int = 100) -> Dict[str, Any]:
        """Get summary statistics for a metric"""
        with self.lock:
            if metric_name not in self.metrics:
                return {}
            
            values = [m['value'] for m in self.metrics[metric_name][-last_n:]]
            
            if not values:
                return {}
            
            summary = {
                'name': metric_name,
                'count': len(values),
                'latest': values[-1],
                'mean': sum(values) / len(values),
                'min': min(values),
                'max': max(values),
                'std': (sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5 if len(values) > 1 else 0.0
            }
            
            return summary
    
    def get_comprehensive_summary(self) -> Dict[str, Any]:
        """Get comprehensive summary of all metrics"""
        with self.lock:
            summary = {
                'timestamp': datetime.now().isoformat(),
                'metrics': {}
            }
            
            for metric_name in self.metrics:
                summary['metrics'][metric_name] = self.get_metric_summary(metric_name)
            
            return summary
    
    def add_callback(self, callback: Callable):
        """Add callback for metric changes"""
        self.callbacks.append(callback)
    
    def save_metrics(self, filepath: str):
        """Save metrics to file"""
        with self.lock:
            data = {
                'name': self.name,
                'timestamp': datetime.now().isoformat(),
                'metrics': self.metrics
            }
            
            Path(filepath).parent.mkdir(parents=True, exist_ok=True)
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            
            print(f"Metrics saved to {filepath}")
    
    def load_metrics(self, filepath: str):
        """Load metrics from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.name = data.get('name', self.name)
        
        with self.lock:
            self.metrics = data.get('metrics', {})
        
        print(f"Metrics loaded from {filepath}")

class PerformanceMonitor:
    """Monitor simulation performance in real-time"""
    
    def __init__(self, log_interval: float = 1.0):
        self.log_interval = log_interval
        self.monitor_thread = None
        self.stop_event = threading.Event()
        self.callbacks: List[Callable] = []
        self.metrics = {}
        
    def _monitor_loop(self, brain_getter: Callable, world_getter: Callable):
        """Main monitoring loop"""
        while not self.stop_event.is_set():
            try:
                step = self.metrics.get('step', 0)
                brain = brain_getter()
                world = world_getter()
                
                if brain and world:
                    # Get performance metrics
                    start_time = time.time()
                    
                    body = world.getAgentBody()
                    brain_obj = brain.getBrain()
                    
                    # Calculate performance metrics
                    processing_time = time.time() - start_time
                    neurons_per_second = brain_obj.getTotalSpikeCount() / processing_time if processing_time > 0 else 0
                    memory_usage = len(str(brain_obj))  # Rough estimate
                    
                    # Record metrics
                    self.metrics['step'] = step
                    self.metrics['processing_time'] = processing_time
                    self.metrics['neurons_per_second'] = neurons_per_second
                    self.metrics['memory_estimate'] = memory_usage
                    self.metrics['agent_energy'] = body.energy
                    self.metrics['agent_health'] = body.health
                    
                    # Call callbacks
                    for callback in self.callbacks:
                        try:
                            callback(self.metrics.copy())
                        except Exception as e:
                            print(f"Performance callback error: {type(e).__name__}: {e}")
                
                time.sleep(self.log_interval)
                
            except Exception as e:
                print(f"Performance monitor error: {type(e).__name__}: {e}")
                break
    
    def start(self, brain_getter: Callable, world_getter: Callable):
        """Start performance monitoring"""
        if self.monitor_thread is not None:
            print("Performance monitor is already running")
            return
        
        self.stop_event.clear()
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(brain_getter, world_getter))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print(f"Performance monitor started (interval: {self.log_interval}s)")
    
    def stop(self):
        """Stop performance monitoring"""
        if self.monitor_thread is None:
            print("Performance monitor is not running")
            return
        
        self.stop_event.set()
        self.monitor_thread.join(timeout=5.0)
        self.monitor_thread = None
        print("Performance monitor stopped")
    
    def add_callback(self, callback: Callable):
        """Add callback for performance metrics"""
        self.callbacks.append(callback)
    
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics"""
        return self.metrics.copy()

class SimulationProgressReporter:
    """Report simulation progress with various output formats"""
    
    def __init__(self, logger: Optional['SimulationLogger'] = None):
        self.logger = logger or SimulationLogger()
        self.formatters = {
            'text': self._text_formatter,
            'json': self._json_formatter,
            'csv': self._csv_formatter
        }
    
    def _text_formatter(self, step: int, stats: Dict[str, Any]) -> str:
        """Format progress as text"""
        body = stats.get('body')
        brain = stats.get('brain')
        agent = stats.get('agent')
        
        lines = [
            f"=== Simulation Progress ===",
            f"Step: {step}",
            f"Position: ({body.x if body else 'N/A'}, {body.y if body else 'N/A'})",
            f"Energy: {body.energy if body else 'N/A'} ({body.health if body else 'N/A'} health)",
            f"Development: {agent.getDevelopmentalStage() if agent else 'N/A'}",
            f"Neuromod: {agent.getNeuromodulationLevel() if agent else 'N/A'} (curiosity: {agent.getCuriosityLevel() if agent else 'N/A'}, novelty: {agent.getNoveltyLevel() if agent else 'N/A'})"
        ]
        
        if brain:
            lines.extend([
                f"",
                f"Brain Statistics:",
                f"  Neurons: {brain.getBrain().getTotalNeuronCount()}",
                f"  Active: {brain.getBrain().getActiveNeuronCount()}",
                f"  Firing: {brain.getBrain().getFiringNeuronCount()}",
                f"  Spikes: {brain.getBrain().getTotalSpikeCount()}",
                f"  Firing Rate: {brain.getBrain().getAverageFiringRate():.2f} Hz",
                f"  EI Ratio: {brain.getBrain().getExcitationInhibitionRatio():.3f}"
            ])
        
        return "\n".join(lines)
    
    def _json_formatter(self, step: int, stats: Dict[str, Any]) -> str:
        """Format progress as JSON"""
        import json
        return json.dumps({
            'step': step,
            'timestamp': datetime.now().isoformat(),
            **{k: v for k, v in stats.items() if k not in ['body', 'brain', 'agent']}
        }, indent=2)
    
    def _csv_formatter(self, step: int, stats: Dict[str, Any]) -> str:
        """Format progress as CSV"""
        body = stats.get('body')
        brain = stats.get('brain')
        agent = stats.get('agent')
        
        return f"{step},{body.x if body else 'N/A'},{body.y if body else 'N/A'},{body.energy if body else 'N/A'},{agent.getNeuromodulationLevel() if agent else 'N/A'},{brain.getBrain().getTotalNeuronCount() if brain else 'N/A'},{brain.getBrain().getAverageFiringRate() if brain else 'N/A'}\n"
    
    def report_progress(self, step: int, stats: Dict[str, Any], format_type: str = 'text'):
        """Report progress in specified format"""
        if format_type not in self.formatters:
            print(f"Unknown format type: {format_type}")
            return
        
        formatted = self.formatters[format_type](step, stats)
        
        if format_type == 'text':
            print(formatted)
        elif format_type == 'json':
            self.logger.log_event('progress', formatted, 'INFO', step=step)
        elif format_type == 'csv':
            self.logger.log_event('progress_csv', formatted, 'INFO', step=step)

def create_default_logging_config(log_dir: str = "logs") -> SimulationLogger:
    """Create default logging configuration"""
    logger = SimulationLogger()
    
    # Add console handler
    logger.add_console_handler("INFO")
    
    # Add file handler with rotation
    log_file = Path(log_dir) / f"nlm_simulation_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
    logger.add_file_handler(str(log_file))
    
    return logger

def create_metrics_collector(name: str = "nlm_metrics") -> MetricsCollector:
    """Create metrics collector"""
    return MetricsCollector(name)

def create_performance_monitor(log_interval: float = 1.0) -> PerformanceMonitor:
    """Create performance monitor"""
    return PerformanceMonitor(log_interval)

# Example usage functions
import sys

def example_logging_usage():
    """Example showing how to use the logging utilities"""
    print("=== Example: Logging Usage ===")
    
    # Create logger
    logger = create_default_logging_config()
    
    # Log simulation start
    config = {
        'world_width': 20,
        'world_height': 20,
        'max_steps': 100,
        'enable_reward_modulation': True
    }
    logger.log_simulation_start(config)
    
    # Log some events
    logger.log_event('trial_start', 'Starting trial', 'INFO', trial_id='trial_001')
    logger.log_event('action_taken', 'Agent moved forward', 'INFO', action='MoveForward', reward=1.0)
    
    # Log error
    try:
        raise ValueError("Test error")
    except Exception as e:
        logger.log_error('simulation_error', 'Something went wrong', e, step=50)
    
    # Log simulation end
    stats = {'steps_completed': 100, 'final_energy': 75.0}
    logger.log_simulation_end(stats)
    
    print("Example complete!")

def example_metrics_usage():
    """Example showing how to use the metrics collector"""
    print("=== Example: Metrics Usage ===")
    
    # Create metrics collector
    metrics = create_metrics_collector("phase6_experiment")
    
    # Record some metrics
    for step in range(10):
        metrics.record('step', step)
        metrics.record('neurons_per_second', step * 10)
        metrics.record('temperature', 20.0 + step * 0.5)
        metrics.record('pressure', 1.0 + step * 0.01)
    
    # Get summary
    step_summary = metrics.get_metric_summary('step', last_n=5)
    print(f"Step summary: {step_summary}")
    
    # Save metrics
    metrics.save_metrics("experiment_results.json")
    
    print("Example complete!")

def example_monitoring_usage():
    """Example showing how to use the performance monitor"""
    print("=== Example: Monitoring Usage ===")
    
    # Create monitor
    monitor = create_performance_monitor(log_interval=2.0)
    
    # Mock functions for monitoring
    def get_brain():
        return None  # Would return actual brain object
    
    def get_world():
        return None  # Would return actual world object
    
    # Start monitoring
    monitor.start(get_brain, get_world)
    
    # Simulate some work
    for i in range(5):
        time.sleep(1.0)
        monitor.metrics['step'] = i
    
    # Get current metrics
    current = monitor.get_current_metrics()
    print(f"Current metrics: {current}")
    
    # Stop monitoring
    monitor.stop()
    
    print("Example complete!")

if __name__ == "__main__":
    # Run examples
    example_logging_usage()
    print()
    example_metrics_usage()
    print()
    example_monitoring_usage()