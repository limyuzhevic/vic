"""Agent implementation with Pythonic interface"""

import time
from typing import List, Dict, Any, Optional
from dataclasses import dataclass
import numpy as np

@dataclass
class AgentState:
    """Agent state information."""
    
    position_x: float = 0.0
    position_y: float = 0.0
    orientation: float = 0.0
    velocity_x: float = 0.0
    velocity_y: float = 0.0
    energy: float = 1.0
    health: float = 1.0
    age: float = 0.0
    is_moving: bool = False
    is_turning: bool = False
class Agent:
    """Enhanced agent implementation with Pythonic interface."""
    
    def __init__(self, brain, world=None):
        self._brain = brain
        self._world = world
        self._state = AgentState()
        self._sensory_percept = None
        self._action_history: List[Dict[str, Any]] = []
        self._reward_history: List[float] = []
        self._novelty_history: List[float] = []
        self._curiosity_history: List[float] = []
        self._prediction_error_history: List[float] = []
    
    def initialize(self, world):
        """Initialize agent with world."""
        self._world = world
        self._reset_for_episode()
        return self
    
    def _reset_for_episode(self):
        """Reset agent state for new episode."""
        self._state = AgentState()
        self._sensory_percept = None
        self._action_history.clear()
        self._reward_history.clear()
        self._novelty_history.clear()
        self._curiosity_history.clear()
        self._prediction_error_history.clear()
    
    def update(self, timestep: float = 0.1):
        """Update agent state."""
        self._state.age += timestep
        
        # Update energy decay
        self._state.energy *= (1.0 - 0.01 * timestep)  # 1% energy decay per second
        
        # Clamp energy to valid range
        self._state.energy = max(0.0, min(1.0, self._state.energy))
        
        # Update health decay
        self._state.health *= (1.0 - 0.005 * timestep)  # 0.5% health decay per second
        
        return self
    
    def process_sensory_input(self, percept):
        """Process sensory input from environment."""
        self._sensory_percept = percept
        
        # Update agent state based on sensory input
        if hasattr(percept, 'get_vision'):
            vision = percept.get_vision()
            if vision:
                # Simple center-of-mass calculation
                total_weight = sum(vision)
                if total_weight > 0:
                    cx = sum(i * v for i, v in enumerate(vision)) / total_weight
                    cy = sum((i // 8) * v for i, v in enumerate(vision)) / total_weight  # Assuming 8x8 vision
                    
                    # Update position towards center of mass
                    target_x = cx * 2.0 - 1.0  # Map to [-1, 1]
                    target_y = cy * 2.0 - 1.0
                    
                    dx = target_x - self._state.position_x
                    dy = target_y - self._state.position_y
                    distance = np.sqrt(dx * dx + dy * dy)
                    
                    if distance > 0.01:
                        move_speed = min(0.1 * timestep, distance)
                        self._state.position_x += dx * move_speed / distance
                        self._state.position_y += dy * move_speed / distance
                        self._state.is_moving = True
        
        # Update brain through agent brain interface
        if hasattr(self._brain, 'processSensoryInput'):
            self._brain.processSensoryInput(percept)
        
        return self
    
    def decode_motor_command(self):
        """Decode motor command from brain."""
        if not self._brain:
            return None
        
        # Get motor command from brain
        if hasattr(self._brain, 'decodeMotorCommand'):
            motor_cmd = self._brain.decodeMotorCommand()
            
            # Record action
            action_record = {
                'timestamp': time.time(),
                'action': str(motor_cmd) if motor_cmd else None,
                'position': (self._state.position_x, self._state.position_y),
                'energy': self._state.energy
            }
            self._action_history.append(action_record)
            
            # Apply action to agent state
            self._apply_motor_command(motor_cmd)
            
            return motor_cmd
        
        return None
    
    def _apply_motor_command(self, motor_cmd):
        """Apply motor command to agent state."""
        if not motor_cmd:
            return
        
        move_speed = 0.2
        turn_speed = 0.1
        
        # Map motor command to movement
        if hasattr(motor_cmd, 'MoveForward'):
            self._state.velocity_x += move_speed * np.cos(self._state.orientation)
            self._state.velocity_y += move_speed * np.sin(self._state.orientation)
            self._state.is_moving = True
        
        elif hasattr(motor_cmd, 'MoveBackward'):
            self._state.velocity_x -= move_speed * np.cos(self._state.orientation)
            self._state.velocity_y -= move_speed * np.sin(self._state.orientation)
            self._state.is_moving = True
        
        elif hasattr(motor_cmd, 'TurnLeft'):
            self._state.orientation -= turn_speed
            self._state.is_turning = True
        
        elif hasattr(motor_cmd, 'TurnRight'):
            self._state.orientation += turn_speed
            self._state.is_turning = True
        
        # Apply velocity damping
        self._state.velocity_x *= 0.95
        self._state.velocity_y *= 0.95
        
        # Update position
        self._state.position_x += self._state.velocity_x
        self._state.position_y += self._state.velocity_y
        
        # Clamp position to reasonable bounds
        self._state.position_x = max(-10.0, min(10.0, self._state.position_x))
        self._state.position_y = max(-10.0, min(10.0, self._state.position_y))
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0):
        """Apply reward modulation to brain."""
        if self._brain and hasattr(self._brain, 'applyRewardModulation'):
            self._brain.applyRewardModulation(reward, predicted_reward)
        
        # Record reward
        self._reward_history.append(reward)
    
    def update_development(self, timestep: float = 0.1):
        """Update agent development."""
        if self._brain and hasattr(self._brain, 'updateDevelopment'):
            self._brain.updateDevelopment(timestep)
        
        # Update brain development
        if self._brain and hasattr(self._brain, 'getBrain'):
            brain = self._brain.getBrain()
            if brain and hasattr(brain, 'setDevelopmentalStage'):
                # Simple developmental progression
                age = self._state.age
                if age < 60:
                    brain.setDevelopmentalStage('Initial')
                elif age < 300:
                    brain.setDevelopmentalStage('CriticalPeriod')
                elif age < 900:
                    brain.setDevelopmentalStage('Maturation')
                else:
                    brain.setDevelopmentalStage('Adult')
    
    def reset(self):
        """Reset agent for new episode."""
        self._reset_for_episode()
        if self._brain and hasattr(self._brain, 'reset'):
            self._brain.reset()
        
        return self
    
    @property
    def position(self) -> tuple:
        """Get current agent position."""
        return (self._state.position_x, self._state.position_y)
    
    @position.setter
    def position(self, value: tuple):
        """Set agent position."""
        self._state.position_x, self._state.position_y = value
    
    @property
    def velocity(self) -> tuple:
        """Get current agent velocity."""
        return (self._state.velocity_x, self._state.velocity_y)
    
    @property
    def orientation(self) -> float:
        """Get current agent orientation."""
        return self._state.orientation
    
    @orientation.setter
    def orientation(self, value: float):
        """Set agent orientation."""
        self._state.orientation = value
    
    @property
    def energy(self) -> float:
        """Get current agent energy."""
        return self._state.energy
    
    @property
    def health(self) -> float:
        """Get current agent health."""
        return self._state.health
    
    @property
    def age(self) -> float:
        """Get current agent age."""
        return self._state.age
    
    @property
    def is_moving(self) -> bool:
        """Check if agent is moving."""
        return self._state.is_moving
    
    @property
    def is_turning(self) -> bool:
        """Check if agent is turning."""
        return self._state.is_turning
    
    @property
    def action_count(self) -> int:
        """Get number of actions taken."""
        return len(self._action_history)
    
    @property
    def reward_history(self) -> List[float]:
        """Get reward history."""
        return self._reward_history.copy()
    
    @property
    def total_reward(self) -> float:
        """Get total reward."""
        return sum(self._reward_history)
    
    @property
    def average_reward(self) -> float:
        """Get average reward."""
        return self.total_reward / max(1, len(self._reward_history))
    
    def get_sensory_percept(self):
        """Get current sensory percept."""
        return self._sensory_percept
    
    def get_agent_stats(self) -> Dict[str, Any]:
        """Get comprehensive agent statistics."""
        stats = {
            'position': self.position,
            'velocity': self.velocity,
            'orientation': self.orientation,
            'energy': self.energy,
            'health': self.health,
            'age': self.age,
            'is_moving': self.is_moving,
            'is_turning': self.is_turning,
            'action_count': self.action_count,
            'total_reward': self.total_reward,
            'average_reward': self.average_reward,
            'reward_history_length': len(self._reward_history)
        }
        
        # Add brain-related stats if available
        if self._brain:
            if hasattr(self._brain, 'getBrain'):
                brain = self._brain.getBrain()
                if brain:
                    stats['brain'] = {
                        'total_neurons': brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0,
                        'total_synapses': brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0,
                        'firing_neurons': brain.getFiringNeuronCount() if hasattr(brain, 'getFiringNeuronCount') else 0,
                        'average_firing_rate': brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0,
                        'e_i_ratio': brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 0
                    }
            
            # Add agent brain specific stats
            if hasattr(self._brain, 'getNeuromodulationLevel'):
                stats['neuromodulation'] = self._brain.getNeuromodulationLevel()
            
            if hasattr(self._brain, 'getCuriosityLevel'):
                stats['curiosity'] = self._brain.getCuriosityLevel()
            
            if hasattr(self._brain, 'getNoveltyLevel'):
                stats['novelty'] = self._brain.getNoveltyLevel()
            
            if hasattr(self._brain, 'getPredictionError'):
                stats['prediction_error'] = self._brain.getPredictionError()
            
            if hasattr(self._brain, 'getDevelopmentalStage'):
                stats['developmental_stage'] = self._brain.getDevelopmentalStage()
        
        return stats
    
    def __repr__(self):
        return f"<Agent(position={self.position}, age={self.age:.1f}s, energy={self.energy:.2f})>"
    
    def __str__(self):
        return f"Agent at {self.position}, age {self.age:.1f}s, energy {self.energy:.2f}"


# Factory function

def create_agent(brain, world=None) -> Agent:
    """Create an agent with brain and optional world."""
    return Agent(brain, world)


def create_test_agent(world=None) -> Agent:
    """Create a test agent with minimal configuration."""
    # This would need to create a simple brain, which is complex
    # For now, return None and let user create properly configured agent
    raise NotImplementedError("create_test_agent requires a proper brain configuration")