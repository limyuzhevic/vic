"""
NLM (Neural Learning Machine) Python Bindings
A Python interface to the NLM C++ neural simulation framework.

This module provides classes and functions for creating and simulating virtual brains
using spiking neural networks with learning capabilities.

Key Features:
- Virtual brain creation and initialization
- Sensory-motor integration
- Learning through plasticity mechanisms
- Developmental stage progression
- World simulation and interaction
"""

import warnings
from typing import Optional, List, Dict, Any, Union
import numpy as np

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "nlm-project@nlm.org"

# Import the main classes from C++ bindings
from pynlm_core import (
    Brain,
    Config,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,
    SensoryPercept,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Re-export everything for easy access
__all__ = [
    # Main classes
    "Brain",
    "Config", 
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryInput",
    "SensoryPercept",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    # Factory functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID", 
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    # Helper classes and utilities
    "NLMException",
    "BrainConfig",
    "SimulationMetrics",
    "PerformanceMonitor",
]

# Global warnings configuration
warnings.filterwarnings("always", category=UserWarning, module="pynlm")

# Global configuration
_default_config: Optional[Config] = None
_brain_registry: Dict[str, Brain] = {}
_world_registry: Dict[str, SimpleWorld] = {}
_agent_registry: Dict[str, AgentBrain] = {}


class NLMException(Exception):
    """Base exception class for NLM errors."""
    
    def __init__(self, message: str, error_code: int = 1):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
    
    def __str__(self) -> str:
        return f"NLM Error {self.error_code}: {self.message}"


class BrainConfig:
    """Helper class for creating and managing brain configurations."""
    
    @staticmethod
    def create_default() -> Config:
        """Create a default brain configuration."""
        return createDefaultConfig()
    
    @staticmethod
    def with_parameters(
        neuron_count: int = 1000,
        synapse_density: float = 0.1,
        learning_rate: float = 0.001,
        **kwargs
    ) -> Config:
        """Create a custom brain configuration with specified parameters."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        config.set("learning.rate", learning_rate)
        
        for key, value in kwargs.items():
            config.set(key, value)
        
        return config
    
    @staticmethod
    def from_file(filepath: str) -> Config:
        """Load configuration from a JSON file."""
        config = createDefaultConfig()
        if not config.loadFromFile(filepath):
            raise NLMException(f"Failed to load configuration from {filepath}")
        return config
    
    @staticmethod
    def save_to_file(config: Config, filepath: str) -> bool:
        """Save configuration to a JSON file."""
        return config.saveToFile(filepath)


class SimulationMetrics:
    """Helper class for collecting and reporting simulation metrics."""
    
    @staticmethod
    def collect(brain: Brain, world: Optional[SimpleWorld] = None) -> Dict[str, Any]:
        """Collect simulation metrics from a brain and optional world."""
        metrics = {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "total_spike_count": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "developmental_stage": brain.getDevelopmentalStage(),
        }
        
        if world:
            body = world.getAgentBody()
            metrics.update({
                "agent_position": (body.x, body.y),
                "agent_orientation": body.orientation,
                "agent_energy": body.energy,
                "agent_health": body.health,
                "agent_age": body.age,
                "simulation_time": world.getSimulationTime(),
            })
        
        return metrics
    
    @staticmethod
    def format(metrics: Dict[str, Any], precision: int = 3) -> str:
        """Format metrics dictionary as a readable string."""
        lines = ["Simulation Metrics:"]
        lines.append("=" * 50)
        
        for key, value in metrics.items():
            if isinstance(value, float):
                formatted = f"{value:.{precision}f}"
            elif isinstance(value, tuple):
                formatted = "(" + ", ".join(f"{x:.{precision}f}" for x in value) + ")"
            else:
                formatted = str(value)
            lines.append(f"{key.replace('_', ' ').title()}: {formatted}")
        
        return "\n".join(lines)


class PerformanceMonitor:
    """Performance monitoring for NLM simulations."""
    
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.start_time = None
    
    def start(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
    
    def stop(self) -> Dict[str, float]:
        """Stop performance monitoring and return results."""
        if self.start_time is None:
            raise NLMException("Performance monitor not started")
        
        import time
        total_time = time.time() - self.start_time
        
        return {
            "total_time": total_time,
            "avg_step_time": np.mean(self.step_times) if self.step_times else 0.0,
            "min_step_time": np.min(self.step_times) if self.step_times else 0.0,
            "max_step_time": np.max(self.step_times) if self.step_times else 0.0,
            "steps_per_second": len(self.step_times) / total_time if total_time > 0 else 0.0,
        }
    
    def record_step(self, step_time: float):
        """Record step execution time."""
        self.step_times.append(step_time)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current performance statistics."""
        if not self.step_times:
            return {"status": "no_data"}
        
        return {
            "step_count": len(self.step_times),
            "avg_step_time": np.mean(self.step_times),
            "min_step_time": np.min(self.step_times),
            "max_step_time": np.max(self.step_times),
            "std_step_time": np.std(self.step_times),
        }


def register_brain(name: str, brain: Brain) -> None:
    """Register a brain instance for easy access."""
    _brain_registry[name] = brain


def get_brain(name: str) -> Brain:
    """Get a registered brain instance."""
    if name not in _brain_registry:
        raise NLMException(f"Brain '{name}' not found in registry")
    return _brain_registry[name]


def unregister_brain(name: str) -> None:
    """Unregister a brain instance."""
    if name not in _brain_registry:
        raise NLMException(f"Brain '{name}' not found in registry")
    del _brain_registry[name]


def list_brains() -> List[str]:
    """List all registered brain names."""
    return list(_brain_registry.keys())


def run_experiment(
    config: Config,
    world: SimpleWorld,
    num_steps: int,
    enable_learning: bool = True,
    enable_development: bool = True,
    enable_curiosity: bool = True,
    log_interval: int = 100
) -> Dict[str, Any]:
    """
    Run a complete agent simulation experiment.
    
    This is a convenience function that sets up a complete agent simulation
    with all the common subsystems enabled.
    
    Args:
        config: Brain configuration
        world: World instance
        num_steps: Number of simulation steps
        enable_learning: Enable reward modulation learning
        enable_development: Enable developmental progression
        enable_curiosity: Enable curiosity-driven exploration
        log_interval: Interval for logging progress
        
    Returns:
        Dictionary containing experiment results and metrics
    """
    # Create brain and agent
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    
    # Initialize everything
    brain.initialize()
    agent.initialize(world)
    
    # Enable subsystems
    if enable_learning:
        agent.enableRewardModulation(True)
    if enable_development:
        agent.enableDevelopment(True)
    if enable_curiosity:
        agent.enableCuriosity(True)
    
    # Run simulation
    results = {
        "metrics": [],
        "final_brain": brain,
        "final_agent": agent,
        "final_world": world,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if learning is enabled
        if enable_learning:
            # Simple reward based on energy
            reward = world.getAgentBody().energy / 100.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if enable_development:
            agent.updateDevelopment(0.1)
        
        # Log progress
        if step % log_interval == 0:
            metrics = SimulationMetrics.collect(brain, world)
            metrics["step"] = step
            results["metrics"].append(metrics)
    
    return results


def benchmark_brain_creation(num_brains: int = 10, **config_params) -> Dict[str, float]:
    """
    Benchmark brain creation performance.
    
    Args:
        num_brains: Number of brains to create for benchmarking
        **config_params: Configuration parameters for brain creation
        
    Returns:
        Dictionary containing benchmark results
    """
    import time
    
    config = BrainConfig.with_parameters(**config_params)
    
    start_time = time.time()
    brains = []
    
    for _ in range(num_brains):
        brain = createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    creation_time = time.time() - start_time
    
    # Cleanup
    for brain in brains:
        brain.reset()
    
    return {
        "creation_time": creation_time,
        "brains_per_second": num_brains / creation_time,
        "avg_creation_time": creation_time / num_brains,
    }


def benchmark_simulation(brain: Brain, world: SimpleWorld, num_steps: int) -> Dict[str, float]:
    """
    Benchmark simulation performance.
    
    Args:
        brain: Brain instance
        world: World instance
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary containing benchmark results
    """
    import time
    
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        brain.step(step)
    
    simulation_time = time.time() - start_time
    
    return {
        "simulation_time": simulation_time,
        "steps_per_second": num_steps / simulation_time,
        "avg_step_time": simulation_time / num_steps,
    }


# Create module-level constants from C++ constants
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

# Set up default config if needed
def _get_default_config() -> Config:
    """Get or create the default configuration."""
    global _default_config
    if _default_config is None:
        _default_config = createDefaultConfig()
    return _default_config

# Example usage and demonstrations
examples = {
    "minimal": """
import pynlm

# Create and initialize a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a short simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
""",
    
    "agent_world": """
import pynlm

# Complete agent example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning and development
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
"""
}

# Add examples to the module
__examples__ = examples

# Module cleanup function
_original_teardown = None

def cleanup() -> None:
    """Clean up module resources."""
    global _brain_registry, _world_registry, _agent_registry
    
    # Reset all registered brains
    for name, brain in _brain_registry.items():
        try:
            brain.reset()
        except Exception:
            pass
    
    _brain_registry.clear()
    _world_registry.clear()
    _agent_registry.clear()
    _default_config = None
    
    # Call original teardown if exists
    if _original_teardown:
        _original_teardown()

# Register cleanup with Python exit
import atexit
atexit.register(cleanup)

# Print module info when imported
if __name__ == "__main__":
    print(f"NLM Python module version {__version__}")
    print("Type 'help(pynlm)' for more information.")
