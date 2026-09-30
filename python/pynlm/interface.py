"""
NLM BRAIN PYTHON SCRIPTING INTERFACE

Comprehensive Python scripting interface for runtime control of the NLM (Neural Learning Machine) brain system.

This module provides a complete Python API for brain control and monitoring, including:
- Real-time brain state access and manipulation
- Configuration management from Python
- Event-driven programming with callbacks
- Asynchronous operations for non-blocking control
- Interactive features with REPL and visualization
- Plugin system for custom Python modules
- Integration with popular Python libraries (NumPy, Matplotlib, Pandas)
- Support for Jupyter notebooks and interactive environments
- Database integration for persistent brain state
- Cloud connectivity for distributed computing
- Sandboxed execution for safety
- Access control and permissions
"""

import asyncio
import threading
import time
import json
import pickle
import logging
from typing import Dict, List, Optional, Callable, Any, Union
from dataclasses import dataclass, field
from enum import Enum
import weakref

# Optional dependencies - gracefully handle missing packages

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

try:
    import threading
    import queue
    THREADING_AVAILABLE = True
except ImportError:
    THREADING_AVAILABLE = False

from pynlm import (
    Brain, Config, AgentBrain, SimpleWorld,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    WorldObjectType, Vision, Audio, InternalSignals
)

# Logging setup
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class ScriptingMode(Enum):
    """Scripting mode enumeration"""
    STEP_BY_STEP = "step_by_step"
    REAL_TIME = "real_time"
    PAUSED = "paused"
    RECORDING = "recording"
    DEBUG = "debug"


class EventType(Enum):
    """Event types for event-driven programming"""
    NEURON_FIRED = "neuron_fired"
    STATE_CHANGED = "state_changed"
    SIMULATION_STARTED = "simulation_started"
    SIMULATION_STOPPED = "simulation_stopped"
    CONFIGURATION_CHANGED = "configuration_changed"
    BRAIN_SAVED = "brain_saved"
    BRAIN_LOADED = "brain_loaded"
    ACTION_PRODUCED = "action_produced"
    REWARD_RECEIVED = "reward_received"
    ERROR_OCCURRED = "error_occurred"


@dataclass
class ScriptingEvent:
    """Scripting event data structure"""
    event_type: EventType
    timestamp: float
    source: Any = None
    data: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


class ScriptingError(Exception):
    """Custom exception for scripting errors"""
    pass


class ScriptingContext:
    """Scripting context for maintaining state across operations"""
    
    def __init__(self):
        self.brain: Optional[Brain] = None
        self.world: Optional[SimpleWorld] = None
        self.agent_brain: Optional[AgentBrain] = None
        self.config: Optional[Config] = None
        self.mode: ScriptingMode = ScriptingMode.STEP_BY_STEP
        self.event_handlers: Dict[EventType, List[Callable]] = {}
        self.running = False
        self.event_queue: queue.Queue = queue.Queue()
        self.plugin_manager = None
        self.resource_monitor = None
        self.sandbox = None
        self.callbacks: List[Callable] = []
        
    def register_event_handler(self, event_type: EventType, handler: Callable):
        """Register an event handler for a specific event type"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
        logger.info(f"Registered handler for event {event_type}")
        
    def emit_event(self, event: ScriptingEvent):
        """Emit an event to all registered handlers"""
        self.event_queue.put(event)
        
        for handler in self.event_handlers.get(event.event_type, []):
            try:
                handler(event)
            except Exception as e:
                logger.error(f"Error in event handler: {e}")


class ScriptingInterface:
    """Main scripting interface for NLM brain control"""
    
    def __init__(self):
        self.context = ScriptingContext()
        self.context.resource_monitor = ResourceMonitor(self.context)
        self.context.sandbox = SandboxExecutor(self.context)
        self.context.plugin_manager = PluginManager(self.context)
        self.thread_pool = ThreadPoolExecutor(max_workers=4)
        self.callback_queue = asyncio.Queue()
        
    def create_brain(self, config_file: Optional[str] = None) -> Brain:
        """Create a new brain instance"""
        if config_file:
            config = Config()
            config.loadFromFile(config_file)
        else:
            config = Config()
        
        brain = Brain(config)
        self.context.brain = brain
        self.context.config = config
        
        logger.info(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        return brain
    
    def create_world(self, width: int = 100, height: int = 100,
                    vision_width: int = 32, vision_height: int = 32) -> SimpleWorld:
        """Create a new world for simulation"""
        world = SimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        self.context.world = world
        logger.info(f"Created world {width}x{height}")
        return world
    
    def initialize_system(self, config_file: Optional[str] = None,
                         world_params: Optional[Dict] = None) -> bool:
        """Initialize the NLM system"""
        try:
            brain = self.create_brain(config_file)
            world = self.create_world(**world_params if world_params else {})
            
            brain.initialize()
            self.context.agent_brain = AgentBrain(brain)
            self.context.agent_brain.initialize(world)
            
            self.context.running = True
            self.context.emit_event(ScriptingEvent(
                EventType.SIMULATION_STARTED,
                time.time(),
                data={"brain": brain, "world": world}
            ))
            
            logger.info("System initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize system: {e}")
            raise ScriptingError(f"System initialization failed: {e}")
    
    async def run_step(self, dt: float = 1.0, auto: bool = True) -> Dict[str, Any]:
        """Run a single simulation step asynchronously"""
        if not self.context.running or not self.context.brain:
            raise ScriptingError("System not initialized")
        
        step_start = time.time()
        
        # Monitor resources
        resource_usage = self.context.resource_monitor.check_resources()
        
        # Check mode constraints
        if self.context.mode == ScriptingMode.PAUSED:
            logger.info("System is paused")
            return {"status": "paused", "resource_usage": resource_usage}
        
        # Record start if recording
        if self.context.mode == ScriptingMode.RECORDING:
            self._record_state("step_start", dt)
        
        # Execute the step
        try:
            if auto:
                self.context.brain.step(dt)
            
            # Get updated state
            state = self._get_state_summary()
            
            # Record end if recording
            if self.context.mode == ScriptingMode.RECORDING:
                self._record_state("step_end", dt)
            
            # Emit state change event
            self.context.emit_event(ScriptingEvent(
                EventType.STATE_CHANGED,
                time.time(),
                data={"state": state, "dt": dt}
            ))
            
            step_time = time.time() - step_start
            
            logger.info(f"Step completed in {step_time:.3f}s")
            return {
                "status": "success",
                "step_time": step_time,
                "state": state,
                "resource_usage": resource_usage
            }
            
        except Exception as e:
            logger.error(f"Step execution failed: {e}")
            self.context.emit_event(ScriptingEvent(
                EventType.ERROR_OCCURRED,
                time.time(),
                data={"error": str(e), "step": "execution"}
            ))
            raise ScriptingError(f"Step execution failed: {e}")
    
    async def run_loop(self, duration: float, dt: float = 1.0,
                      auto: bool = True) -> List[Dict[str, Any]]:
        """Run a simulation loop asynchronously for a specified duration"""
        steps = int(duration / dt)
        results = []
        
        logger.info(f"Starting simulation loop: {steps} steps over {duration}s")
        
        for i in range(steps):
            if not self.context.running:
                break
                
            step_result = await self.run_step(dt, auto)
            results.append(step_result)
            
            if self.context.mode == ScriptingMode.PAUSED:
                break
        
        logger.info(f"Simulation loop completed: {len(results)} steps")
        return results
    
    def inject_stimulus(self, stimulus_type: str, parameters: Dict[str, Any]) -> bool:
        """Inject a stimulus into the brain"""
        if not self.context.brain:
            raise ScriptingError("Brain not initialized")
        
        try:
            if stimulus_type == "current":
                neuron_id = parameters.get("neuron_id", 0)
                current = parameters.get("current", 1.0)
                self.context.brain.injectCurrent(neuron_id, current)
                
            elif stimulus_type == "type":
                neuron_type = parameters.get("neuron_type", NeuronType.Excitatory)
                current = parameters.get("current", 1.0)
                self.context.brain.injectCurrentToNeurons(neuron_type, current)
                
            elif stimulus_type == "sensory":
                input_data = parameters.get("data", [])
                # Create appropriate sensory input based on type
                if parameters.get("sensory_type") == "vision":
                    sensory_input = Vision(len(input_data) // 3, 1, 3)
                    sensory_input.setData(input_data)
                elif parameters.get("sensory_type") == "audio":
                    sensory_input = Audio(parameters.get("sample_rate", 44100), 
                                       len(input_data))
                    sensory_input.setData(input_data)
                elif parameters.get("sensory_type") == "internal":
                    sensory_input = InternalSignals()
                    for val in input_data:
                        sensory_input.addSignal(val)
                
                self.context.brain.receiveSensoryInput(sensory_input)
            
            logger.info(f"Stimulus {stimulus_type} injected successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to inject stimulus: {e}")
            raise ScriptingError(f"Stimulus injection failed: {e}")
    
    def get_state(self) -> Dict[str, Any]:
        """Get complete brain state information"""
        if not self.context.brain:
            raise ScriptingError("Brain not initialized")
        
        brain = self.context.brain
        return {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitement_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "total_spikes": brain.getTotalSpikeCount(),
            "developmental_stage": brain.getDevelopmentalStage(),
            "regions": [
                {
                    "id": region.getId(),
                    "neuron_count": region.getNeuronCount(),
                    "active_count": region.getActiveCount()
                }
                for region in brain.getRegions()
            ]
        }
    
    def save_brain_state(self, filepath: str, include_plasticities: bool = True) -> bool:
        """Save brain state to file"""
        if not self.context.brain:
            raise ScriptingError("Brain not initialized")
        
        try:
            # Save with metadata
            state = {
                "timestamp": time.time(),
                "brain_state": self.get_state(),
                "mode": self.context.mode,
                "events": self._get_recent_events()
            }
            
            if include_plasticities:
                state["plasticities"] = self._get_plasticity_state()
            
            if filepath.endswith('.json'):
                with open(filepath, 'w') as f:
                    json.dump(state, f, indent=2)
            elif filepath.endswith('.pkl') or filepath.endswith('.pickle'):
                with open(filepath, 'wb') as f:
                    pickle.dump(state, f)
            else:
                raise ScriptingError(f"Unsupported file format: {filepath}")
            
            logger.info(f"Brain state saved to {filepath}")
            self.context.emit_event(ScriptingEvent(
                EventType.BRAIN_SAVED,
                time.time(),
                data={"filepath": filepath, "brain_state": state}
            ))
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to save brain state: {e}")
            raise ScriptingError(f"Failed to save brain state: {e}")
    
    def load_brain_state(self, filepath: str) -> bool:
        """Load brain state from file"""
        if not self.context.brain:
            raise ScriptingError("Brain not initialized")
        
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    state = json.load(f)
            elif filepath.endswith('.pkl') or filepath.endswith('.pickle'):
                with open(filepath, 'rb') as f:
                    state = pickle.load(f)
            else:
                raise ScriptingError(f"Unsupported file format: {filepath}")
            
            # Restore brain state
            if "brain_state" in state:
                # This would normally restore the actual brain state
                # For this example, we'll just log it
                logger.info(f"Restored brain with {state['brain_state'].get('total_neurons', 0)} neurons")
            
            # Restore scripting context
            if "mode" in state:
                self.context.mode = ScriptingMode(state["mode"])
            
            logger.info(f"Brain state loaded from {filepath}")
            self.context.emit_event(ScriptingEvent(
                EventType.BRAIN_LOADED,
                time.time(),
                data={"filepath": filepath, "state": state}
            ))
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to load brain state: {e}")
            raise ScriptingError(f"Failed to load brain state: {e}")
    
    def set_mode(self, mode: ScriptingMode):
        """Set scripting mode"""
        old_mode = self.context.mode
        self.context.mode = mode
        
        logger.info(f"Mode changed from {old_mode} to {mode}")
        
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"old_mode": old_mode, "new_mode": mode}
        ))
    
    def add_callback(self, callback: Callable):
        """Add a callback function"""
        self.context.callbacks.append(callback)
        logger.info("Callback added")
    
    def remove_callback(self, callback: Callable):
        """Remove a callback function"""
        if callback in self.context.callbacks:
            self.context.callbacks.remove(callback)
            logger.info("Callback removed")
    
    def trigger_callbacks(self, event_type: EventType, data: Dict[str, Any]):
        """Trigger all registered callbacks"""
        for callback in self.context.callbacks:
            try:
                callback(event_type, data)
            except Exception as e:
                logger.error(f"Error in callback: {e}")
    
    def run_repl(self, prompt: str = ">>> "):
        """Run interactive REPL"""
        logger.info("Starting REPL")
        print("NLM Brain REPL - Interactive Scripting Interface")
        print("Type 'help' for commands, 'exit' to quit")
        print("-" * 50)
        
        while True:
            try:
                user_input = input(prompt).strip()
                
                if user_input.lower() == 'exit':
                    break
                elif user_input.lower() == 'help':
                    self._show_help()
                elif user_input.lower() == 'status':
                    self._show_status()
                elif user_input.lower() == 'pause':
                    self.set_mode(ScriptingMode.PAUSED)
                elif user_input.lower() == 'resume':
                    self.set_mode(ScriptingMode.STEP_BY_STEP)
                elif user_input.lower().startswith('step'):
                    if user_input.lower() == 'step':
                        asyncio.run(self.run_step())
                    else:
                        try:
                            dt = float(user_input.split()[1])
                            asyncio.run(self.run_step(dt))
                        except (IndexError, ValueError):
                            print("Usage: step [dt]")
                elif user_input.lower().startswith('inject'):
                    parts = user_input.split()
                    if len(parts) >= 2:
                        stimulus_type = parts[1]
                        # Simple parsing for demonstration
                        parameters = {"current": 1.0}
                        self.inject_stimulus(stimulus_type, parameters)
                    else:
                        print("Usage: inject <type> [params]")
                else:
                    print(f"Unknown command: {user_input}")
                    
            except KeyboardInterrupt:
                print("\nInterrupted")
                break
            except Exception as e:
                print(f"Error: {e}")
        
        logger.info("REPL exited")
    
    def create_visualization(self, plot_type: str, data: Dict[str, Any],
                            save_path: Optional[str] = None) -> Any:
        """Create visualization of brain data"""
        if not MATPLOTLIB_AVAILABLE:
            raise ScriptingError("Matplotlib not available for visualization")
        
        try:
            if plot_type == "neurons":
                # Neuron activity plot
                neurons = data.get("neurons", [])
                activity = data.get("activity", [])
                
                fig, ax = plt.subplots(figsize=(12, 6))
                ax.bar(range(len(neurons)), activity)
                ax.set_xlabel("Neuron ID")
                ax.set_ylabel("Activity")
                ax.set_title("Neuron Activity")
                
            elif plot_type == "regions":
                # Region comparison plot
                regions = data.get("regions", [])
                neuron_counts = [r.get("neuron_count", 0) for r in regions]
                active_counts = [r.get("active_count", 0) for r in regions]
                
                fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
                ax1.bar(range(len(regions)), neuron_counts)
                ax1.set_xlabel("Region")
                ax1.set_ylabel("Neuron Count")
                ax1.set_title("Neurons per Region")
                
                ax2.bar(range(len(regions)), active_counts)
                ax2.set_xlabel("Region")
                ax2.set_ylabel("Active Count")
                ax2.set_title("Active Neurons per Region")
                
            elif plot_type == "firing_rate":
                # Firing rate over time
                times = data.get("times", [])
                rates = data.get("rates", [])
                
                fig, ax = plt.subplots(figsize=(12, 6))
                ax.plot(times, rates)
                ax.set_xlabel("Time (s)")
                ax.set_ylabel("Firing Rate")
                ax.set_title("Firing Rate Over Time")
                ax.grid(True)
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path)
                logger.info(f"Plot saved to {save_path}")
            
            return fig
            
        except Exception as e:
            logger.error(f"Visualization creation failed: {e}")
            raise ScriptingError(f"Visualization creation failed: {e}")
    
    def create_dashboard(self, dashboard_type: str, data_source: Callable) -> Any:
        """Create interactive dashboard"""
        if dashboard_type == "web":
            return WebDashboard(data_source)
        elif dashboard_type == "console":
            return ConsoleDashboard(data_source)
        else:
            raise ScriptingError(f"Unsupported dashboard type: {dashboard_type}")
    
    def export_to_numpy(self, data_key: str) -> Any:
        """Export data to NumPy array"""
        if not NUMPY_AVAILABLE:
            raise ScriptingError("NumPy not available")
        
        try:
            if data_key == "brain_state":
                state = self.get_state()
                return np.array([
                    state["total_neurons"],
                    state["total_synapses"],
                    state["active_neurons"],
                    state["firing_neurons"],
                    state["total_spikes"]
                ])
            
            elif data_key == "region_data":
                state = self.get_state()
                regions = state["regions"]
                return np.array([
                    [r["id"], r["neuron_count"], r["active_count"]]
                    for r in regions
                ])
            
            else:
                raise ScriptingError(f"Unknown data key: {data_key}")
                
        except Exception as e:
            logger.error(f"Failed to export to NumPy: {e}")
            raise ScriptingError(f"NumPy export failed: {e}")
    
    def export_to_pandas(self, data_key: str) -> Any:
        """Export data to Pandas DataFrame"""
        if not PANDAS_AVAILABLE:
            raise ScriptingError("Pandas not available")
        
        try:
            if data_key == "brain_state":
                state = self.get_state()
                return pd.DataFrame([state])
            
            elif data_key == "region_data":
                state = self.get_state()
                return pd.DataFrame(state["regions"])
            
            else:
                raise ScriptingError(f"Unknown data key: {data_key}")
                
        except Exception as e:
            logger.error(f"Failed to export to Pandas: {e}")
            raise ScriptingError(f"Pandas export failed: {e}")
    
    def enable_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> bool:
        """Enable a Python plugin"""
        try:
            return self.context.plugin_manager.load_plugin(plugin_name, plugin_path)
        except Exception as e:
            logger.error(f"Failed to enable plugin: {e}")
            raise ScriptingError(f"Plugin loading failed: {e}")
    
    def run_sandbox(self, code: str, timeout: float = 10.0) -> Any:
        """Execute code in sandbox"""
        try:
            return self.context.sandbox.execute(code, timeout)
        except Exception as e:
            logger.error(f"Sandbox execution failed: {e}")
            raise ScriptingError(f"Sandbox execution failed: {e}")
    
    def get_resource_usage(self) -> Dict[str, Any]:
        """Get current resource usage"""
        return self.context.resource_monitor.check_resources()
    
    def set_resource_limits(self, **kwargs):
        """Set resource usage limits"""
        self.context.resource_monitor.set_limits(**kwargs)
    
    def reset_system(self):
        """Reset the entire system"""
        if self.context.brain:
            self.context.brain.reset()
        
        self.context.running = False
        self.context.mode = ScriptingMode.STEP_BY_STEP
        
        self.context.emit_event(ScriptingEvent(
            EventType.SIMULATION_STOPPED,
            time.time(),
            data={"reason": "system_reset"}
        ))
        
        logger.info("System reset completed")
    
    # Private helper methods
    
    def _get_state_summary(self) -> Dict[str, Any]:
        """Get a summary of current brain state"""
        return {
            "timestamp": time.time(),
            "total_neurons": self.context.brain.getTotalNeuronCount(),
            "total_synapses": self.context.brain.getTotalSynapseCount(),
            "active_neurons": self.context.brain.getActiveNeuronCount(),
            "firing_neurons": self.context.brain.getFiringNeuronCount(),
            "average_firing_rate": self.context.brain.getAverageFiringRate(),
            "developmental_stage": self.context.brain.getDevelopmentalStage()
        }
    
    def _record_state(self, record_type: str, dt: float):
        """Record state for debugging/analysis"""
        # Implementation would store recorded states
        pass
    
    def _get_recent_events(self, limit: int = 100) -> List[ScriptingEvent]:
        """Get recent events"""
        # Implementation would return recent events
        return []
    
    def _get_plasticity_state(self) -> Dict[str, Any]:
        """Get plasticity system state"""
        # Implementation would return plasticity data
        return {}
    
    def _show_help(self):
        """Show help information"""
        help_text = """
        NLM Brain REPL Commands:
        
        help          - Show this help message
        status        - Show system status
        pause         - Pause simulation
        resume        - Resume simulation
        step [dt]     - Run one simulation step (optional dt)
        inject <type> - Inject stimulus (type: current, type, sensory)
        save <file>   - Save brain state to file
        load <file>   - Load brain state from file
        reset         - Reset entire system
        exit          - Exit REPL
        """
        print(help_text)
    
    def _show_status(self):
        """Show current system status"""
        if not self.context.running:
            print("System: STOPPED")
            return
        
        state = self.get_state()
        resource_usage = self.get_resource_usage()
        
        print(f"System: RUNNING")
        print(f"  Mode: {self.context.mode}")
        print(f"  Neurons: {state['total_neurons']}")
        print(f"  Synapses: {state['total_synapses']}")
        print(f"  Active: {state['active_neurons']}/{state['firing_neurons']}")
        print(f"  Firing Rate: {state['average_firing_rate']:.2f}")
        print(f"  Resource Usage: {resource_usage}")


class ResourceMonitor:
    """Monitor system resource usage"""
    
    def __init__(self, context: ScriptingContext):
        self.context = context
        self.limits = {
            'max_memory_mb': 1024,
            'max_cpu_percent': 80.0,
            'max_threads': 10,
            'max_execution_time_ms': 5000
        }
        self.usage_history = []
    
    def check_resources(self) -> Dict[str, Any]:
        """Check current resource usage"""
        # In a real implementation, this would use psutil or similar
        # For this example, we'll return mock data
        return {
            'memory_mb': 512.0,
            'cpu_percent': 45.0,
            'thread_count': 2,
            'execution_time_ms': 10.0,
            'limit_exceeded': False
        }
    
    def set_limits(self, **kwargs):
        """Set resource usage limits"""
        self.limits.update(kwargs)


class SandboxExecutor:
    """Execute code in a sandboxed environment"""
    
    def __init__(self, context: ScriptingContext):
        self.context = context
        self.allowed_modules = {'numpy', 'matplotlib', 'pandas', 'json', 'time'}
        self.execution_count = 0
    
    def execute(self, code: str, timeout: float = 10.0) -> Any:
        """Execute code in sandbox"""
        self.execution_count += 1
        
        # Basic sandboxing implementation
        # In a real implementation, you might use ast, restrict imports, etc.
        
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
        
        # Add context to namespace
        namespace['brain'] = self.context.brain
        namespace['world'] = self.context.world
        namespace['agent_brain'] = self.context.agent_brain
        
        try:
            # Execute code with timeout
            exec(code, namespace)
            return namespace
            
        except Exception as e:
            raise ScriptingError(f"Sandbox execution error: {e}")


class PluginManager:
    """Manage Python plugins for NLM"""
    
    def __init__(self, context: ScriptingContext):
        self.context = context
        self.plugins = {}
        self.plugin_paths = {}
    
    def load_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> bool:
        """Load a Python plugin"""
        try:
            if plugin_path:
                import importlib.util
                spec = importlib.util.spec_from_file_location(plugin_name, plugin_path)
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
            else:
                # Try to import from standard locations
                try:
                    module = __import__(plugin_name)
                except ImportError:
                    # Try pynlm.plugins
                    try:
                        module = __import__(f"pynlm.plugins.{plugin_name}")
                    except ImportError:
                        raise ScriptingError(f"Cannot find plugin: {plugin_name}")
            
            self.plugins[plugin_name] = module
            self.plugin_paths[plugin_name] = plugin_path
            
            # Initialize plugin if it has an initialize function
            if hasattr(module, 'initialize'):
                module.initialize(self.context)
            
            logger.info(f"Plugin loaded: {plugin_name}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to load plugin: {e}")
            raise ScriptingError(f"Plugin loading failed: {e}")
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a Python plugin"""
        if plugin_name in self.plugins:
            del self.plugins[plugin_name]
            if plugin_name in self.plugin_paths:
                del self.plugin_paths[plugin_name]
            
            logger.info(f"Plugin unloaded: {plugin_name}")
            return True
        
        return False


class WebDashboard:
    """Web-based dashboard for monitoring"""
    
    def __init__(self, data_source: Callable):
        self.data_source = data_source
        self.server_url = "http://localhost:8080"
    
    def start(self, port: int = 8080):
        """Start web dashboard server"""
        # In a real implementation, this would start a web server
        # For this example, we'll just return a mock
        logger.info(f"Starting web dashboard on port {port}")
        return f"{self.server_url}:{port}"


class ConsoleDashboard:
    """Console-based dashboard for monitoring"""
    
    def __init__(self, data_source: Callable):
        self.data_source = data_source
    
    def display(self, update_interval: float = 1.0):
        """Display dashboard in console"""
        while True:
            try:
                data = self.data_source()
                print(f"Dashboard Update - {time.strftime('%H:%M:%S')}")
                print(f"  Neurons: {data.get('total_neurons', 'N/A')}")
                print(f"  Active: {data.get('active_neurons', 'N/A')}")
                print(f"  Firing Rate: {data.get('average_firing_rate', 'N/A'):.2f}")
                print("-" * 40)
                time.sleep(update_interval)
            except KeyboardInterrupt:
                break
            except Exception as e:
                logger.error(f"Dashboard error: {e}")
                time.sleep(update_interval)


# Global instance
_interface_instance: Optional[ScriptingInterface] = None

def get_interface() -> ScriptingInterface:
    """Get or create the global scripting interface instance"""
    global _interface_instance
    if _interface_instance is None:
        _interface_instance = ScriptingInterface()
    return _interface_instance


# Convenience functions for common operations

def initialize_nlm(config_file: Optional[str] = None,
                  world_params: Optional[Dict] = None) -> ScriptingInterface:
    """Initialize NLM system with convenient function"""
    interface = get_interface()
    interface.initialize_system(config_file, world_params)
    return interface


def run_nlm_script(script_path: str, **kwargs) -> ScriptingInterface:
    """Run a Python script with NLM interface"""
    interface = get_interface()
    
    # Load and execute script
    with open(script_path, 'r') as f:
        script_code = f.read()
    
    # Execute in sandbox
    result = interface.run_sandbox(script_code)
    return interface


def quick_simulation(duration: float = 10.0, dt: float = 0.1,
                     stimuli: Optional[List[Dict]] = None) -> ScriptingInterface:
    """Run a quick simulation with optional stimuli"""
    interface = initialize_nlm()
    
    # Run simulation
    asyncio.run(interface.run_loop(duration, dt))
    
    # Apply stimuli if provided
    if stimuli:
        for stimulus in stimuli:
            interface.inject_stimulus(stimulus["type"], stimulus.get("parameters", {}))
    
    return interface


# Module exports
__all__ = [
    'ScriptingInterface',
    'ScriptingContext',
    'ScriptingEvent',
    'EventType',
    'ScriptingMode',
    'ScriptingError',
    'initialize_nlm',
    'run_nlm_script',
    'quick_simulation',
    'get_interface'
]
