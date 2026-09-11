"""
NLM - Neural Learning Machine Python Bindings

High-level Python interface for the NLM brain simulator.

This module provides Python bindings for the NLM C++ neural simulation framework,
with an emphasis on ease of use for beginners and extensibility for advanced users.
"""

import sys
from typing import Optional, List, Tuple, Dict, Any, Union

# Try to import the C++ bindings
try:
    from pynlm import (
        # Core types
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enums
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        
        # Core classes
        Config,
        Brain,
        SimpleWorld,
        AgentBrain,
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        SensoryPercept,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
        
        # Integrated systems
        SpikeSystem,
        STDP,
        Hebbian,
        StructuralPlasticity,
        Neuromodulator,
        Dopamine,
        Curiosity,
        Novelty,
        PredictionError,
        NeuralWorkingMemory,
        NeuralEpisodicMemory,
        NeuralAssociativeMemory,
        PredictionSystem,
        NeuralPlanner,
        ConceptFormation,
        AttentionalSelection,
        DevelopmentSystem,
    )
    
    PYBIND_AVAILABLE = True
except ImportError as e:
    PYBIND_AVAILABLE = False
    print(f"Warning: C++ bindings not available: {e}")
    print("Please ensure pynlm is built with 'pip install -e .'")

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine)"


class NLMException(Exception):
    """Base exception for NLM errors."""
    pass
class NLMConfig:
    """Configuration builder for NLM systems with a fluent API."""
    
    @staticmethod
    def create_default() -> Config:
        """Create a default configuration with sensible values."""
        config = Config()
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        config.set("neuron_count", 500)
        config.set("region_count", 1)
        config.set("connection_probability", 0.15f)
        config.set("stdp_ltp_weight", 0.02f)
        config.set("stdp_ltd_weight", 0.015f)
        config.set("stdp_tau", 20.0f)
        config.set("synaptogenesis_rate", 0.0001f)
        config.set("pruning_rate", 0.00001f)
        return config
    
    @staticmethod
    def create_simulation_config(
        timestep: float = 0.001,
        neurons: int = 1000,
        regions: int = 2,
        connection_prob: float = 0.2,
        random_seed: int = 42
    ) -> Config:
        """Create a simulation-specific configuration.
        
        Args:
            timestep: Simulation timestep in seconds
            neurons: Total number of neurons to create
            regions: Number of brain regions
            connection_prob: Probability of synaptic connections
            random_seed: Random seed for reproducibility
            
        Returns:
            Configured Config object
        """
        config = Config()
        config.set("random_seed", random_seed)
        config.set("simulation_timestep", timestep)
        config.set("neuron_count", neurons)
        config.set("region_count", regions)
        config.set("connection_probability", connection_prob)
        return config
    
    @staticmethod
    def create_learning_config() -> Config:
        """Create a configuration optimized for learning.
        
        Returns:
            Config object with learning-oriented parameters
        """
        config = Config.create_default()
        
        # Learning-specific parameters
        config.set("stdp_ltp_weight", 0.03f)  # Stronger LTP for learning
        config.set("stdp_ltd_weight", 0.02f)  # Stronger LTD for forgetting
        config.set("stdp_tau", 25.0f)  # Longer STDP window
        config.set("synaptogenesis_rate", 0.0002f)  # More structural learning
        config.set("pruning_rate", 0.00005f)  # Less pruning
        
        # Enable plasticity
        config.set("enable_reward_modulation", True)
        config.set("enable_structural_plasticity", True)
        config.set("enable_development", True)
        config.set("enable_curiosity", True)
        
        return config
class NLMNeuron:
    """High-level wrapper for neuron operations with Pythonic API."""
    
    def __init__(self, neuron: Brain):
        self._neuron = neuron
    
    @property
    def id(self) -> NeuronId:
        """Get neuron ID."""
        return self._neuron.getId()
    
    @property
    def type(self) -> NeuronType:
        """Get neuron type."""
        return self._neuron.getType()
    
    @type.setter
    def type(self, value: NeuronType):
        """Set neuron type."""
        self._neuron.setType(value)
    
    @property
    def membrane_potential(self) -> float:
        """Get current membrane potential (mV)."""
        return self._neuron.getMembranePotential()
    
    @property
    def threshold(self) -> float:
        """Get firing threshold (mV)."""
        return self._neuron.getThreshold()
    
    @threshold.setter
    def threshold(self, value: float):
        """Set firing threshold (mV)."""
        self._neuron.setThreshold(value)
    
    @property
    def is_firing(self) -> bool:
        """Check if neuron is currently firing."""
        return self._neuron.isFiring()
    
    @property
    def is_refractory(self) -> bool:
        """Check if neuron is in refractory period."""
        return self._neuron.isRefractory()
    
    @property
    def refractory_period(self) -> int:
        """Get refractory period in steps."""
        return self._neuron.getRefractoryPeriod()
    
    @property
    def firing_rate(self) -> float:
        """Get current firing rate (Hz)."""
        return self._neuron.getFiringRate()
    
    @property
    def total_spikes(self) -> int:
        """Get total spike count."""
        return self._neuron.getTotalSpikeCount()
    
    @property
    def active_synapses(self) -> int:
        """Get number of active synapses."""
        return self._neuron.getActiveSynapseCount()
    
    def inject_current(self, current: float) -> None:
        """Inject current into neuron.
        
        Args:
            current: Current to inject (arbitrary units)
        """
        self._neuron.injectCurrent(current)
    
    def record_spike(self, timestamp: float) -> None:
        """Record a spike at given timestamp.
        
        Args:
            timestamp: Time when spike occurred
        """
        self._neuron.recordSpike(timestamp)
    
    def reset(self) -> None:
        """Reset neuron to initial state."""
        self._neuron.reset()
    
    def step(self, current_time: float) -> None:
        """Advance neuron by one simulation step.
        
        Args:
            current_time: Current simulation time
        """
        self._neuron.step(current_time)
    
    def __repr__(self) -> str:
        return f"<NLMNeuron id={self.id} type={self.type} spikes={self.total_spikes}>"
class NLMWorld:
    """High-level wrapper for world/simulation operations."""
    
    def __init__(self, world: SimpleWorld):
        self._world = world
    
    @property
    def width(self) -> int:
        """Get world width in cells."""
        return self._world.getWidth()
    
    @property
    def height(self) -> int:
        """Get world height in cells."""
        return self._world.getHeight()
    
    @property
    def vision_width(self) -> int:
        """Get vision width (perception range)."""
        return self._world.getVisionWidth()
    
    @property
    def vision_height(self) -> int:
        """Get vision height (perception range)."""
        return self._world.getVisionHeight()
    
    @property
    def simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def configure(
        self,
        width: int,
        height: int,
        vision_width: int,
        vision_height: int
    ) -> None:
        """Configure world dimensions and perception.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self) -> None:
        """Reset world to initial state."""
        self._world.reset()
    
    def update(self, timestep: float) -> None:
        """Update world simulation by given timestep.
        
        Args:
            timestep: Time to advance (seconds)
        """
        self._world.update(timestep)
    
    def apply_action(self, action: Action, timestamp: float) -> ActionResult:
        """Apply motor command to world.
        
        Args:
            action: Action to apply
            timestamp: Time when action occurs
            
        Returns:
            Result of applying action
        """
        return self._world.applyAction(action, timestamp)
    
    def get_percept(self) -> SensoryPercept:
        """Get current sensory percept from world.
        
        Returns:
            Current sensory percept
        """
        return self._world.getSensoryPercept()
    
    def add_object(self, obj: WorldObject) -> None:
        """Add object to world.
        
        Args:
            obj: World object to add
        """
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float) -> None:
        """Remove object at given position.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid in world.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        return self._world.isValidPosition(x, y)
class NLMEnvironment:
    """High-level wrapper for NLM agent-environment interaction."""
    
    def __init__(self, brain: Brain, world: SimpleWorld):
        self._brain = brain
        self._world = world
        self._agent = AgentBrain(brain)
        self._agent.initialize(world)
    
    def run_step(self, timestep: float = 0.1) -> Dict[str, Any]:
        """Run a single simulation step.
        
        This performs the complete world-agent loop:
        1. Update world
        2. Get perception
        3. Process sensory input
        4. Brain computation
        5. Motor command
        6. Apply action
        
        Args:
            timestep: Simulation timestep
            
        Returns:
            Dictionary with step results
        """
        # Update world
        self._world.update(timestep)
        
        # Get what the agent sees
        percept = self._world.getSensoryPercept()
        
        # Tell the brain what the agent sees
        self._agent.processSensoryInput(percept)
        
        # Brain thinks/decides
        brain_step = self._brain.step(self._world.getSimulationTime())
        
        # Get action from brain
        action = self._agent.decodeMotorCommand()
        
        # Apply action in world
        result = self._world.applyAction(action, self._world.getSimulationTime())
        
        # Return step results
        return {
            "perception": percept,
            "brain_step": brain_step,
            "action": action,
            "action_result": result,
            "simulation_time": self._world.getSimulationTime(),
            "neurons_firing": self._brain.getFiringNeuronCount(),
            "total_spikes": self._brain.getTotalSpikeCount(),
            "reward": result.reward,
            "success": result.success,
        }
    
    def run_episode(
        self,
        max_steps: int = 1000,
        enable_learning: bool = True,
        verbose: bool = False
    ) -> List[Dict[str, Any]]:
        """Run a complete simulation episode.
        
        Args:
            max_steps: Maximum number of simulation steps
            enable_learning: Whether to enable learning systems
            verbose: Whether to print progress
            
        Returns:
            List of step results
        """
        # Enable learning systems if requested
        if enable_learning:
            self._agent.enableRewardModulation(True)
            self._agent.enableCuriosity(True)
            self._agent.enableDevelopment(True)
        
        episode_data = []
        
        for step in range(max_steps):
            if verbose and step % 100 == 0:
                print(f"Step {step}/{max_steps}")
            
            result = self.run_step()
            episode_data.append(result)
            
            # Check if episode should end
            if result["success"] is False and result["action_result"].success:
                # Action failed, end episode
                if verbose:
                    print(f"Episode ended at step {step}: {result['action_result'].message}")
                break
        
        if verbose:
            print(f"Episode completed: {len(episode_data)} steps")
        
        return episode_data
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics.
        
        Returns:
            Dictionary with brain statistics
        """
        return {
            "total_neurons": self._brain.getTotalNeuronCount(),
            "firing_neurons": self._brain.getFiringNeuronCount(),
            "total_synapses": self._brain.getTotalSynapseCount(),
            "total_spikes": self._brain.getTotalSpikeCount(),
            "average_firing_rate": self._brain.getAverageFiringRate(),
            "excitatory_inhibitory_ratio": self._brain.getExcitationInhibitionRatio(),
            "developmental_stage": self._agent.getDevelopmentalStage(),
            "neuromodulation_level": self._agent.getNeuromodulationLevel(),
            "curiosity_level": self._agent.getCuriosityLevel(),
            "novelty_level": self._agent.getNoveltyLevel(),
            "prediction_error": self._agent.getPredictionError(),
        }
    
    def save_brain(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            True if save successful
        """
        return self._brain.save(filepath)
    
    def load_brain(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            True if load successful
        """
        return self._brain.load(filepath)
class NLMResearch:
    """Tools for running NLM experiments and research protocols."""
    
    @staticmethod
    def create_experiment(
        brain: Brain,
        world: SimpleWorld,
        name: str,
        max_steps: int = 1000
    ) -> 'Experiment':
        """Create a research experiment.
        
        Args:
            brain: Brain to use in experiment
            world: World for simulation
            name: Experiment name
            max_steps: Maximum simulation steps
            
        Returns:
            Configured experiment object
        """
        return Experiment(brain, world, name, max_steps)
    
    @staticmethod
    def benchmark_system(
        brain: Brain,
        world: SimpleWorld,
        test_name: str = "default"
    ) -> Dict[str, Any]:
        """Run performance benchmark on NLM system.
        
        Args:
            brain: Brain to benchmark
            world: World for simulation
            test_name: Name for benchmark test
            
        Returns:
            Benchmark results
        """
        # Implementation depends on benchmark system
        pass
    
    @staticmethod
    def analyze_brain_state(brain: Brain) -> Dict[str, Any]:
        """Analyze brain state and return insights.
        
        Args:
            brain: Brain to analyze
            
        Returns:
            Analysis results
        """
        # Implementation depends on analysis tools
        pass
class Experiment:
    """Research experiment container."""
    
    def __init__(self, brain: Brain, world: SimpleWorld, name: str, max_steps: int):
        self.brain = brain
        self.world = world
        self.name = name
        self.max_steps = max_steps
        self.environment = NLMEnvironment(brain, world)
        self.results = []
    
    def run(self, **kwargs) -> List[Dict[str, Any]]:
        """Run the experiment.
        
        Args:
            **kwargs: Additional parameters for run
            
        Returns:
            Experiment results
        """
        self.results = self.environment.run_episode(
            max_steps=self.max_steps, **kwargs
        )
        return self.results
    
    def get_summary(self) -> Dict[str, Any]:
        """Get experiment summary statistics.
        
        Returns:
            Summary statistics
        """
        if not self.results:
            return {}
        
        total_steps = len(self.results)
        
        # Calculate statistics
        neurons_firing = [r["neurons_firing"] for r in self.results]
        total_spikes = [r["total_spikes"] for r in self.results]
        rewards = [r["reward"] for r in self.results]
        
        return {
            "name": self.name,
            "total_steps": total_steps,
            "average_neurons_firing": sum(neurons_firing) / total_steps,
            "total_spike_count": sum(total_spikes),
            "total_reward": sum(rewards),
            "average_reward": sum(rewards) / total_steps,
            "max_reward": max(rewards),
            "min_reward": min(rewards),
        }
# High-level convenience functions
def create_brain(
    neuron_count: int = 500,
    region_count: int = 1,
    connection_probability: float = 0.15,
    random_seed: int = 42
) -> Brain:
    """Create and initialize a brain with specified parameters.
    
    Args:
        neuron_count: Number of neurons to create
        region_count: Number of brain regions
        connection_probability: Probability of synaptic connections
        random_seed: Random seed for reproducibility
        
    Returns:
        Initialized Brain object
    """
    config = Config()
    config.set("random_seed", random_seed)
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    
    brain = Brain(config)
    brain.initialize()
    return brain
def create_default_environment() -> Tuple[Brain, SimpleWorld, NLMEnvironment]:
    """Create a default NLM environment for quick testing.
    
    Returns:
        Tuple of (brain, world, environment)
    """
    config = Config.create_default()
    brain = Brain(config)
    brain.initialize()
    
    world = SimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    environment = NLMEnvironment(brain, world)
    
    return brain, world, environment
def run_demo_silent_brain() -> Dict[str, Any]:
    """Run a silent brain demonstration (just neural computation).
    
    Returns:
        Demo results
    """
    brain, world, env = create_default_environment()
    
    # Just let the brain think for 100 steps
    for step in range(100):
        env.run_step(0.1)
    
    stats = env.get_brain_stats()
    stats["demo_type"] = "silent_brain"
    
    return stats
def run_demo_brain_watching_world() -> Dict[str, Any]:
    """Run brain watching world demonstration.
    
    Returns:
        Demo results
    """
    brain, world, env = create_default_environment()
    
    # Watch the world for 50 steps
    for step in range(50):
        env.run_step(0.1)
    
    stats = env.get_brain_stats()
    stats["demo_type"] = "brain_watching_world"
    
    return stats
def run_demo_complete_agent() -> Dict[str, Any]:
    """Run complete agent demonstration with learning.
    
    Returns:
        Demo results
    """
    brain, world, env = create_default_environment()
    
    # Enable learning
    env._agent.enableRewardModulation(True)
    env._agent.enableCuriosity(True)
    
    # Run agent
    for step in range(100):
        env.run_step(0.1)
    
    stats = env.get_brain_stats()
    stats["demo_type"] = "complete_agent"
    
    return stats
# Main function for command-line usage
def main() -> None:
    """Example usage of NLM Python API."""
    print("NLM - Neural Learning Machine Python Example")
    print("=" * 50)
    
    # Example 1: Silent brain
    print("\n1. Silent Brain Example:")
    result1 = run_demo_silent_brain()
    print(f"   Neurons: {result1['total_neurons']}")
    print(f"   Spikes: {result1['total_spikes']}")
    
    # Example 2: Brain watching world
    print("\n2. Brain Watching World Example:")
    result2 = run_demo_brain_watching_world()
    print(f"   Neurons: {result2['total_neurons']}")
    print(f"   Average firing rate: {result2['average_firing_rate']:.2f} Hz")
    
    # Example 3: Complete agent
    print("\n3. Complete Agent Example:")
    result3 = run_demo_complete_agent()
    print(f"   Neurons: {result3['total_neurons']}")
    print(f"   Total reward: {result3['total_reward']:.2f}")
    print(f"   Curiosity: {result3['curiosity_level']:.2f}")
    
    print("\n" + "=" * 50)
    print("Examples completed successfully!")
    print("\nFor more examples, see:")
    print("  - easy_usage.md: Quick start guide")
    print("  - HOW_TO_USE.md: Detailed documentation")
    print("  - docs/ARCHITECTURE.md: Technical architecture")
if __name__ == "__main__":
    if PYBIND_AVAILABLE:
        main()
    else:
        print("Error: C++ bindings not available.")
        print("Please build pynlm first: pip install -e .")
        sys.exit(1)
