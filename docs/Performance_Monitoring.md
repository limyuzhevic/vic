# NLM Performance Monitoring and Debugging Tools

This document provides tools and utilities for performance monitoring, debugging, and system analysis of NLM simulations.

## Overview

Performance monitoring is essential for:
- Understanding system behavior
- Identifying performance bottlenecks
- Optimizing simulation parameters
- Debugging issues
- Validating configuration

## Tools and Utilities

### 1. Performance Monitor Class

```python
import pynlm
import time
import psutil
import os
import threading
from typing import Dict, List, Callable
import json
from datetime import datetime

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain = None,
                 world: pynlm.SimpleWorld = None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.process = psutil.Process(os.getpid())
        
        # Performance data collection
        self.performance_data = []
        self.monitoring = False
        self.monitor_thread = None
        
        # Metrics collection
        self.metrics = {
            'time': [],
            'memory': [],
            'cpu': [],
            'neurons': [],
            'synapses': [],
            'firing': [],
            'spikes': [],
            'learning': [],
            'system_status': []
        }
        
    def start_monitoring(self, interval: float = 0.1):
        """Start continuous performance monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop, args=(interval,)
        )
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print(f"Performance monitoring started with {interval}s interval")
        
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        self.monitor_thread = None
        print("Performance monitoring stopped")
        
    def _monitor_loop(self, interval: float):
        """Main monitoring loop."""
        while self.monitoring:
            self._collect_metrics()
            time.sleep(interval)
            
    def _collect_metrics(self):
        """Collect current performance metrics."""
        timestamp = time.time()
        
        # System metrics
        memory_info = self.process.memory_info()
        cpu_percent = self.process.cpu_percent()
        
        # Brain metrics
        neurons = self.brain.getTotalNeuronCount()
        synapses = self.brain.getTotalSynapseCount()
        firing = self.brain.getFiringNeuronCount()
        spikes = self.brain.getTotalSpikeCount()
        
        # Learning metrics (if agent is available)
        learning_stats = {}
        if self.agent:
            learning_stats = {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation': self.agent.getNeuromodulationLevel()
            }
        
        # System status
        system_status = {}
        if self.brain:
            system_status = pynlm.getSystemStatus(self.brain)
        
        # Store metrics
        self.metrics['time'].append(timestamp)
        self.metrics['memory'].append(memory_info.rss)
        self.metrics['cpu'].append(cpu_percent)
        self.metrics['neurons'].append(neurons)
        self.metrics['synapses'].append(synapses)
        self.metrics['firing'].append(firing)
        self.metrics['spikes'].append(spikes)
        self.metrics['learning'].append(learning_stats)
        self.metrics['system_status'].append(system_status)
        
        # Store in performance data
        self.performance_data.append({
            'timestamp': timestamp,
            'memory_mb': memory_info.rss / 1024 / 1024,
            'cpu_percent': cpu_percent,
            'neurons': neurons,
            'synapses': synapses,
            'firing_neurons': firing,
            'total_spikes': spikes,
            'learning_stats': learning_stats,
            'system_status': system_status
        })
        
    def get_performance_summary(self) -> Dict:
        """Get summary statistics of collected performance data."""
        if not self.performance_data:
            return {'error': 'No performance data collected'}
        
        summary = {
            'total_samples': len(self.performance_data),
            'time_range': {
                'start': self.performance_data[0]['timestamp'],
                'end': self.performance_data[-1]['timestamp']
            },
            'memory': {
                'min': min(d['memory_mb'] for d in self.performance_data),
                'max': max(d['memory_mb'] for d in self.performance_data),
                'avg': sum(d['memory_mb'] for d in self.performance_data) / len(self.performance_data)
            },
            'cpu': {
                'min': min(d['cpu_percent'] for d in self.performance_data),
                'max': max(d['cpu_percent'] for d in self.performance_data),
                'avg': sum(d['cpu_percent'] for d in self.performance_data) / len(self.performance_data)
            },
            'brain': {
                'neurons': {
                    'min': min(d['neurons'] for d in self.performance_data),
                    'max': max(d['neurons'] for d in self.performance_data),
                    'avg': sum(d['neurons'] for d in self.performance_data) / len(self.performance_data)
                },
                'synapses': {
                    'min': min(d['synapses'] for d in self.performance_data),
                    'max': max(d['synapses'] for d in self.performance_data),
                    'avg': sum(d['synapses'] for d in self.performance_data) / len(self.performance_data)
                },
                'firing_rate': {
                    'min': min(d['firing_neurons'] for d in self.performance_data),
                    'max': max(d['firing_neurons'] for d in self.performance_data),
                    'avg': sum(d['firing_neurons'] for d in self.performance_data) / len(self.performance_data)
                },
                'spike_rate': {
                    'min': min(d['total_spikes'] for d in self.performance_data),
                    'max': max(d['total_spikes'] for d in self.performance_data),
                    'avg': sum(d['total_spikes'] for d in self.performance_data) / len(self.performance_data)
                }
            }
        }
        
        return summary
        
    def save_performance_data(self, filename: str):
        """Save performance data to JSON file."""
        data = {
            'metadata': {
                'collected_at': datetime.now().isoformat(),
                'brain_info': f"Neurons: {self.brain.getTotalNeuronCount()}, "
                            f"Synapses: {self.brain.getTotalSynapseCount()}"
            },
            'performance_data': self.performance_data
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
            
        print(f"Performance data saved to {filename}")
        
    def plot_performance(self, save_path: str = None):
        """Create performance visualization (requires matplotlib)."""
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            fig, axes = plt.subplots(2, 2, figsize=(15, 10))
            fig.suptitle('NLM Performance Monitoring', fontsize=16)
            
            # Memory usage
            ax = axes[0, 0]
            times = [d['timestamp'] - self.performance_data[0]['timestamp'] 
                    for d in self.performance_data]
            memory = [d['memory_mb'] for d in self.performance_data]
            ax.plot(times, memory, 'b-', linewidth=1)
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Memory (MB)')
            ax.set_title('Memory Usage Over Time')
            ax.grid(True, alpha=0.3)
            
            # CPU usage
            ax = axes[0, 1]
            cpu = [d['cpu_percent'] for d in self.performance_data]
            ax.plot(times, cpu, 'r-', linewidth=1)
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('CPU (%)')
            ax.set_title('CPU Usage Over Time')
            ax.grid(True, alpha=0.3)
            
            # Neuron count
            ax = axes[1, 0]
            neurons = [d['neurons'] for d in self.performance_data]
            ax.plot(times, neurons, 'g-', linewidth=1)
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Neuron Count')
            ax.set_title('Neuron Count Over Time')
            ax.grid(True, alpha=0.3)
            
            # Spike rate
            ax = axes[1, 1]
            spikes = [d['total_spikes'] for d in self.performance_data]
            ax.plot(times, spikes, 'm-', linewidth=1)
            ax.set_xlabel('Time (s)')
            ax.set_ylabel('Total Spikes')
            ax.set_title('Spike Count Over Time')
            ax.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=150, bbox_inches='tight')
                print(f"Performance plot saved to {save_path}")
            
            plt.show()
            
        except ImportError:
            print("matplotlib not available. Skipping plot generation.")
            
    def get_real_time_stats(self) -> Dict:
        """Get current real-time performance statistics."""
        if not self.performance_data:
            return {'error': 'No performance data available'}
        
        current = self.performance_data[-1]
        
        stats = {
            'timestamp': current['timestamp'],
            'memory_mb': current['memory_mb'],
            'cpu_percent': current['cpu_percent'],
            'neurons': current['neurons'],
            'synapses': current['synapses'],
            'firing_neurons': current['firing_neurons'],
            'total_spikes': current['total_spikes'],
            'learning_stats': current['learning_stats'],
            'system_status': current['system_status']
        }
        
        return stats
```

### 2. Simulation Profiler

```python
import pynlm
import time
import cProfile
import pstats
from io import StringIO
from contextlib import contextmanager

class SimulationProfiler:
    """Profiler for NLM simulations."""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.profiling_data = []
        
    @contextmanager
    def profile_step(self, step_name: str):
        """Profile a single simulation step."""
        pr = cProfile.Profile()
        pr.enable()
        start_time = time.time()
        
        try:
            yield
        finally:
            pr.disable()
            end_time = time.time()
            
            # Get current brain state
            neurons = self.brain.getTotalNeuronCount()
            firing = self.brain.getFiringNeuronCount()
            spikes = self.brain.getTotalSpikeCount()
            
            # Process profile results
            s = StringIO()
            ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
            ps.print_stats()
            
            # Store profiling data
            self.profiling_data.append({
                'step_name': step_name,
                'duration': end_time - start_time,
                'neurons': neurons,
                'firing': firing,
                'spikes': spikes,
                'profile_stats': s.getvalue()
            })
            
    def get_profiling_summary(self) -> Dict:
        """Get profiling summary."""
        if not self.profiling_data:
            return {'error': 'No profiling data collected'}
        
        summary = {
            'total_steps': len(self.profiling_data),
            'total_duration': sum(d['duration'] for d in self.profiling_data),
            'avg_duration_per_step': sum(d['duration'] for d in self.profiling_data) / len(self.profiling_data),
            'max_duration': max(d['duration'] for d in self.profiling_data),
            'min_duration': min(d['duration'] for d in self.profiling_data),
            'brain_stats': {
                'final_neurons': self.profiling_data[-1]['neurons'],
                'final_firing': self.profiling_data[-1]['firing'],
                'final_spikes': self.profiling_data[-1]['spikes']
            }
        }
        
        return summary
        
    def get_step_analysis(self, step_index: int) -> Dict:
        """Get analysis for a specific step."""
        if step_index >= len(self.profiling_data):
            return {'error': 'Step index out of range'}
        
        return self.profiling_data[step_index]
```

### 3. Debug Logger

```python
import pynlm
import logging
from datetime import datetime
from typing import Dict, Any, List
import json

class DebugLogger:
    """Enhanced debug logger for NLM simulations."""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain = None,
                 world: pynlm.SimpleWorld = None, log_file: str = "nlm_debug.log"):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.log_file = log_file
        
        # Configure logging
        self.logger = logging.getLogger('nlm_debug')
        self.logger.setLevel(logging.DEBUG)
        
        # File handler
        fh = logging.FileHandler(log_file)
        fh.setLevel(logging.DEBUG)
        
        # Console handler
        ch = logging.StreamHandler()
        ch.setLevel(logging.INFO)
        
        # Formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        fh.setFormatter(formatter)
        ch.setFormatter(formatter)
        
        self.logger.addHandler(fh)
        self.logger.addHandler(ch)
        
        # Debug data collection
        self.debug_events = []
        self.simulation_log = []
        
    def log_simulation_step(self, step: int, timestamp: float = None,
                           additional_data: Dict[str, Any] = None):
        """Log a simulation step with detailed information."""
        if timestamp is None:
            timestamp = time.time()
        
        # Collect current state
        event = {
            'timestamp': timestamp,
            'step': step,
            'brain_state': {
                'neurons': self.brain.getTotalNeuronCount(),
                'synapses': self.brain.getTotalSynapseCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'active': self.brain.getActiveNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'developmental_stage': static_cast<int>(self.brain.getDevelopmentalStage())
            }
        }
        
        # Add agent state if available
        if self.agent:
            event['agent_state'] = {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation': self.agent.getNeuromodulationLevel(),
                'developmental_stage': self.agent.getDevelopmentalStage()
            }
        
        # Add world state if available
        if self.world:
            world_obj = self.world.getAgentBody()
            event['world_state'] = {
                'position': {'x': world_obj.x, 'y': world_obj.y},
                'orientation': world_obj.orientation,
                'velocity': {'x': world_obj.velocityX, 'y': world_obj.velocityY},
                'energy': world_obj.energy,
                'health': world_obj.health,
                'age': world_obj.age,
                'simulation_time': self.world.getSimulationTime()
            }
        
        # Add additional data
        if additional_data:
            event.update(additional_data)
        
        # Log to file
        self.logger.debug(f"Step {step}: {json.dumps(event, indent=2)}")
        
        # Store in simulation log
        self.simulation_log.append(event)
        
    def log_event(self, event_type: str, message: str, 
                  severity: str = 'INFO', data: Dict[str, Any] = None):
        """Log a specific event with custom data."""
        event = {
            'timestamp': time.time(),
            'event_type': event_type,
            'message': message,
            'severity': severity,
            'data': data or {}
        }
        
        # Log to file
        log_method = getattr(self.logger, severity.lower())
        log_method(f"{event_type}: {message} - Data: {event}")
        
        # Store event
        self.debug_events.append(event)
        
    def log_brain_analysis(self, analysis_type: str, result: Dict[str, Any]):
        """Log brain state analysis."""
        self.log_event(
            'BRAIN_ANALYSIS',
            f"Brain analysis ({analysis_type})",
            'DEBUG',
            {
                'analysis_type': analysis_type,
                'result': result,
                'brain_info': {
                    'neurons': self.brain.getTotalNeuronCount(),
                    'synapses': self.brain.getTotalSynapseCount(),
                    'developmental_stage': static_cast<int>(self.brain.getDevelopmentalStage())
                }
            }
        )
        
    def log_learning_event(self, event_type: str, details: Dict[str, Any]):
        """Log learning-related events."""
        self.log_event(
            'LEARNING',
            f"Learning event: {event_type}",
            'INFO',
            {
                'event_type': event_type,
                'details': details,
                'agent_stats': (self.agent.getLearningStatistics() 
                              if self.agent else None)
            }
        )
        
    def analyze_simulation_patterns(self) -> Dict:
        """Analyze patterns in simulation data."""
        if not self.simulation_log:
            return {'error': 'No simulation data available'}
        
        analysis = {
            'total_steps': len(self.simulation_log),
            'neuron_growth': [],
            'firing_patterns': [],
            'learning_patterns': [],
            'world_patterns': []
        }
        
        # Analyze neuron growth
        for event in self.simulation_log:
            analysis['neuron_growth'].append({
                'step': event['step'],
                'neurons': event['brain_state']['neurons'],
                'synapses': event['brain_state']['synapses']
            })
        
        # Analyze firing patterns
        for event in self.simulation_log:
            analysis['firing_patterns'].append({
                'step': event['step'],
                'firing': event['brain_state']['firing'],
                'firing_rate': event['brain_state']['firing_rate']
            })
        
        # Analyze learning patterns (if agent available)
        if self.agent:
            for event in self.simulation_log:
                if 'agent_state' in event:
                    analysis['learning_patterns'].append({
                        'step': event['step'],
                        'curiosity': event['agent_state']['curiosity'],
                        'novelty': event['agent_state']['novelty'],
                        'prediction_error': event['agent_state']['prediction_error']
                    })
        
        return analysis
        
    def export_simulation_log(self, filename: str):
        """Export simulation log to JSON file."""
        data = {
            'metadata': {
                'exported_at': datetime.now().isoformat(),
                'simulation_steps': len(self.simulation_log),
                'debug_events': len(self.debug_events)
            },
            'simulation_log': self.simulation_log,
            'debug_events': self.debug_events
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
            
        print(f"Simulation log exported to {filename}")
        
    def get_simulation_summary(self) -> Dict:
        """Get summary of simulation log."""
        if not self.simulation_log:
            return {'error': 'No simulation data available'}
        
        summary = {
            'total_steps': len(self.simulation_log),
            'start_step': self.simulation_log[0]['step'],
            'end_step': self.simulation_log[-1]['step'],
            'neuron_range': {
                'min': min(event['brain_state']['neurons'] for event in self.simulation_log),
                'max': max(event['brain_state']['neurons'] for event in self.simulation_log)
            },
            'firing_range': {
                'min': min(event['brain_state']['firing'] for event in self.simulation_log),
                'max': max(event['brain_state']['firing'] for event in self.simulation_log)
            },
            'time_range': {
                'start': self.simulation_log[0]['timestamp'],
                'end': self.simulation_log[-1]['timestamp']
            }
        }
        
        return summary
```

### 4. Configuration Validator

```python
import pynlm
import json
from typing import Dict, List, Any
from pathlib import Path

class ConfigurationValidator:
    """Validate NLM configurations and provide recommendations."""
    
    def __init__(self):
        self.validation_rules = self._load_validation_rules()
        self.warnings = []
        self.errors = []
        
    def _load_validation_rules(self) -> Dict:
        """Load validation rules."""
        rules = {
            'brain.neuron_count': {
                'type': 'int',
                'min': 1,
                'max': 10000,
                'recommended': [100, 500, 1000, 5000]
            },
            'brain.synapse_density': {
                'type': 'float',
                'min': 0.0,
                'max': 1.0,
                'recommended': [0.05, 0.1, 0.15, 0.2]
            },
            'plasticity.stdp.enable': {
                'type': 'bool',
                'recommended_true_for': ['learning', 'plasticity']
            },
            'plasticity.hebbian.enable': {
                'type': 'bool',
                'recommended_true_for': ['learning', 'association']
            },
            'neuromod.curiosity.enable': {
                'type': 'bool',
                'recommended_true_for': ['exploration', 'novelty']
            },
            'development.critical_period_end': {
                'type': 'float',
                'min': 0.0,
                'recommended': [50.0, 100.0, 200.0]
            }
        }
        return rules
        
    def validate_config(self, config: pynlm.Config) -> Dict:
        """Validate configuration and return results."""
        self.warnings = []
        self.errors = []
        
        validation_result = {
            'valid': True,
            'warnings': [],
            'errors': [],
            'recommendations': [],
            'score': 100
        }
        
        # Get all configuration keys
        keys = config.getKeys()
        
        for key in keys:
            # Get value
            value = config.getOr[key]  # This is a placeholder, use actual method
            
            # Get rule for this key
            rule = self.validation_rules.get(key)
            if not rule:
                continue
                
            # Validate type
            if rule['type'] == 'int' and not isinstance(value, int):
                self.errors.append(f"Key '{key}' should be int, got {type(value).__name__}")
                validation_result['score'] -= 10
                
            elif rule['type'] == 'float' and not isinstance(value, (int, float)):
                self.errors.append(f"Key '{key}' should be float, got {type(value).__name__}")
                validation_result['score'] -= 10
                
            elif rule['type'] == 'bool' and not isinstance(value, bool):
                self.errors.append(f"Key '{key}' should be bool, got {type(value).__name__}")
                validation_result['score'] -= 10
            
            # Validate range
            if 'min' in rule and value < rule['min']:
                self.errors.append(f"Key '{key}' value {value} is below minimum {rule['min']}")
                validation_result['score'] -= 5
                
            if 'max' in rule and value > rule['max']:
                self.errors.append(f"Key '{key}' value {value} exceeds maximum {rule['max']}")
                validation_result['score'] -= 5
            
            # Check recommendations
            if 'recommended' in rule and value not in rule['recommended']:
                self.warnings.append(
                    f"Key '{key}' value {value} is not in recommended values: {rule['recommended']}"
                )
                validation_result['score'] -= 3
                
            if rule.get('recommended_true_for') and value:
                # Check if other related keys are enabled
                for required_key in rule['recommended_true_for']:
                    if required_key in keys and not config.getOr[required_key]:
                        self.warnings.append(
                            f"Key '{key}' is true but related key '{required_key}' is false"
                        )
                        validation_result['score'] -= 2
        
        # Convert to validation result
        validation_result['valid'] = len(self.errors) == 0
        validation_result['warnings'] = self.warnings
        validation_result['errors'] = self.errors
        
        # Generate recommendations
        if validation_result['score'] < 80:
            validation_result['recommendations'].append(
                "Consider reviewing and adjusting configuration parameters"
            )
        
        if any('learning' in str(w).lower() for w in self.warnings):
            validation_result['recommendations'].append(
                "Enable learning systems for better learning performance"
            )
        
        if any('plasticity' in str(w).lower() for w in self.warnings):
            validation_result['recommendations'].append(
                "Enable plasticity systems for neural adaptation"
            )
        
        return validation_result
        
    def validate_configuration_file(self, filepath: str) -> Dict:
        """Validate configuration from JSON file."""
        try:
            with open(filepath, 'r') as f:
                config_data = json.load(f)
                
            # Create config
            config = pynlm.createDefaultConfig()
            
            # Load configuration
            for key, value in config_data.items():
                config.set(key, value)
                
            # Validate
            return self.validate_config(config)
            
        except Exception as e:
            return {
                'valid': False,
                'errors': [f"Failed to load configuration file: {str(e)}"],
                'warnings': [],
                'recommendations': []
            }
            
    def compare_configurations(self, config1: pynlm.Config, config2: pynlm.Config) -> Dict:
        """Compare two configurations and identify differences."""
        differences = {
            'only_in_config1': [],
            'only_in_config2': [],
            'different_values': [],
            'summary': {}
        }
        
        keys1 = set(config1.getKeys())
        keys2 = set(config2.getKeys())
        
        differences['only_in_config1'] = keys1 - keys2
        differences['only_in_config2'] = keys2 - keys1
        
        for key in keys1 & keys2:
            value1 = config1.getOr[key]
            value2 = config2.getOr[key]
            
            if value1 != value2:
                differences['different_values'].append({
                    'key': key,
                    'value1': value1,
                    'value2': value2
                })
        
        differences['summary'] = {
            'total_keys_config1': len(keys1),
            'total_keys_config2': len(keys2),
            'common_keys': len(keys1 & keys2),
            'different_values_count': len(differences['different_values'])
        }
        
        return differences

# Usage example
if __name__ == "__main__":
    # Create validator
    validator = ConfigurationValidator()
    
    # Create configurations
    config1 = pynlm.createDevelopmentConfig()
    config2 = pynlm.createExplorationConfig()
    
    # Validate configurations
    result1 = validator.validate_config(config1)
    result2 = validator.validate_config(config2)
    
    print("Configuration 1 Validation:")
    print(f"  Valid: {result1['valid']}")
    print(f"  Score: {result1['score']}")
    print(f"  Warnings: {len(result1['warnings'])}")
    print(f"  Errors: {len(result1['errors'])}")
    
    print("\nConfiguration 2 Validation:")
    print(f"  Valid: {result2['valid']}")
    print(f"  Score: {result2['score']}")
    print(f"  Warnings: {len(result2['warnings'])}")
    print(f"  Errors: {len(result2['errors'])}")
    
    # Compare configurations
    comparison = validator.compare_configurations(config1, config2)
    print(f"\nConfiguration Comparison:")
    print(f"  Total keys in config1: {comparison['summary']['total_keys_config1']}")
    print(f"  Total keys in config2: {comparison['summary']['total_keys_config2']}")
    print(f"  Different values: {comparison['summary']['different_values_count']}")
```

### 5. System Health Monitor

```python
import pynlm
import time
import psutil
import os
from datetime import datetime
from typing import Dict, List, Any
import json

class SystemHealthMonitor:
    """Monitor system health and performance for NLM simulations."""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain = None,
                 world: pynlm.SimpleWorld = None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.process = psutil.Process(os.getpid())
        
        # Health metrics
        self.health_metrics = {
            'cpu_usage': [],
            'memory_usage': [],
            'neuron_count': [],
            'firing_rate': [],
            'spike_rate': [],
            'learning_activity': [],
            'simulation_time': []
        }
        
        # Health alerts
        self.alerts = []
        self.thresholds = {
            'cpu_max': 90.0,
            'memory_max_mb': 2048,
            'neuron_max': 10000,
            'firing_rate_max': 0.9,
            'spike_rate_max': 1000,
            'learning_activity_max': 1.0
        }
        
    def collect_health_metrics(self):
        """Collect current health metrics."""
        timestamp = time.time()
        
        # System metrics
        cpu_percent = self.process.cpu_percent()
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        # Brain metrics
        neurons = self.brain.getTotalNeuronCount()
        firing = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        simulation_time = self.brain.getConfig().getOr<float>('simulation_timestep', 0.001)
        
        # Calculate rates
        if len(self.health_metrics['cpu_usage']) > 0:
            # Calculate firing rate (neurons per simulation step)
            firing_rate = firing / neurons if neurons > 0 else 0.0
            
            # Calculate spike rate (spikes per simulation second)
            spike_rate = total_spikes / (len(self.health_metrics['simulation_time']) * simulation_time) \
                        if len(self.health_metrics['simulation_time']) > 0 else 0.0
            
            # Learning activity (if agent available)
            learning_activity = 0.0
            if self.agent:
                stats = self.agent.getLearningStatistics()
                learning_activity = (
                    stats.get('curiosity', 0.0) +
                    stats.get('novelty', 0.0) +
                    stats.get('prediction_error', 0.0)
                ) / 3.0 if stats else 0.0
        else:
            firing_rate = 0.0
            spike_rate = 0.0
            learning_activity = 0.0
        
        # Store metrics
        self.health_metrics['cpu_usage'].append(cpu_percent)
        self.health_metrics['memory_usage'].append(memory_mb)
        self.health_metrics['neuron_count'].append(neurons)
        self.health_metrics['firing_rate'].append(firing_rate)
        self.health_metrics['spike_rate'].append(spike_rate)
        self.health_metrics['learning_activity'].append(learning_activity)
        self.health_metrics['simulation_time'].append(simulation_time)
        
        # Check for alerts
        self._check_alerts(timestamp, cpu_percent, memory_mb, firing_rate, spike_rate, learning_activity)
        
    def _check_alerts(self, timestamp: float, cpu: float, memory_mb: float,
                     firing_rate: float, spike_rate: float, learning_activity: float):
        """Check for health alerts and generate warnings."""
        alerts = []
        
        # CPU alert
        if cpu > self.thresholds['cpu_max']:
            alerts.append({
                'timestamp': timestamp,
                'type': 'CPU_HIGH',
                'message': f"CPU usage is high: {cpu:.1f}%",
                'severity': 'WARNING' if cpu < 95 else 'CRITICAL',
                'value': cpu,
                'threshold': self.thresholds['cpu_max']
            })
        
        # Memory alert
        if memory_mb > self.thresholds['memory_max_mb']:
            alerts.append({
                'timestamp': timestamp,
                'type': 'MEMORY_HIGH',
                'message': f"Memory usage is high: {memory_mb:.1f} MB",
                'severity': 'WARNING' if memory_mb < 3000 else 'CRITICAL',
                'value': memory_mb,
                'threshold': self.thresholds['memory_max_mb']
            })
        
        # Neuron count alert
        if len(self.health_metrics['neuron_count']) > 0:
            current_neuron_count = self.health_metrics['neuron_count'][-1]
            if current_neuron_count > self.thresholds['neuron_max']:
                alerts.append({
                    'timestamp': timestamp,
                    'type': 'NEURON_COUNT_HIGH',
                    'message': f"Neuron count is high: {current_neuron_count}",
                    'severity': 'WARNING',
                    'value': current_neuron_count,
                    'threshold': self.thresholds['neuron_max']
                })
        
        # Firing rate alert
        if firing_rate > self.thresholds['firing_rate_max']:
            alerts.append({
                'timestamp': timestamp,
                'type': 'FIRING_RATE_HIGH',
                'message': f"Firing rate is high: {firing_rate:.3f}",
                'severity': 'INFO',
                'value': firing_rate,
                'threshold': self.thresholds['firing_rate_max']
            })
        
        # Spike rate alert
        if spike_rate > self.thresholds['spike_rate_max']:
            alerts.append({
                'timestamp': timestamp,
                'type': 'SPIKE_RATE_HIGH',
                'message': f"Spike rate is high: {spike_rate:.1f}",
                'severity': 'INFO',
                'value': spike_rate,
                'threshold': self.thresholds['spike_rate_max']
            })
        
        # Learning activity alert
        if learning_activity > self.thresholds['learning_activity_max']:
            alerts.append({
                'timestamp': timestamp,
                'type': 'LEARNING_ACTIVITY_HIGH',
                'message': f"Learning activity is high: {learning_activity:.3f}",
                'severity': 'INFO',
                'value': learning_activity,
                'threshold': self.thresholds['learning_activity_max']
            })
        
        # Add alerts to alert list
        self.alerts.extend(alerts)
        
    def get_health_report(self) -> Dict:
        """Get comprehensive health report."""
        if not self.health_metrics['cpu_usage']:
            return {'error': 'No health metrics collected'}
        
        # Calculate averages and ranges
        health_report = {
            'timestamp': datetime.now().isoformat(),
            'system_status': 'HEALTHY',
            'metrics': {},
            'alerts': self.alerts,
            'summary': {}
        }
        
        # Calculate averages
        health_report['metrics'] = {
            'cpu': {
                'current': self.health_metrics['cpu_usage'][-1],
                'average': sum(self.health_metrics['cpu_usage']) / len(self.health_metrics['cpu_usage']),
                'min': min(self.health_metrics['cpu_usage']),
                'max': max(self.health_metrics['cpu_usage'])
            },
            'memory': {
                'current': self.health_metrics['memory_usage'][-1],
                'average': sum(self.health_metrics['memory_usage']) / len(self.health_metrics['memory_usage']),
                'min': min(self.health_metrics['memory_usage']),
                'max': max(self.health_metrics['memory_usage'])
            },
            'neurons': {
                'current': self.health_metrics['neuron_count'][-1],
                'average': sum(self.health_metrics['neuron_count']) / len(self.health_metrics['neuron_count']),
                'min': min(self.health_metrics['neuron_count']),
                'max': max(self.health_metrics['neuron_count'])
            },
            'firing_rate': {
                'current': self.health_metrics['firing_rate'][-1],
                'average': sum(self.health_metrics['firing_rate']) / len(self.health_metrics['firing_rate']),
                'min': min(self.health_metrics['firing_rate']),
                'max': max(self.health_metrics['firing_rate'])
            },
            'spike_rate': {
                'current': self.health_metrics['spike_rate'][-1],
                'average': sum(self.health_metrics['spike_rate']) / len(self.health_metrics['spike_rate']),
                'min': min(self.health_metrics['spike_rate']),
                'max': max(self.health_metrics['spike_rate'])
            },
            'learning_activity': {
                'current': self.health_metrics['learning_activity'][-1],
                'average': sum(self.health_metrics['learning_activity']) / len(self.health_metrics['learning_activity']),
                'min': min(self.health_metrics['learning_activity']),
                'max': max(self.health_metrics['learning_activity'])
            }
        }
        
        # Determine system status
        if any(alert['severity'] == 'CRITICAL' for alert in self.alerts):
            health_report['system_status'] = 'CRITICAL'
        elif any(alert['severity'] == 'WARNING' for alert in self.alerts):
            health_report['system_status'] = 'WARNING'
        elif len(self.alerts) > 5:
            health_report['system_status'] = 'DEGRADED'
        else:
            health_report['system_status'] = 'HEALTHY'
        
        # Calculate summary
        total_duration = len(self.health_metrics['cpu_usage']) * 0.1  # Assuming 0.1s intervals
        health_report['summary'] = {
            'monitoring_duration_seconds': total_duration,
            'total_samples': len(self.health_metrics['cpu_usage']),
            'cpu_excess_count': sum(1 for cpu in self.health_metrics['cpu_usage'] if cpu > self.thresholds['cpu_max']),
            'memory_excess_count': sum(1 for mem in self.health_metrics['memory_usage'] if mem > self.thresholds['memory_max_mb'])
        }
        
        return health_report
        
    def reset_health_metrics(self):
        """Reset health metrics."""
        self.health_metrics = {
            'cpu_usage': [],
            'memory_usage': [],
            'neuron_count': [],
            'firing_rate': [],
            'spike_rate': [],
            'learning_activity': [],
            'simulation_time': []
        }
        self.alerts = []
        
    def get_recent_alerts(self, minutes: int = 10) -> List:
        """Get recent alerts within specified time window."""
        cutoff_time = time.time() - (minutes * 60)
        return [alert for alert in self.alerts if alert['timestamp'] > cutoff_time]
```

---

## Usage Examples

### Example: Monitoring a Learning Simulation

```python
import pynlm
from performance_monitor import PerformanceMonitor
from debug_logger import DebugLogger
from system_health_monitor import SystemHealthMonitor

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllSystems()

# Initialize monitoring tools
perf_monitor = PerformanceMonitor(brain, agent, world)
debug_logger = DebugLogger(brain, agent, world)
health_monitor = SystemHealthMonitor(brain, agent, world)

# Start monitoring
perf_monitor.start_monitoring(interval=0.1)

print("Starting learning simulation with monitoring...")

# Run simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log simulation step
    debug_logger.log_simulation_step(step, additional_data={
        'action_type': action.getType(),
        'action_params': action.getParameters()
    })
    
    # Collect health metrics
    health_monitor.collect_health_metrics()
    
    if step % 100 == 0:
        print(f"Step {step}: Health status = {health_monitor.get_health_report()['system_status']}")

# Stop monitoring
perf_monitor.stop_monitoring()

# Get reports
performance_summary = perf_monitor.get_performance_summary()
health_report = health_monitor.get_health_report()
debug_analysis = debug_logger.analyze_simulation_patterns()

print("\nPerformance Summary:")
print(f"Total samples: {performance_summary['total_samples']}")
print(f"Average memory: {performance_summary['memory']['avg']:.1f} MB")
print(f"Average CPU: {performance_summary['cpu']['avg']:.1f}%")

print("\nHealth Report:")
print(f"System status: {health_report['system_status']}")
print(f"Active alerts: {len(health_report['alerts'])}")

print("\nDebug Analysis:")
print(f"Total simulation steps logged: {debug_analysis['total_steps']}")
print(f"Neuron growth: {len(debug_analysis['neuron_growth'])} data points")
```

### Example: Configuration Validation

```python
import pynlm
from configuration_validator import ConfigurationValidator

# Create validator
validator = ConfigurationValidator()

# Create configurations
config1 = pynlm.createDevelopmentConfig()
config2 = pynlm.createExplorationConfig()

# Validate configurations
result1 = validator.validate_config(config1)
result2 = validator.validate_config(config2)

print("Configuration Validation Results:")
print(f"Config1 valid: {result1['valid']}, score: {result1['score']}")
print(f"Config2 valid: {result2['valid']}, score: {result2['score']}")

if not result1['valid']:
    print("\nConfig1 Errors:")
    for error in result1['errors']:
        print(f"  - {error}")

if result1['warnings']:
    print("\nConfig1 Warnings:")
    for warning in result1['warnings']:
        print(f"  - {warning}")

if result1['recommendations']:
    print("\nConfig1 Recommendations:")
    for recommendation in result1['recommendations']:
        print(f"  - {recommendation}")
```

---

## Integration with Development Environment

### Jupyter Notebook Integration

For use in Jupyter notebooks, the monitoring tools can be easily integrated:

```python
# In a Jupyter notebook
import matplotlib.pyplot as plt
import pynlm
from performance_monitor import PerformanceMonitor

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllSystems()

# Initialize monitor
monitor = PerformanceMonitor(brain, agent, world)
monitor.start_monitoring(interval=0.1)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Stop monitoring
monitor.stop_monitoring()

# Display real-time statistics
stats = monitor.get_real_time_stats()
print(f"Current stats: {stats}")

# Generate plots
monitor.plot_performance('simulation_performance.png')
```

### Command Line Interface

The monitoring tools can be used via command line interfaces:

```bash
# Install dependencies
pip install psutil matplotlib

# Run a simulation with monitoring
python -c "
import pynlm
from performance_monitor import PerformanceMonitor

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllSystems()

monitor = PerformanceMonitor(brain, agent, world)
monitor.start_monitoring(interval=0.1)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

monitor.stop_monitoring()
summary = monitor.get_performance_summary()
print(f'Performance: {summary}')
"
```

---

## Best Practices

1. **Start Monitoring Early**: Begin monitoring from the first simulation step
2. **Use Appropriate Intervals**: Choose monitoring intervals based on simulation step duration
3. **Monitor Memory Usage**: Keep track of memory usage to prevent out-of-memory errors
4. **Balance Performance and Detail**: Use appropriate detail levels for different monitoring scenarios
5. **Integrate with Logging**: Combine monitoring with comprehensive logging
6. **Set Up Alerts**: Configure alerts for performance thresholds
7. **Regular Maintenance**: Clean up old monitoring data periodically
8. **Use Configuration Files**: Store monitoring configurations in configuration files

---

## Files Included

- `performance_monitor.py`: Performance monitoring tools
- `debug_logger.py`: Comprehensive debug logging
- `system_health_monitor.py`: System health monitoring
- `configuration_validator.py`: Configuration validation tools
- `simulation_profiler.py`: Simulation profiling tools

---

## Dependencies

These tools require additional Python packages:

```bash
pip install psutil matplotlib cProfile4PyPy
```

The monitoring tools can be used without these dependencies, though with reduced functionality.

---

This documentation provides comprehensive tools for performance monitoring, debugging, and system analysis of NLM simulations. These tools help ensure optimal performance, identify issues, and provide insights into system behavior.
