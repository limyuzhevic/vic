"""
NLM Enhanced Python API - Quick Access to Improved Features

This module provides enhanced Python APIs for NLM with improved features:
- Configuration validation
- Better error handling
- Convenience methods for common tasks
- Enhanced brain control
- World management utilities

Usage:
    # Import enhanced API
    import nlm_enhanced
    
    # Create enhanced brain with validation
    brain = nlm_enhanced.create_brain(neuron_count=1000, timestep=0.001)
    brain.initialize()
    
    # Run simulation with progress monitoring
    brain.run_simulation(1000, verbose=True)
    
    # Save state
    brain.save_state("checkpoint.bin")
    
    # Create enhanced world
    world = nlm_enhanced.create_world(width=20, height=20)
    
    # Add resources to world
    world.add_resource(5, 5, 1.0)
    world.add_hazard(15, 15, -0.5)
"""

import pynlm
import numpy as np
from typing import Optional, Dict, Any, List
import logging
import json
import os

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Validation schema for configuration
VALIDATION_SCHEMA = {
    "brain": {
        "neuron_count": {"type": "int", "min": 100, "max": 100000, "default": 1000},
        "synapse_density": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.1},
        "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.05},
        "timestep": {"type": "float", "min": 0.0001, "max": 0.1, "default": 0.001},
    },
    "simulation": {
        "max_steps": {"type": "int", "min": 1, "max": 1000000, "default": 10000},
    }
}

class ValidationError(Exception):
    """Exception raised when configuration validation fails."""
    pass

def validate_config(config_dict: Dict[str, Any]) -> List[str]:
    """Validate configuration dictionary against schema.
    
    Args:
        config_dict: Configuration dictionary to validate
        
    Returns:
        List of validation errors, empty if valid
    """
    errors = []
    
    for section, params in VALIDATION_SCHEMA.items():
        for param, rules in params.items():
            key = f"{section}.{param}" if section != "simulation" else f"simulation.{param}"
            
            if key in config_dict:
                value = config_dict[key]
                
                # Check type
                expected_type = rules["type"]
                if expected_type == "int" and not isinstance(value, int):
                    errors.append(f"Invalid type for {key}: expected int, got {type(value)}")
                elif expected_type == "float" and not isinstance(value, (int, float)):
                    errors.append(f"Invalid type for {key}: expected float, got {type(value)}")
                
                # Check range
                if "min" in rules and value < rules["min"]:
                    errors.append(f"Value for {key} is below minimum: {value} < {rules['min']}")
                
                if "max" in rules and value > rules["max"]:
                    errors.append(f"Value for {key} is above maximum: {value} > {rules['max']}")
    
    return errors

class EnhancedBrain:
    """Enhanced brain interface with validation and convenience methods."""
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """Initialize enhanced brain.
        
        Args:
            config: Optional existing config object
        """
        self._config = config if config else pynlm.createDefaultConfig()
        self._brain = pynlm.createBrain(self._config)
        self._initialized = False
        self._stats = {
            'total_spikes': 0,
            'firing_neurons': 0,
            'average_firing_rate': 0.0,
            'development_stage': None,
            'simulation_steps': 0
        }
        
        logger.info("EnhancedBrain initialized")
    
    def initialize(self, config_dict: Optional[Dict[str, Any]] = None, 
                   verbose: bool = False) -> 'EnhancedBrain':
        """Initialize brain with optional configuration.
        
        Args:
            config_dict: Optional configuration dictionary
            verbose: Enable verbose logging
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If configuration is invalid
        """
        if config_dict:
            # Validate configuration
            errors = validate_config(config_dict)
            if errors:
                error_msg = "Configuration validation errors:\n" + "\n".join(f"  - {err}" for err in errors)
                raise ValidationError(error_msg)
            
            # Apply configuration
            for key, value in config_dict.items():
                if '.' in key:
                    section, param = key.split('.', 1)
                    self._config.set(param, value)
                else:
                    self._config.set(key, value)
                
                if verbose:
                    logger.info(f"Configuration set: {key} = {value}")
        
        # Initialize brain
        self._brain.initialize()
        self._initialized = True
        
        if verbose:
            neuron_count = self._config.getOr('brain.neuron_count', 1000)
            timestep = self._config.getOr('simulation.timestep', 0.001)
            logger.info(f"Brain initialized: {neuron_count} neurons, timestep: {timestep}s")
        
        return self
    
    def run_simulation(self, steps: int, verbose: bool = False, 
                       callback: Optional[callable] = None) -> 'EnhancedBrain':
        """Run simulation with progress monitoring.
        
        Args:
            steps: Number of simulation steps
            verbose: Enable verbose logging
            callback: Optional callback function for each step
            
        Returns:
            Self for method chaining
            
        Raises:
            RuntimeError: If brain is not initialized
            ValueError: If steps is invalid
        """
        if not self._initialized:
            raise RuntimeError("Brain must be initialized before running simulation")
        
        if steps <= 0:
            raise ValueError(f"Number of steps must be positive, got: {steps}")
        
        logger.info(f"Starting simulation: {steps} steps")
        
        # Run simulation
        for step in range(steps):
            self._brain.step(step)
            self._stats['simulation_steps'] += 1
            
            # Update statistics
            self._stats['total_spikes'] = self._brain.getTotalSpikeCount()
            self._stats['firing_neurons'] = self._brain.getFiringNeuronCount()
            self._stats['average_firing_rate'] = self._brain.getAverageFiringRate()
            self._stats['development_stage'] = self._brain.getDevelopmentalStage()
            
            # Call callback if provided
            if callback:
                try:
                    callback(step, self)
                except Exception as e:
                    logger.warning(f"Callback error at step {step}: {e}")
            
            # Log progress periodically
            if verbose and step % 100 == 0:
                logger.info(f"Step {step}/{steps}: " +
                           f"Firing: {self._stats['firing_neurons']}, " +
                           f"Spikes: {self._stats['total_spikes']}, " +
                           f"Dev Stage: {self._stats['development_stage']}")
        
        logger.info(f"Simulation completed: {steps} steps processed")
        return self
    
    def save_state(self, filepath: str) -> 'EnhancedBrain':
        """Save brain state to file.
        
        Args:
            filepath: Path to save state file
            
        Returns:
            Self for method chaining
        """
        self._brain.save(filepath)
        logger.info(f"Brain state saved to: {filepath}")
        return self
    
    def load_state(self, filepath: str) -> 'EnhancedBrain':
        """Load brain state from file.
        
        Args:
            filepath: Path to state file
            
        Returns:
            Self for method chaining
        """
        self._brain.load(filepath)
        self._initialized = True
        logger.info(f"Brain state loaded from: {filepath}")
        return self
    
    def reset(self) -> 'EnhancedBrain':
        """Reset brain to initial state.
        
        Returns:
            Self for method chaining
        """
        self._brain.reset()
        self._initialized = True
        self._stats = {
            'total_spikes': 0,
            'firing_neurons': 0,
            'average_firing_rate': 0.0,
            'development_stage': None,
            'simulation_steps': 0
        }
        logger.info("Brain reset to initial state")
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """Get brain statistics.
        
        Returns:
            Dictionary of brain statistics
        """
        return self._stats.copy()
    
    def __repr__(self) -> str:
        return f"EnhancedBrain(neurons={self._stats['firing_neurons']}, " + \
               f"spikes={self._stats['total_spikes']}, " + \
               f"dev_stage={self._stats['development_stage']})"

class EnhancedWorld:
    """Enhanced world interface with utilities."""
    
    def __init__(self, width: int = 20, height: int = 20,
                 vision_width: int = 8, vision_height: int = 8):
        """Initialize enhanced world.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self._world = pynlm.createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        self._world.reset()
        
        logger.info(f"EnhancedWorld created: {width}x{height}, vision: {vision_width}x{vision_height}")
    
    def configure(self, width: int = None, height: int = None,
                 vision_width: int = None, vision_height: int = None,
                 **kwargs) -> 'EnhancedWorld':
        """Configure world parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            **kwargs: Additional configuration parameters
            
        Returns:
            Self for method chaining
        """
        if width is not None:
            self._world.configure(width, self._world.getHeight(),
                                vision_width if vision_width is not None else self._world.getVisionWidth(),
                                vision_height if vision_height is not None else self._world.getVisionHeight())
        
        if height is not None:
            current_width = self._world.getWidth()
            self._world.configure(current_width, height,
                                vision_width if vision_width is not None else self._world.getVisionWidth(),
                                vision_height if vision_height is not None else self._world.getVisionHeight())
        
        # Apply additional configuration
        for key, value in kwargs.items():
            if key == "setAgentStart":
                self._world.setAgentStart(value[0], value[1])
            elif key == "addObject":
                self._world.addObject(value)
        
        return self
    
    def reset(self) -> 'EnhancedWorld':
        """Reset world to initial state.
        
        Returns:
            Self for method chaining
        """
        self._world.reset()
        return self
    
    def update(self, timestep: float = 0.1) -> 'EnhancedWorld':
        """Update world state.
        
        Args:
            timestep: Time step for update
            
        Returns:
            Self for method chaining
        """
        self._world.update(timestep)
        return self
    
    def get_sensory_percept(self) -> Any:
        """Get current sensory percept from world.
        
        Returns:
            Sensory percept object
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """Get agent body state.
        
        Returns:
            Agent body object
        """
        return self._world.getAgentBody()
    
    def apply_motor_command(self, motor_cmd: Any, current_time: float = 0.0) -> 'EnhancedWorld':
        """Apply motor command to world.
        
        Args:
            motor_cmd: Motor command to apply
            current_time: Current simulation time
            
        Returns:
            Self for method chaining
        """
        self._world.applyMotorCommand(motor_cmd, current_time)
        return self
    
    def set_agent_start(self, x: float, y: float) -> 'EnhancedWorld':
        """Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Self for method chaining
        """
        self._world.setAgentStart(x, y)
        return self
    
    def add_resource(self, x: float, y: float, value: float = 1.0) -> 'EnhancedWorld':
        """Add resource to world.
        
        Args:
            x: X coordinate
            y: Y coordinate
            value: Resource value
            
        Returns:
            Self for method chaining
        """
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value)
        self._world.addObject(resource)
        return self
    
    def add_hazard(self, x: float, y: float, value: float = -1.0) -> 'EnhancedWorld':
        """Add hazard to world.
        
        Args:
            x: X coordinate
            y: Y coordinate
            value: Hazard value (typically negative)
            
        Returns:
            Self for method chaining
        """
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value)
        self._world.addObject(hazard)
        return self
    
    def get_internal_signals(self) -> List[float]:
        """Get internal signals from world (reward-related).
        
        Returns:
            List of internal signals
        """
        percept = self.get_sensory_percept()
        return percept.getInternal() if hasattr(percept, 'getInternal') else []
    
    def get_vision(self) -> List[float]:
        """Get vision data from world.
        
        Returns:
            Vision data as list
        """
        percept = self.get_sensory_percept()
        return percept.getVision() if hasattr(percept, 'getVision') else []
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid in world.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid, False otherwise
        """
        return self._world.isValidPosition(x, y)
    
    def run_episode(self, brain: EnhancedBrain, agent: 'EnhancedAgent', 
                   steps: int, verbose: bool = False) -> float:
        """Run complete episode with brain and agent.
        
        Args:
            brain: Enhanced brain instance
            agent: Enhanced agent instance
            steps: Number of steps for episode
            verbose: Enable verbose logging
            
        Returns:
            Total reward accumulated during the episode
        """
        total_reward = 0.0
        
        for step in range(steps):
            # Update world
            self.update(0.1)
            
            # Get sensory input
            percept = self.get_sensory_percept()
            
            # Process sensory input with agent
            agent.process_sensory_input(percept)
            
            # Run brain step
            brain.run_simulation(1)
            
            # Get motor command from agent
            motor_cmd = agent.decode_motor_command()
            
            # Apply motor command to world
            self.apply_motor_command(motor_cmd, self._world.getSimulationTime())
            
            # Get reward from world
            reward = self.get_internal_signals()[0] if self.get_internal_signals() else 0.0
            total_reward += reward
            
            # Apply reward modulation
            agent.apply_reward_modulation(reward, 0.0)
            
            # Update development
            agent.update_development(0.1)
            
            # Log progress
            if verbose and step % 50 == 0:
                logger.info(f"Episode step {step}/{steps}: Reward: {reward}, " +
                           f"Total Reward: {total_reward}, " +
                           f"Curiosity: {agent.get_curiosity_level():.3f}")
        
        return total_reward

class EnhancedAgent:
    """Enhanced agent interface with convenience methods."""
    
    def __init__(self, brain: EnhancedBrain, world: EnhancedWorld):
        """Initialize enhanced agent.
        
        Args:
            brain: Enhanced brain instance
            world: Enhanced world instance
        """
        self._brain = brain
        self._world = world
        self._agent = pynlm.createAgentBrain(brain._brain._brain)
        self._agent.initialize(world._world)
        
        # Enable all subsystems by default
        self.enable_all_subsystems()
        
        logger.info("EnhancedAgent initialized")
    
    def enable_all_subsystems(self) -> 'EnhancedAgent':
        """Enable all agent subsystems.
        
        Returns:
            Self for method chaining
        """
        self._agent.enableRewardModulation(True)
        self._agent.enableStructuralPlasticity(True)
        self._agent.enableDevelopment(True)
        self._agent.enableCuriosity(True)
        return self
    
    def enable_subsystem(self, subsystem: str, enable: bool = True) -> 'EnhancedAgent':
        """Enable or disable specific subsystem.
        
        Args:
            subsystem: Subsystem name ('reward', 'structural', 'development', 'curiosity')
            enable: Enable or disable
            
        Returns:
            Self for method chaining
        """
        if subsystem == 'reward':
            self._agent.enableRewardModulation(enable)
        elif subsystem == 'structural':
            self._agent.enableStructuralPlasticity(enable)
        elif subsystem == 'development':
            self._agent.enableDevelopment(enable)
        elif subsystem == 'curiosity':
            self._agent.enableCuriosity(enable)
        else:
            raise ValueError(f"Unknown subsystem: {subsystem}")
        
        return self
    
    def process_sensory_input(self, percept: Any) -> 'EnhancedAgent':
        """Process sensory input.
        
        Args:
            percept: Sensory percept object
            
        Returns:
            Self for method chaining
        """
        self._agent.processSensoryInput(percept)
        return self
    
    def decode_motor_command(self) -> Any:
        """Decode motor command from brain.
        
        Returns:
            Motor command object
        """
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> 'EnhancedAgent':
        """Apply reward modulation.
        
        Args:
            reward: Actual reward
            predicted_reward: Predicted reward (default: 0.0)
            
        Returns:
            Self for method chaining
        """
        self._agent.applyRewardModulation(reward, predicted_reward)
        return self
    
    def update_development(self, timestep: float) -> 'EnhancedAgent':
        """Update development system.
        
        Args:
            timestep: Time step for development update
            
        Returns:
            Self for method chaining
        """
        self._agent.updateDevelopment(timestep)
        return self
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level.
        
        Returns:
            Curiosity level (0.0 to 1.0)
        """
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level.
        
        Returns:
            Novelty level (0.0 to 1.0)
        """
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error.
        
        Returns:
            Prediction error
        """
        return self._agent.getPredictionError()
    
    def get_neuromodulation_level(self) -> float:
        """Get neuromodulation level.
        
        Returns:
            Neuromodulation level
        """
        return self._agent.getNeuromodulationLevel()
    
    def get_developmental_stage(self):
        """Get developmental stage.
        
        Returns:
            Developmental stage
        """
        return self._agent.getDevelopmentalStage()
    
    def reset(self) -> 'EnhancedAgent':
        """Reset agent for new episode.
        
        Returns:
            Self for method chaining
        """
        self._agent.reset()
        return self
    
    def run_episode(self, world: EnhancedWorld, steps: int, verbose: bool = False) -> float:
        """Run complete episode.
        
        Args:
            world: Enhanced world instance
            steps: Number of steps for episode
            verbose: Enable verbose logging
            
        Returns:
            Total reward accumulated
        """
        return world.run_episode(self._brain, self, steps, verbose)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get brain statistics.
        
        Returns:
            Brain statistics
        """
        return self._brain.get_stats()

# Convenience factory functions

def create_brain(config_dict: Optional[Dict[str, Any]] = None,
                 **kwargs) -> EnhancedBrain:
    """Create enhanced brain with configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        **kwargs: Configuration parameter overrides
        
    Returns:
        EnhancedBrain instance
    """
    # Create brain
    brain = EnhancedBrain()
    
    # Apply configuration from dictionary
    if config_dict:
        errors = validate_config(config_dict)
        if errors:
            raise ValidationError("Configuration validation errors:\n" + "\n".join(f"  - {err}" for err in errors))
        
        for key, value in config_dict.items():
            if '.' in key:
                section, param = key.split('.', 1)
                brain._config.set(param, value)
            else:
                brain._config.set(key, value)
    
    # Apply kwargs
    for key, value in kwargs.items():
        if '.' in key:
            section, param = key.split('.', 1)
            brain._config.set(param, value)
        else:
            brain._config.set(key, value)
    
    return brain

def create_world(width: int = 20, height: int = 20,
                 vision_width: int = 8, vision_height: int = 8) -> EnhancedWorld:
    """Create enhanced world.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        
    Returns:
        EnhancedWorld instance
    """
    return EnhancedWorld(width, height, vision_width, vision_height)

def create_agent(brain: EnhancedBrain, world: EnhancedWorld) -> EnhancedAgent:
    """Create enhanced agent.
    
    Args:
        brain: Enhanced brain instance
        world: Enhanced world instance
        
    Returns:
        EnhancedAgent instance
    """
    return EnhancedAgent(brain, world)

# Legacy compatibility functions

def create_default_config() -> Dict[str, Any]:
    """Create default configuration dictionary."""
    return {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "brain.connection_probability": 0.05,
        "simulation.timestep": 0.001,
        "simulation.max_steps": 10000
    }

def create_default_brain() -> EnhancedBrain:
    """Create brain with default configuration."""
    brain = EnhancedBrain()
    brain.initialize(create_default_config())
    return brain

def create_default_world() -> EnhancedWorld:
    """Create world with default parameters."""
    return EnhancedWorld()

# Example usage function
def example_usage():
    """Print example usage of enhanced API."""
    print("=== NLM Enhanced API Example Usage ===\n")
    
    print("1. Create enhanced brain:")
    print("   brain = EnhancedBrain()")
    print("   brain.initialize({'brain.neuron_count': 2000, 'simulation.timestep': 0.01})")
    print()
    
    print("2. Run simulation with progress monitoring:")
    print("   brain.run_simulation(1000, verbose=True)")
    print()
    
    print("3. Create enhanced world:")
    print("   world = EnhancedWorld(width=30, height=30)")
    print()
    
    print("4. Add resources to world:")
    print("   world.add_resource(5, 5, 2.0)")
    print("   world.add_hazard(25, 25, -1.0)")
    print()
    
    print("5. Create agent and run episode:")
    print("   agent = EnhancedAgent(brain, world)")
    print("   reward = agent.run_episode(world, steps=500, verbose=True)")
    print()

# Export public symbols
__all__ = [
    'EnhancedBrain',
    'EnhancedWorld',
    'EnhancedAgent',
    'create_brain',
    'create_world',
    'create_agent',
    'create_default_config',
    'create_default_brain',
    'create_default_world',
    'validate_config',
    'ValidationError',
    'example_usage'
]

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

# Example usage when module is run directly
if __name__ == "__main__":
    example_usage()
