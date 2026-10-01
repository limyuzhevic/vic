# Improved Python wrapper classes for NLM
# This provides a more Pythonic API on top of the existing pybind11 bindings

import json
import time
from typing import Dict, List, Optional, Union, Any, Callable, Iterator, overload
from contextlib import contextmanager
from enum import Enum

# Import the existing pybind11 bindings
try:
    import pynlm
    pynlm_available = True
except ImportError:
    pynlm_available = False

if not pynlm_available:
    raise ImportError("pynlm bindings not available. Please build the NLM Python bindings first.")

# Type aliases for better documentation
NeuronID = Any  # pynlm.NeuronId
SynapseID = Any  # pynlm.SynapseId
RegionID = Any  # pynlm.RegionId
ConfigValue = Union[int, float, bool, str, List, Dict]

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class Config(pynlm.Config):
    """Enhanced configuration class with Pythonic API"""
    
    def __init__(self):
        super().__init__()
    
    def set_from_dict(self, config_dict: Dict[str, ConfigValue], 
                     source: pynlm.ConfigSource = pynlm.ConfigSource.Runtime) -> None:
        """Set multiple configuration values from a dictionary"""
        for key, value in config_dict.items():
            self.set(key, value, source)
    
    def get_dict(self) -> Dict[str, Any]:
        """Get all configuration as a dictionary with proper type conversion"""
        config_dict = {}
        for key in self.getKeys():
            value = self.get(key)
            config_dict[key] = value
        return config_dict
    
    def save_as_json(self, filepath: str) -> bool:
        """Save configuration as JSON file"""
        return self.saveToFile(filepath)
    
    def load_from_json(self, filepath: str) -> bool:
        """Load configuration from JSON file"""
        return self.loadFromFile(filepath)
    
    def get_nested(self, key: str, default: Any = None) -> Any:
        """Get nested configuration value using dot notation"""
        # Simple nested access - could be enhanced
        if '.' in key:
            parts = key.split('.')
            current = self
            for part in parts:
                if not hasattr(current, 'get') or current.get(part) is None:
                    return default
                current = current.get(part)
            return current
        return self.get(key, default)
    
    def validate(self, schema: Dict[str, type]) -> List[str]:
        """Validate configuration against a schema"""
        errors = []
        for key, expected_type in schema.items():
            if key in self.getKeys():
                value = self.get(key)
                if not isinstance(value, expected_type):
                    errors.append(f"Key '{key}' expected {expected_type}, got {type(value)}")
        return errors
    
    def get_summary_dict(self) -> Dict[str, Any]:
        """Get configuration summary as dictionary"""
        # Parse the summary string into a structured dict
        summary = self.summary()
        result = {"keys": self.getKeys()}
        if summary:
            result["summary"] = summary
        return result

class Brain(pynlm.Brain):
    """Enhanced brain class with Pythonic API and context manager support"""
    
    def __init__(self, config: Optional[Config] = None):
        if config is None:
            config = Config()
        super().__init__(config)
        self._initialized = False
    
    def initialize(self) -> bool:
        """Initialize the brain with enhanced error handling"""
        try:
            result = super().initialize()
            self._initialized = result
            return result
        except Exception as e:
            raise NLMError(f"Failed to initialize brain: {e}")
    
    def step(self, step_num: int, timestamp: Optional[float] = None) -> None:
        """Perform a simulation step"""
        if not self._initialized:
            raise NLMError("Brain not initialized. Call initialize() first.")
        
        if timestamp is not None:
            super().step(step_num, timestamp)
        else:
            super().step(step_num)
    
    def inject_sensory(self, sensory_data: Any) -> None:
        """Inject sensory input with type validation"""
        try:
            self.receiveSensoryInput(sensory_data)
        except Exception as e:
            raise NLMError(f"Failed to inject sensory input: {e}")
    
    def get_neuron_stats(self) -> Dict[str, Any]:
        """Get comprehensive neuron statistics"""
        return {
            "total_neurons": self.getTotalNeuronCount(),
            "total_synapses": self.getTotalSynapseCount(),
            "firing_neurons": self.getFiringNeuronCount(),
            "active_spikes": self.getPendingSpikeEventCount(),
            "total_spikes": self.getTotalSpikeCount(),
            "avg_firing_rate": self.getAverageFiringRate(),
            "exc_inhibition_ratio": self.getExcitationInhibitionRatio(),
            "development_stage": self.getDevelopmentalStage().name,
            "region_count": self.getRegionCount()
        }
    
    def get_region_ids(self) -> List[RegionID]:
        """Get list of all region IDs"""
        return self.getRegionIds()
    
    def find_neurons_by_type(self, neuron_type: pynlm.NeuronType) -> List[NeuronID]:
        """Find neurons of a specific type (requires implementation)"""
        # This would need additional C++ exposure or implementation
        # For now, return empty list
        return []
    
    def set_development_stage(self, stage: Union[pynlm.DevelopmentalStage, str]) -> None:
        """Set developmental stage with string support"""
        if isinstance(stage, str):
            stage = pynlm.DevelopmentalStage[stage]
        self.setDevelopmentalStage(stage)
    
    def get_development_info(self) -> Dict[str, Any]:
        """Get developmental information"""
        return {
            "stage": self.getDevelopmentalStage().name,
            "stage_order": list(range(len(pynlm.DevelopmentalStage))),
            "current_age": self.getConfig().getOr("development.age", 0.0)
        }
    
    @contextmanager
    def simulation_step(self, step_num: int, timestamp: Optional[float] = None):
        """Context manager for a single simulation step"""
        try:
            if timestamp is not None:
                yield
                self.step(step_num, timestamp)
            else:
                yield
                self.step(step_num)
        except Exception as e:
            raise NLMError(f"Error during simulation step {step_num}: {e}")
    
    def __enter__(self):
        """Context manager entry"""
        if not self._initialized:
            self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup resources"""
        self.reset()
        self._initialized = False

class AgentBrain(pynlm.AgentBrain):
    """Enhanced agent brain with Pythonic API"""
    
    def __init__(self, brain: Brain):
        super().__init__(brain)
        self._world = None
    
    def initialize_with_world(self, world: 'SimpleWorld') -> bool:
        """Initialize agent with world"""
        try:
            self._world = world
            self.initialize(world)
            return True
        except Exception as e:
            raise NLMError(f"Failed to initialize agent brain: {e}")
    
    def process_sensory_batch(self, sensory_inputs: List[Any]) -> None:
        """Process multiple sensory inputs"""
        for sensory_input in sensory_inputs:
            self.processSensoryInput(sensory_input)
    
    def get_agent_stats(self) -> Dict[str, Any]:
        """Get comprehensive agent statistics"""
        brain = self.getBrain()
        return {
            "brain_stats": brain.get_neuron_stats(),
            "neuromodulation_level": self.getNeuromodulationLevel(),
            "curiosity_level": self.getCuriosityLevel(),
            "novelty_level": self.getNoveltyLevel(),
            "prediction_error": self.getPredictionError(),
            "development_stage": self.getDevelopmentalStage().name,
            "enabled_features": {
                "reward_modulation": self.isRewardModulationEnabled(),
                "structural_plasticity": self.isStructuralPlasticityEnabled(),
                "development": self.isDevelopmentEnabled(),
                "curiosity": self.isCuriosityEnabled()
            }
        }
    
    def enable_all_features(self) -> None:
        """Enable all agent features for exploration"""
        self.enableRewardModulation(True)
        self.enableStructuralPlasticity(True)
        self.enableDevelopment(True)
        self.enableCuriosity(True)
    
    def configure_neuromodulation(self, dopamine: Optional[float] = None, 
                                  curiosity: Optional[float] = None,
                                  novelty: Optional[float] = None) -> None:
        """Configure neuromodulation parameters"""
        # This would require additional C++ exposure
        # For now, just update internal state if accessible
        pass

class SimpleWorld(pynlm.SimpleWorld):
    """Enhanced simple world with Pythonic API"""
    
    def __init__(self):
        super().__init__()
    
    def configure_world(self, width: int, height: int, 
                       vision_width: int = 8, vision_height: int = 8,
                       max_energy: float = 100.0, decay_rate: float = 0.1) -> None:
        """Configure world with common parameters"""
        self.configure(width, height, vision_width, vision_height)
        self.setMaxEnergy(max_energy)
        self.setEnergyDecayRate(decay_rate)
    
    def add_object_safe(self, obj: Any, max_retries: int = 3) -> bool:
        """Add world object with retry logic"""
        for attempt in range(max_retries):
            if self.isValidPosition(obj.x, obj.y):
                self.addObject(obj)
                return True
            time.sleep(0.1 * (attempt + 1))  # Small delay between attempts
        return False
    
    def simulate_step(self, timestep: float = 0.1) -> Dict[str, Any]:
        """Perform a world simulation step and return status"""
        self.update(timestep)
        percept = self.getSensoryPercept()
        agent_body = self.getAgentBody()
        
        return {
            "timestamp": self.getSimulationTime(),
            "perception_available": percept is not None,
            "agent_position": (agent_body.x, agent_body.y) if agent_body else None,
            "agent_orientation": agent_body.orientation if agent_body else None,
            "agent_energy": agent_body.energy if agent_body else None
        }
    
    def run_episode(self, steps: int, timestep: float = 0.1) -> List[Dict[str, Any]]:
        """Run a complete episode and return episode history"""
        episode_history = []
        for step in range(steps):
            step_data = self.simulate_step(timestep)
            episode_history.append(step_data)
        return episode_history

# Factory functions with enhanced API
def create_enhanced_config(config_dict: Optional[Dict[str, ConfigValue]] = None) -> Config:
    """Create a configuration with optional initial values"""
    config = Config()
    if config_dict:
        config.set_from_dict(config_dict)
    return config

def create_enhanced_brain(config: Optional[Config] = None, 
                         auto_initialize: bool = True) -> Brain:
    """Create a brain with optional auto-initialization"""
    brain = Brain(config)
    if auto_initialize:
        brain.initialize()
    return brain

def create_enhanced_world(config: Optional[Dict[str, ConfigValue]] = None) -> SimpleWorld:
    """Create a world with optional configuration"""
    world = SimpleWorld()
    if config:
        world.configure_world(
            width=config.get('world.width', 20),
            height=config.get('world.height', 20),
            vision_width=config.get('world.vision_width', 8),
            vision_height=config.get('world.vision_height', 8),
            max_energy=config.get('world.max_energy', 100.0),
            decay_rate=config.get('world.energy_decay_rate', 0.1)
        )
    return world

def create_enhanced_agent(brain: Brain, world: SimpleWorld) -> AgentBrain:
    """Create an enhanced agent brain"""
    agent = AgentBrain(brain)
    agent.initialize_with_world(world)
    return agent

# Convenience functions for common patterns
def run_agent_simulation(steps: int, timestep: float = 0.1) -> Dict[str, Any]:
    """Run a complete agent simulation with all components"""
    # Create components
    config = create_enhanced_config({
        'simulation.steps': steps,
        'simulation.timestep': timestep,
        'development.enabled': True,
        'plasticity.structural': True,
        'neuromodulation.reward': True,
        'neuromodulation.curiosity': True
    })
    
    brain = create_enhanced_brain(config)
    world = create_enhanced_world(config)
    agent = create_enhanced_agent(brain, world)
    
    # Enable features
    agent.enable_all_features()
    
    # Run simulation
    episode_history = world.run_episode(steps, timestep)
    
    # Collect final statistics
    final_stats = {
        "brain_stats": brain.get_neuron_stats(),
        "agent_stats": agent.get_agent_stats(),
        "world_stats": {
            "final_time": world.getSimulationTime(),
            "simulation_steps": len(episode_history)
        }
    }
    
    return {
        "episode_history": episode_history,
        "final_stats": final_stats
    }

def save_brain_state(brain: Brain, filepath: str) -> bool:
    """Save brain state to file"""
    try:
        brain.save(filepath)
        return True
    except Exception as e:
        raise NLMError(f"Failed to save brain state: {e}")

def load_brain_state(filepath: str, config: Optional[Config] = None) -> Brain:
    """Load brain state from file"""
    brain = create_enhanced_brain(config)
    try:
        brain.load(filepath)
        brain.initialize()
        return brain
    except Exception as e:
        raise NLMError(f"Failed to load brain state: {e}")

# Example usage functions for documentation
def example_basic_brain() -> None:
    """Example: Create and run a basic brain simulation"""
    print("Example: Basic Brain Simulation")
    brain = create_enhanced_brain()
    with brain:
        for i in range(10):
            brain.step(i)
            print(f"Step {i}: {brain.getTotalSpikeCount()} total spikes")
    print("Example completed!\n")

def example_agent_world() -> None:
    """Example: Create and run an agent in a world"""
    print("Example: Agent in World")
    brain = create_enhanced_brain()
    world = create_enhanced_world()
    agent = create_enhanced_agent(brain, world)
    agent.enable_all_features()
    
    # Run a simple interaction
    for step in range(20):
        world.simulate_step(0.1)
        # Agent processes sensory input (in real implementation)
        brain.step(step)
    
    print(f"Final brain stats: {brain.get_neuron_stats()}")
    print(f"Final agent stats: {agent.get_agent_stats()}")
    print("Example completed!\n")

def example_simulation_run() -> Dict[str, Any]:
    """Example: Run a complete simulation"""
    print("Example: Complete Simulation Run")
    result = run_agent_simulation(50)
    print(f"Simulation completed with {len(result['episode_history'])} steps")
    print(f"Final spike count: {result['final_stats']['brain_stats']['total_spikes']}")
    print("Example completed!\n")
    return result

# Version and info
__version__ = "2.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python API for NLM (Neural Learning Machine)"

# Example usage dictionary for documentation
examples = {
    "basic": example_basic_brain,
    "agent_world": example_agent_world,
    "simulation": example_simulation_run
}

# Main function for direct script execution
if __name__ == "__main__":
    print("=== NLM Enhanced Python API ===")
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print()
    
    # Run all examples
    example_basic_brain()
    example_agent_world()
    example_simulation_run()
    
    print("All examples completed successfully!")