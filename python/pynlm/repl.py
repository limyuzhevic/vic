pynlm/repl.py
Name: REPL module
Summary: Live REPL (Read-Eval-Print Loop) for interactive exploration

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType, ScriptingMode
import readline
import sys
import traceback
import time
from typing import Dict, List, Any, Optional, Callable
from enum import Enum
import threading
import queue


class REPLMode(Enum):
    """REPL mode enumeration"""
    NORMAL = "normal"
    DEBUG = "debug"
    SCRIPT = "script"
    EXPLORATION = "exploration"


class CommandInfo:
    """Command metadata"""
    
    def __init__(self, name: str, description: str, func: Callable, 
                 args: Optional[List[str]] = None, category: str = 'General'):
        self.name = name
        self.description = description
        self.func = func
        self.args = args or []
        self.category = category
        self.history = []


class REPL:
    """Interactive REPL for NLM brain control"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.command_history = []
        self.command_history_limit = 1000
        self.variables = {}
        self.auto_complete_enabled = True
        self.syntax_highlighting = True
        self.show_hints = True
        self.prompt_mode = REPLMode.NORMAL
        self.input_queue = queue.Queue()
        self.output_queue = queue.Queue()
        self.command_history_file = os.path.join(os.getcwd(), '.pynlm_repl_history')
        
        # Initialize readline
        readline.set_completer(self._completer)
        readline.set_history_length(self.command_history_limit)
        
        # Load command history
        self._load_command_history()
        
        # Setup commands
        self._setup_commands()
    
    def start(self, prompt: str = '>>> '):
        """Start the REPL"""
        print("NLM Brain REPL - Interactive Scripting Interface")
        print("Type 'help' for commands, 'exit' to quit")
        print("-" * 50)
        
        if self.show_hints:
            print("Quick start: Initialize with 'init', step with 'step [dt]',")
            print("inject stimuli with 'inject <type>', save with 'save <file>'")
        
        print("-" * 50)
        
        try:
            self._repl_loop(prompt)
        except KeyboardInterrupt:
            print("\nInterrupted")
        except Exception as e:
            print(f"\nREPL error: {e}")
            traceback.print_exc()
        
        # Save command history
        self._save_command_history()
    
    def _repl_loop(self, prompt: str):
        """Main REPL loop"""
        while True:
            try:
                # Get input with timeout
                try:
                    line = input(prompt).strip()
                except EOFError:
                    print("\nEOF received, exiting...")
                    break
                
                if not line:
                    continue
                
                # Process command
                self._process_command(line)
                
            except KeyboardInterrupt:
                print("\nType 'exit' to quit or 'help' for commands")
            except Exception as e:
                print(f"Error: {e}")
    
    def _process_command(self, line: str):
        """Process a command line"""
        # Parse command
        parts = line.split()
        command = parts[0].lower()
        args = parts[1:] if len(parts) > 1 else []
        
        # Record command in history
        self.command_history.append({
            'command': command,
            'args': args,
            'line': line,
            'timestamp': time.time(),
            'success': False,
            'error': None
        })
        
        # Execute command
        try:
            if command in self._commands:
                cmd_info = self._commands[command]
                self._execute_command(cmd_info, args)
                
                # Record success
                for entry in self.command_history:
                    if entry['command'] == command:
                        entry['success'] = True
                        break
            else:
                # Try to evaluate as Python code
                self._evaluate_code(line)
                
        except Exception as e:
            # Record error
            for entry in self.command_history:
                if entry['command'] == command:
                    entry['error'] = str(e)
                    break
            
            print(f"Error: {e}")
            if self.prompt_mode == REPLMode.DEBUG:
                traceback.print_exc()
    
    def _execute_command(self, cmd_info: CommandInfo, args: List[str]):
        """Execute a command"""
        try:
            # Check arguments
            if cmd_info.args and len(args) < len(cmd_info.args):
                print(f"Usage: {cmd_info.name} {cmd_info.args}")
                return
            
            # Execute command function
            if cmd_info.args:
                cmd_info.func(*args)
            else:
                cmd_info.func()
                
        except Exception as e:
            raise RuntimeError(f"Command '{cmd_info.name}' failed: {e}")
    
    def _evaluate_code(self, code: str):
        """Evaluate Python code in REPL context"""
        # Create local namespace with available objects
        namespace = {
            '__builtins__': {
                'print': print,
                'len': len,
                'range': range,
                'dict': dict,
                'list': list,
                'tuple': tuple,
                'float': float,
                'int': int,
                'str': str,
                'abs': abs,
                'max': max,
                'min': min,
                'sum': sum,
                'time': time,
                'json': json,
                'numpy': np if NUMPY_AVAILABLE else None,
                'matplotlib': plt if MATPLOTLIB_AVAILABLE else None,
                'pandas': pd if PANDAS_AVAILABLE else None,
            }
        }
        
        # Add context objects
        namespace.update({
            'brain': self.context.brain,
            'world': self.context.world,
            'agent_brain': self.context.agent_brain,
            'config': self.context.config,
            'interface': self.interface,
            'ScriptingMode': ScriptingMode,
            'EventType': EventType,
            'NeuronType': NeuronType,
            'SynapseType': SynapseType,
            'ActionType': ActionType,
            'Vision': Vision,
            'Audio': Audio,
            'InternalSignals': InternalSignals
        })
        
        # Add variables
        namespace.update(self.variables)
        
        try:
            # Execute code
            exec(code, namespace)
            
            # Update variables
            for key, value in namespace.items():
                if key not in ['print', 'len', 'range', 'dict', 'list', 'tuple',
                              'float', 'int', 'str', 'abs', 'max', 'min', 'sum',
                              'time', 'json', 'numpy', 'matplotlib', 'pandas',
                              'brain', 'world', 'agent_brain', 'config', 'interface',
                              'ScriptingMode', 'EventType', 'NeuronType', 'SynapseType',
                              'ActionType', 'Vision', 'Audio', 'InternalSignals']:
                    self.variables[key] = value
                    
        except Exception as e:
            raise RuntimeError(f"Code evaluation failed: {e}")
    
    def _completer(self, text: str, state: int) -> Optional[str]:
        """Readline completer"""
        if self.auto_complete_enabled:
            # Get matching commands
            matches = [cmd for cmd in self._commands.keys() 
                      if cmd.startswith(text)]
            
            # Add Python variable names
            if text and not text.startswith('_'):
                var_matches = [var for var in self.variables.keys() 
                              if var.startswith(text)]
                matches.extend(var_matches)
            
            # Get match at state
            if state < len(matches):
                return matches[state]
        
        return None
    
    def _setup_commands(self):
        """Setup built-in commands"""
        self._commands = {}
        
        # System commands
        self._commands['help'] = CommandInfo(
            'help', 'Show help for commands', self._cmd_help
        )
        
        self._commands['help-all'] = CommandInfo(
            'help-all', 'Show all available commands', self._cmd_help_all
        )
        
        self._commands['init'] = CommandInfo(
            'init', 'Initialize NLM system', self._cmd_init
        )
        
        self._commands['reset'] = CommandInfo(
            'reset', 'Reset the entire system', self._cmd_reset
        )
        
        self._commands['status'] = CommandInfo(
            'status', 'Show system status', self._cmd_status
        )
        
        self._commands['pause'] = CommandInfo(
            'pause', 'Pause simulation', self._cmd_pause
        )
        
        self._commands['resume'] = CommandInfo(
            'resume', 'Resume simulation', self._cmd_resume
        )
        
        # Simulation commands
        self._commands['step'] = CommandInfo(
            'step', 'Run simulation step', self._cmd_step, ['dt']
        )
        
        self._commands['run'] = CommandInfo(
            'run', 'Run simulation for duration', self._cmd_run, ['duration', 'dt']
        )
        
        self._commands['step-count'] = CommandInfo(
            'step-count', 'Run multiple steps', self._cmd_step_count, ['count', 'dt']
        )
        
        # Brain commands
        self._commands['inject'] = CommandInfo(
            'inject', 'Inject stimulus into brain', self._cmd_inject, ['type', 'params']
        )
        
        self._commands['save'] = CommandInfo(
            'save', 'Save brain state', self._cmd_save, ['filepath']
        )
        
        self._commands['load'] = CommandInfo(
            'load', 'Load brain state', self._cmd_load, ['filepath']
        )
        
        self._commands['state'] = CommandInfo(
            'state', 'Get brain state', self._cmd_state
        )
        
        # Visualization commands
        self._commands['plot'] = CommandInfo(
            'plot', 'Create visualization', self._cmd_plot, ['type', 'save']
        )
        
        self._commands['visualize'] = CommandInfo(
            'visualize', 'Start visualization dashboard', self._cmd_visualize
        )
        
        # Plugin commands
        self._commands['plugins'] = CommandInfo(
            'plugins', 'List loaded plugins', self._cmd_list_plugins
        )
        
        self._commands['plugin'] = CommandInfo(
            'plugin', 'Load or unload plugin', self._cmd_plugin, ['action', 'name']
        )
        
        # Configuration commands
        self._commands['config'] = CommandInfo(
            'config', 'Load configuration', self._cmd_config, ['filepath']
        )
        
        self._commands['config-save'] = CommandInfo(
            'config-save', 'Save configuration', self._cmd_config_save, ['filepath']
        )
        
        # Debug commands
        self._commands['trace'] = CommandInfo(
            'trace', 'Enable debug tracing', self._cmd_trace
        )
        
        self._commands['vars'] = CommandInfo(
            'vars', 'List variables', self._cmd_vars
        )
        
        self._commands['history'] = CommandInfo(
            'history', 'Show command history', self._cmd_history
        )
        
        # Control commands
        self._commands['exit'] = CommandInfo(
            'exit', 'Exit REPL', self._cmd_exit
        )
        
        self._commands['quit'] = CommandInfo(
            'quit', 'Exit REPL', self._cmd_exit
        )
    
    def _cmd_help(self):
        """Show help for commands"""
        print("\nAvailable commands:")
        
        # Group commands by category
        categories = {}
        for cmd_name, cmd_info in self._commands.items():
            if cmd_info.category not in categories:
                categories[cmd_info.category] = []
            categories[cmd_info.category].append(cmd_info)
        
        for category, commands in sorted(categories.items()):
            print(f"\n{category.upper()}:")
            for cmd in commands:
                args_str = f" {cmd.args[0]}" if cmd.args else ""
                if len(cmd.args) > 1:
                    args_str = " " + " ".join(cmd.args)
                
                print(f"  {cmd.name}{args_str}: {cmd.description}")
    
    def _cmd_help_all(self):
        """Show all available commands"""
        self._cmd_help()
    
    def _cmd_init(self):
        """Initialize NLM system"""
        if self.interface.initialize_system():
            print("NLM system initialized successfully")
        else:
            raise RuntimeError("Failed to initialize NLM system")
    
    def _cmd_reset(self):
        """Reset the entire system"""
        self.interface.reset_system()
        print("System reset completed")
    
    def _cmd_status(self):
        """Show system status"""
        if not self.context.running:
            print("System: STOPPED")
            return
        
        state = self.interface.get_state()
        resource_usage = self.interface.get_resource_usage()
        
        print(f"System: RUNNING")
        print(f"  Mode: {self.context.mode}")
        print(f"  Neurons: {state['total_neurons']}")
        print(f"  Synapses: {state['total_synapses']}")
        print(f"  Active: {state['active_neurons']}/{state['firing_neurons']}")
        print(f"  Firing Rate: {state['average_firing_rate']:.2f}")
        print(f"  Resource Usage: {resource_usage}")
    
    def _cmd_pause(self):
        """Pause simulation"""
        self.interface.set_mode(ScriptingMode.PAUSED)
        print("Simulation paused")
    
    def _cmd_resume(self):
        """Resume simulation"""
        self.interface.set_mode(ScriptingMode.STEP_BY_STEP)
        print("Simulation resumed")
    
    def _cmd_step(self, dt_arg: str):
        """Run simulation step"""
        try:
            dt = float(dt_arg)
        except ValueError:
            dt = 1.0
        
        asyncio.run(self.interface.run_step(dt))
        print(f"Step completed (dt={dt})")
    
    def _cmd_run(self, duration_arg: str, dt_arg: str):
        """Run simulation for duration"""
        try:
            duration = float(duration_arg)
            dt = float(dt_arg) if dt_arg else 1.0
        except ValueError:
            raise RuntimeError("Invalid duration or dt parameter")
        
        results = asyncio.run(self.interface.run_loop(duration, dt))
        print(f"Simulation completed: {len(results)} steps")
    
    def _cmd_step_count(self, count_arg: str, dt_arg: str):
        """Run multiple steps"""
        try:
            count = int(count_arg)
            dt = float(dt_arg) if dt_arg else 1.0
        except ValueError:
            raise RuntimeError("Invalid count or dt parameter")
        
        results = asyncio.run(self.interface.run_loop(count * dt, dt))
        print(f"Completed {count} steps")
    
    def _cmd_inject(self, type_arg: str, params_arg: str = ""):
        """Inject stimulus into brain"""
        params = {}
        
        if params_arg:
            # Parse simple params (for demo)
            try:
                import json
                params = json.loads(params_arg)
            except json.JSONDecodeError:
                # Try to parse as key=value pairs
                for param in params_arg.split(','):
                    if '=' in param:
                        key, value = param.split('=', 1)
                        try:
                            params[key.strip()] = float(value) if '.' in value else int(value)
                        except ValueError:
                            params[key.strip()] = value.strip("'\"")
        
        self.interface.inject_stimulus(type_arg, params)
        print(f"Stimulus {type_arg} injected: {params}")
    
    def _cmd_save(self, filepath: str):
        """Save brain state"""
        if self.interface.save_brain_state(filepath):
            print(f"Brain state saved to {filepath}")
        else:
            raise RuntimeError("Failed to save brain state")
    
    def _cmd_load(self, filepath: str):
        """Load brain state"""
        if self.interface.load_brain_state(filepath):
            print(f"Brain state loaded from {filepath}")
        else:
            raise RuntimeError("Failed to load brain state")
    
    def _cmd_state(self):
        """Get brain state"""
        state = self.interface.get_state()
        print("\nBrain State:")
        for key, value in state.items():
            print(f"  {key}: {value}")
    
    def _cmd_plot(self, type_arg: str, save_arg: str = ""):
        """Create visualization"""
        data = self.interface.get_state()
        
        try:
            if type_arg == 'neurons':
                fig = self.interface.create_visualization('neurons', {'neurons': [], 'activity': []})
            elif type_arg == 'regions':
                fig = self.interface.create_visualization('regions', {'regions': []})
            elif type_arg == 'firing':
                fig = self.interface.create_visualization('firing_rate', 
                                                        {'times': [], 'rates': []})
            else:
                raise ValueError(f"Unknown plot type: {type_arg}")
            
            if save_arg:
                self.interface.create_visualization(type_arg, data, save_arg)
                print(f"Plot saved to {save_arg}")
            
            print(f"Plot created: {type_arg}")
            
        except Exception as e:
            raise RuntimeError(f"Plot creation failed: {e}")
    
    def _cmd_visualize(self):
        """Start visualization dashboard"""
        dashboard = self.interface.create_dashboard('console')
        print("Starting visualization dashboard...")
        # In a real implementation, this would start the dashboard
    
    def _cmd_list_plugins(self):
        """List loaded plugins"""
        plugins = self.interface.context.plugin_manager.list_plugins()
        
        print(f"Loaded plugins ({len(plugins)}):")
        for plugin in plugins:
            print(f"  {plugin.name} ({plugin.version}) - {plugin.plugin_type.value}")
            print(f"    {plugin.description}")
            print(f"    Status: {plugin.status.value}")
    
    def _cmd_plugin(self, action_arg: str, name_arg: str):
        """Load or unload plugin"""
        if action_arg == 'load':
            self.interface.enable_plugin(name_arg)
            print(f"Plugin loaded: {name_arg}")
        elif action_arg == 'unload':
            if self.interface.context.plugin_manager.unload_plugin(name_arg):
                print(f"Plugin unloaded: {name_arg}")
            else:
                raise RuntimeError(f"Plugin not found: {name_arg}")
        else:
            raise ValueError(f"Unknown action: {action_arg}")
    
    def _cmd_config(self, filepath: str):
        """Load configuration"""
        summary = self.interface.context.plugin_manager.get_plugin('config_manager')
        if summary:
            summary.load_config_file(filepath)
            print(f"Configuration loaded from {filepath}")
        else:
            raise RuntimeError("Config manager not available")
    
    def _cmd_config_save(self, filepath: str):
        """Save configuration"""
        summary = self.interface.context.plugin_manager.get_plugin('config_manager')
        if summary:
            summary.save_config_file(filepath)
            print(f"Configuration saved to {filepath}")
        else:
            raise RuntimeError("Config manager not available")
    
    def _cmd_trace(self):
        """Enable debug tracing"""
        self.prompt_mode = REPLMode.DEBUG
        print("Debug tracing enabled")
    
    def _cmd_vars(self):
        """List variables"""
        print(f"\nVariables ({len(self.variables)}):")
        for var in sorted(self.variables.keys()):
            value = self.variables[var]
            print(f"  {var}: {type(value).__name__} = {value}")
    
    def _cmd_history(self):
        """Show command history"""
        print(f"\nCommand history (last {len(self.command_history)} commands):")
        
        for i, entry in enumerate(self.command_history[-20:]):  # Last 20 commands
            status = "✓" if entry['success'] else "✗"
            timestamp = time.strftime('%H:%M:%S', time.localtime(entry['timestamp']))
            print(f"  {timestamp} {status} {entry['line']}")
            
            if entry['error']:
                print(f"    Error: {entry['error']}")
    
    def _cmd_exit(self):
        """Exit REPL"""
        print("Exiting...")
        sys.exit(0)
    
    def _load_command_history(self):
        """Load command history from file"""
        try:
            with open(self.command_history_file, 'r') as f:
                lines = f.readlines()
                
                for line in lines:
                    if line.strip():
                        parts = line.strip().split('|')
                        if len(parts) >= 5:
                            command, args, full_line, timestamp_str, success_str = parts[:5]
                            
                            self.command_history.append({
                                'command': command,
                                'args': eval(args) if args != 'None' else [],
                                'line': full_line,
                                'timestamp': float(timestamp_str),
                                'success': success_str == 'True',
                                'error': None
                            })
        except FileNotFoundError:
            pass
    
    def _save_command_history(self):
        """Save command history to file"""
        try:
            with open(self.command_history_file, 'w') as f:
                for entry in self.command_history:
                    line = f"{entry['command']}|{entry['args']}|{entry['line']}|"
                    line += f"{entry['timestamp']}|{entry['success']}|"
                    line += f"{entry.get('error', 'None')}\n"
                    f.write(line)
        except Exception:
            pass
    
    def add_command(self, name: str, description: str, func: Callable,
                   args: Optional[List[str]] = None, category: str = 'Custom'):
        """Add custom command to REPL"""
        self._commands[name] = CommandInfo(name, description, func, args, category)
        
        # Update readline completer
        readline.set_completer(self._completer)
    
    def remove_command(self, name: str):
        """Remove command from REPL"""
        if name in self._commands:
            del self._commands[name]
            
            # Update readline completer
            readline.set_completer(self._completer)
    
    def set_prompt_mode(self, mode: REPLMode):
        """Set REPL prompt mode"""
        self.prompt_mode = mode
    
    def enable_auto_complete(self, enabled: bool = True):
        """Enable or disable auto-complete"""
        self.auto_complete_enabled = enabled
    
    def set_syntax_highlighting(self, enabled: bool = True):
        """Enable or disable syntax highlighting"""
        self.syntax_highlighting = enabled
    
    def set_show_hints(self, enabled: bool = True):
        """Enable or disable help hints"""
        self.show_hints = enabled
    
    def get_command_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get command history"""
        return self.command_history[-limit:]


# Import statements for missing dependencies
try:
    import os
    import json
    import asyncio
    
    # Try to import optional dependencies
    try:
        import numpy as np
        NUMPY_AVAILABLE = True
    except ImportError:
        NUMPY_AVAILABLE = False
        np = None
    
    try:
        import matplotlib.pyplot as plt
        MATPLOTLIB_AVAILABLE = True
    except ImportError:
        MATPLOTLIB_AVAILABLE = False
        plt = None
    
    try:
        import pandas as pd
        PANDAS_AVAILABLE = True
    except ImportError:
        PANDAS_AVAILABLE = False
        pd = None
    
    # Import NLM classes
    from pynlm.interface import (
        ScriptingInterface, ScriptingEvent, EventType, ScriptingMode
    )
    from pynlm.brain_control import BrainController, ConfigManager
    from pynlm.simulation import SimulationController
    from pynlm.plugin_system import PluginManager
    from pynlm.dashboard import Dashboard
    from pynlm.visualization import VisualizationDashboard
    
    # Import NLM types
    from pynlm import (
        NeuronType, SynapseType, ActionType, Vision, Audio, 
        InternalSignals, RegionId, PopulationId
    )
    
except ImportError as e:
    print(f"Warning: Some dependencies not available: {e}")
    # Create placeholder classes
    class ScriptingInterface: pass
    class ScriptingEvent: pass
    class EventType: pass
    class ScriptingMode: pass
    class BrainController: pass
    class ConfigManager: pass
    class SimulationController: pass
    class PluginManager: pass
    class Dashboard: pass
    class VisualizationDashboard: pass
    class NeuronType: pass
    class SynapseType: pass
    class ActionType: pass
    class Vision: pass
    class Audio: pass
    class InternalSignals: pass
    class RegionId: pass
    class PopulationId: pass
    class PluginError(Exception): pass

# Export key classes and functions
__all__ = [
    'REPL',
    'REPLMode',
    'CommandInfo'
]
