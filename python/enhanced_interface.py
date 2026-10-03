"""
Enhanced NLM Python Interface

This module provides a Pythonic interface to NLM with:
- Full type hints
- Async/await support
- Better error handling
- Advanced utilities
- Pythonic API design

This file should be generated alongside the C++ bindings.cpp file
as an enhanced Python interface.
"""

import asyncio
import time
import json
from typing import List, Dict, Optional, Tuple, Union, Any, Callable
from pathlib import Path
import warnings
import threading
from concurrent.futures import ThreadPoolExecutor

# Import the actual NLM bindings
try:
    import pynlm
    _pynlm_available = True
# Handle ImportError exception
    _pynlm_available = False
    pynlm = None

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration related errors."""
    pass

class SimulationError(NLMError):
    """Simulation related errors."""
    pass
class EnhancedConfig:
    """
    Class: 
    Class: 
    Class: Enhanced configuration class with better UX.
    
    
    """""""""
    
    def __init__(self):
        if not _pynlm_available:
            raise NLMError("NLM bindings not available. Please install pynlm.")
        
        self._config = pynlm.createDefaultConfig()
        
    def set_brain_parameters(self, neuron_count: int = 1000,
                             region_count: int = 1,
                             connection_probability: float = 0.1,
                             v_thresh: float = -50.0,
                             v_rest: float = -70.0,
                             v_reset: float = -75.0,
                             tau_mem: float = 20.0,
                             tau_ref: float = 2.0) -> Any:
    """
    Function: Set brain parameters with validation.
    """""
        if neuron_count <= 0:
            raise ConfigurationError("neuron_count must be positive")
        if not 0.0 <= connection_probability <= 1.0:
            raise ConfigurationError("connection_probability must be between 0 and 1")
            
        self._config.set("brain.neuron_count", neuron_count)
        self._config.set("brain.region_count", region_count)
        self._config.set("brain.connection_probability", connection_probability)
        self._config.set("brain.v_thresh", v_thresh)
        self._config.set("brain.v_rest", v_rest)
        self._config.set("brain.v_reset", v_reset)
        self._config.set("brain.tau_mem", tau_mem)
        self._config.set("brain.tau_ref", tau_ref)
        
        return self
        
    def set_plasticity_parameters(self, stdp_enabled: bool = True,
                                   hebbian_enabled: bool = True,
                                   structural_enabled: bool = True,
                                   stdp_ltp_weight: float = 0.01,
                                   stdp_ltd_weight: float = 0.012,
                                   stdp_tau: float = 20.0,
                                   hebbian_rate: float = 0.001) -> Any:
    """
    Function: Set plasticity parameters.
    """""
        self._config.set("plasticity.stdp.enable", stdp_enabled)
        self._config.set("plasticity.hebbian.enable", hebbian_enabled)
        self._config.set("plasticity.structural.enable", structural_enabled)
        self._config.set("plasticity.stdp.learning_rate", stdp_ltp_weight)
        self._config.set("plasticity.stdp.ltd_weight", stdp_ltd_weight)
        self._config.set("plasticity.stdp.tau_plus", stdp_tau)
        self._config.set("plasticity.stdp.tau_minus", stdp_tau)
        self._config.set("plasticity.hebbian.learning_rate", hebbian_rate)
        
        return self
        
    def set_neuromodulation_parameters(self, dopamine_scale: float = 1.0,
                                        curiosity_enabled: bool = True,
                                        novelty_enabled: bool = True,
                                        reward_scale: float = 1.0) -> Any:
    """
    Function: Set neuromodulation parameters.
    """""
        self._config.set("neuromod.dopamine.scale", dopamine_scale)
        self._config.set("neuromod.curiosity.enable", curiosity_enabled)
        self._config.set("neuromod.novelty.enable", novelty_enabled)
        self._config.set("neuromod.reward.scale", reward_scale)
        
        return self
        
    def set_development_parameters(self, enable_development: bool = True,
                                   initial_stage: str = "initial") -> Any:
    """
    Function: Set development parameters.
    """""
        self._config.set("development.enabled", enable_development)
        self._config.set("development.initial_stage", initial_stage)
        
        return self
        
    def set_learning_parameters(self, enable_reward_modulation: bool = True,
                                enable_structural_plasticity: bool = True,
                                enable_curiosity: bool = True,
                                enable_development: bool = True) -> Any:
    """
    Function: Enable learning subsystems.
    """""
        self._config.set("learning.reward_modulation", enable_reward_modulation)
        self._config.set("learning.structural_plasticity", enable_structural_plasticity)
        self._config.set("learning.curiosity", enable_curiosity)
        self._config.set("learning.development", enable_development)
        
        return self
        
    def load_from_file(self, filepath: Union[str, Path]) -> Any:
    """
    Function: Load configuration from JSON file.
    """""
        path = Path(filepath)
        if not path.exists():
            warnings.warn(f"Configuration file {filepath} not found, using defaults")
            return self
            
        with open(path, 'r') as f:
            config_data = json.load(f)
            
        # Convert JSON to NLM config format
        for key, value in config_data.items():
            if '.' in key:
                # Nested key (e.g., "brain.neuron_count")
                parts = key.split('.')
                if len(parts) == 2:
                    section, param = parts
                    self._config.set(f"{section}.{param}", value)
                    
        return self
        
    def save_to_file(self, filepath: Union[str, Path]) -> Any:
    """
    Function: Save configuration to JSON file.
    """""
        path = Path(filepath)
        
        # Extract configuration to dict
        config_dict = {}
        keys = self._config.getKeys()
        
        for key in keys:
            if '.' in key:
                parts = key.split('.')
                if len(parts) == 2:
                    section, param = parts
                    if section not in config_dict:
                        config_dict[section] = {}
                    config_dict[section][param] = self._config.get(key)
                    
        with open(path, 'w') as f:
            json.dump(config_dict, f, indent=2)
            
        return self
        
    def get(self, key: str) -> Any:
        """Get configuration value."""
        if key in self._config.getKeys():
            return self._config.get(key)
        return None
        
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return key in self._config.getKeys()
        
    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
        
    def __repr__(self) -> str:
        return f"EnhancedConfig({self.summary()})"
class EnhancedBrain:
    """
    Class: 
    Class: 
    Class: Enhanced brain interface with better Pythonic API.
    
    
    """""""""
    
    def __init__(self, config: EnhancedConfig):
        if not _pynlm_available:
            raise NLMError("NLM bindings not available. Please install pynlm.")
            
        self._config = config._config
        self._brain = pynlm.createBrain(self._config)
        self._brain.initialize()
        
    def initialize(self) -> 'EnhancedBrain':
        """Initialize the brain."""
        self._brain.initialize()
        return self
        
    def step(self, step_number: int = 0, current_time: Optional[float] = None) -> 'EnhancedBrain':
        """Perform a simulation step."""
        if current_time is None:
            current_time = step_number * 0.001  # Default timestep
        self._brain.step(step_number, current_time)
        return self
        
    def reset(self) -> 'EnhancedBrain':
        """Reset brain state."""
        self._brain.reset()
        return self
        
    def run_steps(self, num_steps: int, timestep: float = 0.001) -> 'EnhancedBrain':
        """Run multiple steps efficiently."""
        for step in range(num_steps):
            self.step(step, step * timestep)
        return self
        
    # Property access with type hints
    @property
    def total_neurons(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
        
    @property
    def total_synapses(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
        
    @property
    def firing_neurons(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
        
    @property
    def active_neurons(self) -> int:
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()
        
    @property
    def total_spikes(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
        
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
        
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get E/I balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
        
    @property
    def developmental_stage(self) -> str:
        """Get developmental stage."""
        return self._brain.getDevelopmentalStage()
        
    def set_developmental_stage(self, stage: str) -> 'EnhancedBrain':
        """Set developmental stage."""
        self._brain.setDevelopmentalStage(stage)
        return self
        
    def get_region(self, region_id: int) -> Optional[Any]:
        """Get region by ID."""
        return self._brain.getRegion(region_id)
        
    def get_all_regions(self) -> List[Any]:
        """Get all regions."""
        return self._brain.getRegions()
        
    def add_region(self, name: str = "") -> int:
        """Add a new region."""
        return self._brain.addRegion(name)
        
    def log_status(self) -> 'EnhancedBrain':
        """Log brain status."""
        self._brain.logStatus()
        return self
        
    def save(self, filepath: Union[str, Path]) -> 'EnhancedBrain':
        """Save brain state."""
        self._brain.save(str(filepath))
        return self
        
    def load(self, filepath: Union[str, Path]) -> 'EnhancedBrain':
        """Load brain state."""
        self._brain.load(str(filepath))
        return self
        
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return {
            'total_neurons': self.total_neurons,
            'total_synapses': self.total_synapses,
            'firing_neurons': self.firing_neurons,
            'active_neurons': self.active_neurons,
            'total_spikes': self.total_spikes,
            'average_firing_rate': self.average_firing_rate,
            'excitation_inhibition_ratio': self.excitation_inhibition_ratio,
            'developmental_stage': self.developmental_stage,
            'region_count': len(self.get_all_regions()),
        }
        
    def __repr__(self) -> str:
        return f"EnhancedBrain(neurons={self.total_neurons}, synapses={self.total_synapses}, spikes={self.total_spikes})"
class AsyncEnhancedBrain:
    """
    Class: 
    Class: 
    Class: Async interface for brain operations.
    
    
    """""""""
    
    def __init__(self, config: EnhancedConfig):
        self.config = config
        self._loop = None
        self._executor = ThreadPoolExecutor(max_workers=2)
        
    async def _get_event_loop(self) -> Any:
    """
    Function: Get or create event loop.
    """""
        if self._loop is None:
            self._loop = asyncio.get_event_loop()
        return self._loop
        
    async def initialize_async(self) -> 'AsyncEnhancedBrain':
        """Initialize brain asynchronously."""
        loop = await self._get_event_loop()
        
        def init_brain():
            return EnhancedBrain(self.config)
            
        self._brain = await loop.run_in_executor(None, init_brain)
        return self
        
    async def step_async(self, step_number: int = 0, 
                        current_time: Optional[float] = None) -> 'AsyncEnhancedBrain':
        """Perform step asynchronously."""
        if current_time is None:
            current_time = step_number * 0.001
            
        loop = await self._get_event_loop()
        await loop.run_in_executor(
            None, 
            lambda: self._brain.step(step_number, current_time)
        )
        return self
        
    async def run_steps_async(self, num_steps: int, 
                             timestep: float = 0.001) -> 'AsyncEnhancedBrain':
        """Run multiple steps asynchronously."""
        tasks = []
        for step in range(num_steps):
            task = self.step_async(step, step * timestep)
            tasks.append(task)
            
        await asyncio.gather(*tasks)
        return self
        
    @property
    async def total_neurons(self) -> int:
        """Get total neuron count asynchronously."""
        loop = await self._get_event_loop()
        return await loop.run_in_executor(None, lambda: self._brain.total_neurons)
        
    @property
    async def total_spikes(self) -> int:
        """Get total spike count asynchronously."""
        loop = await self._get_event_loop()
        return await loop.run_in_executor(None, lambda: self._brain.total_spikes)
        
    async def get_stats_async(self) -> Dict[str, Any]:
        """Get brain statistics asynchronously."""
        loop = await self._get_event_loop()
        return await loop.run_in_executor(None, lambda: self._brain.get_stats())
        
    def shutdown(self) -> Any:
    """
    Function: Shutdown executor.
    """""
        self._executor.shutdown(wait=True)
class EnhancedAgent:
    """
    Class: 
    Class: 
    Class: Enhanced agent interface.
    
    
    """""""""
    
    def __init__(self, brain: Union[EnhancedBrain, AsyncEnhancedBrain]):
        if _pynlm_available:
            self._brain = brain._brain if hasattr(brain, '_brain') else brain._brain
            self._agent = pynlm.createAgentBrain(self._brain)
        else:
            raise NLMError("NLM bindings not available.")
            
    def initialize(self, world: 'EnhancedWorld') -> 'EnhancedAgent':
        """Initialize agent with world."""
        if _pynlm_available:
            self._agent.initialize(world._world)
        return self
        
    def process_sensory_input(self, percept: Any) -> 'EnhancedAgent':
        """Process sensory input."""
        if _pynlm_available:
            self._agent.processSensoryInput(percept)
        return self
        
    def decode_motor_command(self) -> Any:
        """Decode motor command."""
        if _pynlm_available:
            return self._agent.decodeMotorCommand()
        return None
        
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> 'EnhancedAgent':
        """Apply reward modulation."""
        if _pynlm_available:
            self._agent.applyRewardModulation(reward, predicted_reward)
        return self
        
    def update_development(self, timestep: float) -> 'EnhancedAgent':
        """Update development system."""
        if _pynlm_available:
            self._agent.updateDevelopment(timestep)
        return self
        
    # Subsystem control methods
    def enable_reward_modulation(self, enable: bool) -> 'EnhancedAgent':
        """Enable/disable reward modulation."""
        if _pynlm_available:
            self._agent.enableRewardModulation(enable)
        return self
        
    def enable_structural_plasticity(self, enable: bool) -> 'EnhancedAgent':
        """Enable/disable structural plasticity."""
        if _pynlm_available:
            self._agent.enableStructuralPlasticity(enable)
        return self
        
    def enable_development(self, enable: bool) -> 'EnhancedAgent':
        """Enable/disable development."""
        if _pynlm_available:
            self._agent.enableDevelopment(enable)
        return self
        
    def enable_curiosity(self, enable: bool) -> 'EnhancedAgent':
        """Enable/disable curiosity."""
        if _pynlm_available:
            self._agent.enableCuriosity(enable)
        return self
        
    # State properties
    @property
    def curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent.getCuriosityLevel() if _pynlm_available else 0.0
        
    @property
    def novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent.getNoveltyLevel() if _pynlm_available else 0.0
        
    @property
    def prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent.getPredictionError() if _pynlm_available else 0.0
        
    @property
    def neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self._agent.getNeuromodulationLevel() if _pynlm_available else 0.0
        
    @property
    def developmental_stage(self) -> str:
        """Get developmental stage."""
        return self._agent.getDevelopmentalStage() if _pynlm_available else "unknown"
        
    def get_brain(self) -> EnhancedBrain:
        """Get the underlying brain."""
        return self._brain
        
    def reset(self) -> 'EnhancedAgent':
        """Reset agent for new episode."""
        if _pynlm_available:
            self._agent.reset()
        return self
class EnhancedWorld:
    """
    Class: 
    Class: 
    Class: Enhanced world interface.
    
    
    """""""""
    
    def __init__(self):
        if _pynlm_available:
            self._world = pynlm.createSimpleWorld()
        else:
            raise NLMError("NLM bindings not available.")
            
    def configure(self, width: int, height: int,
                  vision_width: int = 8, vision_height: int = 8,
                  max_energy: float = 100.0,
                  energy_decay_rate: float = 0.001) -> 'EnhancedWorld':
        """Configure world parameters."""
        self._world.configure(width, height, vision_width, vision_height)
        self._world.setMaxEnergy(max_energy)
        self._world.setEnergyDecayRate(energy_decay_rate)
        return self
        
    def reset(self) -> 'EnhancedWorld':
        """Reset world to initial state."""
        self._world.reset()
        return self
        
    def set_agent_start(self, x: float, y: float) -> 'EnhancedWorld':
        """Set agent starting position."""
        self._world.setAgentStart(x, y)
        return self
        
    def update(self, timestep: float) -> 'EnhancedWorld':
        """Update world state."""
        self._world.update(timestep)
        return self
        
    def apply_motor_command(self, command: Any, current_time: float) -> 'EnhancedWorld':
        """Apply motor command to world."""
        self._world.applyMotorCommand(command, current_time)
        return self
        
    def get_sensory_percept(self) -> Any:
        """Get sensory percept from world."""
        return self._world.getSensoryPercept()
        
    def get_agent_body(self) -> Any:
        """Get agent body state."""
        return self._world.getAgentBody()
        
    # World object management
    def add_object(self, obj: Any) -> 'EnhancedWorld':
        """Add object to world."""
        self._world.addObject(obj)
        return self
        
    def remove_object(self, x: float, y: float) -> 'EnhancedWorld':
        """Remove object from world."""
        self._world.removeObject(x, y)
        return self
        
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return self._world.isValidPosition(x, y)
        
    # State properties
    @property
    def width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
        
    @property
    def height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
        
    @property
    def simulation_time(self) -> float:
        """Get simulation time."""
        return self._world.getSimulationTime()
        
    @property
    def max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
        
    @property
    def energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
        
    @property
    def random_seed(self) -> Optional[int]:
        """Get random seed."""
        return self._world.getRandomSeed()
        
    def set_random_seed(self, seed: int) -> 'EnhancedWorld':
        """Set random seed."""
        self._world.setRandomSeed(seed)
        return self
        
    def __repr__(self) -> str:
        return f"EnhancedWorld(width={self.width}, height={self.height})"
# Factory functions for easy creation
def create_enhanced_config() -> EnhancedConfig:
    """Create enhanced configuration with recommended defaults."""
    return EnhancedConfig()
    

def create_enhanced_brain(config: Optional[EnhancedConfig] = None) -> EnhancedBrain:
    """Create enhanced brain with optional configuration."""
    if config is None:
        config = create_enhanced_config()
    return EnhancedBrain(config)
    

def create_enhanced_world() -> EnhancedWorld:
    """Create enhanced world."""
    return EnhancedWorld()
    

def create_enhanced_agent(brain: EnhancedBrain) -> EnhancedAgent:
    """Create enhanced agent with brain."""
    return EnhancedAgent(brain)
    

def create_agent_simulation(brain: EnhancedBrain, 
                           world: EnhancedWorld) -> EnhancedAgent:
    """Create agent and initialize with world."""
    agent = EnhancedAgent(brain)
    agent.initialize(world)
    return agent
# Advanced utility classes
class NLMExperimentSuite:
    """
    Class: 
    Class: 
    Class: Collection of predefined NLM experiments.
    
    
    """""""""
    
    @staticmethod
    def connectivity_test(config: Optional[EnhancedConfig] = None) -> Tuple[EnhancedBrain, EnhancedWorld]:
        """Run connectivity test experiment."""
        if config is None:
            config = create_enhanced_config()
            
        brain = create_enhanced_brain(config)
        world = create_enhanced_world()
        agent = create_agent_simulation(brain, world)
        
        # Configure world
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Enable key subsystems
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.simulation_time)
            
        return brain, world, agent
        
    @staticmethod
    def plasticity_test(config: Optional[EnhancedConfig] = None) -> Tuple[EnhancedBrain, EnhancedWorld]:
        """Run plasticity learning experiment."""
        if config is None:
            config = create_enhanced_config()
            config.set_plasticity_parameters(stdp_enabled=True, hebbian_enabled=True)
            
        brain = create_enhanced_brain(config)
        world = create_enhanced_world()
        agent = create_agent_simulation(brain, world)
        
        world.configure(30, 30, 10, 10)
        world.reset()
        
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        
        # Run learning simulation
        for step in range(500):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.simulation_time)
            
            # Apply small reward for exploration
            reward = 0.1 if step % 100 == 0 else 0.0
            agent.apply_reward_modulation(reward, 0.0)
            agent.update_development(0.1)
            
        return brain, world, agent
        
    @staticmethod
    def development_test(config: Optional[EnhancedConfig] = None) -> Tuple[EnhancedBrain, EnhancedWorld]:
        """Run development experiment."""
        if config is None:
            config = create_enhanced_config()
            
        brain = create_enhanced_brain(config)
        world = create_enhanced_world()
        agent = create_agent_simulation(brain, world)
        
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        
        # Run development simulation with varying stages
        for step in range(200):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.simulation_time)
            
            # Change developmental stage every 50 steps
            if step % 50 == 0:
                stage = "critical_period" if step < 100 else "maturation"
                brain.set_developmental_stage(stage)
                
        return brain, world, agent
# Main example function
def main() -> Any:
    """
    Function: Example usage of enhanced NLM interface.
    """""
    # print("NLM Enhanced Python Interface Examples")
    # print("=" * 50)
    
    # Example 1: Basic enhanced brain
    # print("\n1. Basic Enhanced Brain:")
    brain = create_enhanced_brain()
    # print(f"Created brain: {brain}")
    # print(f"Initial stats: {brain.get_stats()}")
    
    # Run some steps
    brain.run_steps(50)
    # print(f"After 50 steps: {brain.get_stats()}")
    
    # Example 2: Agent simulation
    # print("\n2. Enhanced Agent Simulation:")
    world = create_enhanced_world()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = create_agent_simulation(brain, world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    for step in range(100):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
    # print(f"Agent simulation complete:")
    # print(f"  - Brain stats: {brain.get_stats()}")
    # print(f"  - Agent metrics: curiosity={agent.curiosity_level:.3f}, "
          f"novelty={agent.novelty_level:.3f}")
    
    # Example 3: Predefined experiments
    # print("\n3. Predefined Experiments:")
    
    # print("  Running connectivity test...")
    brain1, world1, agent1 = NLMExperimentSuite.connectivity_test()
    # print(f"    Result: {brain1.total_spikes} spikes in {world1.simulation_time:.1f}s")
    
    # print("  Running plasticity test...")
    brain2, world2, agent2 = NLMExperimentSuite.plasticity_test()
    # print(f"    Result: {brain2.total_spikes} spikes, "
          f"E/I ratio: {brain2.excitation_inhibition_ratio:.3f}")
    
    # print("  Running development test...")
    brain3, world3, agent3 = NLMExperimentSuite.development_test()
    # print(f"    Result: Final stage={brain3.developmental_stage}, "
          f"rate={brain3.average_firing_rate:.2f} Hz")
    
    # Example 4: Configuration management
    # print("\n4. Configuration Management:")
    config = create_enhanced_config()
    config.set_brain_parameters(neuron_count=2000, connection_probability=0.05)
    config.set_plasticity_parameters(learning_rate=0.005)
    config.set_learning_parameters()
    
    # print(f"Configuration: {config.summary()}")
    
    # Save to file
    config.save_to_file("example_config.json")
    # print(f"Configuration saved to example_config.json")
    
    # Load from file
    new_config = EnhancedConfig().load_from_file("example_config.json")
    # print(f"Loaded configuration: {new_config.summary()}")
    
    # print("\n" + "=" * 50)
    # print("All examples completed successfully!")
    # print("\nKey improvements over original NLM:")
    # print("✓ Full type hints for better IDE support")
    # print("✓ Pythonic API with method chaining")
    # print("✓ Enhanced error handling and validation")
    # print("✓ Async/await support for non-blocking operations")
    # print("✓ Comprehensive configuration management")
    # print("✓ Built-in experiment suites")
    # print("✓ Better statistics and monitoring")
    # print("✓ Factory functions for easy setup")
if __name__ == "__main__":
    main()
