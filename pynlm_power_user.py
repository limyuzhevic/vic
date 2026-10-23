#!/usr/bin/env python3
"""
NLM Power User Tools
=====================

Advanced tools and utilities for power users of the NLM (Neural Learning Machine) framework.
Provides enhanced functionality, debugging tools, and custom simulation capabilities.
"""

import numpy as np
import matplotlib.pyplot as plt
import json
import time
import os
from typing import Dict, List, Any, Optional, Callable
from datetime import datetime

from nlm import (
    Config, Brain, SimpleWorld, AgentBrain, SensoryPercept,
    create_default_config, create_brain, create_simple_world, create_agent_brain
)

# Global configuration for power user tools
POWER_USER_CONFIG = {
    'enable_detailed_logging': True,
    'enable_performance_monitoring': True,
    'enable_visualization': True,
    'custom_simulation_hooks': {},
    'debug_mode': False
}


class PerformanceMonitor:
    """
    High-performance monitoring system for NLM simulations.
    
    Tracks simulation performance metrics, provides real-time analysis,
    and helps identify bottlenecks in neural simulations.
    """
    
    def __init__(self):
        self.metrics_history = []
        self.start_time = None
        self.current_step = 0
        self.performance_data = {
            'step_times': [],
            'neuron_activity': [],
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_time': 0.0
        }
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self.start_time = time.time()
        self.current_step = 0
        self.metrics_history = []
        self.performance_data = {
            'step_times': [],
            'neuron_activity': [],
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_time': 0.0
        }
        return self
    
    def record_step(self, step: int, brain: Brain, world: SimpleWorld = None):
        """
        Record performance metrics for a simulation step.
        
        Args:
            step: Current simulation step
            brain: Brain instance
            world: Optional world instance for additional metrics
        """
        step_start = time.time()
        
        # Basic brain metrics
        step_metrics = {
            'step': step,
            'timestamp': time.time(),
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'active_synapses': brain.getTotalSynapseCount(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        # World metrics if available
        if world:
            step_metrics.update({
                'world_time': world.getSimulationTime(),
                'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
                'agent_energy': world.getAgentBody().energy,
                'agent_health': world.getAgentBody().health
            })
        
        self.metrics_history.append(step_metrics)
        self.current_step = step
        
        # Calculate step duration
        step_duration = time.time() - step_start
        self.performance_data['step_times'].append(step_duration)
        
        return step_metrics
    
    def stop_monitoring(self):
        """Stop performance monitoring and return summary."""
        if self.start_time is None:
            return self.get_summary()
        
        self.performance_data['simulation_time'] = time.time() - self.start_time
        
        summary = self.get_summary()
        self.start_time = None
        return summary
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self.metrics_history:
            return {'error': 'No data collected'}
        
        step_times = self.performance_data['step_times']
        
        return {
            'total_steps': self.current_step,
            'total_simulation_time': self.performance_data['simulation_time'],
            'steps_per_second': self.current_step / self.performance_data['simulation_time'] if self.performance_data['simulation_time'] > 0 else 0,
            'avg_step_time': np.mean(step_times) if step_times else 0,
            'min_step_time': np.min(step_times) if step_times else 0,
            'max_step_time': np.max(step_times) if step_times else 0,
            'step_time_stddev': np.std(step_times) if step_times else 0,
            'final_neuron_count': self.metrics_history[-1]['neuron_count'],
            'final_spike_count': self.metrics_history[-1]['total_spikes'],
            'final_firing_rate': self.metrics_history[-1]['average_firing_rate'],
            'average_neural_activity': np.mean([m['firing_neurons'] / m['neuron_count'] for m in self.metrics_history]) if self.metrics_history else 0
        }
    
    def analyze_bottlenecks(self) -> List[str]:n"""
        Analyze simulation data to identify performance bottlenecks.
        
        Returns:
            List of bottleneck descriptions
        """
        bottlenecks = []
        
        if not self.metrics_history:
            return bottlenecks
        
        # Analyze step time variability
        step_times = self.performance_data['step_times']
        if len(step_times) > 10:
            cv = np.std(step_times) / np.mean(step_times)  # Coefficient of variation
            if cv > 0.5:  # High variability indicates potential bottlenecks
                bottlenecks.append(f"High step time variability (CV: {cv:.2f}) - potential inconsistent performance")
        
        # Analyze firing rate trends
        firing_rates = [m['average_firing_rate'] for m in self.metrics_history]
        if len(firing_rates) > 20:
            recent_rates = firing_rates[-20:]
            if np.mean(recent_rates) > 100:  # High firing rate
                bottlenecks.append("High firing rates detected - may indicate neural saturation or instability")
        
        # Check for increasing step times (cascading failure)
        if len(step_times) > 30:
            recent_times = step_times[-30:]
            if np.mean(recent_times[-10:]) > np.mean(recent_times[:-10]) * 1.5:
                bottlenecks.append("Step times are increasing - potential cascading failure")
        
        return bottlenecks
    
    def export_data(self, filepath: str):
        """Export performance data to file."""
        data = {
            'metadata': {
                'export_time': datetime.now().isoformat(),
                'total_steps': self.current_step,
                'simulation_time': self.performance_data['simulation_time']
            },
            'metrics': self.metrics_history,
            'performance_data': self.performance_data
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        return filepath


class AdvancedBrainBuilder:
    """
    Advanced brain builder with sophisticated configuration options.
    
    Provides fine-grained control over brain architecture, connectivity patterns,
    and initial conditions for research and custom simulations.
    """
    
    def __init__(self):
        self.config = create_default_config()
        self.neural_architectures = {}
        self.connectivity_patterns = {}
    
    def add_region(self, name: str, neuron_type: str, count: int,
                   position: Optional[tuple] = None) -> 'AdvancedBrainBuilder':
        """
        Add a neural region with specific properties.
        
        Args:
            name: Region name (e.g., 'cortex', 'hippocampus')
            neuron_type: Primary neuron type ('excitatory', 'inhibitory', 'sensory', etc.)
            count: Number of neurons in this region
            position: Optional 3D position for spatial arrangement
            
        Returns:
            Self for method chaining
        """
        self.neural_architectures[name] = {
            'neuron_type': neuron_type,
            'count': count,
            'position': position or (0, 0, 0),
            'connection_probability': 0.1,
            'plasticity_enabled': True
        }
        return self
    
    def set_connectivity(self, source: str, target: str, probability: float,
                         weight_mean: float = 1.0, weight_std: float = 0.1) -> 'AdvancedBrainBuilder':
        """
        Set connectivity between brain regions.
        
        Args:
            source: Source region name
            target: Target region name
            probability: Connection probability (0.0 to 1.0)
            weight_mean: Mean synaptic weight
            weight_std: Standard deviation of synaptic weight
            
        Returns:
            Self for method chaining
        """
        if source not in self.connectivity_patterns:
            self.connectivity_patterns[source] = {}
        
        self.connectivity_patterns[source][target] = {
            'probability': probability,
            'weight_mean': weight_mean,
            'weight_std': weight_std,
            'plasticity': True
        }
        return self
    
    def set_plasticity_rules(self, rule_type: str, parameters: Dict[str, float]) -> 'AdvancedBrainBuilder':
        """
        Set plasticity rules for the brain.
        
        Args:
            rule_type: Type of plasticity rule ('stdp', 'hebbian', 'structural')
            parameters: Rule-specific parameters
            
        Returns:
            Self for method chaining
        """
        if 'plasticity' not in self.config:
            self.config.set('plasticity', {})
        
        self.config.getOr('plasticity', {}).set(rule_type, parameters)
        return self
    
    def set_neural_dynamics(self, v_thresh: float = -55.0, v_rest: float = -70.0,
                           tau_mem: float = 20.0, tau_ref: float = 2.0) -> 'AdvancedBrainBuilder':
        """
        Set neural dynamics parameters.
        
        Args:
            v_thresh: Threshold potential (mV)
            v_rest: Resting potential (mV)
            tau_mem: Membrane time constant (ms)
            tau_ref: Refractory period (ms)
            
        Returns:
            Self for method chaining
        """
        self.config.set("brain.v_thresh", v_thresh)
        self.config.set("brain.v_rest", v_rest)
        self.config.set("brain.tau_mem", tau_mem)
        self.config.set("brain.tau_ref", tau_ref)
        return self
    
    def set_development_parameters(self, stages: Dict[str, Dict]) -> 'AdvancedBrainBuilder':
        """
        Set developmental stages and parameters.
        
        Args:
            stages: Dictionary mapping stage names to parameters
                    (e.g., {'initial': {'plasticity': 1.0}, 'adult': {'plasticity': 0.1}})
            
        Returns:
            Self for method chaining
        """
        if 'development' not in self.config:
            self.config.set('development', {})
        
        self.config.getOr('development', {}).set('stages', stages)
        return self
    
    def build(self) -> Brain:
        """
        Build the advanced brain with all configurations applied.
        
        Returns:
            Configured brain instance
        """
        # Apply neural architecture
        total_neurons = sum(arch['count'] for arch in self.neural_architectures.values())
        self.config.set("brain.neuron_count", total_neurons)
        
        # Apply connectivity patterns
        for source, targets in self.connectivity_patterns.items():
            for target, params in targets.items():
                # In a real implementation, these would configure actual connectivity
                pass
        
        # Create and initialize brain
        brain = create_brain(self.config)
        brain.initialize()
        return brain


class SimulationDebugger:
    """
    Advanced debugging tools for NLM simulations.
    
    Provides real-time debugging capabilities, state inspection,
    and error detection for complex simulations.
    """
    
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.debug_log = []
        self.watch_variables = {}
        self.break_conditions = {}
    
    def add_watch(self, name: str, get_func: Callable, condition: Optional[Callable] = None):
        """
        Add a variable to monitor during simulation.
        
        Args:
            name: Variable name for logging
            get_func: Function that returns the variable value
            condition: Optional condition function that returns True if simulation should stop
        """
        self.watch_variables[name] = {
            'get_func': get_func,
            'condition': condition,
            'history': [],
            'violations': []
        }
    
    def add_break_condition(self, name: str, condition: Callable):
        """
        Add a break condition that stops simulation when met.
        
        Args:
            name: Break condition name
            condition: Function that returns True when simulation should stop
        """
        self.break_conditions[name] = {
            'condition': condition,
            'triggered': False
        }
    
    def run_with_debugging(self, num_steps: int, timestep: float = 0.1,
                          log_interval: int = 10) -> Dict[str, Any]:
        """
        Run simulation with debugging enabled.
        
        Args:
            num_steps: Number of simulation steps
            timestep: Time step duration
            log_interval: How often to log debug information
            
        Returns:
            Debug information and simulation results
        """
        simulation_results = {
            'steps_completed': 0,
            'debug_log': [],
            'watch_variable_history': {},
            'break_conditions': {},
            'errors': [],
            'warnings': []
        }
        
        for step in range(num_steps):
            # Check break conditions
            for name, cond_data in self.break_conditions.items():
                if not cond_data['triggered']:
                    if cond_data['condition'](step, self.brain, self.world):
                        cond_data['triggered'] = True
                        simulation_results['debug_log'].append(f"BREAK CONDITION TRIGGERED: {name}")
                        simulation_results['errors'].append(f"Simulation stopped due to condition: {name}")
                        simulation_results['steps_completed'] = step
                        return simulation_results
            
            # Record watch variables
            for name, watch_data in self.watch_variables.items():
                value = watch_data['get_func'](step, self.brain, self.world)
                watch_data['history'].append(value)
                
                if watch_data['condition'] and watch_data['condition'](value):
                    watch_data['violations'].append({
                        'step': step,
                        'value': value,
                        'condition': str(watch_data['condition'])
                    })
            
            # Run simulation step
            if step % log_interval == 0:
                # Log debug information
                debug_entry = {
                    'step': step,
                    'timestamp': time.time(),
                    'neuron_count': self.brain.getTotalNeuronCount(),
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'average_firing_rate': self.brain.getAverageFiringRate()
                }
                
                if self.world:
                    debug_entry.update({
                        'world_time': self.world.getSimulationTime(),
                        'agent_position': (self.world.getAgentBody().x, self.world.getAgentBody().y)
                    })
                
                simulation_results['debug_log'].append(debug_entry)
            
            # Execute simulation step
            try:
                self.brain.step(step, timestep)
                
                if self.world:
                    self.world.update(timestep)
                    
            except Exception as e:
                simulation_results['errors'].append(f"Error at step {step}: {e}")
                simulation_results['debug_log'].append(f"ERROR at step {step}: {e}")
                break
        
        # Compile watch variable history
        for name, watch_data in self.watch_variables.items():
            simulation_results['watch_variable_history'][name] = {
                'history': watch_data['history'],
                'violations': watch_data['violations']
            }
        
        # Compile break condition status
        for name, cond_data in self.break_conditions.items():
            simulation_results['break_conditions'][name] = {
                'triggered': cond_data['triggered']
            }
        
        simulation_results['steps_completed'] = num_steps
        return simulation_results
    
    def analyze_debug_log(self, debug_log: List[Dict]) -> Dict[str, Any]:
        """
        Analyze debug log for patterns and anomalies.
        
        Args:
            debug_log: Debug log data
            
        Returns:
            Analysis results
        """
        if not debug_log:
            return {'error': 'Empty debug log'}
        
        analysis = {
            'total_steps_analyzed': len(debug_log),
            'neuron_count_range': {
                'min': min(entry.get('neuron_count', 0) for entry in debug_log),
                'max': max(entry.get('neuron_count', 0) for entry in debug_log)
            },
            'firing_rate_analysis': {
                'average': np.mean([entry.get('average_firing_rate', 0) for entry in debug_log]),
                'min': min(entry.get('average_firing_rate', 0) for entry in debug_log),
                'max': max(entry.get('average_firing_rate', 0) for entry in debug_log)
            },
            'potential_issues': []
        }
        
        # Detect potential issues
        neuron_counts = [entry.get('neuron_count', 0) for entry in debug_log]
        if len(neuron_counts) > 10:
            neuron_trend = np.polyfit(range(len(neuron_counts)), neuron_counts, 1)[0]
            if abs(neuron_trend) > 0.1 * np.mean(neuron_counts):
                analysis['potential_issues'].append(f"Neuron count trend: {neuron_trend:.2f} per step")
        
        firing_rates = [entry.get('average_firing_rate', 0) for entry in debug_log]
        high_firing_episodes = sum(1 for rate in firing_rates if rate > 100)
        if high_firing_episodes > len(firing_rates) * 0.1:
            analysis['potential_issues'].append(f"High firing rate in {high_firing_episodes}/{len(firing_rates)} steps")
        
        return analysis


class CustomSimulationEnvironment:
    """
    Custom simulation environment with user-defined dynamics.
    
    Allows users to create custom world environments with specific
    physical laws, object behaviors, and interaction rules.
    """
    
    def __init__(self, width: int = 20, height: int = 20):
        self.width = width
        self.height = height
        self.objects = []
        self.agent = None
        self.physics_engine = {}
        self.custom_rules = []
        self.state_history = []
    
    def add_object(self, obj_type: str, x: float, y: float, properties: Dict):
        """
        Add a custom object to the environment.
        
        Args:
            obj_type: Type of object ('resource', 'hazard', 'agent', 'static')
            x: X position
            y: Y position
            properties: Object properties (value, radius, behavior, etc.)
        """
        obj = {
            'type': obj_type,
            'x': x,
            'y': y,
            'properties': properties,
            'active': True,
            'history': []
        }
        
        self.objects.append(obj)
        
        # Store initial state
        self.state_history.append({
            'step': len(self.state_history),
            'objects': [{'type': o['type'], 'x': o['x'], 'y': o['y'], 'properties': o['properties']} for o in self.objects]
        })
    
    def set_agent(self, agent):
        """Set the agent in the environment."""
        self.agent = agent
    
    def add_physics_rule(self, rule_name: str, rule_func: Callable):
        """
        Add a custom physics rule.
        
        Args:
            rule_name: Name of the physics rule
            rule_func: Function that implements the rule
        """
        self.physics_engine[rule_name] = rule_func
        self.custom_rules.append(rule_name)
    
    def update(self, timestep: float):
        """
        Update the environment state with custom physics rules.
        
        Args:
            timestep: Time step duration
        """
        # Apply custom physics rules
        for rule_name, rule_func in self.physics_engine.items():
            try:
                rule_func(self, timestep)
            except Exception as e:
                print(f"Physics rule {rule_name} error: {e}")
        
        # Record state
        self.state_history.append({
            'step': len(self.state_history),
            'objects': [{'type': o['type'], 'x': o['x'], 'y': o['y'], 'properties': o['properties']} for o in self.objects],
            'timestamp': time.time()
        })
        
        # Apply agent actions if present
        if self.agent:
            self._process_agent_actions(timestep)
    
    def _process_agent_actions(self, timestep: float):
        """Process agent actions and update environment."""
        # This would be implemented based on the agent interface
        pass
    
    def get_state(self) -> Dict:
        """Get current environment state."""
        return {
            'width': self.width,
            'height': self.height,
            'objects': self.objects,
            'agent_present': self.agent is not None,
            'custom_rules': list(self.custom_rules),
            'total_objects': len(self.objects)
        }
    
    def reset(self):
        """Reset environment to initial state."""
        self.state_history = self.state_history[:1] if self.state_history else []
        # Reset objects to initial positions
        for obj in self.objects:
            obj['active'] = True
            obj['x'] = obj['properties'].get('initial_x', obj['x'])
            obj['y'] = obj['properties'].get('initial_y', obj['y'])
    
    def export_history(self, filepath: str):
        """Export environment state history to file."""
        data = {
            'metadata': {
                'width': self.width,
                'height': self.height,
                'total_objects': len(self.objects),
                'custom_rules': list(self.custom_rules),
                'export_time': datetime.now().isoformat()
            },
            'history': self.state_history
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        return filepath


class ExperimentDesigner:
    """
    Design and manage scientific experiments with NLM.
    
    Provides tools for designing controlled experiments, managing
    experimental parameters, and analyzing experimental results.
    """
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.parameters = {}
        self.conditions = {}
        self.trials = []
        self.results = {}
        self.design_doc = {
            'name': experiment_name,
            'description': '',
            'parameters': {},
            'conditions': {},
            'trials': [],
            'created_at': datetime.now().isoformat()
        }
    
    def add_parameter(self, name: str, values: List, description: str = ""):
        """
        Add an experimental parameter.
        
        Args:
            name: Parameter name
            values: List of possible values
            description: Parameter description
        """
        self.parameters[name] = {
            'values': values,
            'description': description,
            'type': 'categorical' if isinstance(values[0], str) else 'numeric'
        }
        self.design_doc['parameters'][name] = {
            'values': values,
            'description': description
        }
    
    def add_condition(self, name: str, condition_func: Callable, description: str = ""):
        """
        Add an experimental condition.
        
        Args:
            name: Condition name
            condition_func: Function that implements the condition
            description: Condition description
        """
        self.conditions[name] = {
            'func': condition_func,
            'description': description
        }
        self.design_doc['conditions'][name] = {
            'description': description
        }
    
    def add_trial(self, trial_name: str, parameter_values: Dict, condition: str):
        """
        Add an experimental trial.
        
        Args:
            trial_name: Trial identifier
            parameter_values: Values for all parameters
            condition: Condition to apply
        """
        trial = {
            'name': trial_name,
            'parameters': parameter_values,
            'condition': condition,
            'status': 'pending',
            'results': None
        }
        self.trials.append(trial)
        self.design_doc['trials'].append(trial.copy())
    
    def run_experiment(self) -> Dict[str, Any]:
        """
        Run the designed experiment.
        
        Returns:
            Experiment results
        """
        results = {
            'experiment_name': self.experiment_name,
            'total_trials': len(self.trials),
            'completed_trials': 0,
            'trial_results': {},
            'statistics': {},
            'start_time': datetime.now().isoformat()
        }
        
        for trial in self.trials:
            try:
                # Apply parameter values
                config = create_default_config()
                for param_name, value in trial['parameters'].items():
                    if param_name in self.parameters:
                        if self.parameters[param_name]['type'] == 'categorical':
                            config.set(f"param.{param_name}", value)
                        else:
                            config.set(f"param.{param_name}", float(value))
                
                # Apply condition
                if trial['condition'] in self.conditions:
                    condition_func = self.conditions[trial['condition']]['func']
                    config = condition_func(config, trial['parameters'])
                
                # Run simulation
                brain = create_brain(config)
                brain.initialize()
                
                # Run simulation steps
                for step in range(100):  # Default 100 steps
                    brain.step(step)
                
                # Store results
                trial_results = {
                    'brain_stats': {
                        'total_neurons': brain.getTotalNeuronCount(),
                        'total_synapses': brain.getTotalSynapseCount(),
                        'total_spikes': brain.getTotalSpikeCount(),
                        'average_firing_rate': brain.getAverageFiringRate()
                    },
                    'success': True,
                    'timestamp': datetime.now().isoformat()
                }
                
                trial['results'] = trial_results
                trial['status'] = 'completed'
                results['trial_results'][trial_name] = trial_results
                results['completed_trials'] += 1
                
            except Exception as e:
                trial['results'] = {'error': str(e), 'success': False}
                trial['status'] = 'failed'
                results['trial_results'][trial_name] = trial['results']
                results['completed_trials'] += 1
        
        # Calculate statistics
        successful_trials = [t for t in self.trials if t['status'] == 'completed']
        if successful_trials:
            all_spikes = [t['results']['brain_stats']['total_spikes'] for t in successful_trials]
            all_rates = [t['results']['brain_stats']['average_firing_rate'] for t in successful_trials]
            
            results['statistics'] = {
                'successful_trials': len(successful_trials),
                'failed_trials': len(successful_trials) - len(successful_trials),
                'total_spikes': {
                    'mean': np.mean(all_spikes) if all_spikes else 0,
                    'std': np.std(all_spikes) if all_spikes else 0,
                    'min': np.min(all_spikes) if all_spikes else 0,
                    'max': np.max(all_spikes) if all_spikes else 0
                },
                'firing_rates': {
                    'mean': np.mean(all_rates) if all_rates else 0,
                    'std': np.std(all_rates) if all_rates else 0,
                    'min': np.min(all_rates) if all_rates else 0,
                    'max': np.max(all_rates) if all_rates else 0
                }
            }
        
        results['end_time'] = datetime.now().isoformat()
        return results
    
    def analyze_results(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """
        Analyze experiment results.
        
        Args:
            results: Experiment results dictionary
            
        Returns:
            Analysis results
        """
        analysis = {
            'experiment_name': self.experiment_name,
            'analysis_timestamp': datetime.now().isoformat(),
            'trial_count': results['total_trials'],
            'success_rate': results['completed_trials'] / results['total_trials'] if results['total_trials'] > 0 else 0,
            'key_findings': [],
            'statistical_significance': {},
            'recommendations': []
        }
        
        if 'statistics' in results:
            stats = results['statistics']
            
            # Analyze spike variability
            spike_stats = stats['total_spikes']
            if spike_stats['std'] / spike_stats['mean'] > 0.5 if spike_stats['mean'] > 0 else False:
                analysis['key_findings'].append("High variability in spike counts across trials")
            
            # Analyze firing rate patterns
            rate_stats = stats['firing_rates']
            if rate_stats['mean'] > 50:
                analysis['key_findings'].append("High average firing rates observed")
            
            # Check for outliers
            if len(results['trial_results']) > 3:
                spike_values = [t['brain_stats']['total_spikes'] for t in results['trial_results'].values() if 'brain_stats' in t]
                if spike_values:
                    mean_spikes = np.mean(spike_values)
                    std_spikes = np.std(spike_values)
                    outliers = [x for x in spike_values if abs(x - mean_spikes) > 2 * std_spikes]
                    if outliers:
                        analysis['key_findings'].append(f"Found {len(outliers)} outlier trials with unusual spike counts")
        
        return analysis


# Power user convenience functions
def create_advanced_simulation(config_overrides: Optional[Dict] = None) -> Dict[str, Any]:
    """
    Create an advanced simulation with power user features.
    
    Args:
        config_overrides: Optional configuration overrides
        
    Returns:
        Dictionary with simulation components and monitoring tools
    """
    # Create brain with advanced configuration
    brain_builder = AdvancedBrainBuilder()
    brain_builder.set_neural_dynamics(v_thresh=-50.0, v_rest=-70.0, tau_mem=25.0)
    brain_builder.set_plasticity_rules('stdp', {'learning_rate': 0.002, 'tau': 30.0})
    
    if config_overrides:
        for key, value in config_overrides.items():
            if hasattr(brain_builder, f"set_{key}"):
                getattr(brain_builder, f"set_{key}")(value)
    
    brain = brain_builder.build()
    
    # Create monitoring system
    monitor = PerformanceMonitor()
    
    # Create custom world
    world = CustomSimulationEnvironment(width=30, height=30)
    
    # Add some objects
    world.add_object('resource', 10, 10, {'value': 10.0, 'radius': 1.0})
    world.add_object('hazard', 20, 20, {'value': -5.0, 'radius': 1.5})
    world.add_object('static', 5, 15, {'value': 0.0, 'radius': 2.0})
    
    # Create agent
    agent = create_agent_brain(brain)
    world.set_agent(agent)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'monitor': monitor,
        'environment': world,
        'type': 'advanced_power_user_simulation'
    }


def run_power_user_analysis(brain: Brain, world: Optional[SimpleWorld] = None,
                          analysis_type: str = 'comprehensive') -> Dict[str, Any]:
    """
    Run advanced analysis on brain and world simulation data.
    
    Args:
        brain: Brain instance to analyze
        world: Optional world instance
        analysis_type: Type of analysis ('comprehensive', 'performance', 'neural', 'behavioral')
        
    Returns:
        Analysis results
    """
    analysis = {
        'analysis_type': analysis_type,
        'timestamp': datetime.now().isoformat(),
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
        }
    }
    
    if world:
        analysis['world_stats'] = {
            'simulation_time': world.getSimulationTime(),
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health
        }
    
    if analysis_type == 'comprehensive':
        # Add detailed analysis
        analysis['detailed_analysis'] = {
            'neural_dynamics': {
                'firing_rate_range': (brain.getAverageFiringRate() * 0.5, brain.getAverageFiringRate() * 1.5),
                'stability_indicator': 'stable' if brain.getAverageFiringRate() < 100 else 'unstable',
                'learning_potential': min(brain.getAverageFiringRate() / 50.0, 1.0)
            },
            'performance_metrics': {
                'computational_efficiency': brain.getTotalSynapseCount() / brain.getTotalSpikeCount() if brain.getTotalSpikeCount() > 0 else 0,
                'memory_efficiency': brain.getTotalNeuronCount() / brain.getTotalSynapseCount() if brain.getTotalSynapseCount() > 0 else 0
            },
            'behavioral_indicators': {
                'exploration_drive': min(brain.getAverageFiringRate() / 200.0, 1.0),
                'adaptability': abs(brain.getAverageFiringRate() - 50.0) / 50.0
            }
        }
    
    elif analysis_type == 'performance':
        # Performance-focused analysis
        analysis['performance_analysis'] = {
            'scalability': 'linear' if brain.getTotalSynapseCount() < 10000 else 'sublinear',
            'bottleneck_probability': 0.3 if brain.getTotalSpikeCount() > 1000 else 0.1,
            'resource_utilization': min(brain.getTotalSynapseCount() / 10000.0, 1.0)
        }
    
    elif analysis_type == 'neural':
        # Neural dynamics focused analysis
        analysis['neural_analysis'] = {
            'network_topology': 'small_world' if brain.getTotalSynapseCount() < brain.getTotalNeuronCount() * 2 else 'scale_free',
            'connection_density': brain.getTotalSynapseCount() / (brain.getTotalNeuronCount() * (brain.getTotalNeuronCount() - 1) / 2),
            'sync_capacity': min(brain.getAverageFiringRate() / 100.0, 1.0)
        }
    
    elif analysis_type == 'behavioral':
        # Behavioral focused analysis
        analysis['behavioral_analysis'] = {
            'action_capacity': min(brain.getTotalSynapseCount() / 100.0, 10),
            'learning_rate_estimate': min(brain.getAverageFiringRate() / 500.0, 1.0),
            'decision_consistency': 0.8 if brain.getAverageFiringRate() > 10 else 0.5
        }
    
    return analysis


# Global instance for power user tools
power_user_tools = {
    'PerformanceMonitor': PerformanceMonitor,
    'AdvancedBrainBuilder': AdvancedBrainBuilder,
    'SimulationDebugger': SimulationDebugger,
    'CustomSimulationEnvironment': CustomSimulationEnvironment,
    'ExperimentDesigner': ExperimentDesigner
}


def get_power_user_tools() -> Dict[str, Any]:
    """Get all power user tools."""
    return power_user_tools.copy()


def configure_power_user_settings(settings: Dict[str, Any]):
    """
    Configure global power user settings.
    
    Args:
        settings: Settings dictionary
    """
    global POWER_USER_CONFIG
    POWER_USER_CONFIG.update(settings)


def get_power_user_config() -> Dict[str, Any]:
    """Get current power user configuration."""
    return POWER_USER_CONFIG.copy()