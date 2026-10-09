#!/usr/bin/env python3
"""
NLM Utility Scripts for Enhanced Usability

This module provides convenient scripts and utilities for working with NLM (Neural Learning Machine).
It includes:
- Brain training utilities
- Simulation helpers
- Data analysis tools
- Configuration management
- Experiment management
"""

import pynlm
import json
import time
from typing import List, Dict, Optional, Callable, Any
from dataclasses import dataclass
import threading
import queue

@dataclass
class TrainingConfig:
    """Configuration for brain training sessions."""
    steps: int = 1000
    time_step: float = 0.001
    world_width: int = 20
    world_height: int = 20
    vision_width: int = 8
    vision_height: int = 8
    enable_reward_modulation: bool = True
    enable_curiosity: bool = True
    enable_development: bool = True
    enable_structural_plasticity: bool = True
    save_interval: int = 100
    checkpoint_path: Optional[str] = None

@dataclass
class SimulationResult:
    """Results from a simulation run."""
    steps_completed: int
    total_reward: float
    avg_firing_rate: float
    max_firing_rate: float
    final_neuromodulation: float
    final_curiosity: float
    final_novelty: float
    final_prediction_error: float
    neurons_firing: int
    total_spikes: int
    simulation_time: float

def create_enhanced_brain(config_dict: Optional[Dict] = None) -> pynlm.PyBrain:
    """
    Create an enhanced brain with optimized default settings.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        PyBrain instance with enhanced capabilities
    """
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply enhanced default settings
    config.set("brain.neuron_count", 500, pynlm.ConfigSource.Default)
    config.set("brain.synapse_density", 0.1, pynlm.ConfigSource.Default)
    config.set("brain.connection_probability", 0.15, pynlm.ConfigSource.Default)
    config.set("brain.v_thresh", -50.0, pynlm.ConfigSource.Default)
    config.set("brain.v_rest", -70.0, pynlm.ConfigSource.Default)
    config.set("brain.v_reset", -75.0, pynlm.ConfigSource.Default)
    config.set("brain.tau_mem", 20.0, pynlm.ConfigSource.Default)
    config.set("brain.tau_ref", 2.0, pynlm.ConfigSource.Default)
    
    # Apply custom configuration if provided
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value, pynlm.ConfigSource.Default)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    return pynlm.PyBrain(brain)

def create_training_environment(config: TrainingConfig) -> tuple:
    """
    Create a complete training environment with brain, world, and agent.
    
    Args:
        config: Training configuration
        
    Returns:
        Tuple of (brain, world, agent)
    """
    # Create brain
    brain_config = {
        "brain.neuron_count": 800,
        "brain.synapse_density": 0.12,
        "brain.connection_probability": 0.18,
        "brain.v_thresh": -45.0,
        "brain.v_rest": -70.0,
        "brain.v_reset": -75.0,
    }
    brain = create_enhanced_brain(brain_config)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=config.world_width,
        height=config.world_height,
        visionWidth=config.vision_width,
        visionHeight=config.vision_height
    )
    world.setAgentStart(config.world_width // 2, config.world_height // 2)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain.brain)
    agent.initialize(world)
    
    # Enable features based on configuration
    if config.enable_reward_modulation:
        agent.enableRewardModulation(True)
    if config.enable_curiosity:
        agent.enableCuriosity(True)
    if config.enable_development:
        agent.enableDevelopment(True)
    if config.enable_structural_plasticity:
        agent.enableStructuralPlasticity(True)
    
    return brain, world, agent

class EpisodeRunner:
    """Manages training episodes with advanced features."""
    
    def __init__(self, config: TrainingConfig):
        self.config = config
        self.current_episode = 0
        self.total_steps = 0
        self.episode_rewards = []
        self.episode_lengths = []
        
    def run_episode(self, brain, world, agent, episode_id: int = None) -> SimulationResult:
        """
        Run a single training episode.
        
        Args:
            brain: Brain instance
            world: World instance
            agent: Agent instance
            episode_id: Optional episode ID for tracking
            
        Returns:
            SimulationResult with episode metrics
        """
        if episode_id is not None:
            self.current_episode = episode_id
            
        start_time = time.time()
        
        # Reset environment
        world.reset()
        world.setAgentStart(world.getWidth() // 2, world.getHeight() // 2)
        agent.reset()
        brain.reset()
        
        # Track metrics
        step_rewards = []
        firing_rates = []
        neuromodulation_levels = []
        
        for step in range(self.config.steps):
            # Update world
            world.update(self.config.time_step)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step, step * self.config.time_step)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            step_reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            step_rewards.append(step_reward)
            firing_rates.append(brain.getAverageFiringRate())
            neuromodulation_levels.append(agent.getNeuromodulationLevel())
            
            # Save checkpoint if needed
            if self.config.save_interval > 0 and step % self.config.save_interval == 0:
                if self.config.checkpoint_path:
                    checkpoint_file = f"{self.config.checkpoint_path}_episode_{self.current_episode}_step_{step}.bin"
                    brain.save(checkpoint_file)
            
            # Log progress for long episodes
            if step % 100 == 0:
                print(f"Episode {self.current_episode}, Step {step}: "
                      f"Reward: {step_reward:.3f}, "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Neuromod: {neuromodulation_levels[-1]:.3f}")
        
        # Calculate final metrics
        end_time = time.time()
        simulation_time = end_time - start_time
        
        final_neuromodulation = neuromodulation_levels[-1] if neuromodulation_levels else 0.0
        final_curiosity = agent.getCuriosityLevel()
        final_novelty = agent.getNoveltyLevel()
        final_prediction_error = agent.getPredictionError()
        
        result = SimulationResult(
            steps_completed=self.config.steps,
            total_reward=sum(step_rewards),
            avg_firing_rate=sum(firing_rates) / len(firing_rates) if firing_rates else 0.0,
            max_firing_rate=max(firing_rates) if firing_rates else 0.0,
            final_neuromodulation=final_neuromodulation,
            final_curiosity=final_curiosity,
            final_novelty=final_novelty,
            final_prediction_error=final_prediction_error,
            neurons_firing=brain.getFiringNeuronCount(),
            total_spikes=brain.getTotalSpikeCount(),
            simulation_time=simulation_time
        )
        
        # Record episode statistics
        self.episode_rewards.append(result.total_reward)
        self.episode_lengths.append(self.config.steps)
        self.total_steps += self.config.steps
        
        return result
    
    def get_training_stats(self) -> Dict:
        """Get training statistics across all episodes."""
        if not self.episode_rewards:
            return {}
            
        return {
            "total_episodes": len(self.episode_rewards),
            "total_steps": self.total_steps,
            "avg_reward_per_episode": sum(self.episode_rewards) / len(self.episode_rewards),
            "avg_steps_per_episode": sum(self.episode_lengths) / len(self.episode_lengths),
            "best_reward": max(self.episode_rewards),
            "worst_reward": min(self.episode_rewards),
            "reward_std": (sum((r - sum(self.episode_rewards) / len(self.episode_rewards)) ** 2 
                             for r in self.episode_rewards) / len(self.episode_rewards)) ** 0.5
        }

class ExperimentManager:
    """Manages complex experiments with multiple conditions."""
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
        
    def add_experiment(self, name: str, config: TrainingConfig, brain_config: Dict = None):
        """Add an experiment condition."""
        self.experiments[name] = {
            'config': config,
            'brain_config': brain_config or {}
        }
        
    def run_experiment(self, name: str) -> SimulationResult:
        """Run a single experiment."""
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
            
        exp = self.experiments[name]
        brain, world, agent = create_training_environment(exp['config'])
        
        runner = EpisodeRunner(exp['config'])
        result = runner.run_episode(brain, world, agent, episode_id=0)
        
        self.results[name] = {
            'result': result,
            'runner': runner
        }
        
        return result
        
    def run_all_experiments(self) -> Dict[str, SimulationResult]:
        """Run all experiments and return results."""
        all_results = {}
        
        for name in self.experiments:
            print(f"Running experiment: {name}")
            result = self.run_experiment(name)
            all_results[name] = result
            
        return all_results
        
    def compare_experiments(self) -> Dict:
        """Compare results across experiments."""
        if not self.results:
            return {}
            
        comparison = {}
        metric_names = ['total_reward', 'avg_firing_rate', 'max_firing_rate', 
                       'final_neuromodulation', 'final_curiosity', 'final_novelty']
        
        for metric in metric_names:
            values = []
            for exp_name, data in self.results.items():
                if hasattr(data['result'], metric):
                    values.append(getattr(data['result'], metric))
            
            if values:
                comparison[metric] = {
                    'mean': sum(values) / len(values),
                    'min': min(values),
                    'max': max(values),
                    'std': (sum((v - sum(values) / len(values)) ** 2 for v in values) / len(values)) ** 0.5
                }
                
        return comparison

def save_results_to_file(results: Dict, filename: str):
    """Save results to JSON file."""
    # Convert results to JSON-serializable format
    serializable_results = {}
    for name, result in results.items():
        serializable_results[name] = {
            'steps_completed': result.steps_completed,
            'total_reward': result.total_reward,
            'avg_firing_rate': result.avg_firing_rate,
            'max_firing_rate': result.max_firing_rate,
            'final_neuromodulation': result.final_neuromodulation,
            'final_curiosity': result.final_curiosity,
            'final_novelty': result.final_novelty,
            'final_prediction_error': result.final_prediction_error,
            'neurons_firing': result.neurons_firing,
            'total_spikes': result.total_spikes,
            'simulation_time': result.simulation_time
        }
    
    with open(filename, 'w') as f:
        json.dump(serializable_results, f, indent=2)
    
    print(f"Results saved to {filename}")

def load_config_from_file(filename: str) -> Dict:
    """Load configuration from JSON file."""
    with open(filename, 'r') as f:
        return json.load(f)

def create_preset_config(name: str) -> Dict:
    """Create preset configuration by name."""
    presets = {
        'quick_test': {
            "brain.neuron_count": 100,
            "brain.synapse_density": 0.1,
            "brain.connection_probability": 0.05,
            "simulation.timestep": 0.01,
        },
        'standard_training': {
            "brain.neuron_count": 500,
            "brain.synapse_density": 0.12,
            "brain.connection_probability": 0.18,
            "simulation.timestep": 0.001,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
        },
        'advanced_research': {
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "brain.connection_probability": 0.25,
            "simulation.timestep": 0.0005,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.2,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        },
        'minimal_brain': {
            "brain.neuron_count": 50,
            "brain.synapse_density": 0.05,
            "brain.connection_probability": 0.02,
        }
    }
    
    if name in presets:
        return presets[name]
    else:
        raise ValueError(f"Unknown preset: {name}")

def create_training_script() -> str:
    """Create a complete training script template."""
    return '''
#!/usr/bin/env python3
"""
Complete NLM Training Script Template
This template provides a structure for training NLM brains.
"""

import pynlm
import time
import json
from typing import Dict, List, Optional

def main():
    """Main training function."""
    print("Starting NLM training...")
    
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.15)
    
    # Create brain
    brain = pynlm.createBrain(config)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15, 15)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting simulation...")
    start_time = time.time()
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Reward: {agent.getNeuromodulationLevel():.3f}, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Save results
    results = {
        'final_reward': agent.getNeuromodulationLevel(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_prediction_error': agent.getPredictionError(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'simulation_time': end_time - start_time
    }
    
    with open('training_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print("Results saved to training_results.json")
    print("Training completed successfully!")

if __name__ == "__main__":
    main()
'''

# Convenience aliases
create_brain = create_enhanced_brain
create_world = pynlm.createSimpleWorld
create_agent = pynlm.createAgentBrain
run_simulation = EpisodeRunner.run_episode

print("NLM Utility Module loaded successfully!")
print("Available functions:")
print("  - create_brain(config_dict=None): Create enhanced brain")
print("  - create_world(): Create simple world")
print("  - create_agent(brain): Create agent brain interface")
print("  - EpisodeRunner: Manage training episodes")
print("  - ExperimentManager: Manage multiple experiments")
print("  - save_results_to_file(results, filename): Save results")
print("  - create_preset_config(name): Create preset configurations")
print("  - create_training_script(): Generate training script template")
