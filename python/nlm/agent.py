"""
Agent interface for NLM simulation.

Provides high-level Python API for agent control and interaction with the world.
"""

from typing import Optional, Dict, Any, List, Callable
import time

from .exceptions import NLMRuntimeError, NLMValidationError
class Agent:
    """NLM agent with brain and world interaction.
    
    High-level Python interface for agents that can sense, think, and act
    in NLM simulation environments.
    """
    
    def __init__(self, brain=None, world=None):
        """Initialize agent.
        
        Args:
            brain: Brain instance (if None, creates default)
            world: World instance (if None, creates default)
        """
        from .brain import BrainSimulator
        
        # Create brain if not provided
        if brain is None:
            self.brain = BrainSimulator()
        else:
            self.brain = brain
            
        # Create world if not provided
        if world is None:
            from .world import SimpleWorld
            self.world = SimpleWorld()
        else:
            self.world = world
            
        # Create agent brain interface if brain is available
        if hasattr(self.brain, 'brain') and self.brain.brain is not None:
            from pynlm import AgentBrain as PyAgentBrain
            self.agent_brain = PyAgentBrain(self.brain.brain)
            self.agent_brain.initialize(self.world)
        else:
            self.agent_brain = None
    
    def process_sensory_input(self, sensory_data: Optional[Dict[str, Any]] = None):
        """Process sensory input from world.
        
        Args:
            sensory_data: Sensory data (if None, uses world sensor data)
        """
        if sensory_data is None:
            sensory_data = self.world.get_sensory_percept()
            
        if self.agent_brain:
            from pynlm import SensoryPercept
            percept = SensoryPercept()
            
            # Convert world sensory data to percept
            if "vision" in sensory_data:
                vision_data = []
                for row in sensory_data["vision"]:
                    vision_data.extend(row)
                percept.setVision(vision_data)
                
            if "touch" in sensory_data:
                percept.setTouch(sensory_data["touch"])
                
            if "internal" in sensory_data:
                percept.setInternal(sensory_data["internal"])
                
            if "proprioception" in sensory_data:
                percept.setProprioception(sensory_data["proprioception"])
                
            self.agent_brain.processSensoryInput(percept)
    
    def make_action(self) -> Dict[str, Any]:
        """Make action based on brain processing.
        
        Returns:
            Dictionary with action result
        """
        if self.agent_brain:
            motor_command = self.agent_brain.decodeMotorCommand()
            
            # Convert motor command to action
            action_result = self.world.apply_motor_command(
                int(motor_command), self.world.get_simulation_time()
            )
            
            # Update brain development
            if hasattr(self.agent_brain, 'updateDevelopment'):
                self.agent_brain.updateDevelopment(0.1)
                
            return action_result
        else:
            raise NLMRuntimeError("No agent brain interface available")
    
    def update_brain(self, step: int, time: Optional[float] = None):
        """Update brain state.
        
        Args:
            step: Step number
            time: Optional simulation time
        """
        if time is None:
            time = step * 0.001
            
        if hasattr(self.brain, 'run_step'):
            self.brain.run_step(step, time)
        elif hasattr(self.brain, 'brain') and self.brain.brain:
            self.brain.brain.step(step, time)
    
    def run_episode(self, num_steps: int, timestep: float = 0.1) -> List[Dict[str, Any]]:
        """Run complete episode.
        
        Args:
            num_steps: Number of steps to run
            timestep: Time per step
            
        Returns:
            List of step results
        """
        episode_data = []
        
        # Reset environment
        self.world.reset()
        if self.agent_brain:
            self.agent_brain.reset()
            
        # Run episode
        for step in range(num_steps):
            # Process sensory input
            self.process_sensory_input()
            
            # Update brain
            self.update_brain(step, step * timestep)
            
            # Make action
            action_result = self.make_action()
            
            # Update world
            world_result = self.world.update(timestep)
            
            # Apply reward modulation if available
            if self.agent_brain and "reward" in action_result:
                reward = action_result["reward"]
                predicted_reward = 0.0  # Could use prediction system
                self.agent_brain.apply_reward_modulation(reward, predicted_reward)
            
            # Collect episode data
            step_data = {
                "step": step,
                "time": step * timestep,
                "world": world_result,
                "action": action_result,
                "sensory": self.world.get_sensory_percept(),
                "brain": self.brain.get_stats() if hasattr(self.brain, 'get_stats') else {},
                "agent": self.agent_brain.get_stats() if hasattr(self.agent_brain, 'get_stats') else {},
            }
            
            episode_data.append(step_data)
            
        return episode_data
    
    def enable_learning(self, enable: bool = True):
        """Enable/disable learning features.
        
        Args:
            enable: Whether to enable learning
        """
        # Implementation depends on available C++ bindings
        pass
    
    def get_agent_stats(self) -> Dict[str, Any]:
        """Get agent statistics.
        
        Returns:
            Dictionary with agent statistics
        """
        stats = {
            "brain": self.brain.get_stats() if hasattr(self.brain, 'get_stats') else {},
        }
        
        if self.agent_brain:
            stats["neuromodulation"] = {
                "dopamine_level": self.agent_brain.get_neuromodulation_level(),
                "curiosity_level": self.agent_brain.get_curiosity_level(),
                "novelty_level": self.agent_brain.get_novelty_level(),
                "prediction_error": self.agent_brain.get_prediction_error(),
                "developmental_stage": self.agent_brain.get_developmental_stage().name,
            }
            
        return stats
    
    def save(self, filepath: str) -> bool:
        """Save agent state.
        
        Args:
            filepath: Path to save agent state
            
        Returns:
            True if successful, False otherwise
        """
        # Implementation depends on available C++ bindings
        return False
    
    def load(self, filepath: str) -> bool:
        """Load agent state.
        
        Args:
            filepath: Path to load agent state from
            
        Returns:
            True if successful, False otherwise
        """
        # Implementation depends on available C++ bindings
        return False
class AgentBrain:
    """Enhanced AgentBrain interface with Pythonic API.
    
    Provides extended agent brain capabilities for simulation and learning.
    """
    
    def __init__(self, brain_interface=None):
        """Initialize enhanced agent brain interface.
        
        Args:
            brain_interface: Optional AgentBrain instance
        """
        if brain_interface is None:
            self._brain = None
        else:
            self._brain = brain_interface
    
    def set_world(self, world):
        """Set world for agent brain.
        
        Args:
            world: World instance
        """
        if self._brain:
            self._brain.initialize(world)
    
    def process_input(self, percept):
        """Process sensory percept.
        
        Args:
            percept: Sensory percept data
        """
        if self._brain:
            self._brain.processSensoryInput(percept)
    
    def get_command(self) -> int:
        """Get motor command.
        
        Returns:
            Motor command (0-7)
        """
        if self._brain:
            return int(self._brain.decodeMotorCommand())
        return 7  # Wait command
    
    def apply_reward(self, reward: float, predicted: float = 0.0):
        """Apply reward modulation.
        
        Args:
            reward: Received reward
            predicted: Predicted reward
        """
        if self._brain:
            self._brain.applyRewardModulation(reward, predicted)
    
    def update_development(self, timestep: float):
        """Update development system.
        
        Args:
            timestep: Time step
        """
        if self._brain:
            self._brain.updateDevelopment(timestep)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get neuromodulation statistics.
        
        Returns:
            Dictionary with stats
        """
        if not self._brain:
            return {}
            
        return {
            "dopamine_level": self._brain.getNeuromodulationLevel(),
            "curiosity_level": self._brain.getCuriosityLevel(),
            "novelty_level": self._brain.getNoveltyLevel(),
            "prediction_error": self._brain.getPredictionError(),
            "developmental_stage": self._brain.getDevelopmentalStage().name,
            "reward_modulation_enabled": self._brain.isRewardModulationEnabled(),
            "structural_plasticity_enabled": self._brain.isStructuralPlasticityEnabled(),
            "development_enabled": self._brain.isDevelopmentEnabled(),
            "curiosity_enabled": self._brain.isCuriosityEnabled(),
        }
    
    def reset(self):
        """Reset agent brain."""
        if self._brain:
            self._brain.reset()
    
    def enable_subsystem(self, subsystem: str, enable: bool = True):
        """Enable/disable subsystem.
        
        Args:
            subsystem: Subsystem name (reward_modulation, structural_plasticity, development, curiosity)
            enable: Whether to enable subsystem
        """
        if not self._brain:
            return
            
        if subsystem == "reward_modulation":
            self._brain.enableRewardModulation(enable)
        elif subsystem == "structural_plasticity":
            self._brain.enableStructuralPlasticity(enable)
        elif subsystem == "development":
            self._brain.enableDevelopment(enable)
        elif subsystem == "curiosity":
            self._brain.enableCuriosity(enable)