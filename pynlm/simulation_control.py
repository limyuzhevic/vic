"""
Advanced simulation control for NLM

This module provides advanced simulation control capabilities including
real-time simulation control, pausing/resuming simulations, checkpoint/restart,
and simulation parameter optimization.
"""

import time
import threading
import queue
import os
import json
import pickle
from typing import Dict, List, Any, Optional, Union, Callable, Tuple
from enum import Enum
from dataclasses import dataclass
from pathlib import Path
from abc import ABC, abstractmethod

class SimulationState(Enum):
    """Simulation state enumeration."""
    STOPPED = "stopped"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    ERROR = "error"

class ControlMode(Enum):
    """Control mode enumeration."""
    MANUAL = "manual"
    AUTOMATIC = "automatic"
    SCHEDULED = "scheduled"
    CONTINUOUS = "continuous"

@dataclass
class Checkpoint:
    """Simulation checkpoint."""
    
    checkpoint_id: str
    timestamp: float
    brain_state: Any
    world_state: Any
    agent_state: Any
    simulation_step: int
    simulation_time: float
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class ControlCommand:
    """Control command for simulation."""
    
    command_type: str
    parameters: Dict[str, Any]
    priority: int = 1
    timestamp: float = None
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = time.time()

class AdvancedSimulationController:
    """Advanced simulation controller for NLM."""
    
    def __init__(self, brain: Any, world: Any, agent: Any, config: Optional[Dict[str, Any]] = None):
        """Initialize simulation controller.
        
        Args:
            brain: Brain instance
            world: World instance
            agent: Agent instance
            config: Configuration for simulation control
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.config = config or {}
        
        # Simulation control
        self.state = SimulationState.STOPPED
        self.current_step = 0
        self.current_time = 0.0
        self.control_mode = ControlMode.MANUAL
        self.control_commands = []
        self.command_queue = queue.Queue()
        
        # Checkpoints
        self.checkpoints_dir = self.config.get('checkpoints_dir', './checkpoints')
        self.max_checkpoints = self.config.get('max_checkpoints', 10)
        self.checkpoint_interval = self.config.get('checkpoint_interval', 100)
        self.checkpoints: List[Checkpoint] = []
        
        # Real-time monitoring
        self.monitoring_active = False
        self.monitoring_interval = self.config.get('monitoring_interval', 0.1)
        self.performance_metrics = {}
        self.error_handler = self.config.get('error_handler', self._default_error_handler)
        
        # Performance optimization
        self.optimizer = self.config.get('optimizer', None)
        self.optimization_rules = self.config.get('optimization_rules', {})
        
        # Statistics
        self.start_time = 0.0
        self.end_time = 0.0
        self.total_steps = 0
        self.step_times = []
        
        # Setup
        self._setup()
    
    def _setup(self):
        """Setup simulation controller."""
        # Create checkpoints directory
        Path(self.checkpoints_dir).mkdir(parents=True, exist_ok=True)
        
        # Start monitoring if enabled
        if self.config.get('enable_monitoring', True):
            self.start_monitoring()
    
    def start_simulation(self, initial_step: int = 0, initial_time: float = 0.0):
        """Start simulation.
        
        Args:
            initial_step: Initial step number
            initial_time: Initial simulation time
        """
        self.state = SimulationState.RUNNING
        self.current_step = initial_step
        self.current_time = initial_time
        self.start_time = time.time()
        
        if self.config.get('enable_logging', True):
            print(f"Simulation started at step {initial_step}, time {initial_time:.2f}s")
    
    def stop_simulation(self):
        """Stop simulation."""
        if self.state == SimulationState.RUNNING:
            self.state = SimulationState.STOPPED
            self.end_time = time.time()
            self._save_last_checkpoint()
            
            if self.config.get('enable_logging', True):
                duration = self.end_time - self.start_time
                print(f"Simulation stopped. Duration: {duration:.2f}s, Total steps: {self.current_step}")
    
    def pause_simulation(self):
        """Pause simulation."""
        if self.state == SimulationState.RUNNING:
            self.state = SimulationState.PAUSED
            if self.config.get('enable_logging', True):
                print(f"Simulation paused at step {self.current_step}, time {self.current_time:.2f}s")
    
    def resume_simulation(self):
        """Resume simulation."""
        if self.state == SimulationState.PAUSED:
            self.state = SimulationState.RUNNING
            if self.config.get('enable_logging', True):
                print(f"Simulation resumed at step {self.current_step}, time {self.current_time:.2f}s")
    
    def step_simulation(self, timestep: float = 0.1, current_time: Optional[float] = None):
        """Execute one simulation step.
        
        Args:
            timestep: Time step duration
            current_time: Current simulation time (calculated if None)
            
        Returns:
            True if step executed, False if simulation should stop
        """
        if self.state != SimulationState.RUNNING:
            return False
        
        step_start_time = time.time()
        
        try:
            # Check for control commands
            command = self._get_next_command()
            if command is not None:
                self._execute_command(command)
            
            # Execute simulation step
            self._execute_step(timestep, current_time)
            
            # Update step timing
            step_time = time.time() - step_start_time
            self.step_times.append(step_time)
            self.current_step += 1
            self.current_time += timestep
            self.total_steps += 1
            
            # Save checkpoint if interval reached
            if self.current_step % self.checkpoint_interval == 0:
                self._save_checkpoint()
            
            # Run performance optimization
            if self.optimizer:
                self._optimize_performance()
            
            # Update performance metrics
            self._update_performance_metrics()
            
            return True
        
        except Exception as e:
            error_result = self.error_handler(e, self.current_step, self.current_time)
            if error_result == 'stop':
                self.state = SimulationState.ERROR
                return False
            elif error_result == 'continue':
                return self._execute_step(timestep, current_time)
            else:
                self.state = SimulationState.ERROR
                return False
    
    def add_control_command(self, command_type: str, parameters: Dict[str, Any], 
                           priority: int = 1):
        """Add control command.
        
        Args:
            command_type: Type of command
            parameters: Command parameters
            priority: Command priority
        """
        command = ControlCommand(
            command_type=command_type,
            parameters=parameters,
            priority=priority
        )
        
        # Insert into command queue with priority
        self.command_queue.put((-priority, command))  # Negative for high priority
    
    def get_simulation_status(self) -> Dict[str, Any]:
        """Get simulation status.
        
        Returns:
            Simulation status dictionary
        """
        status = {
            'state': self.state.value,
            'current_step': self.current_step,
            'current_time': self.current_time,
            'control_mode': self.control_mode.value,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'total_steps': self.total_steps,
        }
        
        if self.step_times:
            status['avg_step_time'] = sum(self.step_times) / len(self.step_times)
            status['min_step_time'] = min(self.step_times)
            status['max_step_time'] = max(self.step_times)
        
        if self.performance_metrics:
            status['performance_metrics'] = self.performance_metrics
        
        status['checkpoint_count'] = len(self.checkpoints)
        
        return status
    
    def save_checkpoint(self, name: Optional[str] = None):
        """Save simulation checkpoint.
        
        Args:
            name: Checkpoint name (generated if None)
        """
        if name is None:
            name = f"checkpoint_{self.current_step}_{int(time.time())}"
        
        checkpoint = Checkpoint(
            checkpoint_id=name,
            timestamp=time.time(),
            brain_state=self._save_brain_state(),
            world_state=self._save_world_state(),
            agent_state=self._save_agent_state(),
            simulation_step=self.current_step,
            simulation_time=self.current_time,
            metadata={
                'version': '1.0',
                'config': self.config,
                'optimizer_enabled': self.optimizer is not None,
            }
        )
        
        self.checkpoints.append(checkpoint)
        
        # Remove old checkpoints if limit exceeded
        if len(self.checkpoints) > self.max_checkpoints:
            self.checkpoints.pop(0)
        
        # Save to disk
        self._save_checkpoint_to_disk(checkpoint)
        
        if self.config.get('enable_logging', True):
            print(f"Checkpoint saved: {name} (step {self.current_step})")
    
    def load_checkpoint(self, name: str) -> bool:
        """Load simulation checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            True if checkpoint loaded, False otherwise
        """
        for checkpoint in self.checkpoints:
            if checkpoint.checkpoint_id == name:
                # Load checkpoint
                self._load_brain_state(checkpoint.brain_state)
                self._load_world_state(checkpoint.world_state)
                self._load_agent_state(checkpoint.agent_state)
                
                self.current_step = checkpoint.simulation_step
                self.current_time = checkpoint.simulation_time
                
                if self.config.get('enable_logging', True):
                    print(f"Checkpoint loaded: {name} (step {self.current_step})")
                
                return True
        
        if self.config.get('enable_logging', True):
            print(f"Checkpoint not found: {name}")
        
        return False
    
    def list_checkpoints(self) -> List[str]:
        """List available checkpoints.
        
        Returns:
            List of checkpoint names
        """
        return [checkpoint.checkpoint_id for checkpoint in self.checkpoints]
    
    def clear_checkpoints(self):
        """Clear all checkpoints."""
        self.checkpoints.clear()
        
        # Remove checkpoint files
        import shutil
        if Path(self.checkpoints_dir).exists():
            shutil.rmtree(self.checkpoints_dir)
        
        Path(self.checkpoints_dir).mkdir(parents=True, exist_ok=True)
        
        if self.config.get('enable_logging', True):
            print("All checkpoints cleared")
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get performance report.
        
        Returns:
            Performance report
        """
        report = {
            'simulation_summary': self.get_simulation_status(),
            'performance_metrics': self.performance_metrics,
            'step_statistics': self._get_step_statistics(),
            'checkpoint_statistics': self._get_checkpoint_statistics(),
            'optimization_opportunities': self._get_optimization_opportunities(),
        }
        
        return report
    
    def start_monitoring(self):
        """Start real-time monitoring."""
        if not self.monitoring_active:
            self.monitoring_active = True
            self._monitoring_thread = threading.Thread(target=self._monitoring_worker, daemon=True)
            self._monitoring_thread.start()
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitoring_active = False
        if hasattr(self, '_monitoring_thread'):
            self._monitoring_thread.join(timeout=1.0)
    
    def run_simulation_episode(self, steps: int, timestep: float = 0.1,
                              progress_callback: Optional[Callable] = None,
                              checkpoint_callback: Optional[Callable] = None):
        """Run complete simulation episode.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time step duration
            progress_callback: Progress callback function
            checkpoint_callback: Checkpoint callback function
        """
        self.start_simulation()
        
        try:
            for step in range(steps):
                # Execute step
                success = self.step_simulation(timestep)
                if not success:
                    break
                
                # Call progress callback
                if progress_callback:
                    progress = (step + 1) / steps * 100
                    callback_data = {
                        'step': step + 1,
                        'total_steps': steps,
                        'progress': progress,
                        'current_time': self.current_time,
                    }
                    progress_callback(callback_data)
                
                # Call checkpoint callback
                if checkpoint_callback and (step + 1) % 100 == 0:
                    checkpoint_data = {
                        'step': step + 1,
                        'current_time': self.current_time,
                        'checkpoint_id': f"episode_{step + 1}"
                    }
                    checkpoint_callback(checkpoint_data)
        
        except KeyboardInterrupt:
            print("Simulation interrupted by user")
            self.stop_simulation()
        
        except Exception as e:
            print(f"Simulation error: {e}")
            self.stop_simulation()
    
    # Internal methods
    
    def _execute_command(self, command: ControlCommand):
        """Execute control command.
        
        Args:
            command: Control command to execute
        """
        if command.command_type == 'pause':
            self.pause_simulation()
        elif command.command_type == 'resume':
            self.resume_simulation()
        elif command.command_type == 'stop':
            self.stop_simulation()
        elif command.command_type == 'step':
            self.step_simulation(command.parameters.get('timestep', 0.1))
        elif command.command_type == 'checkpoint':
            self.save_checkpoint(command.parameters.get('name'))
        elif command.command_type == 'load_checkpoint':
            self.load_checkpoint(command.parameters.get('name'))
        elif command.command_type == 'set_time':
            self.current_time = command.parameters.get('time', 0.0)
        elif command.command_type == 'set_step':
            self.current_step = command.parameters.get('step', 0)
    
    def _execute_step(self, timestep: float, current_time: Optional[float]):
        """Execute single simulation step.
        
        Args:
            timestep: Time step duration
            current_time: Current simulation time
        """
        if current_time is None:
            current_time = self.current_time + timestep
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process with agent
        self.agent.processSensoryInput(percept)
        
        # Update world
        self.world.update(timestep)
        
        # Update brain
        self.brain.step(self.current_step, current_time)
        
        # Update agent
        self.agent.update(timestep)
    
    def _get_next_command(self) -> Optional[ControlCommand]:
        """Get next control command.
        
        Returns:
            Next control command or None
        """
        if not self.command_queue.empty():
            priority, command = self.command_queue.get()
            return command
        return None
    
    def _save_brain_state(self) -> Any:
        """Save brain state.
        
        Returns:
            Brain state
        """
        if hasattr(self.brain, 'save'):
            import io
            buffer = io.BytesIO()
            pickle.dump(self.brain, buffer)
            return buffer.getvalue()
        return None
    
    def _load_brain_state(self, state: Any):
        """Load brain state.
        
        Args:
            state: Brain state
        """
        if state is not None:
            import io
            buffer = io.BytesIO(state)
            self.brain = pickle.load(buffer)
    
    def _save_world_state(self) -> Any:
        """Save world state.
        
        Returns:
            World state
        """
        if hasattr(self.world, 'save'):
            import io
            buffer = io.BytesIO()
            pickle.dump(self.world, buffer)
            return buffer.getvalue()
        return None
    
    def _load_world_state(self, state: Any):
        """Load world state.
        
        Args:
            state: World state
        """
        if state is not None:
            import io
            buffer = io.BytesIO(state)
            self.world = pickle.load(buffer)
    
    def _save_agent_state(self) -> Any:
        """Save agent state.
        
        Returns:
            Agent state
        """
        if hasattr(self.agent, 'save'):
            import io
            buffer = io.BytesIO()
            pickle.dump(self.agent, buffer)
            return buffer.getvalue()
        return None
    
    def _load_agent_state(self, state: Any):
        """Load agent state.
        
        Args:
            state: Agent state
        """
        if state is not None:
            import io
            buffer = io.BytesIO(state)
            self.agent = pickle.load(buffer)
    
    def _save_checkpoint(self):
        """Save current state as checkpoint."""
        self.save_checkpoint()
    
    def _save_last_checkpoint(self):
        """Save last state as checkpoint."""
        if self.current_step > 0:
            self.save_checkpoint(f"last_checkpoint_{self.current_step}")
    
    def _save_checkpoint_to_disk(self, checkpoint: Checkpoint):
        """Save checkpoint to disk.
        
        Args:
            checkpoint: Checkpoint to save
        """
        import json
        
        checkpoint_file = Path(self.checkpoints_dir) / f"{checkpoint.checkpoint_id}.json"
        
        checkpoint_data = {
            'checkpoint_id': checkpoint.checkpoint_id,
            'timestamp': checkpoint.timestamp,
            'simulation_step': checkpoint.simulation_step,
            'simulation_time': checkpoint.simulation_time,
            'brain_state': checkpoint.brain_state,
            'world_state': checkpoint.world_state,
            'agent_state': checkpoint.agent_state,
            'metadata': checkpoint.metadata,
        }
        
        with open(checkpoint_file, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
    
    def _default_error_handler(self, error: Exception, step: int, time: float) -> str:
        """Default error handler.
        
        Args:
            error: Exception that occurred
            step: Current simulation step
            time: Current simulation time
            
        Returns:
            Action to take ('stop', 'continue', 'retry')
        """
        print(f"Simulation error at step {step}, time {time:.2f}s: {error}")
        return 'stop'
    
    def _update_performance_metrics(self):
        """Update performance metrics."""
        if self.current_step % 10 == 0:  # Update every 10 steps
            metrics = {
                'step': self.current_step,
                'time': self.current_time,
                'step_time_avg': sum(self.step_times[-10:]) / len(self.step_times[-10:]) if self.step_times else 0,
                'performance': self._calculate_performance(),
            }
            self.performance_metrics[str(self.current_step)] = metrics
    
    def _calculate_performance(self) -> float:
        """Calculate performance score.
        
        Returns:
            Performance score
        """
        if not self.step_times:
            return 0.0
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        target_step_time = self.config.get('target_step_time', 0.01)
        
        # Calculate performance score (higher is better)
        performance = max(0, target_step_time / avg_step_time)
        return performance
    
    def _optimize_performance(self):
        """Optimize performance based on rules."""
        if not self.optimizer:
            return
        
        # Apply optimization rules
        for rule_name, rule_config in self.optimization_rules.items():
            if self._should_apply_rule(rule_config):
                self._apply_optimization_rule(rule_config)
    
    def _should_apply_rule(self, rule_config: Dict[str, Any]) -> bool:
        """Check if optimization rule should be applied.
        
        Args:
            rule_config: Rule configuration
            
        Returns:
            True if rule should be applied
        """
        conditions = rule_config.get('conditions', {})
        
        for condition_key, condition_value in conditions.items():
            current_value = self._get_metric_value(condition_key)
            if current_value is not None and current_value < condition_value:
                return True
        
        return False
    
    def _get_metric_value(self, metric_key: str) -> Optional[float]:
        """Get metric value.
        
        Args:
            metric_key: Metric key
            
        Returns:
            Metric value or None
        """
        if metric_key == 'step_time_avg':
            return sum(self.step_times[-10:]) / len(self.step_times[-10:]) if len(self.step_times) >= 10 else None
        elif metric_key == 'performance':
            return self._calculate_performance()
        elif metric_key == 'step_count':
            return float(self.current_step)
        
        return None
    
    def _apply_optimization_rule(self, rule_config: Dict[str, Any]):
        """Apply optimization rule.
        
        Args:
            rule_config: Rule configuration
        """
        optimizations = rule_config.get('optimizations', {})
        
        for optimization_key, optimization_value in optimizations.items():
            if optimization_key == 'timestep':
                # Adjust timestep
                current_timestep = self.config.get('timestep', 0.1)
                new_timestep = current_timestep * optimization_value
                self.config['timestep'] = new_timestep
            
            elif optimization_key == 'max_steps':
                # Adjust max steps
                current_max_steps = self.config.get('max_steps', 1000)
                self.config['max_steps'] = int(current_max_steps * optimization_value)
            
            elif optimization_key == 'priority':
                # Adjust priority
                current_priority = self.config.get('priority', 1)
                self.config['priority'] = min(current_priority * optimization_value, 10)
    
    def _monitoring_worker(self):
        """Background monitoring worker."""
        while self.monitoring_active:
            try:
                # Collect system metrics
                metrics = self._collect_system_metrics()
                
                # Update performance metrics
                self._update_performance_metrics_from_system(metrics)
                
                time.sleep(self.monitoring_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.monitoring_interval)
    
    def _collect_system_metrics(self) -> Dict[str, Any]:
        """Collect system metrics.
        
        Returns:
            System metrics
        """
        import psutil
        
        process = psutil.Process(os.getpid())
        
        return {
            'cpu_percent': process.cpu_percent(),
            'memory_percent': process.memory_percent(),
            'memory_rss': process.memory_info().rss,
            'memory_vms': process.memory_info().vms,
            'thread_count': process.num_threads(),
            'create_time': process.create_time(),
            'num_fds': process.num_fds() if hasattr(process, 'num_fds') else 0,
        }
    
    def _update_performance_metrics_from_system(self, metrics: Dict[str, Any]):
        """Update performance metrics from system metrics.
        
        Args:
            metrics: System metrics
        """
        timestamp = time.time()
        self.performance_metrics[str(timestamp)] = metrics
        
        # Keep only last N minutes of metrics
        cutoff_time = timestamp - 60  # 1 minute
        to_remove = []
        
        for key in list(self.performance_metrics.keys()):
            if float(key) < cutoff_time:
                to_remove.append(key)
        
        for key in to_remove:
            del self.performance_metrics[key]
    
    def _get_step_statistics(self) -> Dict[str, Any]:
        """Get step statistics.
        
        Returns:
            Step statistics
        """
        if not self.step_times:
            return {}
        
        return {
            'total_steps': len(self.step_times),
            'avg_step_time': sum(self.step_times) / len(self.step_times),
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'std_dev_step_time': (sum((x - sum(self.step_times) / len(self.step_times)) ** 2 for x in self.step_times) / len(self.step_times)) ** 0.5,
        }
    
    def _get_checkpoint_statistics(self) -> Dict[str, Any]:
        """Get checkpoint statistics.
        
        Returns:
            Checkpoint statistics
        """
        return {
            'total_checkpoints': len(self.checkpoints),
            'checkpoints_saved': sum(1 for c in self.checkpoints if c.simulation_step > 0),
            'last_checkpoint_step': self.checkpoints[-1].simulation_step if self.checkpoints else 0,
        }
    
    def _get_optimization_opportunities(self) -> List[str]:
        """Get optimization opportunities.
        
        Returns:
            List of optimization opportunities
        """
        opportunities = []
        
        if self.step_times:
            avg_step_time = sum(self.step_times) / len(self.step_times)
            target_step_time = self.config.get('target_step_time', 0.01)
            
            if avg_step_time > target_step_time * 2:
                opportunities.append(f"Step time ({avg_step_time:.4f}s) exceeds target ({target_step_time:.4f}s)")
        
        if self.performance_metrics:
            # Check for performance degradation
            recent_metrics = list(self.performance_metrics.values())[-10:]
            if len(recent_metrics) >= 2:
                recent_perf = [m.get('cpu_percent', 0) for m in recent_metrics]
                if len(recent_perf) >= 2 and recent_perf[-1] > recent_perf[0] * 1.5:
                    opportunities.append("CPU usage showing significant increase")
        
        if len(self.checkpoints) > self.max_checkpoints:
            opportunities.append("Checkpoint count exceeds maximum. Consider reducing checkpoint frequency.")
        
        return opportunities

# Create singleton instance for global use
_controller_instance: Optional[AdvancedSimulationController] = None

def get_simulation_controller(brain: Any, world: Any, agent: Any, 
                            config: Optional[Dict[str, Any]] = None) -> AdvancedSimulationController:
    """Get global simulation controller instance.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Agent instance
        config: Configuration for simulation controller
        
    Returns:
        AdvancedSimulationController instance
    """
    global _controller_instance
    if _controller_instance is None:
        _controller_instance = AdvancedSimulationController(brain, world, agent, config)
    return _controller_instance

def start_simulation(brain: Any, world: Any, agent: Any, steps: int, 
                    timestep: float = 0.1, config: Optional[Dict[str, Any]] = None):
    """Start simulation using global simulation controller.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Agent instance
        steps: Number of simulation steps
        timestep: Time step duration
        config: Configuration for simulation controller
    """
    controller = get_simulation_controller(brain, world, agent, config)
    controller.run_simulation_episode(steps, timestep)

# Export types and classes
__all__ = [
    'SimulationState',
    'ControlMode',
    'Checkpoint',
    'ControlCommand',
    'AdvancedSimulationController',
    'get_simulation_controller',
    'start_simulation',
]