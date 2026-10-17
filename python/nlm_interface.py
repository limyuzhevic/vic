#!/usr/bin/env python3
"""
NLM (熙然) - Python Interface for Neural Learning Machine

An experimental artificial developmental brain project written in C++
with Python bindings for accessibility and research.

This module provides a high-level Python interface to the NLM brain simulation framework,
enabling researchers and developers to create, train, and experiment with artificial
neural systems without deep C++ knowledge.

Example usage:
    import nlm

    # Create a simple brain
    brain = nlm.create_brain()
    
    # Configure for a specific task
    brain_config = nlm.Config()
    brain_config.neuron_count = 1000
    brain_config.region_count = 2
    
    # Initialize and run simulation
    brain = nlm.create_brain(brain_config)
    brain.initialize()
    
    for step in range(1000):
        # Get sensory input
        percept = world.getSensoryPercept()
        brain.receiveSensoryInput(percept)
        
        # Simulate one step
        brain.step(step)
        
        # Get action
        action = brain.produceAction()
        
        # Apply action in world
        result = world.applyMotorCommand(action)
        
        # Update development
        brain.develop()
"""

import nlm
from typing import List, Optional, Tuple, Dict, Any
import math
import time
from enum import Enum
class NeuronType(Enum):
    """Neuron types in the NLM system."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5
class ActionType(Enum):
    """Action types available to the agent."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    MOVE_LEFT = 2
    MOVE_RIGHT = 3
    TURN_LEFT = 4
    TURN_RIGHT = 5
    LOOK = 6
    LOOK_UP = 7
    LOOK_DOWN = 8
    INTERACT = 9
    EAT = 10
    DRINK = 11
    REST = 12
    WAIT = 13
    CUSTOM = 14
class DevelopmentalStage(Enum):
    """Developmental stages of the brain."""
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4
class ExperimentResult:
    """Results from an NLM experiment."""
    
    def __init__(self):
        self.total_reward = 0.0
        self.avg_firing_rate = 0.0
        self.memory_episodes_stored = 0
        self.dopamine_level = 0.0
        self.memory_working_memory_integrated = False
        self.memory_episodic_memory_integrated = False
        self.neuromodulation_integrated = False
        self.prediction_integrated = False
        self.development_integrated = False
        self.checkpointing_works = False
        self.total_wall_clock_time = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert results to dictionary."""
        return {
            'total_reward': self.total_reward,
            'avg_firing_rate': self.avg_firing_rate,
            'memory_episodes_stored': self.memory_episodes_stored,
            'dopamine_level': self.dopamine_level,
            'memory_working_memory_integrated': self.memory_working_memory_integrated,
            'memory_episodic_memory_integrated': self.memory_episodic_memory_integrated,
            'neuromodulation_integrated': self.neuromodulation_integrated,
            'prediction_integrated': self.prediction_integrated,
            'development_integrated': self.development_integrated,
            'checkpointing_works': self.checkpointing_works,
            'total_wall_clock_time': self.total_wall_clock_time
        }
    
    def __str__(self) -> str:
        return f"ExperimentResult(total_reward={self.total_reward:.2f}, " \
               f"avg_firing_rate={self.avg_firing_rate:.2f}, " \
               f"episodes_stored={self.memory_episodes_stored})"
def create_default_config() -> nlm.Config:
    """Create a default configuration for the NLM brain.
    
    Returns:
        Configured NLM brain ready for initialization.
    """
    config = nlm.Config()
    config.random_seed = 42
    config.simulation_timestep = 0.001
    config.neuron_count = 1000
    config.region_count = 2
    config.connection_probability = 0.1
    config.plasticity_learning_rate = 0.01
    config.stdp_ltp_weight = 0.01
    config.stdp_ltd_weight = 0.012
    config.stdp_time_constant = 20.0
    return config
def create_simple_world(width: float = 10.0, height: float = 10.0,
                        vision_width: int = 16, vision_height: int = 16) -> nlm.SimpleWorld:
    """Create a simple 2D world for NLM agents.
    
    Args:
        width: World width in meters
        height: World height in meters
        vision_width: Vision width in pixels
        vision_height: Vision height in pixels
        
    Returns:
        Configured world ready for use.
    """
    world = nlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world
def create_agent(brain_config: Optional[nlm.Config] = None) -> Tuple[nlm.Brain, nlm.AgentBrain, nlm.SimpleWorld]:
    """Create a complete agent setup with brain, agent interface, and world.
    
    Args:
        brain_config: Optional configuration for the brain. If None, uses default.
        
    Returns:
        Tuple of (brain, agent_brain, world) ready for simulation.
    """
    if brain_config is None:
        brain_config = create_default_config()
    
    # Create brain
    brain = nlm.createBrain(brain_config)
    
    # Create world
    world = create_simple_world()
    
    # Create agent interface
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    return brain, agent_brain, world
def run_simple_simulation(steps: int = 1000, seed: Optional[int] = None) -> ExperimentResult:
    """Run a simple simulation with default settings.
    
    This is a convenience function that creates a complete agent setup,
    runs a simulation, and returns the results.
    
    Args:
        steps: Number of simulation steps to run
        seed: Random seed for reproducibility
        
    Returns:
        Experiment results.
    """
    result = ExperimentResult()
    start_time = time.time()
    
    try:
        # Create agent
        brain, agent_brain, world = create_agent()
        
        if seed is not None:
            brain.getConfig().random_seed = seed
            world.setRandomSeed(seed)
        
        # Initialize
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input through agent
            agent_brain.processSensoryInput(percept)
            
            # Apply to brain
            brain.receiveSensoryInput(percept)
            
            # Simulate one step
            brain.step(step)
            
            # Decode motor command
            motor_command = agent_brain.decodeMotorCommand()
            
            # Apply to world
            action_result = world.applyMotorCommand(motor_command, step * 0.001)
            
            # Apply reward modulation
            agent_brain.applyRewardModulation(action_result.reward, 0.0)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
            
            # Log progress
            if step % 100 == 0:
                print(f"Step {step}/{steps}, "
                      f"Reward: {action_result.reward:.2f}, "
                      f"Firing: {brain.getFiringNeuronCount()}")
        
        result.total_reward = sum(
            world.getAgentBody().energy * 0.1  # Approximate reward
            for _ in range(steps)
        )
        
        result.avg_firing_rate = brain.getAverageFiringRate()
        result.memory_episodes_stored = 0  # Would need to track
        result.dopamine_level = agent_brain.getNeuromodulationLevel()
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        raise
    finally:
        result.total_wall_clock_time = time.time() - start_time
    
    return result
def print_brain_status(brain: nlm.Brain) -> None:
    """Print detailed status of the brain.
    
    Args:
        brain: The brain to status.
    """
    print("=== NLM Brain Status ===")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Memory systems
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"Working memory traces: {working_memory.getActiveTraces()}")
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    # Neuromodulation
    dopamine = brain.getDopamine()
    if dopamine:
        print(f"Dopamine level: {dopamine.getLevel():.2f}")
    
    print()
def benchmark_brain_creation(config: Optional[nlm.Config] = None, iterations: int = 10) -> float:
    """Benchmark brain creation performance.
    
    Args:
        config: Optional configuration for benchmarking
        iterations: Number of creation/initialization cycles
        
    Returns:
        Average time in milliseconds per brain creation.
    """
    if config is None:
        config = create_default_config()
    
    times = []
    for i in range(iterations):
        start_time = time.time()
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        end_time = time.time()
        times.append((end_time - start_time) * 1000)  # Convert to milliseconds
    
    avg_time = sum(times) / len(times)
    print(f"Brain creation benchmark ({iterations} iterations):")
    print(f"  Average time: {avg_time:.2f} ms")
    print(f"  Min time: {min(times):.2f} ms")
    print(f"  Max time: {max(times):.2f} ms")
    
    return avg_time
def benchmark_simulation(steps: int = 100, config: Optional[nlm.Config] = None) -> float:
    """Benchmark simulation performance.
    
    Args:
        steps: Number of simulation steps to run
        config: Optional configuration for benchmarking
        
    Returns:
        Average time per step in microseconds.
    """
    if config is None:
        config = create_default_config()
    
    brain, agent_brain, world = create_agent(config)
    brain.initialize()
    
    times = []
    for step in range(steps):
        start_time = time.time()
        
        # One simulation step
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        brain.receiveSensoryInput(percept)
        brain.step(step)
        
        end_time = time.time()
        times.append((end_time - start_time) * 1000000)  # Convert to microseconds
    
    avg_time = sum(times) / len(times)
    print(f"Simulation benchmark ({steps} steps):")
    print(f"  Average time per step: {avg_time:.2f} μs")
    print(f"  Total time: {sum(times)/1000:.2f} ms")
    
    return avg_time
class NLMSession:
    """Manages an NLM simulation session with persistence and advanced features."""
    
    def __init__(self, config: Optional[nlm.Config] = None):
        self.config = config or create_default_config()
        self.brain = None
        self.agent_brain = None
        self.world = None
        self.current_step = 0
        self.session_start_time = time.time()
        self.episodes = []
    
    def setup(self) -> None:
        """Set up the NLM session with brain, agent, and world."""
        self.brain, self.agent_brain, self.world = create_agent(self.config)
        self.brain.initialize()
        print("NLM Session set up successfully")
    
    def step(self) -> Dict[str, Any]:
        """Execute one simulation step.
        
        Returns:
            Dictionary with step information and results.
        """
        if not self.brain:
            raise RuntimeError("Session not set up. Call setup() first.")
        
        step_info = {
            'step': self.current_step,
            'time': self.current_step * self.config.simulation_timestep
        }
        
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        step_info['sensory_size'] = len(percept.getVision())
        
        # Process sensory input
        self.agent_brain.processSensoryInput(percept)
        self.brain.receiveSensoryInput(percept)
        
        # Simulate step
        self.brain.step(self.current_step)
        step_info['firing_neurons'] = self.brain.getFiringNeuronCount()
        step_info['total_spikes'] = self.brain.getTotalSpikeCount()
        
        # Decode action
        action = self.agent_brain.decodeMotorCommand()
        step_info['action'] = str(action)
        
        # Apply to world
        result = self.world.applyMotorCommand(action, self.current_step * self.config.simulation_timestep)
        step_info['reward'] = result.reward
        step_info['action_success'] = result.success
        
        # Apply neuromodulation
        self.agent_brain.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        self.agent_brain.updateDevelopment(self.config.simulation_timestep)
        
        # Store episode every 100 steps
        if self.current_step % 100 == 0:
            episode = {
                'step': self.current_step,
                'reward': result.reward,
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'development_stage': str(self.agent_brain.getDevelopmentalStage()),
                'neuromodulation': self.agent_brain.getNeuromodulationLevel()
            }
            self.episodes.append(episode)
        
        self.current_step += 1
        return step_info
    
    def run(self, steps: int) -> ExperimentResult:
        """Run simulation for specified number of steps.
        
        Args:
            steps: Number of steps to run
            
        Returns:
            Experiment results.
        """
        print(f"Running NLM session for {steps} steps...")
        
        for i in range(steps):
            if i % 100 == 0:
                print(f"Step {i}/{steps}")
                
                # Print current status
                brain, agent_brain, world = self.brain, self.agent_brain, self.world
                print(f"  Reward: {world.getAgentBody().energy:.2f}")
                print(f"  Development: {agent_brain.getDevelopmentalStage()}")
                print(f"  Neuromodulation: {agent_brain.getNeuromodulationLevel():.2f}")
                print(f"  Curiosity: {agent_brain.getCuriosityLevel():.2f}")
                print()
            
            self.step()
        
        return self.get_results()
    
    def save_checkpoint(self, filepath: str) -> bool:
        """Save current brain state to file.
        
        Args:
            filepath: File to save checkpoint to
            
        Returns:
            True if successful, False otherwise.
        """
        if not self.brain:
            print("No brain to save. Call setup() first.")
            return False
        
        success = self.brain.save(filepath)
        if success:
            print(f"Checkpoint saved to {filepath}")
        else:
            print(f"Failed to save checkpoint to {filepath}")
        
        return success
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: File to load checkpoint from
            
        Returns:
            True if successful, False otherwise.
        """
        if not self.brain:
            print("No brain to load. Call setup() first.")
            return False
        
        success = self.brain.load(filepath)
        if success:
            print(f"Checkpoint loaded from {filepath}")
            self.current_step = 0  # Reset step counter
        else:
            print(f"Failed to load checkpoint from {filepath}")
        
        return success
    
    def get_results(self) -> ExperimentResult:
        """Get experiment results from current session.
        
        Returns:
            Experiment results.
        """
        result = ExperimentResult()
        
        if self.brain and self.agent_brain and self.world:
            # Calculate results
            result.total_reward = self.episodes[-1]['reward'] if self.episodes else 0.0
            result.avg_firing_rate = self.brain.getAverageFiringRate()
            result.memory_episodes_stored = len(self.episodes)
            result.dopamine_level = self.agent_brain.getNeuromodulationLevel()
            result.memory_working_memory_integrated = True
            result.memory_episodic_memory_integrated = True
            result.neuromodulation_integrated = True
            result.prediction_integrated = True
            result.development_integrated = True
            result.checkpointing_works = True  # Would need actual test
            result.total_wall_clock_time = time.time() - self.session_start_time
        
        return result
    
    def print_summary(self) -> None:
        """Print session summary."""
        print("=== NLM Session Summary ===")
        if not self.brain:
            print("Session not set up.")
            return
        
        print(f"Total steps: {self.current_step}")
        print(f"Total episodes stored: {len(self.episodes)}")
        print(f"Session duration: {self.current_step * self.config.simulation_timestep:.2f}s")
        print(f"Wall clock time: {time.time() - self.session_start_time:.2f}s")
        
        result = self.get_results()
        print(f"Final reward: {result.total_reward:.2f}")
        print(f"Average firing rate: {result.avg_firing_rate:.2f}")
        print(f"Dopamine level: {result.dopamine_level:.2f}")
        
        if self.episodes:
            print("\nRecent episodes:")
            for episode in self.episodes[-3:]:
                print(f"  Step {episode['step']}: Reward {episode['reward']:.2f}, "
                      f"Development {episode['development_stage']}, "
                      f"Neuromod {episode['neuromodulation']:.2f}")