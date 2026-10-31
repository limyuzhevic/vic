"""
World simulation module for NLM Python bindings.
"""

import time
import math
from typing import List, Dict, Any, Optional, Tuple
from enum import Enum

try:
    from pynlm.bindings import SimpleWorld, createSimpleWorld, Vision, Audio, InternalSignals
    from pynlm.bindings import WorldObject, WorldObjectType, Action
    from pynlm.bindings import ActionType, MotorCommand, AgentBody
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. World module limited.")

# Import exceptions and utilities
from .exceptions import NLMInitializationError, NLMExecutionError
from .core import NLMConfigurationError, format_number, calculate_statistics, generate_report


class WorldStatus(Enum):
    """World status states."""
    READY = "ready"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    ERROR = "error"


class WorldEvent:
    """Event in world simulation."""
    
    def __init__(self, event_type: str, timestamp: float, data: Dict[str, Any] = None):
        self.type = event_type
        self.timestamp = timestamp
        self.data = data or {}
    
    def __repr__(self) -> str:
        return f"WorldEvent({self.type} at {self.timestamp:.2f})"


class WorldSimulation:
    """Enhanced world simulation with event handling and statistics."""
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """
        Initialize enhanced world simulation.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        if not BINDINGS_AVAILABLE:
            raise NLMInitializationError("C++ bindings not available")
        
        self.world = createSimpleWorld()
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Configuration
        self.max_energy = 100.0
        self.energy_decay_rate = 0.01
        self.random_seed = 42
        
        # Event handling
        self.event_handlers = {}
        self.events = []
        self.status = WorldStatus.READY
        
        # Statistics
        self.stats = {
            'steps_completed': 0,
            'objects_created': 0,
            'objects_removed': 0,
            'total_rewards': 0.0,
            'start_time': 0.0,
            'end_time': 0.0,
        }
        
        # Configure world
        self.configure()
        self.reset()
    
    def configure(self, width: int = None, height: int = None,
                  vision_width: int = None, vision_height: int = None,
                  max_energy: float = None, energy_decay_rate: float = None):
        """
        Configure world parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
            max_energy: Maximum agent energy
            energy_decay_rate: Energy decay rate per step
        """
        if width is not None:
            self.width = width
        if height is not None:
            self.height = height
        if vision_width is not None:
            self.vision_width = vision_width
        if vision_height is not None:
            self.vision_height = vision_height
        
        self.world.configure(self.width, self.height, self.vision_width, self.vision_height)
        
        if max_energy is not None:
            self.max_energy = max_energy
            self.world.setMaxEnergy(self.max_energy)
        
        if energy_decay_rate is not None:
            self.energy_decay_rate = energy_decay_rate
            self.world.setEnergyDecayRate(self.energy_decay_rate)
    
    def reset(self):
        """Reset world to initial state."""
        self.world.reset()
        self.events.clear()
        self.status = WorldStatus.READY
        self.stats['steps_completed'] = 0
        self.stats['objects_created'] = 0
        self.stats['objects_removed'] = 0
        self.stats['total_rewards'] = 0.0
        self.world.setRandomSeed(self.random_seed)
    
    def start(self):
        """Start world simulation."""
        self.status = WorldStatus.RUNNING
        self.stats['start_time'] = time.time()
        self._trigger_event('world_started', self.stats['start_time'])
    
    def stop(self):
        """Stop world simulation."""
        self.status = WorldStatus.STOPPED
        self.stats['end_time'] = time.time()
        self._trigger_event('world_stopped', self.stats['end_time'])
    
    def pause(self):
        """Pause world simulation."""
        self.status = WorldStatus.PAUSED
        self._trigger_event('world_paused', time.time())
    
    def resume(self):
        """Resume world simulation."""
        if self.status == WorldStatus.PAUSED:
            self.status = WorldStatus.RUNNING
            self._trigger_event('world_resumed', time.time())
    
    def update(self, timestep: float = 0.1):
        """
        Update world by timestep.
        
        Args:
            timestep: Time duration to advance
        """
        if self.status != WorldStatus.RUNNING:
            return
        
        try:
            self.world.update(timestep)
            self.stats['steps_completed'] += 1
            
            # Decay agent energy
            body = self.world.getAgentBody()
            body.energy = max(0.0, body.energy * (1.0 - self.energy_decay_rate * timestep))
            
            self._trigger_event('step_completed', {
                'step': self.stats['steps_completed'],
                'timestep': timestep,
                'simulation_time': self.world.getSimulationTime(),
            })
            
        except Exception as e:
            self.status = WorldStatus.ERROR
            self._trigger_event('world_error', str(e))
            raise NLMExecutionError(f"World update failed: {e}")
    
    def add_object(self, obj: WorldObject):
        """Add object to world."""
        self.world.addObject(obj)
        self.stats['objects_created'] += 1
        self._trigger_event('object_added', obj)
    
    def remove_object(self, x: float, y: float):
        """Remove object at position."""
        self.world.removeObject(x, y)
        self.stats['objects_removed'] += 1
        self._trigger_event('object_removed', {'x': x, 'y': y})
    
    def apply_motor_command(self, motor_cmd: MotorCommand, current_time: float):
        """Apply motor command to agent."""
        self.world.applyMotorCommand(motor_cmd, current_time)
        
        # Check for events
        body = self.world.getAgentBody()
        if body.x < 0 or body.x >= self.width or body.y < 0 or body.y >= self.height:
            self._trigger_event('agent_boundary_crossed', {
                'position': (body.x, body.y),
                'bounds': (self.width, self.height),
            })
    
    def get_sensory_percept(self):
        """Get agent's sensory percept."""
        return self.world.getSensoryPercept()
    
    def get_agent_body(self) -> AgentBody:
        """Get agent body state."""
        return self.world.getAgentBody()
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        self.world.setAgentStart(x, y)
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """Add event handler for specific event type."""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
    
    def remove_event_handler(self, event_type: str, handler: Callable):
        """Remove event handler."""
        if event_type in self.event_handlers:
            try:
                self.event_handlers[event_type].remove(handler)
            except ValueError:
                pass
    
    def _trigger_event(self, event_type: str, data: Any = None):
        """Trigger event handlers."""
        event = WorldEvent(event_type, time.time(), data)
        self.events.append(event)
        
        if event_type in self.event_handlers:
            for handler in self.event_handlers[event_type]:
                try:
                    handler(event)
                except Exception as e:
                    print(f"Event handler error for {event_type}: {e}")
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world statistics."""
        stats = self.stats.copy()
        
        # Add simulation time
        stats['simulation_time'] = self.world.getSimulationTime()
        
        # Add agent state
        body = self.world.getAgentBody()
        stats['agent'] = {
            'x': body.x,
            'y': body.y,
            'orientation': body.orientation,
            'velocity_x': body.velocityX,
            'velocity_y': body.velocityY,
            'energy': body.energy,
            'health': body.health,
            'age': body.age,
            'is_moving': body.isMoving,
            'is_turning': body.isTurning,
        }
        
        # Add world dimensions
        stats['dimensions'] = {
            'width': self.world.getWidth(),
            'height': self.world.getHeight(),
            'max_energy': self.world.getMaxEnergy(),
            'energy_decay_rate': self.world.getEnergyDecayRate(),
        }
        
        # Calculate duration
        if stats['start_time'] and stats['end_time']:
            stats['duration'] = stats['end_time'] - stats['start_time']
        elif stats['start_time']:
            stats['duration'] = time.time() - stats['start_time']
        else:
            stats['duration'] = 0.0
        
        return stats
    
    def get_events(self, event_type: str = None) -> List[WorldEvent]:
        """Get events, optionally filtered by type."""
        if event_type:
            return [e for e in self.events if e.type == event_type]
        return self.events.copy()
    
    def clear_events(self):
        """Clear all events."""
        self.events.clear()
    
    def __repr__(self) -> str:
        return f"WorldSimulation(width={self.width}, height={self.height}, status={self.status})"


def create_world_from_config(config_dict: Dict[str, Any]):
    """Create world from configuration dictionary."""
    world = WorldSimulation()
    
    # Apply configuration
    if 'width' in config_dict:
        world.configure(width=config_dict['width'])
    if 'height' in config_dict:
        world.configure(height=config_dict['height'])
    if 'vision_width' in config_dict:
        world.configure(vision_width=config_dict['vision_width'])
    if 'vision_height' in config_dict:
        world.configure(vision_height=config_dict['vision_height'])
    if 'max_energy' in config_dict:
        world.configure(max_energy=config_dict['max_energy'])
    
    return world


def create_world_with_agent(**kwargs):
    """Create world and agent with common configuration."""
    from pynlm.bindings import createAgentBrain
    
    world = WorldSimulation()
    
    # Get agent start position
    start_x = kwargs.get('agent_start_x', world.width // 2)
    start_y = kwargs.get('agent_start_y', world.height // 2)
    world.set_agent_start(start_x, start_y)
    
    # Create brain
    config = configure_default()
    from pynlm.bindings import createBrain
    brain = createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world.world)
    
    # Enable subsystems based on kwargs
    agent.enableRewardModulation(kwargs.get('reward_modulation', True))
    agent.enableStructuralPlasticity(kwargs.get('structural_plasticity', True))
    agent.enableDevelopment(kwargs.get('development', True))
    agent.enableCuriosity(kwargs.get('curiosity', True))
    
    return brain, world, agent


def export_world_data(world, agent, steps: int = 0, filepath: str = "world_data.json"):
    """Export world data to file."""
    import json
    
    data = {
        'world_stats': world.get_stats(),
        'simulation_time': world.world.getSimulationTime(),
        'agent_body': {
            'x': world.get_agent_body().x,
            'y': world.get_agent_body().y,
            'orientation': world.get_agent_body().orientation,
            'energy': world.get_agent_body().energy,
            'health': world.get_agent_body().health,
        },
        'steps_completed': world.stats['steps_completed'],
    }
    
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)


def import_world_data(filepath: str) -> Dict[str, Any]:
    """Import world data from file."""
    import json
    
    with open(filepath, 'r') as f:
        return json.load(f)
