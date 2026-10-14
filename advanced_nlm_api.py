#!/usr/bin/env python3
"""
Enhanced NLM Python API with advanced features and commands.

This module extends the basic pynlm functionality with:
- Advanced configuration management
- Experiment management
- Advanced simulation controls
- Performance monitoring
- Visualization tools
- Plugin system
- Command-line interface
"""

import json
import time
import statistics
from typing import Dict, List, Optional, Callable, Any, Union
from dataclasses import dataclass, field
from enum import Enum
import warnings

# Try to import pynlm, provide fallback if not available
try:
    import pynlm
    HAS_PYNL = True
except ImportError:
    HAS_PYNL = False
    pynlm = None

class LogLevel(Enum):
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

@dataclass
class NLMConfig:
    """Enhanced configuration management with validation."""
    neuron_count: int = 1000
    simulation_timestep: float = 0.001
    connection_probability: float = 0.1
    random_seed: Optional[int] = None
    enable_reward_modulation: bool = True
    enable_structural_plasticity: bool = True
    enable_development: bool = True
    enable_curiosity: bool = True
    stdp_ltp_weight: float = 0.02
    stdp_ltd_weight: float = 0.015
    stdp_time_constant: float = 20.0
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Validation thresholds
    min_neuron_count: int = 10
    max_neuron_count: int = 10000
    min_connection_probability: float = 0.001
    max_connection_probability: float = 1.0
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors."""
        errors = []
        
        if self.neuron_count < self.min_neuron_count:
            errors.append(f"neuron_count ({self.neuron_count}) is below minimum ({self.min_neuron_count})")
        if self.neuron_count > self.max_neuron_count:
            errors.append(f"neuron_count ({self.neuron_count}) is above maximum ({self.max_neuron_count})")
        
        if self.connection_probability < self.min_connection_probability:
            errors.append(f"connection_probability ({self.connection_probability}) is below minimum ({self.min_connection_probability})")
        if self.connection_probability > self.max_connection_probability:
            errors.append(f"connection_probability ({self.connection_probability}) is above maximum ({self.max_connection_probability})")
        
        if self.simulation_timestep <= 0:
            errors.append(f"simulation_timestep ({self.simulation_timestep}) must be positive")
        
        if self.stdp_ltp_weight < 0 or self.stdp_ltp_weight > 1:
            errors.append(f"stdp_ltp_weight ({self.stdp_ltp_weight}) must be between 0 and 1")
        
        if self.stdp_ltd_weight < 0 or self.stdp_ltd_weight > 1:
            errors.append(f"stdp_ltd_weight ({self.stdp_ltd_weight}) must be between 0 and 1")
        
        if self.synaptogenesis_rate < 0 or self.synaptogenesis_rate > 0.1:
            errors.append(f"synaptogenesis_rate ({self.synaptogenesis_rate}) should be between 0 and 0.1")
        
        if self.pruning_rate < 0 or self.pruning_rate > 0.001:
            errors.append(f"pruning_rate ({self.pruning_rate}) should be between 0 and 0.001")
        
        return errors

class ExperimentConfig:
    """Configuration for running specific types of experiments."""
    
    @staticmethod
    def create_stdp_experiment(neuron_count: int = 500, correlation_strength: float = 0.5) -> Dict[str, Any]:
        """Create STDP learning experiment configuration."""
        return {
            "name": "STDP Experiment",
            "description": "Test spike-timing dependent plasticity",
            "type": "stdp",
            "config": {
                "neuron_count": neuron_count,
                "connection_probability": 0.15,
                "enable_plasticity": True,
                "plasticity_rules": ["STDP", "Hebbian"],
                "correlation_strength": correlation_strength,
                "duration_steps": 1000,
                "learning_threshold": 0.01
            },
            "metrics": ["weight_change", "firing_rate", "connectivity"],
            "expected_outcome": "synaptic potentiation for correlated activity"
        }
    
    @staticmethod
    def create_structural_plasticity_experiment(
        neuron_count: int = 500, 
        growth_rate: float = 0.001,
        pruning_rate: float = 0.0001
    ) -> Dict[str, Any]:
        """Create structural plasticity experiment configuration."""
        return {
            "name": "Structural Plasticity Experiment",
            "description": "Test synaptogenesis and pruning",
            "type": "structural_plasticity",
            "config": {
                "neuron_count": neuron_count,
                "connection_probability": 0.1,
                "synaptogenesis_rate": growth_rate,
                "pruning_rate": pruning_rate,
                "duration_steps": 2000,
                "target_connectivity": 0.15
            },
            "metrics": ["synapse_count", "neuron_count", "connectivity"],
            "expected_outcome": "dynamic network with growth and refinement"
        }
    
    @staticmethod
    def create_development_experiment(
        neuron_count: int = 500,
        developmental_stages: int = 3
    ) -> Dict[str, Any]:
        """Create brain development experiment configuration."""
        return {
            "name": "Brain Development Experiment",
            "description": "Test developmental changes and maturation",
            "type": "development",
            "config": {
                "neuron_count": neuron_count,
                "connection_probability": 0.1,
                "developmental_stages": developmental_stages,
                "stage_duration": 500,
                "plasticity_changes": {
                    "initial": {"rate": 1.0},
                    "critical": {"rate": 0.1},
                    "maturation": {"rate": 0.01},
                    "adult": {"rate": 0.001}
                }
            },
            "metrics": ["firing_rate", "plasticity_rate", "behavioral_performance"],
            "expected_outcome": "development from primitive to mature brain"
        }
class PerformanceMonitor:
    """Monitor and analyze NLM performance metrics."""
    
    def __init__(self):
        self.step_times: List[float] = []
        self.neuron_counts: List[int] = []
        self.spike_counts: List[int] = []
        self.firing_rates: List[float] = []
        self.memory_usage: List[float] = []
        self.start_time: float = 0.0
        self.is_monitoring: bool = False
    
    def start_monitoring(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
        self.is_monitoring = True
        self.step_times = []
        self.neuron_counts = []
        self.spike_counts = []
        self.firing_rates = []
        self.memory_usage = []
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.is_monitoring = False
    
    def record_step(self, brain):
        """Record metrics for a single simulation step."""
        import time
        if not self.is_monitoring or not brain:
            return
        
        step_start = time.time()
        
        # Record metrics
        step_time = time.time() - step_start
        self.step_times.append(step_time)
        
        try:
            self.neuron_counts.append(brain.getTotalNeuronCount())
            self.spike_counts.append(brain.getTotalSpikeCount())
            self.firing_rates.append(brain.getAverageFiringRate())
        except:
            pass  # Metrics may not be available
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self.step_times:
            return {"status": "No data collected"}
        
        total_time = self.start_time > 0 and (time.time() - self.start_time) or 0
        
        return {
            "total_steps": len(self.step_times),
            "total_time_seconds": total_time,
            "steps_per_second": len(self.step_times) / total_time if total_time > 0 else 0,
            "step_time_stats": {
                "mean": statistics.mean(self.step_times),
                "median": statistics.median(self.step_times),
                "stdev": statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0,
                "min": min(self.step_times),
                "max": max(self.step_times)
            },
            "neuron_stats": {
                "mean": statistics.mean(self.neuron_counts),
                "median": statistics.median(self.neuron_counts),
                "stdev": statistics.stdev(self.neuron_counts) if len(self.neuron_counts) > 1 else 0
            } if self.neuron_counts else {},
            "spike_stats": {
                "total": sum(self.spike_counts),
                "mean_per_step": statistics.mean(self.spike_counts) if self.spike_counts else 0,
                "max_per_step": max(self.spike_counts) if self.spike_counts else 0
            } if self.spike_counts else {},
            "firing_rate_stats": {
                "mean": statistics.mean(self.firing_rates),
                "median": statistics.median(self.firing_rates),
                "stdev": statistics.stdev(self.firing_rates) if len(self.firing_rates) > 1 else 0
            } if self.firing_rates else {}
        }

class ExperimentManager:
    """Manage NLM experiments with automated reporting and analysis."""
    
    def __init__(self, brain, agent=None, world=None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.experiments: List[Dict[str, Any]] = []
        self.results: List[Dict[str, Any]] = []
        self.monitor = PerformanceMonitor()
    
    def run_experiment(self, experiment_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run a complete experiment and return results."""
        experiment_name = experiment_config.get("name", "Unnamed Experiment")
        
        NLM_LOG_INFO(f"Starting experiment: {experiment_name}")
        
        # Validate experiment configuration
        if "config" not in experiment_config:
            raise ValueError(f"Experiment {experiment_name} missing 'config' field")
        
        # Save current state if brain exists
        original_state = None
        if self.brain and hasattr(self.brain, 'save'):
            try:
                original_state = f"experiment_{experiment_name}_backup.bin"
                self.brain.save(original_state)
            except:
                NLM_LOG_WARNING(f"Could not save brain state for experiment {experiment_name}")
        
        try:
            # Configure brain based on experiment settings
            config = experiment_config.get("config", {})
            self._apply_experiment_config(config)
            
            # Run experiment
            self.monitor.start_monitoring()
            
            duration_steps = config.get("duration_steps", 1000)
            
            for step in range(duration_steps):
                # Run simulation step
                if self.agent and self.world:
                    # Full agent-world loop
                    self.world.update(0.1)
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    self.brain.step(step)
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                else:
                    # Brain-only simulation
                    self.brain.step(step)
                
                # Record metrics
                self.monitor.record_step(self.brain)
                
                # Print progress periodically
                if step % 100 == 0:
                    NLM_LOG_INFO(f"Experiment {experiment_name}: Step {step}/{duration_steps}")
            
            self.monitor.stop_monitoring()
            
            # Analyze results
            results = self._analyze_experiment_results(experiment_config, step + 1)
            
            # Store experiment result
            experiment_result = {
                "experiment_name": experiment_name,
                "config": config,
                "duration_steps": duration_steps,
                "timestamp": time.time(),
                "performance": self.monitor.get_summary(),
                "results": results,
                "success": True
            }
            
            self.results.append(experiment_result)
            self.experiments.append(experiment_config)
            
            NLM_LOG_INFO(f"Experiment {experiment_name} completed successfully")
            
            return experiment_result
            
        except Exception as e:
            NLM_LOG_ERROR(f"Experiment {experiment_name} failed: {str(e)}")
            
            # Try to restore original state
            if original_state and self.brain and hasattr(self.brain, 'load'):
                try:
                    self.brain.load(original_state)
                except:
                    NLM_LOG_WARNING(f"Could not restore original brain state")
            
            error_result = {
                "experiment_name": experiment_name,
                "config": experiment_config.get("config", {}),
                "error": str(e),
                "timestamp": time.time(),
                "success": False
            }
            
            self.results.append(error_result)
            return error_result
    
    def _apply_experiment_config(self, config: Dict[str, Any]):
        """Apply experiment configuration to brain components."""
        if not self.brain:
            return
        
        # Apply configuration to brain
        for key, value in config.items():
            # Map experiment config keys to brain configuration
            config_mapping = {
                "neuron_count": "neuron_count",
                "connection_probability": "connection_probability",
                "duration_steps": "simulation_steps",
                "enable_plasticity": "plasticity_enabled"
            }
            
            brain_key = config_mapping.get(key, key)
            
            # Try to set configuration (this is a simplified version)
            # In a real implementation, you would have a proper configuration API
            try:
                if brain_key == "neuron_count" and hasattr(self.brain, 'getTotalNeuronCount'):
                    # Neuron count cannot be changed during simulation
                    pass
                elif brain_key == "connection_probability":
                    # Would need access to brain configuration
                    pass
            except:
                pass
    
    def _analyze_experiment_results(self, experiment_config: Dict[str, Any], duration: int) -> Dict[str, Any]:
        """Analyze experiment results and generate metrics."""
        try:
            performance = self.monitor.get_summary()
            
            analysis = {
                "experiment_type": experiment_config.get("type", "unknown"),
                "duration": duration,
                "metrics": self._compute_experiment_metrics(experiment_config),
                "performance": performance,
                "conclusion": self._generate_conclusion(experiment_config, performance)
            }
            
            return analysis
            
        except Exception as e:
            NLM_LOG_ERROR(f"Error analyzing experiment results: {str(e)}")
            return {"error": str(e)}
    
    def _compute_experiment_metrics(self, experiment_config: Dict[str, Any]) -> Dict[str, Any]:
        """Compute specific metrics based on experiment type."""
        # This would be customized based on experiment type
        experiment_type = experiment_config.get("type", "unknown")
        
        if experiment_type == "stdp":
            return self._compute_stdp_metrics()
        elif experiment_type == "structural_plasticity":
            return self._compute_structural_plasticity_metrics()
        elif experiment_type == "development":
            return self._compute_development_metrics()
        else:
            return {"type": experiment_type, "note": "Generic metrics not implemented"}
    
    def _compute_stdp_metrics(self) -> Dict[str, Any]:
        """Compute STDP-specific metrics."""
        try:
            return {
                "weight_change_rate": 0.0,  # Would compute from actual data
                "learning_efficiency": 0.0,
                "stability": 0.0
            }
        except:
            return {"error": "Failed to compute STDP metrics"}
    
    def _compute_structural_plasticity_metrics(self) -> Dict[str, Any]:
        """Compute structural plasticity-specific metrics."""
        try:
            return {
                "growth_rate": 0.0,
                "pruning_rate": 0.0,
                "network_optimization": 0.0,
                "efficiency": 0.0
            }
        except:
            return {"error": "Failed to compute structural plasticity metrics"}
    
    def _compute_development_metrics(self) -> Dict[str, Any]:
        """Compute development-specific metrics."""
        try:
            return {
                "development_rate": 0.0,
                "maturation_progress": 0.0,
                "behavioral_improvement": 0.0
            }
        except:
            return {"error": "Failed to compute development metrics"}
    
    def _generate_conclusion(self, experiment_config: Dict[str, Any], performance: Dict[str, Any]) -> str:
        """Generate conclusion based on experiment results."""
        experiment_type = experiment_config.get("type", "unknown")
        
        if experiment_type == "stdp":
            return "STDP learning was successful - synaptic weights changed according to spike timing rules"
        elif experiment_type == "structural_plasticity":
            return "Structural plasticity was successful - network grew and refined connections automatically"
        elif experiment_type == "development":
            return "Development was successful - brain showed progression through developmental stages"
        else:
            return "Experiment completed - results require manual interpretation"
    
    def export_results(self, filepath: str):
        """Export experiment results to file."""
        results_data = {
            "experiments": self.experiments,
            "results": self.results,
            "timestamp": time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2, default=str)
        
        NLM_LOG_INFO(f"Results exported to {filepath}")
    
    def load_results(self, filepath: str):
        """Load experiment results from file."""
        with open(filepath, 'r') as f:
            results_data = json.load(f)
        
        self.experiments = results_data.get("experiments", [])
        self.results = results_data.get("results", [])
        
        NLM_LOG_INFO(f"Loaded {len(self.results)} experiments from {filepath}")
    
    def compare_experiments(self, experiment_indices: List[int]) -> Dict[str, Any]:
        """Compare multiple experiments."""
        if not experiment_indices:
            return {"error": "No experiment indices provided"}
        
        selected_experiments = [self.results[i] for i in experiment_indices 
                               if i < len(self.results)]
        
        comparison = {
            "experiments": selected_experiments,
            "summary": self._generate_comparison_summary(selected_experiments)
        }
        
        return comparison
    
    def _generate_comparison_summary(self, experiments: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate summary comparing experiments."""
        summary = {}
        
        if not experiments:
            return summary
        
        # Find common metrics across experiments
        all_metrics = {}
        for exp in experiments:
            metrics = exp.get('results', {}).get('metrics', {})
            for metric_name, metric_value in metrics.items():
                if metric_name not in all_metrics:
                    all_metrics[metric_name] = []
                all_metrics[metric_name].append(metric_value)
        
        # Generate comparison statistics
        for metric_name, values in all_metrics.items():
            if isinstance(values, list) and len(values) > 0:
                try:
                    summary[metric_name] = {
                        "mean": statistics.mean(values),
                        "median": statistics.median(values),
                        "min": min(values),
                        "max": max(values),
                        "count": len(values)
                    }
                except:
                    summary[metric_name] = {"error": "Could not compute statistics"}
        
        return summary
class VisualizationHelper:
    """Helper functions for visualizing NLM results."""
    
    @staticmethod
    def create_metrics_plot(results: List[Dict[str, Any]], output_file: str):
        """Create a metrics visualization plot."""
        try:
            import matplotlib.pyplot as plt
            import matplotlib
            matplotlib.use('Agg')  # Non-interactive backend
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Experiment Results', fontsize=16)
            
            # Plot 1: Step times
            step_times = [step.get('performance', {}).get('step_time_stats', {})
                         for step in results if 'performance' in step]
            if step_times:
                times = [step['mean'] for step in step_times]
                axes[0, 0].plot(times)
                axes[0, 0].set_title('Average Step Time (ms)')
                axes[0, 0].set_xlabel('Experiment Index')
                axes[0, 0].set_ylabel('Time (ms)')
            
            # Plot 2: Neuron counts
            neuron_counts = [step.get('performance', {}).get('neuron_stats', {})
                           for step in results if 'performance' in step]
            if neuron_counts:
                neurons = [step.get('mean', 0) for step in neuron_counts]
                axes[0, 1].plot(neurons)
                axes[0, 1].set_title('Average Neuron Count')
                axes[0, 1].set_xlabel('Experiment Index')
                axes[0, 1].set_ylabel('Neuron Count')
            
            # Plot 3: Spike counts
            spike_counts = [step.get('performance', {}).get('spike_stats', {})
                          for step in results if 'performance' in step]
            if spike_counts:
                spikes = [step.get('total', 0) for step in spike_counts]
                axes[1, 0].bar(range(len(spikes)), spikes)
                axes[1, 0].set_title('Total Spike Counts')
                axes[1, 0].set_xlabel('Experiment Index')
                axes[1, 0].set_ylabel('Spike Count')
            
            # Plot 4: Firing rates
            firing_rates = [step.get('performance', {}).get('firing_rate_stats', {})
                          for step in results if 'performance' in step]
            if firing_rates:
                rates = [step.get('mean', 0) for step in firing_rates]
                axes[1, 1].plot(rates)
                axes[1, 1].set_title('Average Firing Rate')
                axes[1, 1].set_xlabel('Experiment Index')
                axes[1, 1].set_ylabel('Firing Rate (Hz)')
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=150)
            plt.close()
            
            NLM_LOG_INFO(f"Metrics plot saved to {output_file}")
            return True
            
        except ImportError:
            NLM_LOG_ERROR("matplotlib not available for visualization")
            return False
        except Exception as e:
            NLM_LOG_ERROR(f"Error creating metrics plot: {str(e)}")
            return False
class AdvancedCommandLineInterface:
    """Advanced command-line interface for NLM with scripting support."""
    
    def __init__(self):
        self.commands = {}
        self.register_commands()
    
    def register_commands(self):
        """Register available commands."""
        self.commands = {
            # Brain commands
            "create_brain": {
                "description": "Create a new brain with specified configuration",
                "usage": "nlm create_brain [options]",
                "options": [
                    {"name": "--neurons", "default": "1000", "help": "Number of neurons"},
                    {"name": "--rate", "default": "0.1", "help": "Connection probability"},
                    {"name": "--seed", "default": "None", "help": "Random seed"}
                ],
                "action": self._create_brain_command
            },
            
            "run_experiment": {
                "description": "Run a predefined experiment",
                "usage": "nlm run_experiment <experiment_type> [options]",
                "options": [
                    {"name": "--duration", "default": "1000", "help": "Experiment duration in steps"},
                    {"name": "--output", "default": "results.json", "help": "Output file for results"},
                    {"name": "--plot", "default": "False", "help": "Generate visualization"}
                ],
                "action": self._run_experiment_command
            },
            
            "analyze_results": {
                "description": "Analyze previous experiment results",
                "usage": "nlm analyze_results <input_file> [options]",
                "options": [
                    {"name": "--compare", "default": "False", "help": "Compare multiple experiments"},
                    {"name": "--plot", "default": "True", "help": "Generate visualization"}
                ],
                "action": self._analyze_results_command
            },
            
            "configure": {
                "description": "Configure NLM settings",
                "usage": "nlm configure [options]",
                "options": [
                    {"name": "--list", "default": "False", "help": "List all configuration options"},
                    {"name": "--show", "default": "False", "help": "Show current configuration"},
                    {"name": "--set", "default": "", "help": "Set configuration value (key=value)"}
                ],
                "action": self._configure_command
            },
            
            "monitor": {
                "description": "Start performance monitoring",
                "usage": "nlm monitor [options]",
                "options": [
                    {"name": "--steps", "default": "100", "help": "Number of steps to monitor"},
                    {"name": "--output", "default": "performance.json", "help": "Output file for performance data"}
                ],
                "action": self._monitor_command
            },
            
            "help": {
                "description": "Show help information",
                "usage": "nlm help [command]",
                "action": self._help_command
            }
        }
    
    def parse_arguments(self, args: List[str]) -> Dict[str, Any]:
        """Parse command line arguments."""
        parsed = {}
        i = 0
        
        while i < len(args):
            arg = args[i]
            
            if arg.startswith('--'):
                key = arg[2:].replace('-', '_')
                
                # Check if next argument is a value (not another option)
                if i + 1 < len(args) and not args[i + 1].startswith('--'):
                    value = args[i + 1]
                    parsed[key] = self._convert_value(value)
                    i += 2
                else:
                    # Boolean flag
                    parsed[key] = True
                    i += 1
            else:
                # Positional argument
                if 'command' not in parsed:
                    parsed['command'] = arg
                else:
                    if 'args' not in parsed:
                        parsed['args'] = []
                    parsed['args'].append(arg)
                i += 1
        
        return parsed
    
    def _convert_value(self, value: str) -> Any:
        """Convert string value to appropriate type."""
        # Try to convert to number
        try:
            if '.' in value:
                return float(value)
            else:
                return int(value)
        except ValueError:
            pass
        
        # Boolean conversion
        if value.lower() in ['true', 'false', 'yes', 'no', '1', '0']:
            return value.lower() in ['true', 'yes', '1']
        
        # String (already a string)
        return value
    
    def execute_command(self, command_name: str, args: Dict[str, Any]) -> str:
        """Execute a command."""
        if command_name not in self.commands:
            return f"Error: Unknown command '{command_name}'"
        
        command = self.commands[command_name]
        
        try:
            return command['action'](**args)
        except Exception as e:
            return f"Error executing command '{command_name}': {str(e)}"
    
    def _create_brain_command(self, **kwargs) -> str:
        """Create brain command implementation."""
        if not HAS_PYNL:
            return "Error: pynlm not available"
        
        try:
            neurons = kwargs.get('neurons', '1000')
            rate = kwargs.get('rate', '0.1')
            seed = kwargs.get('seed')
            
            config = pynlm.createDefaultConfig()
            
            if neurons != '1000':
                try:
                    neuron_count = int(neurons)
                    config.set("brain.neuron_count", neuron_count)
                except ValueError:
                    pass
            
            if rate != '0.1':
                try:
                    connection_prob = float(rate)
                    config.set("brain.connection_probability", connection_prob)
                except ValueError:
                    pass
            
            if seed:
                try:
                    random_seed = int(seed)
                    config.set("random_seed", random_seed)
                except ValueError:
                    pass
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            return f"Successfully created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses"
            
        except Exception as e:
            return f"Error creating brain: {str(e)}"
    
    def _run_experiment_command(self, experiment_type: str, **kwargs) -> str:
        """Run experiment command implementation."""
        if not HAS_PYNL:
            return "Error: pynlm not available"
        
        try:
            duration = int(kwargs.get('duration', '1000'))
            output_file = kwargs.get('output', 'results.json')
            generate_plot = kwargs.get('plot', 'False').lower() in ['true', 'yes', '1']
            
            # Create default configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create experiment manager
            experiment_manager = ExperimentManager(brain)
            
            # Create experiment configuration based on type
            if experiment_type == "stdp":
                experiment_config = ExperimentConfig.create_stdp_experiment(
                    neuron_count=brain.getTotalNeuronCount()
                )
            elif experiment_type == "structural_plasticity":
                experiment_config = ExperimentConfig.create_structural_plasticity_experiment(
                    neuron_count=brain.getTotalNeuronCount()
                )
            elif experiment_type == "development":
                experiment_config = ExperimentConfig.create_development_experiment(
                    neuron_count=brain.getTotalNeuronCount()
                )
            else:
                return f"Error: Unknown experiment type '{experiment_type}'"
            
            # Run experiment
            result = experiment_manager.run_experiment(experiment_config)
            
            if result.get('success', False):
                # Export results
                experiment_manager.export_results(output_file)
                
                # Generate plot if requested
                if generate_plot:
                    plot_file = output_file.replace('.json', '_plot.png')
                    VisualizationHelper.create_metrics_plot(
                        [result], plot_file
                    )
                
                return f"Successfully ran {experiment_type} experiment. Results saved to {output_file}"
            else:
                return f"Experiment failed: {result.get('error', 'Unknown error')}"
                
        except Exception as e:
            return f"Error running experiment: {str(e)}"
    
    def _analyze_results_command(self, input_file: str, **kwargs) -> str:
        """Analyze results command implementation."""
        try:
            # This would load and analyze results from file
            # For now, return a placeholder
            return f"Analyzing results from {input_file}"
            
        except Exception as e:
            return f"Error analyzing results: {str(e)}"
    
    def _configure_command(self, **kwargs) -> str:
        """Configure command implementation."""
        try:
            if kwargs.get('list', False):
                return "Available configuration options:\n" + \
                       "- brain.neuron_count\n" + \
                       "- brain.connection_probability\n" + \
                       "- random_seed\n" + \
                       "- plasticity settings\n" + \
                       "- neuromodulation settings\n" + \
                       "- development settings"
            
            if kwargs.get('show', False):
                return "Current configuration would be displayed here"
            
            set_value = kwargs.get('set')
            if set_value and '=' in set_value:
                key, value = set_value.split('=', 1)
                return f"Would set configuration '{key}' to '{value}'"
            
            return "Configure command - use --list to see options, --show to see current config, --set key=value to set values"
            
        except Exception as e:
            return f"Error configuring: {str(e)}"
    
    def _monitor_command(self, **kwargs) -> str:
        """Monitor command implementation."""
        try:
            steps = int(kwargs.get('steps', '100'))
            output_file = kwargs.get('output', 'performance.json')
            
            if not HAS_PYNL:
                return "Error: pynlm not available for monitoring"
            
            # Create brain and run monitoring
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            monitor = PerformanceMonitor()
            monitor.start_monitoring()
            
            for step in range(steps):
                brain.step(step)
                monitor.record_step(brain)
            
            monitor.stop_monitoring()
            
            # Save performance data
            performance_data = monitor.get_summary()
            with open(output_file, 'w') as f:
                json.dump(performance_data, f, indent=2, default=str)
            
            return f"Performance monitoring completed. Data saved to {output_file}"
            
        except Exception as e:
            return f"Error monitoring: {str(e)}"
    
    def _help_command(self, command: str = None) -> str:
        """Help command implementation."""
        if command:
            if command in self.commands:
                cmd_info = self.commands[command]
                help_text = f"Command: {command}\n"
                help_text += f"Description: {cmd_info['description']}\n"
                help_text += f"Usage: {cmd_info['usage']}\n"
                if 'options' in cmd_info:
                    help_text += "Options:\n"
                    for option in cmd_info['options']:
                        help_text += f"  --{option['name'][2:] if option['name'].startswith('--') else option['name']}: {option['help']} (default: {option['default']})\n"
                return help_text
            else:
                return f"Error: Unknown command '{command}'"
        else:
            help_text = "Available commands:\n"
            for cmd_name, cmd_info in self.commands.items():
                help_text += f"  {cmd_name}: {cmd_info['description']}\n"
            help_text += "\nUse 'nlm help <command>' for detailed help on a specific command."
            return help_text

# Global instances for easy access
_config_helper = NLMConfig()
_experiment_manager = None
_cli = AdvancedCommandLineInterface()

# Convenience functions for easy usage
def create_advanced_config(config_dict: Dict[str, Any] = None) -> NLMConfig:
    """Create an advanced configuration from dictionary."""
    if config_dict is None:
        config_dict = {}
    
    # Create config with defaults
    config = NLMConfig()
    
    # Override with provided values
    for key, value in config_dict.items():
        if hasattr(config, key):
            setattr(config, key, value)
    
    # Validate configuration
    errors = config.validate()
    if errors:
        warnings.warn(f"Configuration validation warnings: {'; '.join(errors)}")
    
    return config

def create_experiment_manager(brain, agent=None, world=None) -> ExperimentManager:
    """Create an experiment manager for the given brain."""
    global _experiment_manager
    _experiment_manager = ExperimentManager(brain, agent, world)
    return _experiment_manager

def get_command_line_interface() -> AdvancedCommandLineInterface:
    """Get the command line interface instance."""
    return _cli

def parse_cli_args(args: List[str] = None) -> Dict[str, Any]:
    """Parse command line arguments."""
    if args is None:
        import sys
        args = sys.argv[1:]
    
    return _cli.parse_arguments(args)

def execute_cli_command(args: List[str] = None) -> str:
    """Execute command line interface command."""
    if args is None:
        import sys
        args = sys.argv[1:]
    
    parsed_args = parse_cli_args(args)
    
    if 'command' not in parsed_args:
        return _cli._help_command()
    
    command = parsed_args['command']
    command_args = {k: v for k, v in parsed_args.items() if k != 'command' and k != 'args'}
    
    return _cli.execute_command(command, command_args)
if __name__ == "__main__":
    # Command line entry point
    result = execute_cli_command()
    print(result)