"""
Scripting and automation tools for NLM Python bindings.
"""

from typing import Dict, Any, Optional, List, Callable
from dataclasses import dataclass, field
import json
import os
import re
import tempfile
import subprocess
import threading
from abc import ABC, abstractmethod

class ScriptError(Exception):
    """Exception raised when script execution fails."""
    pass

@dataclass
class ScriptContext:
    """Context for script execution."""
    variables: Dict[str, Any] = field(default_factory=dict)
    brain_state: Any = None
    config: Dict[str, Any] = field(default_factory=dict)
    working_dir: str = "."
    environment: Dict[str, str] = field(default_factory=dict)

class ScriptError(Exception):
    """Exception raised when script execution fails."""
    pass

class NLMScript(ABC):
    """Base class for NLM scripts."""
    
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
        self.context = ScriptContext()
    
    @abstractmethod
    def execute(self, *args, **kwargs) -> Any:
        """Execute the script."""
        pass
    
    def set_context(self, **kwargs):
        """Set context variables."""
        for key, value in kwargs.items():
            self.context.variables[key] = value
    
    def get_variable(self, name: str, default: Any = None) -> Any:
        """Get a variable from context."""
        return self.context.variables.get(name, default)
    
    def run_script_file(self, file_path: str):
        """Run a script from a file."""
        with open(file_path, 'r') as f:
            script_content = f.read()
        
        return self.execute(script_content)

class ExperimentScript(NLMScript):
    """Script for running experiments."""
    
    def execute(self, *args, **kwargs) -> Dict[str, Any]:
        """Run experiment from script."""
        script_text = args[0] if args else ""
        
        # Parse script commands
        commands = self._parse_script(script_text)
        
        results = {}
        for command in commands:
            result = self._execute_command(command)
            results[command['name']] = result
        
        return results
    
    def _parse_script(self, script_text: str) -> List[Dict[str, Any]]:
        """Parse script text into commands."""
        commands = []
        
        # Simple parsing - look for lines with command pattern
        lines = script_text.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            # Parse command: command_name [parameters]
            command_match = re.match(r'(\w+)\s*(.*)', line)
            if command_match:
                name = command_match.group(1)
                params_str = command_match.group(2).strip()
                
                # Parse parameters
                params = {}
                if params_str:
                    param_parts = params_str.split()
                    for i in range(0, len(param_parts), 2):
                        if i + 1 < len(param_parts):
                            key = param_parts[i].strip('=')
                            value = param_parts[i + 1]
                            # Try to convert to number
                            try:
                                if '.' in value:
                                    value = float(value)
                                else:
                                    value = int(value)
                            except ValueError:
                                pass
                            params[key] = value
                
                commands.append({
                    'name': name,
                    'parameters': params
                })
        
        return commands
    
    def _execute_command(self, command: Dict[str, Any]) -> Any:
        """Execute a single command."""
        name = command['name']
        params = command['parameters']
        
        if name == 'create_brain':
            config = self.get_variable('config', {})
            neurons = params.get('neurons', 100)
            config['neuron_count'] = neurons
            return {'status': 'created', 'neurons': neurons}
        
        elif name == 'run_steps':
            steps = params.get('steps', 100)
            timestep = params.get('timestep', 0.001)
            
            if not hasattr(self.context, 'brain') or self.context.brain is None:
                return {'status': 'error', 'message': 'No brain created'}
            
            for step in range(steps):
                self.context.brain.step(step, step * timestep)
            
            return {
                'status': 'completed',
                'steps': steps,
                'final_spikes': self.context.brain.getTotalSpikeCount()
            }
        
        elif name == 'set_variable':
            key = params.get('key')
            value = params.get('value')
            if key:
                self.context.variables[key] = value
            return {'status': 'variable_set', 'key': key, 'value': value}
        
        else:
            return {'status': 'unknown_command', 'command': name}

class AnalysisScript(NLMScript):
    """Script for data analysis."""
    
    def execute(self, *args, **kwargs) -> Dict[str, Any]:
        """Run analysis from script."""
        script_text = args[0] if args else ""
        
        # Parse analysis commands
        commands = self._parse_analysis_script(script_text)
        
        results = {}
        for command in commands:
            result = self._execute_analysis_command(command)
            results[command['name']] = result
        
        return results
    
    def _parse_analysis_script(self, script_text: str) -> List[Dict[str, Any]]:
        """Parse analysis script."""
        commands = []
        
        lines = script_text.split('\n')
        for line in lines:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            # Simple analysis command parsing
            if 'plot' in line.lower():
                commands.append({
                    'name': 'plot',
                    'type': 'visualization',
                    'parameters': {'command': line}
                })
            
            elif 'analyze' in line.lower():
                commands.append({
                    'name': 'analyze',
                    'type': 'statistics',
                    'parameters': {'command': line}
                })
            
            elif 'export' in line.lower():
                commands.append({
                    'name': 'export',
                    'type': 'data_export',
                    'parameters': {'command': line}
                })
        
        return commands
    
    def _execute_analysis_command(self, command: Dict[str, Any]) -> Any:
        """Execute an analysis command."""
        cmd_type = command['type']
        cmd_text = command['parameters']['command']
        
        if cmd_type == 'visualization':
            return self._create_visualization(cmd_text)
        
        elif cmd_type == 'statistics':
            return self._perform_statistics(cmd_text)
        
        elif cmd_type == 'data_export':
            return self._export_data(cmd_text)
        
        else:
            return {'status': 'unknown', 'command': command}
    
    def _create_visualization(self, cmd_text: str) -> Dict[str, Any]:
        """Create visualization."""
        # This would integrate with plotting tools
        return {
            'status': 'visualization_created',
            'command': cmd_text
        }
    
    def _perform_statistics(self, cmd_text: str) -> Dict[str, Any]:
        """Perform statistical analysis."""
        # This would integrate with analysis tools
        return {
            'status': 'statistics_computed',
            'command': cmd_text
        }
    
    def _export_data(self, cmd_text: str) -> Dict[str, Any]:
        """Export data."""
        # This would integrate with data export tools
        return {
            'status': 'data_exported',
            'command': cmd_text
        }

class AutomationTools:
    """General automation tools."""
    
    @staticmethod
    def run_cli_command(command: str, working_dir: str = ".") -> Dict[str, Any]:
        """Run a CLI command."""
        try:
            result = subprocess.run(
                command.split(),
                capture_output=True,
                text=True,
                cwd=working_dir
            )
            
            return {
                'status': 'completed' if result.returncode == 0 else 'failed',
                'stdout': result.stdout,
                'stderr': result.stderr,
                'return_code': result.returncode
            }
        except Exception as e:
            return {
                'status': 'error',
                'error': str(e)
            }
    
    @staticmethod
    def create_experiment_script(name: str, config: Dict[str, Any]) -> str:
        """Create a complete experiment script."""
        script_template = f'''# NLM Experiment Script: {name}
# Generated automatically

# Configuration
neuron_count = {config.get('neuron_count', 100)}
steps = {config.get('steps', 1000)}
timestep = {config.get('timestep', 0.001)}

# Run experiment
run_steps steps={steps} timestep={{timestep}}
'''
        
        return script_template
    
    @staticmethod
    def create_analysis_script(analysis_commands: List[str]) -> str:
        """Create an analysis script."""
        script_template = '''# NLM Analysis Script

'''
        
        for command in analysis_commands:
            script_template += f'# {command}
'
        
        script_template += '''
# Run analysis
for command in analysis_commands:
    analyze {{command}}
'''
        
        return script_template
    
    @staticmethod
    def benchmark_performance(brain_callback, configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Benchmark performance across multiple configurations."""
        results = {}
        
        for i, config in enumerate(configs):
            print(f"Benchmarking configuration {i+1}/{len(configs)}")
            
            import time
            start_time = time.time()
            
            brain = brain_callback(config)
            brain.initialize()
            
            steps = config.get('steps', 1000)
            for step in range(steps):
                brain.step(step, step * config.get('timestep', 0.001))
            
            end_time = time.time()
            
            results[f"config_{i+1}"] = {
                'duration': end_time - start_time,
                'steps_per_second': steps / (end_time - start_time),
                'neurons': config.get('neuron_count', 100),
                'total_spikes': brain.getTotalSpikeCount()
            }
        
        return results