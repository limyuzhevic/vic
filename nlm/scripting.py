#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Scripting Module

This module provides Python scripting engine for simulation control.
It includes experiment DSL, neural analysis API, world builder API, development tool API,
optimization API, and comprehensive scripting capabilities for automated control of NLM simulations.

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

import os
import sys
import numpy as np
import pandas as pd
import yaml
import json
from typing import Dict, List, Any, Optional, Union, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings
import time
import logging
from abc import ABC, abstractmethod
import re
import ast
import tokenize
from io import StringIO
import types

# Import NLM core modules
import nlm_py
from nlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio,
    NeuralRegion, NeuralPopulation, Neuron, Synapse
)

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Version
__version__ = "0.1.0"

class ScriptingError(Exception):
    """Base exception for scripting errors."""
    pass
    
class ScriptingSyntaxError(ScriptingError):
    """Exception for scripting syntax errors."""
    pass
    
class ScriptingRuntimeError(ScriptingError):
    """Exception for scripting runtime errors."""
    pass
    
class ScriptContext:
    """Context for script execution."""
    
    def __init__(self, brain: Brain = None, world: SimpleWorld = None,
                 config: Config = None, variables: Dict[str, Any] = None):
        self.brain = brain
        self.world = world
        self.config = config or Config()
        self.variables = variables or {}
        self.function_registry = {}
        self.event_handlers = {}
        self.simulation_state = 'idle'
        
    def set_variable(self, name: str, value: Any) -> None:
        """Set variable in script context."""
        self.variables[name] = value
        
    def get_variable(self, name: str, default: Any = None) -> Any:
        """Get variable from script context."""
        return self.variables.get(name, default)
        
    def define_function(self, name: str, func: Callable) -> None:
        """Define function in script context."""
        self.function_registry[name] = func
        
    def get_function(self, name: str) -> Callable:
        """Get function from script context."""
        if name in self.function_registry:
            return self.function_registry[name]
        else:
            raise ScriptingError(f"Function '{name}' not defined")
        
    def register_event_handler(self, event_name: str, handler: Callable) -> None:
        """Register event handler."""
        self.event_handlers[event_name] = handler
        
    def trigger_event(self, event_name: str, data: Dict[str, Any] = None) -> None:
        """Trigger event."""
        if event_name in self.event_handlers:
            try:
                self.event_handlers[event_name](data or {})
            except Exception as e:
                raise ScriptingRuntimeError(f"Event handler failed: {e}")
class ScriptingEngine:
    """Python scripting engine for NLM simulation control."""
    
    def __init__(self, context: ScriptContext = None):
        self.context = context or ScriptContext()
        self.execution_history = []
        self.error_handler = ErrorHandler()
        self.scheduler = ScriptScheduler()
        self.debug_mode = False
        
    def execute_script(self, script: str) -> Dict[str, Any]:
        """Execute Python script."""
        execution_record = {
            'timestamp': time.time(),
            'script': script,
            'start_time': time.time(),
            'end_time': None,
            'success': False,
            'output': None,
            'errors': [],
            'execution_time': 0.0
        }
        
        try:
            # Set up execution environment
            self._setup_execution_environment()
            
            # Execute script
            self.context.simulation_state = 'running'
            
            # Compile and execute script
            compiled_script = compile(script, '<script>', 'exec')
            
            # Capture output
            output_buffer = []
            old_stdout = sys.stdout
            old_stderr = sys.stderr
            
            sys.stdout = StringIO()
            sys.stderr = StringIO()
            
            try:
                exec(compiled_script, self.context.__dict__)
                
                # Get output
                output = sys.stdout.getvalue()
                errors = sys.stderr.getvalue()
                
                execution_record['output'] = output
                execution_record['errors'] = errors.split('\n') if errors else []
                
                if not errors:
                    execution_record['success'] = True
                    
            finally:
                sys.stdout = old_stdout
                sys.stderr = old_stderr
                
        except SyntaxError as e:
            error_msg = f"Syntax error: {e}"
            execution_record['errors'].append(error_msg)
            raise ScriptingSyntaxError(error_msg)
            
        except Exception as e:
            error_msg = f"Runtime error: {e}"
            execution_record['errors'].append(error_msg)
            raise ScriptingRuntimeError(error_msg)
            
        finally:
            execution_record['end_time'] = time.time()
            execution_record['execution_time'] = (
                execution_record['end_time'] - execution_record['start_time']
            )
            self.context.simulation_state = 'completed'
            self.execution_history.append(execution_record)
            
        return execution_record
        
    def _setup_execution_environment(self) -> None:
        """Set up execution environment."""
        # Add context objects to execution namespace
        execution_namespace = {
            'context': self.context,
            'brain': self.context.brain,
            'world': self.context.world,
            'config': self.context.config,
            'np': np,
            'pd': pd,
            'plt': plt,
            'time': time,
            'random': random,
            'logging': logging,
            'stats': stats,
            'scipy': scipy,
        }
        
        # Add functions to execution namespace
        for name, func in self.context.function_registry.items():
            execution_namespace[name] = func
            
        # Execute setup code
        setup_code = '''
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import random
import time
import logging
from scipy import stats, signal, optimize

def create_brain(config=None):
    if config is None:
        config = context.config
    return nlm_py.createBrain(config)

def create_world():
    return nlm_py.createSimpleWorld()

def log(message, level="info"):
    if level == "info":
        logging.info(message)
    elif level == "warning":
        logging.warning(message)
    elif level == "error":
        logging.error(message)
    elif level == "debug":
        logging.debug(message)
'''
        
        try:
            exec(setup_code, execution_namespace)
        except Exception as e:
            logger.warning(f"Setup code execution failed: {e}")
            
    def load_script(self, filepath: str) -> str:
        """Load script from file."""
        with open(filepath, 'r') as f:
            return f.read()
            
    def save_script(self, script: str, filepath: str) -> None:
        """Save script to file."""
        with open(filepath, 'w') as f:
            f.write(script)
            
    def list_scripts(self, directory: str) -> List[str]:
        """List scripts in directory."""
        import os
        scripts = []
        for filename in os.listdir(directory):
            if filename.endswith('.py') or filename.endswith('.nlm'):
                scripts.append(os.path.join(directory, filename))
        return scripts
        
    def execute_script_file(self, filepath: str) -> Dict[str, Any]:
        """Execute script from file."""
        script = self.load_script(filepath)
        return self.execute_script(script)
        
    def execute_script_block(self, script_block: str) -> Dict[str, Any]:
        """Execute script block."""
        return self.execute_script(script_block)
        
    def execute_script_from_variable(self, variable_name: str) -> Dict[str, Any]:
        """Execute script stored in variable."""
        script = self.context.get_variable(variable_name)
        if script is None:
            raise ScriptingError(f"Variable '{variable_name}' not found")
        
        return self.execute_script(script)
class ScriptScheduler:
    """Scheduler for script execution."""
    
    def __init__(self):
        self.scheduled_scripts = []
        self.running_scripts = []
        self.completed_scripts = []
        
    def schedule_script(self, script: str, schedule_time: float = None,
                       interval: float = None, repeat: int = 1,
                       name: str = None, parameters: Dict[str, Any] = None,
                       context: ScriptContext = None) -> str:
        """Schedule script for execution."""
        script_id = f"script_{len(self.scheduled_scripts)}"
        
        scheduled_script = {
            'id': script_id,
            'script': script,
            'schedule_time': schedule_time,
            'interval': interval,
            'repeat': repeat,
            'name': name,
            'parameters': parameters or {},
            'context': context,
            'created_time': time.time(),
            'next_execution': time.time() if schedule_time is None else schedule_time,
            'executions': 0,
            'status': 'scheduled'
        }
        
        self.scheduled_scripts.append(scheduled_script)
        logger.info(f"Scheduled script '{name or script_id}'")
        
        return script_id
        
    def cancel_script(self, script_id: str) -> bool:
        """Cancel scheduled script."""
        for scheduled_script in self.scheduled_scripts:
            if scheduled_script['id'] == script_id:
                scheduled_script['status'] = 'cancelled'
                logger.info(f"Cancelled script '{scheduled_script['name'] or script_id}'")
                return True
                
        return False
        
    def get_script_status(self, script_id: str) -> Dict[str, Any]:
        """Get script status."""
        for scheduled_script in self.scheduled_scripts:
            if scheduled_script['id'] == script_id:
                return scheduled_script.copy()
                
        for scheduled_script in self.running_scripts:
            if scheduled_script['id'] == script_id:
                return scheduled_script.copy()
                
        for scheduled_script in self.completed_scripts:
            if scheduled_script['id'] == script_id:
                return scheduled_script.copy()
                
        return {'error': 'Script not found'}
        
    def run_scheduled_scripts(self) -> List[Dict[str, Any]]:
        """Run all scheduled scripts."""
        results = []
        current_time = time.time()
        
        for scheduled_script in self.scheduled_scripts[:]:
            if scheduled_script['status'] == 'scheduled' and current_time >= scheduled_script['next_execution']:
                # Start script execution
                result = self._execute_scheduled_script(scheduled_script)
                results.append(result)
                
        return results
        
    def _execute_scheduled_script(self, scheduled_script: Dict[str, Any]) -> Dict[str, Any]:
        """Execute scheduled script."""
        script_id = scheduled_script['id']
        
        # Move to running state
        scheduled_script['status'] = 'running'
        self.running_scripts.append(scheduled_script)
        
        try:
            # Execute script
            script_engine = ScriptingEngine(scheduled_script['context'])
            result = script_engine.execute_script(scheduled_script['script'])
            
            # Update execution count
            scheduled_script['executions'] += 1
            
            # Schedule next execution if repeating
            if scheduled_script['repeat'] > 1 and scheduled_script['executions'] < scheduled_script['repeat']:
                scheduled_script['next_execution'] += scheduled_script['interval'] or 3600
            else:
                scheduled_script['status'] = 'completed'
                self.completed_scripts.append(scheduled_script)
                
        except Exception as e:
            logger.error(f"Script execution failed: {e}")
            scheduled_script['status'] = 'failed'
            scheduled_script['error'] = str(e)
            
        finally:
            # Remove from running scripts
            self.running_scripts = [s for s in self.running_scripts if s['id'] != script_id]
            
        return result
class ErrorHandler:
    """Handle errors in script execution."""
    
    def __init__(self):
        self.error_types = {}
        self.error_handlers = {}
        self.error_log = []
        
    def register_error_type(self, error_type: str, handler: Callable) -> None:
        """Register error handler."""
        self.error_handlers[error_type] = handler
        self.error_types[error_type] = {
            'handler': handler,
            'count': 0,
            'last_occurrence': None,
            'severity': 'medium'
        }
        
    def handle_error(self, error: Exception, context: Dict[str, Any] = None) -> Dict[str, Any]:
        """Handle error."""
        error_info = {
            'timestamp': time.time(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'handled': False,
            'recovery_action': None,
            'recovery_successful': False
        }
        
        # Try to find registered handler
        error_type = type(error).__name__
        if error_type in self.error_handlers:
            try:
                recovery_result = self.error_handlers[error_type](error, context)
                error_info['handled'] = True
                error_info['recovery_action'] = 'handler_called'
                error_info['recovery_successful'] = recovery_result
                error_info['handler_result'] = recovery_result
            except Exception as handler_error:
                error_info['error_handler_failed'] = str(handler_error)
                
        # Log error
        self.error_log.append(error_info)
        
        # Keep only last 1000 errors
        if len(self.error_log) > 1000:
            self.error_log = self.error_log[-1000:]
            
        return error_info
        
    def get_error_statistics(self) -> Dict[str, Any]:
        """Get error statistics."""
        if not self.error_log:
            return {'total_errors': 0, 'error_types': {}}
            
        statistics = {
            'total_errors': len(self.error_log),
            'error_types': {},
            'recent_errors': self.error_log[-10:] if len(self.error_log) > 10 else self.error_log,
        }
        
        # Count errors by type
        for error_info in self.error_log:
            error_type = error_info['error_type']
            if error_type not in statistics['error_types']:
                statistics['error_types'][error_type] = 0
                statistics['error_types'][error_type]['count'] = 0
                statistics['error_types'][error_type]['handlers'] = []
                statistics['error_types'][error_type]['handled_count'] = 0
                
            statistics['error_types'][error_type]['count'] += 1
            if error_info['handled']:
                statistics['error_types'][error_type]['handled_count'] += 1
                
        # Add handler information
        for error_type, error_info in self.error_types.items():
            if error_type in statistics['error_types']:
                statistics['error_types'][error_type]['handler_registered'] = True
                statistics['error_types'][error_type]['severity'] = error_info['severity']
                
        return statistics
class ExperimentDSL:
    """Domain-specific language for defining NLM experiments."""
    
    @staticmethod
    def parse_experiment_spec(spec: Dict[str, Any]) -> Dict[str, Any]:
        """Parse experiment specification."""
        # Extract experiment parameters
        experiment_config = {
            'name': spec.get('name', 'Unnamed Experiment'),
            'type': spec.get('type', 'simulation'),
            'duration': spec.get('duration', 1000),
            'parameters': spec.get('parameters', {}),
            'agents': spec.get('agents', []),
            'recordings': spec.get('recordings', []),
            'analysis': spec.get('analysis', []),
        }
        return experiment_config
    
    @staticmethod
    def validate_experiment_spec(spec: Dict[str, Any]) -> List[str]:
        """Validate experiment specification."""
        errors = []
        
        # Check required fields
        required_fields = ['name', 'type']
        for field in required_fields:
            if field not in spec:
                errors.append(f"Missing required field: {field}")
                
        # Validate experiment type
        valid_types = ['simulation', 'parameter_sweep', 'monte_carlo', 
                      'developmental', 'learning', 'ablation', 'optimization']
        if 'type' in spec and spec['type'] not in valid_types:
            errors.append(f"Invalid experiment type: {spec['type']}")
            
        # Validate duration
        if 'duration' in spec and spec['duration'] <= 0:
            errors.append("Duration must be positive")
            
        return errors
    
    @staticmethod
    def generate_experiment_code(spec: Dict[str, Any]) -> str:
        """Generate Python code for experiment execution."""
        code_template = f'''
# Generated experiment code
experiment_name = "{spec['name']}"
experiment_type = "{spec['type']}"

# Experiment configuration
config = {{
    "max_steps": {spec.get('duration', 1000)},
    "neuron_count": {spec.get('parameters', {}).get('neuron_count', 1000)},
    "enable_visualization": {str(spec.get('parameters', {}).get('enable_visualization', True)).lower()},
}}

# Create brain and world
brain = createBrain(config)
world = createSimpleWorld()
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Run experiment
brain.initialize()
for step in range(config["max_steps"]):
    brain.step(step)
    
    # Record data if specified
    if "recordings" in {spec.get('recordings', [])}:
        # Implementation would depend on recording specifications
        pass
        
    # Apply agent behavior if specified
    if "agents" in {spec.get('agents', [])}:
        # Implementation would depend on agent specifications
        pass

print(f"Experiment {{experiment_name}} completed successfully")
'''
        return code_template
class NeuralAnalysisAPI:
    """Neural analysis API for analyzing simulation data."""
    
    @staticmethod
    def analyze_spike_trains(spike_times: List[float], sampling_rate: float = 1000.0) -> Dict[str, Any]:
        """Analyze spike train data."""
        if not spike_times:
            return {"error": "No spike times provided"}
            
        analysis = {
            "total_spikes": len(spike_times),
            "firing_rate": len(spike_times) / (spike_times[-1] - spike_times[0]) if len(spike_times) > 1 else 0.0,
            "mean_inter_spike_interval": np.mean(np.diff(spike_times)) if len(spike_times) > 1 else 0.0,
            "std_inter_spike_interval": np.std(np.diff(spike_times)) if len(spike_times) > 1 else 0.0,
            "cv_of_isi": 0.0,
            "first_spike_time": spike_times[0] if spike_times else 0.0,
            "last_spike_time": spike_times[-1] if spike_times else 0.0,
            "spike_count_histogram": {},
            "periodicity": 0.0,
            "bifurcation_index": 0.0,
        }
        
        if len(spike_times) > 1:
            analysis["cv_of_isi"] = analysis["std_inter_spike_interval"] / analysis["mean_inter_spike_interval"]
            
        # Create spike count histogram
        hist, bins = np.histogram(spike_times, bins=50)
        analysis["spike_count_histogram"] = {
            "counts": hist.tolist(),
            "bin_edges": bins.tolist()
        }
        
        return analysis
    
    @staticmethod
    def analyze_neural_connectivity(brain: Brain) -> Dict[str, Any]:
        """Analyze neural connectivity patterns."""
        connectivity_analysis = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "connection_density": 0.0,
            "network_clustering": 0.0,
            "global_efficiency": 0.0,
            "modularity": 0.0,
            "hub_nodes": [],
            "community_structure": {},
            "excitatory_inhibitory_ratio": 0.0,
        }
        
        # Calculate connection density
        if connectivity_analysis["total_neurons"] > 0:
            connectivity_analysis["connection_density"] = (
                connectivity_analysis["total_synapses"] / 
                (connectivity_analysis["total_neurons"] ** 2)
            )
        
        return connectivity_analysis
    
    @staticmethod
    def analyze_population_dynamics(brain: Brain, population_id: int) -> Dict[str, Any]:
        """Analyze population dynamics."""
        population_analysis = {
            "population_id": population_id,
            "neuron_count": 0,
            "active_neurons": 0,
            "firing_rate": 0.0,
            "mean_activity": 0.0,
            "activity_variance": 0.0,
            "activity_skewness": 0.0,
            "activity_kurtosis": 0.0,
            "temporal_patterns": [],
            "spike_train_correlation": 0.0,
            "oscillation_frequency": 0.0,
            "power_spectrum": {},
            "reconstruction_error": 0.0,
        }
        
        # Implementation would require actual population access
        return population_analysis
    
    @staticmethod
    def detect_temporal_patterns(spike_times: List[float], 
                                min_freq: float = 1.0, 
                                max_freq: float = 50.0,
                                fs: float = 1000.0) -> Dict[str, Any]:
        """Detect rhythmic patterns and oscillations in spike trains."""
        if len(spike_times) < 2:
            return {"error": "Insufficient spike data for pattern detection"}
            
        temporal_analysis = {
            "dominant_frequency": 0.0,
            "frequency_power": {},
            "rhythmicity": 0.0,
            "oscillation_index": 0.0,
            "theta_power": 0.0,
            "alpha_power": 0.0,
            "beta_power": 0.0,
            "gamma_power": 0.0,
            "pattern_confidence": 0.0,
        }
        
        # Implementation would use Fourier analysis
        return temporal_analysis
class WorldBuilderAPI:
    """World builder API for creating custom environment configurations."""
    
    @staticmethod
    def create_custom_world(config: Dict[str, Any]) -> SimpleWorld:
        """Create a custom world configuration."""
        world = SimpleWorld()
        
        # Apply custom configuration
        if "width" in config:
            world.configure(width=config["width"], height=config.get("height", 100.0),
                          visionWidth=config.get("vision_width", 20),
                          visionHeight=config.get("vision_height", 20))
                          
        if "objects" in config:
            for obj_config in config["objects"]:
                # Implementation would depend on world object types
                pass
                
        return world
    
    @staticmethod
    def create_dynamic_world(config: Dict[str, Any]) -> SimpleWorld:
        """Create a dynamic world that evolves over time."""
        world = SimpleWorld()
        world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)
        
        # Implementation would include world evolution logic
        return world
    
    @staticmethod
    def create_multi_environment_switcher(configs: List[Dict[str, Any]]) -> Callable:
        """Create a world that can switch between different configurations."""
        current_index = [0]
        
        def switcher(step: int, brain: Brain) -> SimpleWorld:
            # Switch world configuration based on step
            index = (current_index[0] + step) % len(configs)
            config = configs[index]
            return WorldBuilderAPI.create_custom_world(config)
            
        return switcher
    
    @staticmethod
    def reproduce_world(world: SimpleWorld, brain: Brain) -> SimpleWorld:
        """Create an exact copy of a simulated world."""
        # Implementation would create world state copy
        return world
class DevelopmentToolAPI:
    """Development tool API for brain development research."""
    
    @staticmethod
    def manipulate_developmental_stage(brain: Brain, stage: str) -> None:
        """Manipulate brain development stage."""
        # Implementation would modify brain development state
        pass
    
    @staticmethod
    def edit_plasticity_rules(brain: Brain, rules: Dict[str, Any]) -> None:
        """Edit plasticity rules during simulation."""
        # Implementation would modify plasticity parameters
        pass
    
    @staticmethod
    def tune_neuromodulation(brain: Brain, config: Dict[str, Any]) -> None:
        """Adjust neuromodulator effects in real-time."""
        # Implementation would modify neuromodulation parameters
        pass
    
    @staticmethod
    def modify_architecture(brain: Brain, operations: List[Dict[str, Any]]) -> None:
        """Add/remove brain regions dynamically."""
        # Implementation would modify brain architecture
        pass
class OptimizationAPI:
    """Optimization API for parameter optimization and tuning."""
    
    @staticmethod
    def gradient_based_optimization(objective_function: Callable, 
                                   initial_params: Dict[str, Any],
                                   constraints: Dict[str, Any] = None) -> Dict[str, Any]:
        """Use gradient-based methods for parameter tuning."""
        # Implementation would use scipy.optimize
        pass
    
    @staticmethod
    def genetic_algorithm(objective_function: Callable,
                        population_size: int = 100,
                        generations: int = 50,
                        mutation_rate: float = 0.1,
                        crossover_rate: float = 0.8) -> Dict[str, Any]:
        """Use evolutionary algorithms to optimize brain parameters."""
        # Implementation would use genetic algorithm
        pass
    
    @staticmethod
    def parameter_sweep(objective_function: Callable,
                       parameters: Dict[str, List[Any]]) -> Dict[str, Any]:
        """Systematically vary parameters for hyperparameter optimization."""
        # Implementation would perform parameter sweep
        pass
    
    @staticmethod
    def bayesian_inference(data: List[Any], prior: Dict[str, Any] = None) -> Dict[str, Any]:
        """Use Bayesian methods for uncertainty quantification."""
        # Implementation would use Bayesian inference
        pass