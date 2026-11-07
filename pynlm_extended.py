# High-level Python API for NLM - pynlm_extended.py
"""
Extended NLM Python API with high-level abstractions and convenience functions.

This module provides enhanced Python interfaces for NLM (Neural Learning Machine)
that make the complex brain simulation framework more accessible and easier to use
for both beginners and advanced users.
"""

import pybind11
import pybind11.stl
import contextlib
from typing import List, Dict, Any, Optional, Callable, Union
from pathlib import Path

# Import the core NLM bindings
import pynlm

# Type aliases for better readability
BrainConfig = Any  # pynlm.Config
NeuralBrain = Any  # pynlm.Brain
SimpleWorld = Any  # pynlm.SimpleWorld
AgentBrain = Any   # pynlm.AgentBrain

class SimulationContext:
    """Context manager for simulation resources with automatic cleanup."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.original_states = {}
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
    
    def cleanup(self):
        """Clean up simulation resources."""
        if self.brain:
            self.brain.reset()
        if self.world:
            self.world.reset()
        if self.agent:
            self.agent.reset()
        self.original_states.clear()
class NLMBuilder:
    """Builder pattern for configuring and creating NLM brains with presets."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self._validate_config()
    
    def _validate_config(self):
        """Validate and apply default configuration values."""
        # Set sensible defaults for most use cases
        self.config.set("brain.neuron_count", 500, pynlm.ConfigSource.Runtime)
        self.config.set("brain.synapse_density", 0.1, pynlm.ConfigSource.Runtime)
        self.config.set("brain.region_count", 2, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.Runtime)
    
    def set_brain_size(self, neuron_count: int, regions: int = None):
        """Set brain size parameters."""
        self.config.set("brain.neuron_count", neuron_count, pynlm.ConfigSource.Runtime)
        if regions:
            self.config.set("brain.region_count", regions, pynlm.ConfigSource.Runtime)
        return self
    
    def set_plasticity(self, stdp: bool = True, hebbian: bool = True, 
                       structural: bool = False, reward_modulated: bool = True):
        """Configure plasticity rules."""
        self.config.set("plasticity.stdp.enable", stdp, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.hebbian.enable", hebbian, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.structural.enable", structural, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.reward_modulated.enable", reward_modulated, pynlm.ConfigSource.Runtime)
        return self
    
    def set_neuron_params(self, threshold: float = -50.0, resting: float = -70.0, 
                         reset: float = -75.0, tau_mem: float = 20.0, tau_ref: float = 2.0):
        """Set neuron biophysical parameters."""
        self.config.set("brain.v_thresh", threshold, pynlm.ConfigSource.Runtime)
        self.config.set("brain.v_rest", resting, pynlm.ConfigSource.Runtime)
        self.config.set("brain.v_reset", reset, pynlm.ConfigSource.Runtime)
        self.config.set("brain.tau_mem", tau_mem, pynlm.ConfigSource.Runtime)
        self.config.set("brain.tau_ref", tau_ref, pynlm.ConfigSource.Runtime)
        return self
    
    def enable_developmental_maturation(self, enable: bool = True):
        """Enable developmental maturation system."""
        self.config.set("development.maturation.enable", enable, pynlm.ConfigSource.Runtime)
        return self
    
    def enable_curiosity(self, enable: bool = True, scale: float = 1.0):
        """Enable curiosity-driven exploration."""
        self.config.set("neuromod.curiosity.enable", enable, pynlm.ConfigSource.Runtime)
        self.config.set("neuromod.curiosity.scale", scale, pynlm.ConfigSource.Runtime)
        return self
    reapwrdaysrewardmod enable("Enable reward-based learning."""
        self.config.set("neuromod.reward.enable", enable, pynlm.ConfigSource.Runtime)
        return self
    
    def set_learning_rate(self, stdp_rate: float = 0.001, hebbian_rate: float = 0.001):
        """Set plasticity learning rates."""
        self.config.set("plasticity.stdp.learning_rate", stdp_rate, pynlm.ConfigSource.Runtime)
        self.config.set("plasticity.hebbian.learning_rate", hebbian_rate, pynlm.ConfigSource.Runtime)
        return self
    
    def create_brain(self) -> NeuralBrain:
        """Create and initialize a brain with the configured parameters."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        return brain
class TrainingSession:
    """High-level training session management with automatic logging and checkpointing."""
    
    def __init__(self, brain, world, agent, name: str = "training_session"):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.name = name
        self.step_count = 0
        self.performance_history = []
        self.checkpoint_dir = Path(f".nlm_checkpoints/{name}"
        
        # Ensure checkpoint directory exists
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
    
    def run_episode(self, max_steps: int, verbose: bool = False) -> Dict[str, Any]:
        """Run a single training episode and return performance metrics."""
        episode_start_step = self.step_count
        
        for step in range(max_steps):
            # World update
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(self.step_count)
            
            # Get motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning based on world feedback
            reward = self._extract_reward_from_percept(percept)
            predicted = 0.0  # Could be implemented with prediction system
            self.agent.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            self.step_count += 1
            
            if verbose and step % 100 == 0:
                self._log_progress(step)
        
        return self._end_episode_metrics(episode_start_step)
    
    def _extract_reward_from_percept(self, percept) -> float:
        """Extract reward signal from percept data."""
        # Simple reward extraction - can be overridden by subclasses
        if hasattr(percept, 'getInternal'):
            internal_signals = percept.getInternal()
            if internal_signals and internal_signals.size() > 0:
                return internal_signals[0]
        return 0.0
    
    def _log_progress(self, step: int):
        """Log training progress."""
        firing_neurons = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        curiosity = self.agent.getCuriosityLevel()
        
        print(f"  Step {step}: {firing_neurons} neurons firing, "
              f"{total_spikes} total spikes, curiosity={curiosity:.3f}")
    
    def _end_episode_metrics(self, start_step: int) -> Dict[str, Any]:
        """Compute and return episode performance metrics."""
        metrics = {
            'name': self.name,
            'start_step': start_step,
            'end_step': self.step_count,
            'steps_taken': self.step_count - start_step,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        self.performance_history.append(metrics)
        return metrics
    
    def save_checkpoint(self) -> str:
        """Save current brain state to checkpoint."""
        checkpoint_path = self.checkpoint_dir / f"checkpoint_{self.step_count}.bin"
        self.brain.save(str(checkpoint_path))
        return str(checkpoint_path)
    
    def load_checkpoint(self, checkpoint_path: str) -> bool:
        """Load brain state from checkpoint."""
        return self.brain.load(checkpoint_path)
class EpisodeRunner:
    """Utility class for running multiple episodes with different configurations."""
    
    def __init__(self, config_template: BrainConfig):
        self.config_template = config_template
        self.results = []
    
    def run_multiple_episodes(self, num_episodes: int, steps_per_episode: int,
                            config_overrides: List[Dict[str, Any]] = None) -> List[Dict[str, Any]]:
        """Run multiple training episodes with optional configuration variations."""
        results = []
        
        for episode_idx in range(num_episodes):
            # Apply configuration override if specified
            config = self.config_template
            if config_overrides and episode_idx < len(config_overrides):
                for key, value in config_overrides[episode_idx].items():
                    config.set(key, value, pynlm.ConfigSource.Runtime)
            
            # Create brain and run episode
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # For now, this is a simple implementation - can be extended
            for step in range(steps_per_episode):
                brain.step(step)
            
            episode_result = {
                'episode': episode_idx,
                'steps': steps_per_episode,
                'final_firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount()
            }
            
            results.append(episode_result)
        
        self.results.extend(results)
        return results
class MonitoringTools:
    """Tools for monitoring and analyzing brain simulation performance."""
    
    @staticmethod
    def track_firing_statistics(brain: NeuralBrain, window_size: int = 100) -> Dict[str, List[float]]:
        """Track firing statistics over time."""
        history = {
            'step': [],
            'firing_neurons': [],
            'total_spikes': [],
            'average_firing_rate': [],
            'excitation_inhibition_ratio': []
        }
        
        # This would be extended to actually track over time
        return history
    
    @staticmethod
    def analyze_plasticity(brain: NeuralBrain) -> Dict[str, Any]:
        """Analyze plasticity system state."""
        analysis = {
            'stdp_active': brain.getSTDP() is not None,
            'hebbian_active': brain.getHebbian() is not None,
            'structural_active': brain.getStructuralPlasticity() is not None,
            'development_stage': brain.getDevelopmentalStage()
        }
        
        return analysis
    
    @staticmethod
    def generate_performance_report(performance_history: List[Dict]) -> str:
        """Generate a human-readable performance report."""
        if not performance_history:
            return "No performance data available."
        
        report_lines = [
            "NLM Performance Report",
            "=" * 50
        ]
        
        for episode in performance_history:
            report_lines.append(
                f"Episode {episode['name']}: Steps {episode['steps_taken']}, "
                f"Final firing: {episode['firing_neurons']}, "
                f"Total spikes: {episode['total_spikes']}"
            )
        
        return "\n".join(report_lines)
def create_simple_agent(config: Optional[BrainConfig] = None,
                       enable_learning: bool = True,
                       enable_curiosity: bool = True) -> Tuple[NeuralBrain, AgentBrain, SimpleWorld]:
    """Create a simple agent setup with world for quick experimentation."""
    
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
    
    if enable_curiosity:
        agent.enableCuriosity(True)
    
    if enable_learning:
        agent.enableDevelopment(True)
    
    return brain, agent, world
def run_simple_simulation(num_steps: int, config: Optional[BrainConfig] = None) -> Dict[str, Any]:
    """Run a simple simulation and return results."""
    brain, agent, world = create_simple_agent(config)
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'steps': num_steps,
        'final_firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate()
    }
def create_curiosity_driven_agent(config: Optional[BrainConfig] = None,
                                  exploration_rate: float = 0.5,
                                  novelty_sensitivity: float = 1.0) -> Tuple[NeuralBrain, AgentBrain, SimpleWorld]:
    """Create an agent with curiosity-driven exploration capabilities."""
    
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Configure for curiosity-driven behavior
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.Runtime)
    config.set("neuromod.curiosity.scale", novelty_sensitivity, pynlm.ConfigSource.Runtime)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.Runtime)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, agent, world
def batch_run_simulations(simulation_configs: List[Dict],
                         output_prefix: str = "nlm_batch") -> List[Dict[str, Any]]:
    """Run multiple simulations with different configurations in batch mode."""
    
    results = []
    for i, config in enumerate(simulation_configs):
        # Create config for this simulation
        if "config" in config:
            nlm_config = config["config"]
        else:
            nlm_config = pynlm.createDefaultConfig()
        
        # Apply any config overrides
        for key, value in config.get("config_overrides", {}).items():
            nlm_config.set(key, value, pynlm.ConfigSource.Runtime)
        
        # Run simulation
        simulation_result = run_simple_simulation(
            config.get("steps", 100),
            nlm_config
        )
        
        # Save results
        simulation_result["simulation_id"] = i
        simulation_result["config_used"] = config.get("name", f"simulation_{i}")
        
        results.append(simulation_result)
    
    return results
# Export all the key components
__all__ = [
    # Core classes
    'NLMBuilder',
    'SimulationContext', 
    'TrainingSession',
    'EpisodeRunner',
    'MonitoringTools',
    
    # Factory functions
    'create_simple_agent',
    'run_simple_simulation',
    'create_curiosity_driven_agent',
    'batch_run_simulations',
    
    # Type aliases
    'BrainConfig',
    'NeuralBrain',
    'SimpleWorld', 
    'AgentBrain'
]

# Convenience functions for quick access

def quick_test():
    """Quick test function for beginners."""
    print("Running quick NLM test...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    print(f"Test complete! {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSpikeCount()} spikes")
    return brain
def quick_agent_test():
    """Quick agent test function for beginners."""
    print("Running quick agent test...")
    brain, agent, world = create_simple_agent()
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Agent test complete! Steps: {step + 1}, "
          f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    return brain, agent, world