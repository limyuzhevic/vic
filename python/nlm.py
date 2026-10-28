# NLM Python High-Level Interface
# 
# This module provides a Python-friendly API for the NLM (Neural Learning Machine) framework.
# It wraps the C++ bindings to provide a more intuitive Python interface for common operations.

import json
import time
from typing import List, Optional, Dict, Any, Callable
import threading

# Import the C++ bindings
from pynlm import (
    NeuronId, SynapseId, Config, Brain, AgentBrain, SimpleWorld,
    SensoryInput, MotorCommand, Action, SensoryPercept
)

class NLMConfiguration:
    """Python-friendly configuration wrapper for NLM brain configuration."""
    
    def __init__(self, config_file: Optional[str] = None):
        self._config = Config()
        
        # Set sensible defaults for Python users
        self._config.set("random_seed", 42)
        self._config.set("simulation_timestep", 0.001)
        self._config.set("neuron_count", 1000)
        self._config.set("region_count", 2)
        self._config.set("connection_probability", 0.1)
        
        if config_file:
            self.load_from_file(config_file)
    
    def load_from_file(self, config_file: str) -> bool:
        """Load configuration from file."""
        return self._config.loadFromFile(config_file)
    
    def set(self, key: str, value: Any, source: str = "Python"):
        """Set configuration value."""
        self._config.set(key, value, source)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        return self._config.get(key, default)
    
    def get_as(self, key: str, default: Any = None) -> Any:
        """Get configuration value with type conversion."""
        return self._config.getAs(key, default)
    
    def save_to_file(self, filename: str) -> bool:
        """Save configuration to file."""
        return self._config.saveToFile(filename)
    
    def display(self):
        """Display current configuration."""
        print("=== NLM Configuration ===")
        print(f"  random_seed: {self.get('random_seed')}")
        print(f"  simulation_timestep: {self.get('simulation_timestep')}s")
        print(f"  neuron_count: {self.get('neuron_count')}")
        print(f"  region_count: {self.get('region_count')}")
        print(f"  connection_probability: {self.get('connection_probability')}")
        print(f"  stdp_ltp_weight: {self.get('stdp_ltp_weight')}")
        print(f"  stdp_ltd_weight: {self.get('stdp_ltd_weight')}")
        print(f"  synaptogenesis_rate: {self.get('synaptogenesis_rate')}")
        print(f"  pruning_rate: {self.get('pruning_rate')}")

class NLMBrain:
    """Python-friendly brain wrapper with high-level methods."""
    
    def __init__(self, config: Optional[NLMConfiguration] = None):
        self._config = config._config if config else self._create_default_config()
        self._brain = Brain(self._config)
        self._initialized = False
    
    def _create_default_config(self) -> Config:
        """Create a default configuration."""
        config = Config()
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        return config
    
    def initialize(self) -> bool:
        """Initialize the brain."""
        self._initialized = self._brain.initialize()
        return self._initialized
    
    def reset(self):
        """Reset the brain to initial state."""
        if self._initialized:
            self._brain.reset()
            self._brain.initialize()
    
    def step(self, step: int, time: float):
        """Advance brain by one step."""
        if self._initialized:
            self._brain.step(step, time)
    
    def inject_sensory(self, neuron_type: str, current: float):
        """Inject current into sensory neurons."""
        if self._initialized:
            # Map neuron_type string to enum
            neuron_type_enum = getattr(NeuronType, neuron_type, NeuronType::Sensory)
            self._brain.injectCurrentToNeurons(neuron_type_enum, current)
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get current brain metrics."""
        return {
            "total_neuron_count": self._brain.getTotalNeuronCount(),
            "total_synapse_count": self._brain.getTotalSynapseCount(),
            "total_spike_count": self._brain.getTotalSpikeCount(),
            "firing_neuron_count": self._brain.getFiringNeuronCount(),
            "average_firing_rate": self._brain.getAverageFiringRate(),
            "working_memory": self._brain.getWorkingMemory() is not None,
            "episodic_memory": self._brain.getEpisodicMemory() is not None,
            "dopamine_level": self._brain.getDopamine() is not None,
            "curiosity_level": self._brain.getCuriosity() is not None,
            "novelty_level": self._brain.getNovelty() is not None,
            "prediction_system": self._brain.getPredictionSystem() is not None,
            "development_system": self._brain.getDevelopmentSystem() is not None,
        }
    
    def log_status(self):
        """Log brain status."""
        self._brain.logStatus()

class NLMWorld:
    """Python-friendly world interface."""
    
    def __init__(self, width: int = 20, height: int = 20):
        self._world = SimpleWorld()
        self._world.initialize(width, height)
    
    def get_observation(self, brain_region) -> SensoryPercept:
        """Get sensory observation from the brain region."""
        return self._world.observe(brain_region)
    
    def apply_action(self, brain_region, action: MotorCommand) -> Dict[str, Any]:
        """Apply action to the world."""
        return self._world.applyAction(brain_region, action)
    
    def compute_reward(self, brain_region) -> float:
        """Compute reward for the brain region."""
        return self._world.computeReward(brain_region)

class NLMEnvironment:
    """High-level environment manager for closed-loop brain interaction."""
    
    def __init__(self, width: int = 32, height: int = 32):
        self.width = width
        self.height = height
        self.brain = None
        self.agent = None
        self.world = NLMWorld(width, height)
        self.running = False
        self.step_count = 0
        
        # Performance monitoring
        self.step_times = []
        self.metrics_history = []
    
    def create_brain(self, neuron_count: int = 2000) -> 'NLMEnvironment':
        """Create and configure the brain."""
        config = NLMConfiguration()
        config.set("neuron_count", neuron_count)
        config.set("region_count", 2)
        config.set("connection_probability", 0.05)
        
        self.brain = NLMBrain(config)
        return self
    
    def create_agent(self) -> 'NLMEnvironment':
        """Create and configure the agent brain interface."""
        if not self.brain:
            self.create_brain()
        
        self.agent = AgentBrain(self.brain._brain)
        self.agent.initialize(self.world._world)
        
        # Configure agent capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        return self
    
    def run_episode(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Run a single episode of brain-world interaction."""
        if not self.brain or not self.agent:
            raise RuntimeError("Brain and agent must be initialized first")
        
        episode_reward = 0.0
        episode_steps = 0
        
        # Reset environment
        self.brain.reset()
        self.agent.reset()
        
        for step in range(max_steps):
            episode_steps += 1
            
            # Get observation
            brain_region = self.brain._brain.getRegions()[0].get()
            percept = self.world.get_observation(brain_region)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            start_time = time.time()
            self.brain._brain.step(step, step * 0.001)
            brain_step_time = time.time() - start_time
            self.step_times.append(brain_step_time)
            
            # Get motor command
            command = self.agent.decodeMotorCommand()
            
            # Apply action to world
            world_result = self.world.apply_action(brain_region, command)
            
            # Compute reward
            reward = self.world.compute_reward(brain_region)
            episode_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development if enabled
            if self.agent.getDevelopmentSystem():
                self.agent.updateDevelopment(0.001)
            
            # Collect metrics
            metrics = {
                "step": step,
                "reward": reward,
                "action": str(command),
                "brain_step_time": brain_step_time,
                "total_spikes": self.brain._brain.getTotalSpikeCount(),
                "firing_neurons": self.brain._brain.getFiringNeuronCount(),
                "dopamine_level": self.agent.getNeuromodulationLevel(),
                "curiosity_level": self.agent.getCuriosityLevel(),
                "novelty_level": self.agent.getNoveltyLevel(),
            }
            self.metrics_history.append(metrics)
            
            # Log progress
            if step % 100 == 0:
                print(f"Step {step}: reward={reward:.3f}, action={command}, "
                      f"spikes={metrics['total_spikes']}, "
                      f"brain_time={brain_step_time*1000:.1f}ms")
            
            # Check for termination
            if world_result.get("termination", False):
                break
        
        # Episode statistics
        avg_step_time = sum(self.step_times) / len(self.step_times) if self.step_times else 0
        
        results = {
            "total_reward": episode_reward,
            "total_steps": episode_steps,
            "avg_reward_per_step": episode_reward / episode_steps if episode_steps > 0 else 0,
            "avg_step_time_ms": avg_step_time * 1000,
            "final_spikes": self.brain._brain.getTotalSpikeCount(),
            "final_synapses": self.brain._brain.getTotalSynapseCount(),
            "developmental_stage": self.agent.getDevelopmentalStage() if self.agent else None,
        }
        
        print(f"Episode complete: {episode_steps} steps, "
              f"total reward={episode_reward:.2f}, "
              f"avg reward/step={results['avg_reward_per_step']:.4f}")
        
        return results
    
    def run_lifetime_learning(self, num_episodes: int = 10, max_steps_per_episode: int = 500) -> Dict[str, Any]:
        """Run lifetime learning across multiple episodes."""
        print(f"Starting lifetime learning: {num_episodes} episodes")
        
        lifetime_results = []
        total_lifetime_reward = 0.0
        
        for episode in range(num_episodes):
            print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
            results = self.run_episode(max_steps_per_episode)
            lifetime_results.append(results)
            total_lifetime_reward += results["total_reward"]
            
            # Brief pause between episodes
            time.sleep(0.1)
        
        # Compute lifetime statistics
        total_steps = sum(r["total_steps"] for r in lifetime_results)
        avg_reward_per_episode = total_lifetime_reward / num_episodes
        
        final_brain_metrics = self.brain.get_metrics() if self.brain else {}
        
        lifetime_summary = {
            "episodes_completed": num_episodes,
            "total_steps": total_steps,
            "total_lifetime_reward": total_lifetime_reward,
            "avg_reward_per_episode": avg_reward_per_episode,
            "avg_steps_per_episode": total_steps / num_episodes,
            "final_brain_metrics": final_brain_metrics,
            "episode_results": lifetime_results,
        }
        
        print(f"\nLifetime learning complete!")
        print(f"Total episodes: {num_episodes}")
        print(f"Total reward: {total_lifetime_reward:.2f}")
        print(f"Average reward per episode: {avg_reward_per_episode:.2f}")
        
        return lifetime_summary

# Convenience functions
def create_simple_brain_demo(neuron_count: int = 500, steps: int = 1000):
    """Create a simple demonstration brain with basic functionality."""
    config = NLMConfiguration()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    
    brain = NLMBrain(config)
    if brain.initialize():
        # Simple learning demonstration
        for step in range(steps):
            brain.inject_sensory("Sensory", 10.0)
            brain.step(step, step * 0.001)
            
            if step % 200 == 0:
                print(f"Step {step}: "
                      f"spikes={brain._brain.getTotalSpikeCount()}, "
                      f"synapses={brain._brain.getTotalSynapseCount()}")
        
        return brain
    else:
        raise RuntimeError("Failed to initialize brain")

def create_reproduction_demo() -> Dict[str, Any]:
    """Create a complete reproduction of the main.cpp functionality in Python."""
    print("=== NLM Phase 6 Python Reproduction Demo ===")
    
    # Create environment
    env = NLMEnvironment(width=20, height=20)
    
    # Create brain and agent
    env.create_brain(neuron_count=1000).create_agent()
    
    # Run a few episodes
    results = env.run_lifetime_learning(num_episodes=3, max_steps_per_episode=200)
    
    print("\n=== Demo Complete ===")
    return results

# Main function for standalone execution
def main():
    """Main function for standalone Python execution."""
    print("NLM Python Interface - Standalone Mode")
    print("=" * 50)
    
    # Check if pynlm is available
    try:
        from pynlm import NeuronType
        print("✓ NLM bindings loaded successfully")
    except ImportError:
        print("✗ Failed to load NLM bindings")
        return 1
    
    # Run simple demo
    print("\nRunning simple demonstration...")
    try:
        brain = create_simple_brain_demo()
        metrics = brain.get_metrics()
        print(f"Demo complete! Final metrics: {metrics}")
    except Exception as e:
        print(f"Demo failed: {e}")
        return 1
    
    return 0
if __name__ == "__main__":
    exit(main())
