import json
import time
import sys
from typing import Dict, List, Optional, Union, Any, Callable
from enum import Enum
import threading
import logging

# Import the NLM bindings
try:
    import pynlm
except ImportError as e:
    print("Error: Could not import pynlm. Please install the NLM Python bindings.")
    print("Run: pip install -e .")
    sys.exit(1)

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMBrain:
    """
    High-level interface to NLM brain simulation.
    
    Provides an easier-to-use Python API for creating and managing neural network simulations.
    """
    
    def __init__(self, config: Optional[Any] = None, name: str = "brain"):
        """
        Initialize an NLM brain.
        
        Args:
            config: Configuration object or dictionary of settings. If None, uses default config.
            name: Name identifier for the brain
        """
        self.name = name
        self._brain = None
        self._is_initialized = False
        self._step_callback = None
        self._step_interval = 0
        self._step_counter = 0
        
        # Create configuration
        if config is None:
            self.config = pynlm.createDefaultConfig()
        elif isinstance(config, dict):
            self.config = pynlm.createDefaultConfig()
            self._apply_config_dict(config)
        else:
            self.config = config
        
        # Create brain
        self._brain = pynlm.createBrain(self.config)
        
        logger.info(f"Created NLM brain '{name}'")
    
    def _apply_config_dict(self, config_dict: Dict):
        """Apply configuration from dictionary."""
        for key, value in config_dict.items():
            if isinstance(value, bool):
                self.config.set(key, value)
            elif isinstance(value, int):
                self.config.set(key, value)
            elif isinstance(value, float):
                self.config.set(key, value)
            elif isinstance(value, str):
                self.config.set(key, value)
            else:
                self.config.set(key, str(value))
    
    def initialize(self, neuron_count: Optional[int] = None, 
                   region_count: Optional[int] = None,
                   connection_probability: Optional[float] = None,
                   timestep: Optional[float] = None):
        """
        Initialize the brain with optional parameter overrides.
        
        Args:
            neuron_count: Total number of neurons (overrides config if provided)
            region_count: Number of brain regions (overrides config if provided)
            connection_probability: Connection probability (overrides config if provided)
            timestep: Simulation timestep (overrides config if provided)
        """
        if neuron_count is not None:
            self.config.set("brain.neuron_count", neuron_count)
        if region_count is not None:
            self.config.set("brain.region_count", region_count)
        if connection_probability is not None:
            self.config.set("brain.connection_probability", connection_probability)
        if timestep is not None:
            self.config.set("simulation.timestep", timestep)
        
        if self._brain.initialize():
            self._is_initialized = True
            logger.info(f"Brain '{self.name}' initialized successfully")
            logger.info(f"  Total neurons: {self._brain.getTotalNeuronCount()}")
            logger.info(f"  Total synapses: {self._brain.getTotalSynapseCount()}")
        else:
            raise RuntimeError(f"Failed to initialize brain '{self.name}'")
    
    def step(self, steps: int = 1, callback: Optional[Callable] = None):
        """
        Run brain simulation for specified number of steps.
        
        Args:
            steps: Number of simulation steps to run
            callback: Optional function to call after each step (receives step number)
        """
        if not self._is_initialized:
            raise RuntimeError("Brain must be initialized before stepping. Call initialize() first.")
        
        for i in range(steps):
            self._step_counter += 1
            
            # Call step callback if provided
            if callback is not None:
                callback(i)
            
            # Call registered step callback if set
            if self._step_callback is not None:
                self._step_callback(self._step_counter)
            
            # Perform one step
            self._brain.step(self._step_counter)
        
        logger.debug(f"Brain '{self.name}' stepped {steps} times (total: {self._step_counter})")
    
    def run_simulation(self, steps: int, timestep: float = 0.001,
                      save_interval: Optional[int] = None,
                      callback: Optional[Callable] = None):
        """
        Run a complete simulation with logging and optional saving.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time per simulation step
            save_interval: Save brain state every N steps (None for no saving)
            callback: Optional function called after each step with brain stats
        """
        logger.info(f"Starting simulation for {steps} steps with timestep {timestep}")
        
        start_time = time.time()
        
        for step in range(steps):
            # Update timestep
            self.config.set("simulation.timestep", timestep)
            
            # Perform step
            self._brain.step(step)
            
            # Save checkpoint if interval specified
            if save_interval is not None and step > 0 and step % save_interval == 0:
                filename = f"{self.name}_checkpoint_step_{step}.bin"
                self.save(filename)
                logger.info(f"Checkpoint saved: {filename}")
            
            # Call callback if provided
            if callback is not None:
                stats = self.get_stats()
                callback(step, stats)
        
        elapsed_time = time.time() - start_time
        logger.info(f"Simulation completed in {elapsed_time:.2f} seconds")
        
        return elapsed_time
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current brain statistics."""
        return {
            "name": self.name,
            "total_neurons": self._brain.getTotalNeuronCount(),
            "total_synapses": self._brain.getTotalSynapseCount(),
            "active_neurons": self._brain.getActiveNeuronCount(),
            "firing_neurons": self._brain.getFiringNeuronCount(),
            "total_spikes": self._brain.getTotalSpikeCount(),
            "average_firing_rate": self._brain.getAverageFiringRate(),
            "excitatory_inhibition_ratio": self._brain.getExcitationInhibitionRatio(),
            "developmental_stage": self._brain.getDevelopmentalStage(),
            "simulation_step": self._step_counter,
            "is_initialized": self._is_initialized
        }
    
    def save(self, filepath: str):
        """Save brain state to file."""
        if self._brain.save(filepath):
            logger.info(f"Brain state saved to: {filepath}")
            return True
        else:
            logger.error(f"Failed to save brain state to: {filepath}")
            return False
    
    def load(self, filepath: str):
        """Load brain state from file."""
        if self._brain.load(filepath):
            logger.info(f"Brain state loaded from: {filepath}")
            self._is_initialized = True
            return True
        else:
            logger.error(f"Failed to load brain state from: {filepath}")
            return False
    
    def reset(self):
        """Reset brain state."""
        self._brain.reset()
        self._step_counter = 0
        self._is_initialized = False
        logger.info(f"Brain '{self.name}' reset")
    
    def inject_current(self, neuron_id: int, current: float):
        """Inject current into a specific neuron."""
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_by_type(self, neuron_type: str, current: float):
        """Inject current into all neurons of a specific type."""
        # Map string to enum
        type_map = {
            "excitatory": pynlm.NeuronType.Excitatory,
            "inhibitory": pynlm.NeuronType.Inhibitory,
            "sensory": pynlm.NeuronType.Sensory,
            "motor": pynlm.NeuronType.Motor,
            "modulatory": pynlm.NeuronType.Modulatory,
            "internal": pynlm.NeuronType.Internal
        }
        
        if neuron_type.lower() in type_map:
            self._brain.injectCurrentToNeurons(type_map[neuron_type.lower()], current)
        else:
            raise ValueError(f"Unknown neuron type: {neuron_type}")
    
    def produce_action(self) -> Any:
        """Produce motor action based on current brain state."""
        return self._brain.produceAction()
    
    def set_development_stage(self, stage: str):
        """Set developmental stage."""
        stage_map = {
            "initial": pynlm.DevelopmentalStage.Initial,
            "critical_period": pynlm.DevelopmentalStage.CriticalPeriod,
            "maturation": pynlm.DevelopmentalStage.Maturation,
            "adult": pynlm.DevelopmentalStage.Adult,
            "aging": pynlm.DevelopmentalStage.Aging
        }
        
        if stage.lower() in stage_map:
            self._brain.setDevelopmentalStage(stage_map[stage.lower()])
        else:
            raise ValueError(f"Unknown developmental stage: {stage}")
    
    def register_step_callback(self, callback: Callable, interval: int = 1):
        """
        Register a callback function to be called after each N steps.
        
        Args:
            callback: Function to call with step number
            interval: Call callback every N steps
        """
        self._step_callback = callback
        self._step_interval = interval
    
    def unregister_step_callback(self):
        """Unregister the step callback."""
        self._step_callback = None
        self._step_interval = 0
    
    def add_region(self, name: str) -> int:
        """Add a new neural region to the brain."""
        region_id = self._brain.addRegion(name)
        logger.info(f"Added region '{name}' with ID {region_id}")
        return region_id
    
    def get_regions(self) -> List[Dict[str, Any]]:
        """Get information about all regions."""
        regions = []
        for region_id in self._brain.getRegionIds():
            region = self._brain.getRegion(region_id)
            if region:
                regions.append({
                    "id": region_id.index(),
                    "name": region.getName(),
                    "neuron_count": region.getTotalNeuronCount(),
                    "synapse_count": region.getSynapseCount()
                })
        return regions
    
    def __repr__(self):
        return f"<NLMBrain name='{self.name}' neurons={self._brain.getTotalNeuronCount() if self._brain else 'N/A'}>"

class NLMWorld:
    """
    High-level interface to NLM world simulation.
    
    Provides an easier-to-use Python API for creating and managing environments.
    """
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """
        Initialize NLM world.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        self._world = pynlm.createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        
        logger.info(f"Created NLM world: {width}x{height}, vision: {vision_width}x{vision_height}")
    
    def reset(self):
        """Reset world to initial state."""
        self._world.reset()
        logger.info("World reset")
    
    def configure(self, width: Optional[int] = None, height: Optional[int] = None,
                  vision_width: Optional[int] = None, vision_height: Optional[int] = None):
        """Configure world parameters."""
        if width is not None:
            self.width = width
        if height is not None:
            self.height = height
        if vision_width is not None:
            self.vision_width = vision_width
        if vision_height is not None:
            self.vision_height = vision_height
        
        self._world.configure(self.width, self.height, self.vision_width, self.vision_height)
        logger.info(f"World configured: {self.width}x{self.height}, vision: {self.vision_width}x{self.vision_height}")
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        self._world.setAgentStart(x, y)
        logger.info(f"Agent start set to: ({x}, {y})")
    
    def update(self, timestep: float):
        """Update world simulation."""
        self._world.update(timestep)
    
    def get_sensory_percept(self) -> Any:
        """Get current sensory percept from world."""
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """Get current agent body state."""
        return self._world.getAgentBody()
    
    def apply_motor_command(self, command: Any, time: float):
        """Apply motor command to world."""
        self._world.applyMotorCommand(command, time)
    
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float):
        """Set maximum energy."""
        self._world.setMaxEnergy(energy)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world statistics."""
        return {
            "width": self.get_width(),
            "height": self.get_height(),
            "simulation_time": self.get_simulation_time(),
            "max_energy": self.get_max_energy(),
            "agent_position": (self.get_agent_body().x, self.get_agent_body().y)
        }
    
    def __repr__(self):
        return f"<NLMWorld size={self.width}x{self.height}>"

class NLMEnvironment:
    """
    High-level interface for NLM agent-environment interaction.
    
    Provides a simplified API for creating complete agent-environment simulations.
    """
    
    def __init__(self, brain_config: Optional[Dict] = None,
                 world_config: Optional[Dict] = None,
                 agent_config: Optional[Dict] = None):
        """
        Initialize NLM environment with agent and world.
        
        Args:
            brain_config: Brain configuration dictionary
            world_config: World configuration dictionary
            agent_config: Agent configuration dictionary
        """
        self.brain_config = brain_config or {}
        self.world_config = world_config or {}
        self.agent_config = agent_config or {}
        
        # Create components
        self.world = NLMWorld(**self.world_config)
        self.brain = NLMBrain(self.brain_config, "agent_brain")
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain._brain)
        
        # Initialize components
        self._initialize_components()
        
        logger.info("NLM environment initialized")
    
    def _initialize_components(self):
        """Initialize all components."""
        # Configure world
        if "width" in self.world_config:
            self.world.configure(
                width=self.world_config["width"],
                height=self.world_config["height"],
                vision_width=self.world_config.get("vision_width", 8),
                vision_height=self.world_config.get("vision_height", 8)
            )
        
        # Set agent start position
        start_x = self.world_config.get("agent_start_x", 10.0)
        start_y = self.world_config.get("agent_start_y", 10.0)
        self.world.setAgentStart(start_x, start_y)
        
        # Initialize brain
        brain_neuron_count = self.brain_config.get("neuron_count", 1000)
        brain_region_count = self.brain_config.get("region_count", 1)
        self.brain.initialize(
            neuron_count=brain_neuron_count,
            region_count=brain_region_count
        )
        
        # Initialize agent
        self.agent.initialize(self.world._world)
        
        # Enable subsystems if configured
        if self.agent_config.get("enable_reward_modulation", True):
            self.agent.enableRewardModulation(True)
        
        if self.agent_config.get("enable_structural_plasticity", True):
            self.agent.enableStructuralPlasticity(True)
        
        if self.agent_config.get("enable_development", True):
            self.agent.enableDevelopment(True)
        
        if self.agent_config.get("enable_curiosity", True):
            self.agent.enableCuriosity(True)
    
    def run_episode(self, steps: int, timestep: float = 0.1,
                    save_interval: Optional[int] = None,
                    callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run a complete episode of agent-environment interaction.
        
        Args:
            steps: Number of simulation steps to run
            timestep: Time per simulation step
            save_interval: Save agent state every N steps (None for no saving)
            callback: Optional function called after each step
            
        Returns:
            Episode statistics
        """
        logger.info(f"Starting episode for {steps} steps")
        
        self.world.reset()
        self.brain.reset()
        self.agent.reset()
        
        start_time = time.time()
        
        for step in range(steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            # Extract reward from internal signals
            internal_signals = percept.getInternal() if hasattr(percept, 'getInternal') else []
            reward = internal_signals[0] if internal_signals else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Save checkpoint if interval specified
            if save_interval is not None and step > 0 and step % save_interval == 0:
                filename = f"agent_checkpoint_step_{step}.bin"
                self.save(filename)
                logger.info(f"Checkpoint saved: {filename}")
            
            # Call callback if provided
            if callback is not None:
                stats = self.get_stats()
                callback(step, stats)
        
        elapsed_time = time.time() - start_time
        
        # Get final statistics
        stats = self.get_stats()
        stats["episode_time"] = elapsed_time
        stats["steps_completed"] = steps
        
        logger.info(f"Episode completed in {elapsed_time:.2f} seconds")
        
        return stats
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current environment statistics."""
        return {
            "brain": self.brain.get_stats(),
            "world": self.world.get_stats()
        }
    
    def save(self, filepath: str):
        """Save agent state to file."""
        return self.brain.save(filepath)
    
    def load(self, filepath: str):
        """Load agent state from file."""
        return self.brain.load(filepath)
    
    def reset(self):
        """Reset environment."""
        self.world.reset()
        self.brain.reset()
        self.agent.reset()
        logger.info("Environment reset")
    
    def __repr__(self):
        return f"<NLMEnvironment brain={self.brain.name} world={self.world}>"

# Convenience functions
def create_simple_brain(neuron_count: int = 1000, regions: int = 1) -> NLMBrain:
    """Create a simple brain with default configuration."""
    config = {
        "brain.neuron_count": neuron_count,
        "brain.region_count": regions,
        "brain.connection_probability": 0.1,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True
    }
    return NLMBrain(config, "simple_brain")


def create_simple_world(width: int = 20, height: int = 20) -> NLMWorld:
    """Create a simple world with default configuration."""
    return NLMWorld(width, height, 8, 8)


def create_agent_environment(brain_config: Optional[Dict] = None,
                            world_config: Optional[Dict] = None,
                            agent_config: Optional[Dict] = None) -> NLMEnvironment:
    """Create a complete agent-environment setup."""
    return NLMEnvironment(brain_config, world_config, agent_config)


# Global convenience functions
def get_version() -> str:
    """Get NLM version."""
    return "NLM Python API v1.0.0"


def list_available_configs() -> List[str]:
    """List available configuration presets."""
    return [
        "default",
        "minimal", 
        "high_activity",
        "balanced",
        "exploratory",
        "developmental"
    ]


def get_example_config(preset: str) -> Dict[str, Any]:
    """Get example configuration for a preset."""
    presets = {
        "default": {
            "brain.neuron_count": 1000,
            "brain.region_count": 1,
            "brain.connection_probability": 0.1
        },
        "minimal": {
            "brain.neuron_count": 100,
            "brain.region_count": 1,
            "brain.connection_probability": 0.05
        },
        "high_activity": {
            "brain.neuron_count": 5000,
            "brain.region_count": 3,
            "brain.connection_probability": 0.2,
            "plasticity.stdp.enable": True,
            "neuromod.dopamine.scale": 2.0
        },
        "balanced": {
            "brain.neuron_count": 2000,
            "brain.region_count": 2,
            "brain.connection_probability": 0.15,
            "plasticity.stdp.enable": True,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True
        }
    }
    
    return presets.get(preset.lower(), presets["default"])


# Export main classes
__all__ = [
    'NLMBrain', 'NLMWorld', 'NLMEnvironment',
    'create_simple_brain', 'create_simple_world', 'create_agent_environment',
    'get_version', 'list_available_configs', 'get_example_config'
]