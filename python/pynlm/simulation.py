pynlm/simulation.py
Name: Simulation control module
Summary: Provides brain simulation control (step, pause, reset, save/load, replay)

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType, ScriptingMode
import asyncio
import time
import threading
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass
from enum import Enum


class SimulationController:
    """Advanced simulation controller with comprehensive control features"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.simulation_thread = None
        self.current_simulation = None
        self.simulation_history = []
        self.speed_factor = 1.0
        self.record_mode = False
        self.record_data = []
        self.playback_speed = 1.0
        self.checkpoints = {}
        self.simulation_paused = False
        self.pause_reason = None
    
    async def start_simulation(self, duration: Optional[float] = None,
                             dt: float = 1.0, auto: bool = True) -> Dict[str, Any]:
        """Start simulation with optional duration"""
        if not self.context.running:
            raise RuntimeError("System not initialized")
        
        self.context.mode = ScriptingMode.REAL_TIME
        self.simulation_paused = False
        self.pause_reason = None
        
        # Record if in record mode
        if self.record_mode:
            self._start_recording()
        
        if duration is None:
            # Run indefinitely until stopped
            async def infinite_loop():
                while self.context.running and not self.simulation_paused:
                    await self.interface.run_step(dt, auto)
                    await asyncio.sleep(dt * self.speed_factor)
            
            self.current_simulation = asyncio.create_task(infinite_loop())
            result = {"status": "running", "duration": "indefinite"}
        else:
            # Run for specified duration
            async def timed_loop():
                steps = int(duration / dt)
                for i in range(steps):
                    if self.simulation_paused:
                        await asyncio.sleep(0.1)
                        continue
                    
                    await self.interface.run_step(dt, auto)
                    await asyncio.sleep(dt * self.speed_factor)
            
            self.current_simulation = asyncio.create_task(timed_loop())
            result = {"status": "running", "duration": f"{duration}s", "steps": int(duration / dt)}
        
        self.simulation_history.append({
            'action': 'start',
            'timestamp': time.time(),
            'duration': duration,
            'dt': dt,
            'auto': auto,
            'status': 'running'
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.SIMULATION_STARTED,
            time.time(),
            data={"duration": duration, "dt": dt, "auto": auto}
        ))
        
        return result
    
    def pause_simulation(self, reason: Optional[str] = None):
        """Pause the simulation"""
        self.simulation_paused = True
        self.pause_reason = reason
        self.context.mode = ScriptingMode.PAUSED
        
        if self.current_simulation:
            self.current_simulation.cancel()
            self.current_simulation = None
        
        self.simulation_history.append({
            'action': 'pause',
            'timestamp': time.time(),
            'reason': reason,
            'status': 'paused'
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"paused": True, "reason": reason}
        ))
    
    def resume_simulation(self):
        """Resume the simulation"""
        self.simulation_paused = False
        self.pause_reason = None
        self.context.mode = ScriptingMode.STEP_BY_STEP
        
        if self.current_simulation:
            self.current_simulation.cancel()
            self.current_simulation = None
        
        self.simulation_history.append({
            'action': 'resume',
            'timestamp': time.time(),
            'status': 'resumed'
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"paused": False, "reason": "user_resumed"}
        ))
    
    async def step_simulation(self, dt: float = 1.0) -> Dict[str, Any]:
        """Take a single simulation step"""
        if self.simulation_paused:
            raise RuntimeError("Simulation is paused")
        
        result = await self.interface.run_step(dt, auto=True)
        
        self.simulation_history.append({
            'action': 'step',
            'timestamp': time.time(),
            'dt': dt,
            'result': result
        })
        
        return result
    
    async def reset_simulation(self, keep_config: bool = True) -> Dict[str, Any]:
        """Reset the simulation to initial state"""
        self.pause_simulation("reset")
        
        if keep_config and self.context.config:
            # Reload config
            self.context.config.clear()
        
        # Reset brain if available
        if self.context.brain:
            self.context.brain.reset()
        
        # Reset history
        self.simulation_history.append({
            'action': 'reset',
            'timestamp': time.time(),
            'keep_config': keep_config,
            'status': 'reset'
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"action": "reset"}
        ))
        
        return {"status": "reset", "kept_config": keep_config}
    
    def save_checkpoint(self, name: str, metadata: Optional[Dict] = None):
        """Save a simulation checkpoint"""
        checkpoint = {
            'name': name,
            'timestamp': time.time(),
            'state': self.interface.get_state(),
            'mode': self.context.mode,
            'paused': self.simulation_paused,
            'pause_reason': self.pause_reason
        }
        
        if metadata:
            checkpoint['metadata'] = metadata
        
        self.checkpoints[name] = checkpoint
        
        self.simulation_history.append({
            'action': 'checkpoint_save',
            'name': name,
            'timestamp': time.time(),n            'metadata': metadata
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.BRAIN_SAVED,
            time.time(),
            data={"checkpoint_name": name, "metadata": metadata}
        ))
    
    def load_checkpoint(self, name: str) -> Dict[str, Any]:
        """Load a simulation checkpoint"""
        if name not in self.checkpoints:
            raise ValueError(f"Checkpoint not found: {name}")
        
        checkpoint = self.checkpoints[name]
        
        # Restore state
        # In a real implementation, this would restore the actual brain state
        # For this example, we'll just log it
        
        self.simulation_history.append({
            'action': 'checkpoint_load',
            'name': name,
            'timestamp': time.time(),
            'status': 'loaded'
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.BRAIN_LOADED,
            time.time(),
            data={"checkpoint_name": name, "checkpoint": checkpoint}
        ))
        
        return checkpoint
    
    def start_recording(self, filename: Optional[str] = None):
        """Start recording simulation data"""
        self.record_mode = True
        self.record_data = []
        
        self.simulation_history.append({
            'action': 'recording_start',
            'timestamp': time.time(),
            'filename': filename
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.SIMULATION_STARTED,
            time.time(),
            data={"recording": True, "filename": filename}
        ))
    
    def stop_recording(self) -> List[Dict[str, Any]]:
        """Stop recording and return recorded data"""
        self.record_mode = False
        
        # Save recording if filename was specified
        # In a real implementation, this would save to file
        
        self.simulation_history.append({
            'action': 'recording_stop',
            'timestamp': time.time(),
            'data_points': len(self.record_data)
        })
        
        self.context.emit_event(ScriptingEvent(
            EventType.SIMULATION_STOPPED,
            time.time(),
            data={"recording": False, "data_points": len(self.record_data)}
        ))
        
        return self.record_data
    
    async def replay_simulation(self, data: List[Dict[str, Any]], 
                              speed: float = 1.0) -> Dict[str, Any]:
        """Replay simulation from recorded data"""
        self.stop_recording()
        self.playback_speed = speed
        self.record_mode = False
        
        # Clear current state
        await self.reset_simulation(keep_config=True)
        
        # Replay recorded steps
        steps_replayed = 0
        for step_data in data:
            # In a real implementation, this would replay actual brain state
            # For this example, we'll just simulate the step
            await asyncio.sleep(0.01)  # Simulate processing time
            steps_replayed += 1
        
        self.simulation_history.append({
            'action': 'replay',
            'timestamp': time.time(),
            'speed': speed,
            'steps_replayed': steps_replayed,
            'data_points': len(data)
        })
        
        return {
            "status": "replayed",
            "steps": steps_replayed,
            "speed": speed
        }
    
    def get_simulation_stats(self) -> Dict[str, Any]:
        """Get simulation statistics"""
        return {
            'current_mode': self.context.mode,
            'simulation_paused': self.simulation_paused,
            'pause_reason': self.pause_reason,
            'speed_factor': self.speed_factor,
            'record_mode': self.record_mode,
            'checkpoint_count': len(self.checkpoints),
            'simulation_history_length': len(self.simulation_history),
            'record_data_points': len(self.record_data)
        }
    
    def get_simulation_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get simulation history"""
        return self.simulation_history[-limit:]
    
    def set_speed(self, factor: float):
        """Set simulation speed factor"""
        if factor <= 0:
            raise ValueError("Speed factor must be positive")
        
        self.speed_factor = factor
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"speed_factor": factor}
        ))
    
    def clear_history(self):
        """Clear simulation history"""
        self.simulation_history = []
        self.record_data = []
        self.checkpoints = {}
        
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={"action": "history_cleared"}
        ))


class StepController:
    """Controller for step-by-step simulation control"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.step_history = []
        self.max_history = 1000
    
    async def execute_step(self, dt: float = 1.0, 
                          validate: bool = True) -> Dict[str, Any]:
        """Execute a single step with validation"""
        start_time = time.time()
        
        # Get pre-step state
        pre_state = self.interface.get_state() if validate else None
        
        # Execute step
        result = await self.interface.run_step(dt, auto=True)
        
        # Get post-step state
        post_state = self.interface.get_state() if validate else None
        
        step_record = {
            'step_number': len(self.step_history) + 1,
            'timestamp': start_time,
            'duration': time.time() - start_time,
            'dt': dt,
            'pre_state': pre_state,
            'post_state': post_state,
            'result': result
        }
        
        self.step_history.append(step_record)
        
        # Trim history if needed
        if len(self.step_history) > self.max_history:
            self.step_history = self.step_history[-self.max_history:]
        
        return step_record
    
    async def execute_steps(self, steps: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Execute multiple steps"""
        results = []
        
        for step_params in steps:
            dt = step_params.get('dt', 1.0)
            step_result = await self.execute_step(dt)
            results.append(step_result)
        
        return results
    
    def get_step_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get step history"""
        return self.step_history[-limit:]
    
    def get_step_by_number(self, step_number: int) -> Optional[Dict[str, Any]]:
        """Get step by number"""
        if 1 <= step_number <= len(self.step_history):
            return self.step_history[step_number - 1]
        return None


class SimulationExporter:
    """Export simulation data to various formats"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
    
    def export_to_json(self, data: Dict[str, Any], filepath: str) -> bool:
        """Export data to JSON file"""
        import json
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Data exported to JSON: {filepath}")
        
        return True
    
    def export_to_csv(self, data: Dict[str, Any], filepath: str) -> bool:
        """Export data to CSV file"""
        try:
            import pandas as pd
            
            # Convert data to DataFrame
            df = pd.json_normalize(data)
            df.to_csv(filepath, index=False)
            
            logger = __import__('logging').getLogger(__name__)
            logger.info(f"Data exported to CSV: {filepath}")
            
            return True
        except ImportError:
            raise ImportError("Pandas is required for CSV export")
    
    def export_to_matlab(self, data: Dict[str, Any], filepath: str) -> bool:
        """Export data to MATLAB file"""
        try:
            import scipy.io
            
            # Convert to MATLAB-compatible format
            matlab_data = {}
            for key, value in data.items():
                if isinstance(value, dict):
                    matlab_data[key] = value
                elif isinstance(value, list):
                    matlab_data[key] = value
                elif isinstance(value, (int, float, str)):
                    matlab_data[key] = value
            
            scipy.io.savemat(filepath, matlab_data)
            
            logger = __import__('logging').getLogger(__name__)
            logger.info(f"Data exported to MATLAB: {filepath}")
            
            return True
        except ImportError:
            raise ImportError("SciPy is required for MATLAB export")
    
    def export_to_numpy(self, data: Dict[str, Any], filepath: str) -> bool:
        """Export data to NumPy .npz file"""
        try:
            import numpy as np
            
            np.savez(filepath, **data)
            
            logger = __import__('logging').getLogger(__name__)
            logger.info(f"Data exported to NumPy: {filepath}")
            
            return True
        except ImportError:
            raise ImportError("NumPy is required for NumPy export")
