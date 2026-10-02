# NLM Advanced Utilities
# Additional tools and utilities for the NLM project

import os
import sys
import json
import time
import logging
import argparse
import threading
import multiprocessing
from typing import Dict, List, Any, Optional, Callable
from pathlib import Path
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
import pandas as pd
from datetime import datetime, timedelta

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Configuration related errors"""
    pass

class SimulationError(NLMError):
    """Simulation related errors"""
    pass

class ValidationError(NLMError):
    """Validation related errors"""
    pass

@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM operations"""
    start_time: float = field(default_factory=time.time)
    end_time: float = 0.0
    duration: float = 0.0
    memory_usage: float = 0.0
    cpu_usage: float = 0.0
    neurons_processed: int = 0
    spikes_generated: int = 0
    synaptic_operations: int = 0
    
    def finish(self):
        """Mark the end of performance measurement"""
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time

class ProgressTracker:
    """Track progress of long-running operations"""
    
    def __init__(self, total_steps: int, description: str = "Progress"):
        self.total_steps = total_steps
        self.current_step = 0
        self.description = description
        self.start_time = time.time()
        self.last_update_time = self.start_time
        self.update_interval = 0.5  # Update every 0.5 seconds
        self._stop_event = threading.Event()
        self._thread = None
    
    def start(self):
        """Start progress tracking"""
        if self._thread is None or not self._thread.is_alive():
            self._thread = threading.Thread(target=self._update_loop)
            self._thread.daemon = True
            self._thread.start()
    
    def increment(self, steps: int = 1):
        """Increment progress"""
        self.current_step += steps
    
    def stop(self):
        """Stop progress tracking"""
        self._stop_event.set()
        if self._thread and self._thread.is_alive():
            self._thread.join(timeout=1.0)
    
    def _update_loop(self):
        """Background update loop"""
        while not self._stop_event.is_set():
            current_time = time.time()
            if current_time - self.last_update_time >= self.update_interval:
                self._print_progress()
                self.last_update_time = current_time
            time.sleep(0.1)
    
    def _print_progress(self):
        """Print progress information"""
        if self.total_steps > 0:
            percentage = (self.current_step / self.total_steps) * 100
            elapsed = time.time() - self.start_time
            
            # Create progress bar
            bar_length = 50
            filled_length = int(bar_length * self.current_step / self.total_steps)
            bar = "=" * filled_length + ">" + " " * (bar_length - filled_length - 1)
            
            print(f"\r{self.description}: [{bar}] {percentage:.1f}% ({self.current_step}/{self.total_steps}) "
                  f"Elapsed: {elapsed:.1f}s", end="", flush=True)
            
            if self.current_step >= self.total_steps:
                print(f"\n{self.description}: Completed!")
                self.stop()

class ExperimentRunner:
    """Run NLM experiments with comprehensive logging and metrics"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.experiments = []
        self.results = []
        self.logger = logging.getLogger("nlm.experiments")
        self.performance_tracker = None
    
    def add_experiment(self, name: str, experiment_func: Callable, 
                      description: str = "", parameters: Dict = None):
        """
        Add an experiment to be run
        
        Args:
            name: Experiment name
            experiment_func: Function that runs the experiment
            description: Brief description
            parameters: Parameters for the experiment
        """
        self.experiments.append({
            'name': name,
            'func': experiment_func,
            'description': description,
            'parameters': parameters or {}
        })
    
    def run_all(self, output_file: str = None):
        """
        Run all experiments
        
        Args:
            output_file: File to save results
        """
        self.logger.info(f"Starting experiment run with {len(self.experiments)} experiments")
        
        for i, experiment in enumerate(self.experiments):
            self.logger.info(f"Running experiment {i+1}/{len(self.experiments)}: {experiment['name']}")
            print(f"\n=== Experiment {i+1}: {experiment['name']} ===")
            print(f"Description: {experiment['description']}")
            
            try:
                # Run experiment with progress tracking
                if self.config.get('enable_progress', True) and experiment['parameters'].get('steps', 0) > 0:
                    progress = ProgressTracker(
                        experiment['parameters'].get('steps', 100),
                        f"Experiment: {experiment['name']}"
                    )
                    progress.start()
                    
                    # Monkey patch progress updates in experiment function
                    original_increment = progress.increment
                    experiment['func'] = lambda **kwargs: self._wrap_with_progress(
                        experiment['func'], progress, **kwargs
                    )
                
                # Run experiment
                result = experiment['func'](**experiment['parameters'])
                
                if self.config.get('enable_progress', True) and experiment['parameters'].get('steps', 0) > 0:
                    progress.stop()
                
                # Store result
                self.results.append({
                    'name': experiment['name'],
                    'description': experiment['description'],
                    'parameters': experiment['parameters'],
                    'result': result,
                    'status': 'success',
                    'timestamp': datetime.now().isoformat()
                })
                
                self.logger.info(f"Experiment {experiment['name']} completed successfully")
                print(f"Status: ✓ Success")
                
            except Exception as e:
                self.logger.error(f"Experiment {experiment['name']} failed: {e}")
                self.results.append({
                    'name': experiment['name'],
                    'description': experiment['description'],
                    'parameters': experiment['parameters'],
                    'result': None,
                    'status': 'failed',
                    'error': str(e),
                    'timestamp': datetime.now().isoformat()
                })
                
                print(f"Status: ✗ Failed - {e}")
    
    def _wrap_with_progress(self, func: Callable, progress: ProgressTracker, **kwargs):
        """Wrap experiment function with progress tracking"""
        def wrapped(*args, **kwargs):
            result = func(*args, **kwargs)
            if progress:
                progress.increment()
            return result
        return wrapped
    
    def get_results(self) -> List[Dict]:
        """Get experiment results"""
        return self.results
    
    def save_results(self, filepath: str):
        """Save experiment results to file"""
        with open(filepath, 'w') as f:
            json.dump({
                'experiments': self.experiments,
                'results': self.results,
                'config': self.config,
                'timestamp': datetime.now().isoformat()
            }, f, indent=2)
        
        self.logger.info(f"Results saved to {filepath}")
    
    def generate_report(self) -> str:
        """Generate a text report of experiment results"""
        report = f"NLM Experiment Report\n"
        report += f"Generated: {datetime.now().isoformat()}\n"
        report += f"Total Experiments: {len(self.experiments)}\n"
        report += f"Successful: {sum(1 for r in self.results if r['status'] == 'success')}\n"
        report += f"Failed: {sum(1 for r in self.results if r['status'] == 'failed')}\n\n"
        
        for result in self.results:
            report += f"Experiment: {result['name']}\n"
            report += f"Description: {result['description']}\n"
            report += f"Status: {result['status']}\n"
            
            if result['status'] == 'success':
                report += f"Result: {result['result']}\n"
            else:
                report += f"Error: {result['error']}\n"
            
            report += f"Timestamp: {result['timestamp']}\n\n"
        
        return report

class MonitoringDashboard:
    """Real-time monitoring dashboard for NLM"""
    
    def __init__(self, update_interval: float = 1.0):
        self.update_interval = update_interval
        self.metrics_history: List[Dict[str, Any]] = []
        self._monitor_thread = None
        self._stop_event = threading.Event()
        self.logger = logging.getLogger("nlm.monitoring")
    
    def start_monitoring(self, brain_monitor: Callable, other_monitors: List[Callable] = None):
        """
        Start monitoring
        
        Args:
            brain_monitor: Function that returns brain metrics
            other_monitors: List of other monitoring functions
        """
        self._stop_event.clear()
        
        if other_monitors is None:
            other_monitors = []
        
        self._monitor_thread = threading.Thread(
            target=self._monitoring_loop,
            args=(brain_monitor, other_monitors)
        )
        self._monitor_thread.daemon = True
        self._monitor_thread.start()
        
        self.logger.info("Monitoring started")
    
    def stop_monitoring(self):
        """Stop monitoring"""
        self._stop_event.set()
        if self._monitor_thread and self._monitor_thread.is_alive():
            self._monitor_thread.join(timeout=1.0)
        
        self.logger.info("Monitoring stopped")
    
    def _monitoring_loop(self, brain_monitor: Callable, other_monitors: List[Callable]):
        """Main monitoring loop"""
        while not self._stop_event.is_set():
            try:
                # Collect metrics
                metrics = {}
                
                # Brain metrics
                if brain_monitor:
                    metrics['brain'] = brain_monitor()
                
                # Other metrics
                for monitor in other_monitors:
                    try:
                        result = monitor()
                        if result:
                            metrics.update(result)
                    except Exception as e:
                        self.logger.warning(f"Monitoring function error: {e}")
                
                # Add timestamp and store
                metrics['timestamp'] = time.time()
                metrics['uptime'] = time.time() - self.metrics_history[0]['timestamp'] if self.metrics_history else 0
                
                self.metrics_history.append(metrics)
                
                # Keep only last 1000 entries to prevent memory issues
                if len(self.metrics_history) > 1000:
                    self.metrics_history = self.metrics_history[-1000:]
                
                time.sleep(self.update_interval)
                
            except Exception as e:
                self.logger.error(f"Monitoring loop error: {e}")
                time.sleep(self.update_interval)
    
    def get_latest_metrics(self) -> Dict[str, Any]:
        """Get latest metrics"""
        if not self.metrics_history:
            return {}
        return self.metrics_history[-1]
    
    def get_metrics_history(self, duration: float = 60.0) -> List[Dict[str, Any]]:
        """
        Get metrics history for the last duration seconds
        
        Args:
            duration: Duration in seconds
        """
        cutoff_time = time.time() - duration
        return [m for m in self.metrics_history if m['timestamp'] >= cutoff_time]
    
    def generate_dashboard_html(self) -> str:
        """Generate HTML dashboard"""
        html = """
<!DOCTYPE html>
<html>
<head>
    <title>NLM Monitoring Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .metric { background: #f5f5f5; padding: 10px; margin: 10px 0; border-radius: 5px; }
        .metric h3 { margin: 0 0 10px 0; color: #333; }
        .value { font-size: 24px; font-weight: bold; color: #2e7d32; }
        .timestamp { color: #666; font-size: 12px; }
        .section { margin: 20px 0; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>NLM Monitoring Dashboard</h1>
    
    <div class="section">
        <h2>Current Status</h2>
        <div id="current-metrics">Loading...</div>
    </div>
    
    <div class="section">
        <h2>Recent Activity</h2>
        <canvas id="activityChart" width="400" height="200"></canvas>
    </div>
    
    <div class="section">
        <h2>Metric History</h2>
        <table id="metricsTable">
            <thead>
                <tr>
                    <th>Timestamp</th>
                    <th>Neurons</th>
                    <th>Spikes</th>
                    <th>Firing Rate</th>
                    <th>Duration</th>
                </tr>
            </thead>
            <tbody id="metricsBody">
            </tbody>
        </table>
    </div>
    
    <script>
        function updateDashboard() {
            fetch('/api/metrics/latest')
                .then(response => response.json())
                .then(data => {
                    updateCurrentMetrics(data);
                    updateChart(data);
                    updateTable(data);
                })
                .catch(error => console.error('Error fetching metrics:', error));
        }
        
        function updateCurrentMetrics(data) {
            const container = document.getElementById('current-metrics');
            container.innerHTML = `
                <div class="metric">
                    <h3>Brain Status</h3>
                    <div class="value">${data.brain ? data.brain.neurons || 'N/A' : 'N/A'} neurons</div>
                    <div>Spikes: ${data.brain ? data.brain.spikes || 'N/A' : 'N/A'}</div>
                    <div>Firing Rate: ${data.brain ? (data.brain.firing_rate || 0).toFixed(2) : 'N/A'} Hz</div>
                </div>
                <div class="metric">
                    <h3>System Info</h3>
                    <div class="timestamp">Uptime: ${data.uptime ? data.uptime.toFixed(1) : 'N/A'} seconds</div>
                </div>
            `;
        }
        
        function updateChart(data) {
            const ctx = document.getElementById('activityChart').getContext('2d');
            // Chart implementation would go here
        }
        
        function updateTable(data) {
            const tbody = document.getElementById('metricsBody');
            // Table implementation would go here
        }
        
        // Update dashboard every 5 seconds
        setInterval(updateDashboard, 5000);
        updateDashboard();
    </script>
</body>
</html>
        """
        return html

class ConfigurationValidator:
    """Advanced configuration validation with schema support"""
    
    def __init__(self):
        self.schemas = {}
        self.validators = {}
    
    def add_schema(self, name: str, schema: Dict):
        """
        Add a configuration schema
        
        Args:
            name: Schema name
            schema: Schema dictionary
        """
        self.schemas[name] = schema
        self._register_validators_from_schema(name, schema)
    
    def validate_config(self, config: Dict, schema_name: str = None) -> Dict[str, Any]:
        """
        Validate configuration against schema(s)
        
        Args:
            config: Configuration dictionary
            schema_name: Specific schema name to validate against
        
        Returns:
            Validation results
        """
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'schema_applied': schema_name
        }
        
        if schema_name:
            if schema_name not in self.schemas:
                results['errors'].append(f"Schema '{schema_name}' not found")
                results['valid'] = False
                return results
            
            schema = self.schemas[schema_name]
            validation_result = self._validate_against_schema(config, schema)
            results.update(validation_result)
        else:
            # Validate against all schemas
            for schema_name, schema in self.schemas.items():
                validation_result = self._validate_against_schema(config, schema)
                if not validation_result['valid']:
                    results['valid'] = False
                    results['errors'].extend(validation_result['errors'])
                results['warnings'].extend(validation_result['warnings'])
        
        return results
    
    def _validate_against_schema(self, config: Dict, schema: Dict) -> Dict[str, Any]:n        "normal_form_required": true, "min_length": 1
                "type": "range", "normal_form_required": true, "min": 1, "max": 1000
                "type": "enum", "normal_form_required": true, "allowed_values": ["INFO", "DEBUG", "WARNING"]
            }
        }
    
    def validate_required_fields(self, config: Dict, schema: Dict) -> List[str]:
        """Validate that all required fields are present"""
        required_fields = []
        for field_path, field_info in schema.items():
            if field_info.get('required', False):
                required_fields.append(field_path)
        
        missing_fields = []
        for field_path in required_fields:
            if not self._get_nested_value(config, field_path):
                missing_fields.append(field_path)
        
        return missing_fields
    
    def validate_types(self, config: Dict, schema: Dict) -> List[str]:
        """Validate that all field types match schema"""
        errors = []
        
        for field_path, field_info in schema.items():
            config_value = self._get_nested_value(config, field_path)
            if config_value is not None:
                expected_type = field_info.get('type')
                if expected_type and not self._is_valid_type(config_value, expected_type):
                    errors.append(f"{field_path}: expected {expected_type}, got {type(config_value).__name__}")
        
        return errors
    
    def _validate_range(self, value: Any, field_info: Dict) -> List[str]:
        """Validate value against range"""
        errors = []
        if isinstance(value, (int, float)):
            min_val = field_info.get('min', float('-inf'))
            max_val = field_info.get('max', float('inf'))
            if value < min_val:
                errors.append(f"Value {value} is less than minimum {min_val}")
            if value > max_val:
                errors.append(f"Value {value} is greater than maximum {max_val}")
        return errors
    
    def _validate_choices(self, value: Any, field_info: Dict) -> List[str]:
        """Validate value against allowed choices"""
        errors = []
        allowed_values = field_info.get('allowed_values', [])
        if allowed_values and value not in allowed_values:
            errors.append(f"Value '{value}' is not in allowed choices: {allowed_values}")
        return errors
    
    def _validate_pattern(self, value: Any, field_info: Dict) -> List[str]:
        """Validate value against pattern"""
        errors = []
        pattern = field_info.get('pattern')
        if pattern and not isinstance(value, str):
            errors.append(f"Pattern validation only applies to string values, got {type(value).__name__}")
        return errors
    
    def _get_nested_value(self, config: Dict, field_path: str) -> Any:
        """Get nested value from configuration using dot notation"""
        keys = field_path.split('.')
        value = config
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                return None
        return value
    
    def _is_valid_type(self, value: Any, expected_type: str) -> bool:
        """Check if value matches expected type"""
        type_mapping = {
            'int': int,
            'float': (int, float),
            'str': str,
            'bool': bool,
            'list': list,
            'dict': dict,
            'array': (list, tuple),
        }
        
        if expected_type in type_mapping:
            return isinstance(value, type_mapping[expected_type])
        
        return False

class DiagnosticTool:
    """Diagnostic tool for troubleshooting NLM issues"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.diagnostics = []
        self.logger = logging.getLogger("nlm.diagnostics")
    
    def run_diagnostics(self) -> Dict[str, Any]:
        """
        Run comprehensive diagnostics
        
        Returns:
            Dictionary containing diagnostic results
        """
        self.logger.info("Starting NLM diagnostics")
        
        diagnostics = {
            'timestamp': datetime.now().isoformat(),
            'system_info': self._get_system_info(),
            'configuration': self._get_config_info(),
            'validation': self._get_validation_info(),
            'performance': self._get_performance_info(),
            'errors': [],
            'warnings': []
        }
        
        # Run diagnostic checks
        self._check_configuration()
        self._check_dependencies()
        self._check_performance()
        self._check_system_resources()
        
        return diagnostics
    
    def _get_system_info(self) -> Dict[str, Any]:
        """Get system information"""
        import platform
        import psutil
        
        return {
            'platform': platform.system(),
            'platform_version': platform.version(),
            'architecture': platform.machine(),
            'python_version': platform.python_version(),
            'cpu_count': psutil.cpu_count(),
            'memory_total': psutil.virtual_memory().total,
            'memory_available': psutil.virtual_memory().available,
            'disk_usage': psutil.disk_usage('/').percent
        }
    
    def _get_config_info(self) -> Dict[str, Any]:
        """Get configuration information"""
        return {
            'config_keys': list(self.config.keys()),
            'config_summary': {k: str(v)[:100] for k, v in self.config.items()},
            'has_critical_settings': all(
                key in self.config for key in ['neuron_count', 'simulation_timestep']
            )
        }
    
    def _get_validation_info(self) -> Dict[str, Any]:
        """Get validation information"""
        validator = ConfigurationValidator()
        
        # Add basic schema
        basic_schema = {
            'neuron_count': {'type': 'int', 'required': True, 'min': 1, 'max': 100000},
            'simulation_timestep': {'type': 'float', 'required': True, 'min': 0.0, 'max': 1.0},
            'region_count': {'type': 'int', 'required': True, 'min': 1, 'max': 1000},
        }
        
        validator.add_schema('basic', basic_schema)
        
        validation_result = validator.validate_config(self.config, 'basic')
        
        return {
            'valid': validation_result['valid'],
            'errors': validation_result['errors'],
            'warnings': validation_result['warnings']
        }
    
    def _get_performance_info(self) -> Dict[str, Any]:
        """Get performance information"""
        import psutil
        
        return {
            'cpu_usage': psutil.cpu_percent(interval=0.1),
            'memory_usage': psutil.virtual_memory().percent,
            'disk_io': psutil.disk_io_counters().iface_read_kb/s if hasattr(psutil.disk_io_counters(), 'iface_read_kb/s') else 0,
        }
    
    def _check_configuration(self):
        """Check configuration issues"""
        config_issues = []
        
        # Check for suspicious values
        if self.config.get('neuron_count', 0) > 10000:
            config_issues.append("High neuron count may impact performance")
        
        if self.config.get('simulation_timestep', 0.001) > 0.1:
            config_issues.append("Large timestep may reduce accuracy")
        
        if self.config.get('log_level', 'INFO') not in ['DEBUG', 'INFO', 'WARNING', 'ERROR']:
            config_issues.append("Invalid log level")
        
        self.diagnostics.extend(config_issues)
        
        if config_issues:
            self.logger.warning(f"Configuration issues found: {config_issues}")
    
    def _check_dependencies(self):
        """Check dependency issues"""
        try:
            import numpy
            self.diagnostics.append("NumPy version: " + numpy.__version__)
        except ImportError:
            self.logger.error("NumPy not installed - required for performance optimizations")
            self.diagnostics.append("ERROR: NumPy not installed")
        
        try:
            import pandas
            self.diagnostics.append("Pandas version: " + pandas.__version__)
        except ImportError:
            self.logger.warning("Pandas not installed - analytics features limited")
            self.diagnostics.append("WARNING: Pandas not installed")
        
        try:
            import matplotlib
            self.diagnostics.append("Matplotlib version: " + matplotlib.__version__)
        except ImportError:
            self.logger.warning("Matplotlib not installed - visualization features limited")
            self.diagnostics.append("WARNING: Matplotlib not installed")
    
    def _check_performance(self):
        """Check performance issues"""
        try:
            import psutil
            memory_usage = psutil.virtual_memory().percent
            if memory_usage > 90:
                self.logger.warning(f"High memory usage: {memory_usage}%")
                self.diagnostics.append(f"WARNING: High memory usage ({memory_usage}%)")
            
            cpu_usage = psutil.cpu_percent(interval=0.1)
            if cpu_usage > 80:
                self.logger.warning(f"High CPU usage: {cpu_usage}%")
                self.diagnostics.append(f"WARNING: High CPU usage ({cpu_usage}%)")
        except ImportError:
            self.logger.warning("psutil not installed - performance monitoring limited")
            self.diagnostics.append("WARNING: psutil not installed")
    
    def _check_system_resources(self):
        """Check system resource issues"""
        import platform
        
        if platform.system() == 'Linux':
            # Check for memory issues on Linux
            try:
                with open('/proc/meminfo', 'r') as f:
                    meminfo = f.read()
                    if 'SwapTotal' in meminfo and 'SwapFree' in meminfo:
                        lines = meminfo.split('\n')
                        for line in lines:
                            if 'SwapTotal' in line:
                                swap_total = int(line.split(':')[1].strip().split()[0])
                            if 'SwapFree' in line:
                                swap_free = int(line.split(':')[1].strip().split()[0])
                        
                        if swap_free == 0:
                            self.logger.warning("No swap space available")
                            self.diagnostics.append("WARNING: No swap space available")
            except Exception as e:
                self.logger.warning(f"Could not check swap space: {e}")
    
    def get_diagnostic_report(self) -> str:
        """Generate diagnostic report"""
        report = f"NLM Diagnostic Report\n"
        report += f"Generated: {datetime.now().isoformat()}\n\n"
        
        report += f"System Info:\n"
        for key, value in self._get_system_info().items():
            report += f"  {key}: {value}\n"
        
        report += f"\nConfiguration:\n"
        for key, value in self._get_config_info().items():
            report += f"  {key}: {value}\n"
        
        report += f"\nValidation:\n"
        validation_info = self._get_validation_info()
        report += f"  Valid: {validation_info['valid']}\n"
        if validation_info['errors']:
            report += f"  Errors: {validation_info['errors']}\n"
        if validation_info['warnings']:
            report += f"  Warnings: {validation_info['warnings']}\n"
        
        report += f"\nDiagnostics:\n"
        for diagnostic in self.diagnostics:
            report += f"  {diagnostic}\n"
        
        return report

# Example usage
if __name__ == "__main__":
    # Example configuration
    config = {
        'neuron_count': 1000,
        'simulation_timestep': 0.001,
        'region_count': 2,
        'log_level': 'INFO',
    }
    
    # Create diagnostic tool
    diagnostic = DiagnosticTool(config)
    
    # Run diagnostics
    results = diagnostic.run_diagnostics()
    
    # Print report
    print(diagnostic.get_diagnostic_report())
    
    # Save results
    with open('diagnostic_report.json', 'w') as f:
        import json
        json.dump(results, f, indent=2)
    
    print(f"\nDiagnostic report saved to diagnostic_report.json")
