"""
NLM Agent classes
"""

from typing import List, Optional, Any
from .brain import Brain, NeuronId, RegionId
from .world import SimpleWorld
from .sensory import SensoryPercept, Vision
from .motor import Action, ActionType

class AgentBrainError(Exception):
    """Raised when agent brain operations fail."""
    pass
class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    Handles sensory transduction and motor decoding.
    Provides the bridge between neural computation and environmental interaction.
    """
    
    def __init__(self, brain: Brain):
        """Initialize agent brain interface.
        
        Args:
            brain: Underlying brain instance
        """
        self._brain = brain
        self._world_initialized = False
        self._curiosity_enabled = True
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = True
        self._development_enabled = True
        
        print(f"AgentBrain created with brain reference")
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize with world.
        
        Args:
            world: Simple world instance
        """
        self._world_initialized = True
        print(f"AgentBrain initialized with world: {world}")
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Expected sensory input size
        """
        return 300  # Vision (256) + Touch (8) + Internal (4) + Proprioception (6)
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Expected motor output size
        """
        return 6  # One per action type
    
    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept data
        """
        if not self._world_initialized:
            raise AgentBrainError("AgentBrain must be initialized with world before processing sensory input")
        
        print(f"AgentBrain processing sensory input: vision={len(percept.getVision())}, touch={len(percept.getTouch())}")
        
        # Convert to brain's expected format
        all_signals = percept.get_all_signals()
        if all_signals:
            self._brain.receive_sensory_input(all_signals)
    
    def decode_motor_command(self) -> Action:
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command instance
        """
        if not self._world_initialized:
            raise AgentBrainError("AgentBrain must be initialized with world before decoding motor commands")
        
        print("AgentBrain decoding motor command from brain activity")
        
        # For demonstration, return a simple action
        return self._brain.produce_action()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        print(f"AgentBrain applying reward modulation: reward={reward}, predicted={predicted_reward}")
    
    def update_development(self, timestep: float) -> None:
        """Update development system.
        
        Args:
            timestep: Simulation timestep
        """
        print(f"AgentBrain updating development: timestep={timestep}")
    
    def get_developmental_stage(self) -> str:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage
        """
        return self._brain.get_developmental_stage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level
        """
        return 0.5
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level.
        
        Returns:
            Current curiosity level
        """
        return 0.3
    
    def get_novelty_level(self) -> float:
        """Get novelty level.
        
        Returns:
            Current novelty level
        """
        return 0.2
    
    def get_prediction_error(self) -> float:
        """Get prediction error.
        
        Returns:
            Current prediction error
        """
        return 0.1
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        self._world_initialized = False
        print("AgentBrain reset completed")
    
    def get_brain(self) -> Brain:
        """Get the underlying brain.
        
        Returns:
            Underlying brain instance
        """
        return self._brain
    
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable or disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation
        """
        self._reward_modulation_enabled = enable
        print(f"AgentBrain reward modulation {'enabled' if enable else 'disabled'}")
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity
        """
        self._structural_plasticity_enabled = enable
        print(f"AgentBrain structural plasticity {'enabled' if enable else 'disabled'}")
    
    def enable_development(self, enable: bool) -> None:
        """Enable or disable development.
        
        Args:
            enable: Whether to enable development
        """
        self._development_enabled = enable
        print(f"AgentBrain development {'enabled' if enable else 'disabled'}")
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable or disable curiosity.
        
        Args:
            enable: Whether to enable curiosity
        """
        self._curiosity_enabled = enable
        print(f"AgentBrain curiosity {'enabled' if enable else 'disabled'}")
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled
        """
        return self._reward_modulation_enabled
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled
        """
        return self._structural_plasticity_enabled
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            True if development is enabled
        """
        return self._development_enabled
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled
        """
        return self._curiosity_enabled