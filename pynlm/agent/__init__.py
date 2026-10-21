"""Agent brain interface for NLM."""


class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain=None):
        """Initialize agent brain."""
        self._brain = brain
        
    def initialize(self, world):
        """Initialize with world."""
        pass
        
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        return 0
        
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        return 0
        
    def process_sensory_input(self, percept):
        """Process sensory percept and inject into brain."""
        pass
        
    def decode_motor_command(self):
        """Decode brain motor activity into motor command."""
        return None
        
    def apply_reward_modulation(self, reward, predicted_reward):
        """Apply reward-based neuromodulation."""
        pass
        
    def update_development(self, timestep):
        """Update development system."""
        pass
        
    def get_development_stage(self):
        """Get current developmental stage."""
        return None
        
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return 0.0
        
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return 0.0
        
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return 0.0
        
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return 0.0
        
    def reset(self):
        """Reset agent for new episode."""
        pass
        
    def get_brain(self):
        """Get the underlying brain."""
        return self._brain
        
    def enable_reward_modulation(self, enable: bool):
        """Enable reward modulation."""
        pass
        
    def enable_structural_plasticity(self, enable: bool):
        """Enable structural plasticity."""
        pass
        
    def enable_development(self, enable: bool):
        """Enable development."""
        pass
        
    def enable_curiosity(self, enable: bool):
        """Enable curiosity."""
        pass
        
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return False
        
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return False
        
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return False
        
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return False