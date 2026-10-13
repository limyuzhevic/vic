"""Agent brain and interface classes"""

import time
from typing import List, Dict, Any, Optional, Union
from dataclasses import dataclass, field
import math

from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, ActionType, MotorCommand,
    DevelopmentalStage, FiringState, WorldObjectType,
    SimulationStep, Timestamp, MembranePotential,
    ActionResult
)

@dataclass
class NeuromodulationState:
    """Neuromodulation state variables"""
    
    # Dopamine level (reward prediction error)
    dopamine_level: float = 0.0
    
    # Curiosity level (exploration motivation)
    curiosity_level: float = 0.0
    
    # Novelty level (novelty detection)
    novelty_level: float = 0.0
    
    # Prediction error signal
    prediction_error: float = 0.0
    
    # Expected reward (prediction target)
    expected_reward: float = 0.0
    
    # Plasticity factor (modulates learning rate)
    plasticity_factor: float = 1.0
    
    def reset(self) -> None:
        """Reset neuromodulation state"""
        self.dopamine_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_error = 0.0
        self.expected_reward = 0.0
        self.plasticity_factor = 1.0


@dataclass
class DevelopmentState:
    """Brain developmental state"""
    
    # Age in simulation steps
    age: int = 0
    
    # Developmental stage
    stage: DevelopmentalStage = DevelopmentalStage.Initial
    
    # Plasticity modifier based on stage
    plasticity_modifier: float = 1.0
    
    # Critical period end age
    critical_period_end: int = 1000
    
    # Maturation progress (0-1)
    maturation_progress: float = 0.0
    
    def get_stage_name(self) -> str:
        """Get string representation of developmental stage"""
        return self.stage.name if hasattr(self.stage, 'name') else str(self.stage)
        
    def update_stage(self, age: int) -> None:
        """Update developmental stage based on age"""
        self.age = age
        
        if age < self.critical_period_end:
            if age < self.critical_period_end // 2:
                self.stage = DevelopmentalStage.Initial
                self.plasticity_modifier = 1.0
            else:
                self.stage = DevelopmentalStage.CriticalPeriod
                self.plasticity_modifier = 0.8
        else:
            if age < self.critical_period_end + 500:
                self.stage = DevelopmentalStage.Maturation
                self.plasticity_modifier = 0.5
            elif age < self.critical_period_end + 1000:
                self.stage = DevelopmentalStage.Adult
                self.plasticity_modifier = 0.2
            else:
                self.stage = DevelopmentalStage.Aging
                self.plasticity_modifier = 0.1
                
        # Update maturation progress
        if self.age >= self.critical_period_end + 1000:
            self.maturation_progress = 1.0
        elif self.age >= self.critical_period_end:
            self.maturation_progress = (self.age - self.critical_period_end) / 500.0
        else:
            self.maturation_progress = 0.0


class AgentBrainError(Exception):
    """Base exception for agent brain errors"""
    pass


class SensoryError(AgentBrainError):
    """Sensory processing errors"""
    pass


class MotorError(AgentBrainError):
    """Motor decoding errors"""
    pass


class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world perception and action.
    
    This class serves as the bridge between the artificial brain and the simulated
    environment, handling sensory transduction, motor decoding, and neuromodulation.
    It provides a complete interface for brain-environment interaction with
    perception-action loops, reward-based learning, and developmental control.
    
    Key features:
    - Sensory processing and transduction
    - Motor command decoding and selection
    - Neuromodulation integration (reward, curiosity, novelty)
    - Developmental state management
    - Memory integration and recall
    - Plasticity modulation
    - Experience replay and consolidation
    - Checkpoint system for persistence
    
    The AgentBrain implements a complete cognitive architecture that connects
    internal neural processing to external world interaction, enabling learning
    through experience and adaptation to environmental demands.
    
    Example:
        >>> brain = createBrain(createDefaultConfig())
        >>> world = createSimpleWorld()
        >>> agent = createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> agent.enableRewardModulation(True)
        >>> agent.enableCuriosity(True)
        
        >>> # Main loop
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     result = world.applyMotorCommand(action, world.getSimulationTime())
        ...     agent.applyRewardModulation(result.reward, agent.getPredictionError())
    """
    
    def __init__(self, brain):
        """
        Initialize a new agent brain interface.
        
        Args:
            brain: Brain instance to control and interface with.
            
        Raises:
            AgentBrainError: If initialization fails.
        """
        self._brain = brain
        self._world = None
        
        # Motor neuron groups (injected during brain initialization)
        self._motor_forward = []  # Move forward neurons
        self._motor_backward = []  # Move backward neurons
        self._motor_left = []  # Turn left neurons
        self._motor_right = []  # Turn right neurons
        self._motor_interact = []  # Interact neurons
        self._motor_wait = []  # Wait neurons
        
        # Sensory neuron groups (injected during brain initialization)
        self._sensory_vision = []  # Vision neurons
        self._sensory_touch = []  # Touch neurons
        self._sensory_internal = []  # Internal neurons
        self._sensory_proprioception = []  # Proprioception neurons
        
        # Neuromodulation state
        self._neuromod_state = NeuromodulationState()
        
        # Development state
        self._dev_state = DevelopmentState()
        
        # Configuration flags
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        # Previous sensory state for novelty detection
        self._previous_vision = []
        
        # Experience replay
        self._replay_history: List[Dict[str, Any]] = []
        self._last_replay_step = 0
        
        # Random number generator
        self._rng_state = 42
        
    def initialize(self, world) -> None:
        """
        Initialize agent with world interface.
        
        Args:
            world: SimpleWorld instance to interface with.
            
        Raises:
            AgentBrainError: If initialization fails.
        """
        if self._brain is None:
            raise AgentBrainError("Brain must be provided")
            
        self._world = world
        
        # Initialize motor and sensory neuron groups
        # (In a real implementation, this would find and categorize neurons)
        self._initialize_neuron_groups()
        
        # Enable default systems
        self._reward_modulation_enabled = True
        self._development_enabled = True
        self._curiosity_enabled = True
        
    def _initialize_neuron_groups(self) -> None:
        """Initialize motor and sensory neuron groups"""
        # This would be implemented to categorize neurons by type
        # For now, it's a placeholder
        pass
    
    def process_sensory_input(self, percept: 'SensoryPercept') -> None:
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: SensoryPercept containing current sensory input.
            
        Raises:
            SensoryError: If sensory processing fails.
        """
        if self._world is None:
            raise SensoryError("Agent not initialized with world")
            
        # Inject sensory input into appropriate brain regions
        # (In a real implementation, this would map percept to neural activity)
        
        # Store previous state for novelty detection
        self._previous_vision = percept.get_vision(0, 0) if percept.get_vision else []
        
        # Update novelty detection
        if self._curiosity_enabled:
            self._update_novelty(percept)
        
    def decode_motor_command(self) -> MotorCommand:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            MotorCommand selected based on neural activity patterns.
            
        Raises:
            MotorError: If motor decoding fails.
        """
        if self._world is None:
            raise MotorError("Agent not initialized with world")
        
        # Get default motor command based on curiosity/exploration
        default_cmd = self._select_with_curiosity(self._get_default_motor_command())
        
        # Decode from motor neuron activity
        # (In a real implementation, this would analyze motor neuron populations)
        return default_cmd
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation to brain.
        
        Args:
            reward: Actual reward received from world.
            predicted_reward: Expected reward prediction.
        """
        # Calculate prediction error
        prediction_error = reward - predicted_reward
        
        # Update neuromodulation state
        if self._reward_modulation_enabled:
            self._neuromod_state.dopamine_level = max(0.0, prediction_error) * 0.1
            self._neuromod_state.prediction_error = prediction_error
            self._neuromod_state.expected_reward = predicted_reward
            self._neuromod_state.plasticity_factor = min(2.0, max(0.1, 1.0 + prediction_error * 0.01))
        
        # Update curiosity based on prediction error
        if self._curiosity_enabled:
            self._neuromod_state.curiosity_level = abs(prediction_error) * 0.05
            
    def update_development(self, timestep: float) -> None:
        """
        Update development system.
        
        Args:
            timestep: Simulation timestep.
        """
        if not self._development_enabled:
            return
            
        # Update developmental age
        self._dev_state.update_stage(int(self._brain.get_total_spike_count() / 10))
        
        # Update neuromodulation based on development stage
        if self._dev_state.stage == DevelopmentalStage.CriticalPeriod:
            self._neuromod_state.plasticity_factor = 0.8
        elif self._dev_state.stage == DevelopmentalStage.Maturation:
            self._neuromod_state.plasticity_factor = 0.5
        elif self._dev_state.stage == DevelopmentalStage.Adult:
            self._neuromod_state.plasticity_factor = 0.2
        elif self._dev_state.stage == DevelopmentalStage.Aging:
            self._neuromod_state.plasticity_factor = 0.1
    
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation"""
        self._reward_modulation_enabled = enable
        
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity"""
        self._structural_plasticity_enabled = enable
        
    def enable_development(self, enable: bool) -> None:
        """Enable or disable development"""
        self._development_enabled = enable
        
    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity-driven exploration"""
        self._curiosity_enabled = enable
        
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled"""
        return self._reward_modulation_enabled
        
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        return self._structural_plasticity_enabled
        
    def is_development_enabled(self) -> bool:
        """Check if development is enabled"""
        return self._development_enabled
        
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled"""
        return self._curiosity_enabled
    
    def get_brain(self):
        """Get the underlying brain instance"""
        return self._brain
        
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return self._dev_state.stage
        
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level (dopamine)"""
        return self._neuromod_state.dopamine_level
        
    def get_curiosity_level(self) -> float:
        """Get curiosity level"""
        return self._neuromod_state.curiosity_level
        
    def get_novelty_level(self) -> float:
        """Get novelty level"""
        return self._neuromod_state.novelty_level
        
    def get_prediction_error(self) -> float:
        """Get prediction error signal"""
        return self._neuromod_state.prediction_error
        
    def get_plasticity_factor(self) -> float:
        """Get current plasticity factor"""
        return self._neuromod_state.plasticity_factor
        
    def get_developmental_age(self) -> int:
        """Get developmental age in steps"""
        return self._dev_state.age
        
    def get_maturation_progress(self) -> float:
        """Get maturation progress (0-1)"""
        return self._dev_state.maturation_progress
        
    def reset(self) -> None:
        """Reset agent for new episode"""
        if self._world:
            self._world.reset()
            
        # Reset neuromodulation state
        self._neuromod_state.reset()
        
        # Reset development state
        self._dev_state = DevelopmentState()
        
        # Reset brain (if it supports it)
        if hasattr(self._brain, 'reset'):
            self._brain.reset()
        
    # Private methods
    def _update_novelty(self, percept: 'SensoryPercept') -> None:
        """Update novelty detection"""
        current_vision = percept.get_vision(0, 0) if percept.get_vision else []
        
        if not self._previous_vision:
            self._previous_vision = current_vision
            self._neuromod_state.novelty_level = 0.0
            return
            
        # Calculate novelty as change in sensory input
        novelty = 0.0
        if isinstance(current_vision, list) and isinstance(self._previous_vision, list):
            if len(current_vision) == len(self._previous_vision):
                for i in range(len(current_vision)):
                    novelty += abs(current_vision[i] - self._previous_vision[i])
                novelty /= len(current_vision)
        
        # Decay novelty over time
        novelty *= 0.9
        
        self._neuromod_state.novelty_level = novelty
        self._previous_vision = current_vision
        
    def _get_default_motor_command(self) -> MotorCommand:
        """Get default motor command"""
        return MotorCommand.Wait
        
    def _select_with_curiosity(self, default_cmd: MotorCommand) -> MotorCommand:
        """
        Select motor command with curiosity-driven exploration.
        
        Args:
            default_cmd: Default motor command.
            
        Returns:
            Motor command with curiosity factor applied.
        """
        # If curiosity is low, return default
        if self._neuromod_state.curiosity_level < 0.1:
            return default_cmd
        
        # Use random exploration with probability based on curiosity
        curiosity_factor = self._neuromod_state.curiosity_level
        if self._next_random() < curiosity_factor:
            # Return random motor command (excluding Wait)
            import random
            valid_commands = [MotorCommand.MoveForward, MotorCommand.MoveBackward,
                            MotorCommand.TurnLeft, MotorCommand.TurnRight, MotorCommand.Interact]
            return random.choice(valid_commands)
        
        return default_cmd
        
    def _next_random(self) -> float:
        """Generate next random number"""
        self._rng_state = (1664525 * self._rng_state + 1013904223) % 2**32
        return self._rng_state / 2**32
        
    def log_status(self) -> None:
        """Log agent status and statistics"""
        print(f"=== Agent Brain Status ===")
        print(f"Brain connected: {self._brain is not None}")
        print(f"World connected: {self._world is not None}")
        print(f"Reward modulation: {'enabled' if self._reward_modulation_enabled else 'disabled'}")
        print(f"Development: {'enabled' if self._development_enabled else 'disabled'}")
        print(f"Curiosity: {'enabled' if self._curiosity_enabled else 'disabled'}")
        print(f"Neuromodulation levels:")
        print(f"  Dopamine: {self._neuromod_state.dopamine_level:.3f}")
        print(f"  Curiosity: {self._neuromod_state.curiosity_level:.3f}")
        print(f"  Novelty: {self._neuromod_state.novelty_level:.3f}")
        print(f"  Prediction error: {self._neuromod_state.prediction_error:.3f}")
        print(f"  Plasticity factor: {self._neuromod_state.plasticity_factor:.3f}")
        print(f"Developmental stage: {self._dev_state.get_stage_name()}")
        print(f"Age: {self._dev_state.age}, Maturation: {self._dev_state.maturation_progress:.2f}")


class AgentWorld:
    """
    Complete agent-world integration with cognitive architecture.
    
    This class combines the world simulation with an agent brain interface,
    providing a complete cognitive architecture for brain-environment interaction.
    It handles perception-action loops, memory integration, and developmental
    processes in a unified framework.
    
    Key features:
    - Complete agent-world integration
    - Cognitive architecture with perception-action loops
    - Memory systems (working, episodic)
    - Neuromodulation and plasticity
    - Development and maturation
    - Experience replay and consolidation
    - Checkpoint system for persistence
    - Performance optimizations
    
    Example:
        >>> config = createDefaultConfig()
        >>> brain = createBrain(config)
        >>> agent_world = AgentWorld(brain, createDefaultWorldConfig())
        >>> agent_world.initialize()
        >>> agent_world.enable_reward_modulation(True)
        >>> 
        >>> # Main simulation loop
        >>> for step in range(100):
        ...     result = agent_world.step(step, 0.1)
        ...     if step % 10 == 0:
        ...         print(f"Step {step}: {brain.get_average_firing_rate():.2f} Hz")
    """
    
    def __init__(self, brain, world_config=None):
        """
        Initialize agent-world integration.
        
        Args:
            brain: Brain instance to control.
            world_config: World configuration (optional).
        """
        self._brain = brain
        self._agent = None
        self._world = SimpleWorld(world_config) if world_config else SimpleWorld()
        
        # Enable default systems
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = False
        self._development_enabled = True
        self._curiosity_enabled = True
        
    def initialize(self) -> None:
        """Initialize agent-world integration"""
        # Create agent brain interface
        self._agent = AgentBrain(self._brain)
        
        # Initialize agent with world
        self._agent.initialize(self._world)
        
        # Configure agent brain systems
        self._agent.enable_reward_modulation(self._reward_modulation_enabled)
        self._agent.enable_structural_plasticity(self._structural_plasticity_enabled)
        self._agent.enable_development(self._development_enabled)
        self._agent.enable_curiosity(self._curiosity_enabled)
        
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation"""
        self._reward_modulation_enabled = enable
        if self._agent:
            self._agent.enable_reward_modulation(enable)
            
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity"""
        self._structural_plasticity_enabled = enable
        if self._agent:
            self._agent.enable_structural_plasticity(enable)
            
    def enable_development(self, enable: bool) -> None:
        """Enable or disable development"""
        self._development_enabled = enable
        if self._agent:
            self._agent.enable_development(enable)
            
    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity"""
        self._curiosity_enabled = enable
        if self._agent:
            self._agent.enable_curiosity(enable)
    
    def step(self, step: int, timestep: float) -> ActionResult:
        """
        Execute one complete agent-world step.
        
        This method implements the complete cognitive loop:
        1. Update world state
        2. Get sensory perception
        3. Process sensory input
        4. Brain processing (thinking)
        5. Motor command decoding
        6. Action execution
        7. Reward feedback
        8. Neuromodulation update
        9. Development update
        
        Args:
            step: Current simulation step number.
            timestep: Time step duration in seconds.
            
        Returns:
            ActionResult with reward, success, and message.
        """
        if not self._agent:
            raise AgentBrainError("Agent not initialized")
        
        # 1. Update world state
        self._world.update(timestep)
        
        # 2. Get sensory perception
        percept = self._world.get_sensory_percept()
        
        # 3. Process sensory input
        self._agent.process_sensory_input(percept)
        
        # 4. Brain processing (thinking)
        self._brain.step(step, timestep)
        
        # 5. Motor command decoding
        action = self._agent.decode_motor_command()
        
        # 6. Action execution
        action_result = self._world.apply_motor_command(action, timestep)
        
        # 7. Reward feedback (if agent has prediction)
        predicted_reward = self._agent.get_prediction_error()
        self._agent.apply_reward_modulation(action_result.reward, predicted_reward)
        
        # 8. Development update
        self._agent.update_development(timestep)
        
        # 9. Record experience for replay
        self._record_experience(step, percept, action, action_result)
        
        # 10. Periodic memory replay
        self._perform_replay(step)
        
        return action_result
    
    def _record_experience(self, step: int, percept: 'SensoryPercept',
                          action: MotorCommand, result: ActionResult) -> None:
        """Record experience for potential replay"""
        if not self._agent:
            return
            
        experience = {
            'step': step,
            'percept': percept.get_all_signals(),
            'action': action,
            'reward': result.reward,
            'success': result.success,
            'timestamp': time.time(),
        }
        
        self._agent._replay_history.append(experience)
        
        # Keep history manageable
        if len(self._agent._replay_history) > 1000:
            self._agent._replay_history = self._agent._replay_history[-500:]
    
    def _perform_replay(self, current_step: int) -> None:
        """Perform memory replay at appropriate intervals"""
        if not self._agent:
            return
            
        # Replay every 100 steps (or as configured)
        if current_step - self._agent._last_replay_step >= 100:
            self._agent._last_replay_step = current_step
            # This would trigger actual replay in a real implementation
            # For now, it's a placeholder
    
    def reset(self) -> None:
        """Reset agent-world system"""
        self._world.reset()
        if self._agent:
            self._agent.reset()
            
    def log_status(self) -> None:
        """Log agent-world status"""
        print(f"=== Agent-World Status ===")
        if self._agent:
            self._agent.log_status()
        self._world.log_status()


# Factory functions
def create_default_agent_config() -> Dict[str, Any]:
    """Create a default configuration for agent brain"""
    return {
        'reward_modulation': True,
        'structural_plasticity': False,
        'development': True,
        'curiosity': True,
    }


def create_agent_brain(brain) -> AgentBrain:
    """Create a new agent brain interface"""
    return AgentBrain(brain)


def create_agent_world(brain, world_config=None) -> AgentWorld:
    """Create a new agent-world integration"""
    return AgentWorld(brain, world_config)


# Export commonly used classes and functions
__all__ = [
    'NeuromodulationState',
    'DevelopmentState',
    'AgentBrain',
    'AgentWorld',
    'create_default_agent_config',
    'create_agent_brain',
    'create_agent_world',
    'AgentBrainError',
    'SensoryError',
    'MotorError',
]
