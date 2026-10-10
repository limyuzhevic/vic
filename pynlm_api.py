#!/usr/bin/env python3
"""
NLM High-Level API Abstractions

This module provides simplified, high-level APIs for common NLM use cases.
It abstracts away the complexity of the underlying brain simulation framework
while maintaining full access to advanced features when needed.

Common Use Cases:
1. Basic Neural Simulation
2. Agent Training
3. Experiment Setup
4. Result Analysis
5. Visualization
"""

import pynlm
import time
import random
from typing import Optional, List, Dict, Any, Callable
from dataclasses import dataclass
from enum import Enum


class SimulationMode(Enum):
    """Modes for simulation execution"""
    BASIC = "basic"
    AGENT = "agent"
    TRAINING = "training"
    EXPERIMENT = "experiment"


@dataclass
class SimulationResult:
    """Container for simulation results"""
    total_reward: float
    avg_firing_rate: float
    total_spikes: int
    active_neurons: int
    episodic_episodes: float
    novelty_level: float
    curiosity_level: float
    dopamine_level: float
    execution_time: float
    steps_completed: int
    metadata: Dict[str, Any] = None


@dataclass
class TrainingResult:
    """Container for agent training results"""
    performance_metrics: Dict[str, float]
    experience_gained: int
    learning_curve: List[Dict[str, float]]
    final_behavior: Dict[str, Any]
    convergence_score: float


class NLMAPI:
    """
    High-level API for NLM brain simulation with simplified abstractions.
    
    This class provides factory methods and convenience functions for common
    NLM use cases, abstracting away the complexity of the underlying framework
    while maintaining flexibility for advanced use cases.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        """
        Initialize the high-level API.
        
        Args:
            config: Optional configuration. If None, uses default.
        """
        self._config = config or pynlm.createDefaultConfig()
        self._brain = None
        self._world = None
        self._agent = None
    
    # ============================================================================
    # FACTORY FUNCTIONS FOR COMMON SETUP PATTERNS
    # ============================================================================
    
    @staticmethod
    def create_default_config() -> pynlm.Config:
        """Create a default configuration for NLM brain."""
        return pynlm.createDefaultConfig()
    
    @staticmethod
    def create_brain_config(neuron_count: int = 1000,
                           region_count: int = 5,
                           connection_probability: float = 0.1,
                           stdp_ltp_weight: float = 0.01,
                           stdp_ltd_weight: float = 0.012,
                           synaptogenesis_rate: float = 0.0001,
                           pruning_rate: float = 0.00001) -> pynlm.Config:
        """
        Create a brain configuration with common settings.
        
        Args:
            neuron_count: Number of neurons in the brain
            region_count: Number of brain regions
            connection_probability: Probability of synaptic connections
            stdp_ltp_weight: STDP long-term potentiation weight
            stdp_ltd_weight: STDP long-term depression weight
            synaptogenesis_rate: Rate of new synapse formation
            pruning_rate: Rate of synapse removal
            
        Returns:
            Configured Config object
        """
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        config.set("region_count", region_count)
        config.set("connection_probability", connection_probability)
        config.set("stdp_ltp_weight", stdp_ltp_weight)
        config.set("stdp_ltd_weight", stdp_ltd_weight)
        config.set("synaptogenesis_rate", synaptogenesis_rate)
        config.set("pruning_rate", pruning_rate)
        return config
    
    @classmethod
    def create_simple_brain(cls, neuron_count: int = 1000) -> 'NLMAPI':
        """
        Create a simple brain for basic neural simulation.
        
        Args:
            neuron_count: Number of neurons in the brain
            
        Returns:
            NLMAPI instance with initialized brain
        """
        config = cls.create_brain_config(neuron_count)
        api = cls(config)
        api._brain = pynlm.createBrain(config)
        api._brain.initialize()
        return api
    
    @classmethod
    def create_agent_training_brain(cls, neuron_count: int = 2000,
                                   enable_learning: bool = True) -> 'NLMAPI':
        """
        Create a brain optimized for agent training.
        
        Args:
            neuron_count: Number of neurons (more neurons = better learning capacity)
            enable_learning: Whether to enable learning mechanisms
            
        Returns:
            NLMAPI instance with training-ready brain
        """
        config = cls.create_brain_config(neuron_count, 
                                        connection_probability=0.15,
                                        synaptogenesis_rate=0.0005)
        api = cls(config)
        api._brain = pynlm.createBrain(config)
        api._brain.initialize()
        
        # Create and initialize world for training
        api._world = pynlm.createSimpleWorld()
        api._world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
        
        # Create agent
        api._agent = pynlm.createAgentBrain(api._brain)
        api._agent.initialize(api._world)
        
        # Enable learning features
        if enable_learning:
            api._agent.enableRewardModulation(True)
            api._agent.enableStructuralPlasticity(True)
            api._agent.enableDevelopment(True)
            api._agent.enableCuriosity(True)
        
        return api
    
    @classmethod
    def create_experiment_setup(cls, experiment_config: Dict[str, Any]) -> 'NLMAPI':
        """
        Create an experiment setup with pre-defined configuration.
        
        Args:
            experiment_config: Dictionary with experiment parameters
            
        Returns:
            NLMAPI instance ready for experiment
        """
        config = pynlm.createDefaultConfig()
        
        # Apply experiment-specific settings
        if 'neuron_count' in experiment_config:
            config.set("neuron_count", experiment_config['neuron_count'])
        if 'max_steps' in experiment_config:
            config.set("max_steps", experiment_config['max_steps'])
        if 'enable_development' in experiment_config:
            config.set("enable_development", experiment_config['enable_development'])
        
        api = cls(config)
        api._brain = pynlm.createBrain(config)
        api._brain.initialize()
        
        # Setup world if specified
        if experiment_config.get('world_size'):
            api._world = pynlm.createSimpleWorld()
            size = experiment_config['world_size']
            api._world.configure(width=size, height=size,
                                visionWidth=8, visionHeight=8)
            api._world.reset()
        
        # Create agent
        if api._world:
            api._agent = pynlm.createAgentBrain(api._brain)
            api._agent.initialize(api._world)
        
        return api
    
    # ============================================================================
    # CONVENIENCE METHODS FOR COMMON OPERATIONS
    # ============================================================================
    
    def run_basic_simulation(self, steps: int = 100) -> SimulationResult:
        """
        Run a basic neural simulation with the brain.
        
        Args:
            steps: Number of simulation steps to run
            
        Returns:
            SimulationResult with metrics and statistics
        """
        if not self._brain:
            raise ValueError("Brain not initialized. Use create_simple_brain() first.")
        
        start_time = time.time()
        
        for step in range(steps):
            self._brain.step(step)
        
        end_time = time.time()
        
        return SimulationResult(
            total_reward=0.0,  # Basic simulation has no reward
            avg_firing_rate=self._brain.getAverageFiringRate(),
            total_spikes=self._brain.getTotalSpikeCount(),
            active_neurons=self._brain.getFiringNeuronCount(),
            episodic_episodes=0.0,  # No episodic memory in basic simulation
            novelty_level=0.0,
            curiosity_level=0.0,
            dopamine_level=0.0,
            execution_time=end_time - start_time,
            steps_completed=steps,
            metadata={'simulation_type': 'basic'}
        )
    
    def run_agent_simulation(self, steps: int = 1000, 
                           enable_world: bool = True) -> SimulationResult:
        """
        Run a simulation with agent interacting with the world.
        
        Args:
            steps: Number of simulation steps to run
            enable_world: Whether to create and use a world environment
            
        Returns:
            SimulationResult with performance metrics
        """
        if not self._brain:
            raise ValueError("Brain not initialized. Use create_agent_training_brain() first.")
        
        # Reset world if it exists
        if self._world:
            self._world.reset()
        
        start_time = time.time()
        total_reward = 0.0
        
        for step in range(steps):
            # Update world
            if enable_world and self._world:
                self._world.update(0.1)
            
            # Get observation
            if enable_world and self._world:
                percept = self._world.getSensoryPercept()
                self._agent.processSensoryInput(percept)
            
            # Brain step
            self._brain.step(step)
            
            # Get action
            if enable_world and self._world:
                action = self._agent.decodeMotorCommand()
                self._world.applyMotorCommand(action, self._world.getSimulationTime())
            
            # Collect metrics
            if enable_world and self._world:
                reward = self._world.getLastReward()
                total_reward += reward
                self._agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if self._agent:
                self._agent.updateDevelopment(0.1)
        
        end_time = time.time()
        
        return SimulationResult(
            total_reward=total_reward / steps if steps > 0 else 0.0,
            avg_firing_rate=self._brain.getAverageFiringRate(),
            total_spikes=self._brain.getTotalSpikeCount(),
            active_neurons=self._brain.getFiringNeuronCount(),
            episodic_episodes=self._brain.getEpisodicMemory().getEpisodeCount()
                if self._brain.getEpisodicMemory() else 0.0,
            novelty_level=self._agent.getNoveltyLevel() if self._agent else 0.0,
            curiosity_level=self._agent.getCuriosityLevel() if self._agent else 0.0,
            dopamine_level=self._agent.getNeuromodulationLevel() if self._agent else 0.0,
            execution_time=end_time - start_time,
            steps_completed=steps,
            metadata={'simulation_type': 'agent', 'enable_world': enable_world}
        )
    
    def train_agent(self, episodes: int = 50, steps_per_episode: int = 100,
                   exploration_rate: float = 0.1,
                   learning_callback: Optional[Callable] = None) -> TrainingResult:
        """
        Train an agent using reinforcement learning.
        
        Args:
            episodes: Number of training episodes
            steps_per_episode: Steps per training episode
            exploration_rate: Probability of random actions (exploration)
            learning_callback: Optional callback function for monitoring
            
        Returns:
            TrainingResult with training metrics
        """
        if not self._brain or not self._world or not self._agent:
            raise ValueError("Complete training setup required. Use create_agent_training_brain().")
        
        learning_curve = []
        
        for episode in range(episodes):
            episode_reward = 0.0
            episode_steps = 0
            
            # Reset environment
            self._world.reset()
            
            # Run episode
            for step in range(steps_per_episode):
                # Select action (with exploration)
                if random.random() < exploration_rate:
                    # Random action for exploration
                    action = self._random_action()
                else:
                    # Learned action
                    action = self._agent.decodeMotorCommand()
                
                # Execute action
                self._world.applyMotorCommand(action, self._world.getSimulationTime())
                
                # Get reward
                reward = self._world.getLastReward()
                episode_reward += reward
                
                # Update agent
                self._agent.applyRewardModulation(reward, reward * 0.9)
                
                # Get observation and process
                percept = self._world.getSensoryPercept()
                self._agent.processSensoryInput(percept)
                
                # Brain step
                self._brain.step(episode * steps_per_episode + step)
                
                # Update development
                self._agent.updateDevelopment(0.1)
                
                episode_steps += 1
                
                # Notify callback
                if learning_callback:
                    learning_callback(episode, step, episode_reward, reward)
            
            # Record episode metrics
            learning_curve.append({
                'episode': episode,
                'total_reward': episode_reward,
                'avg_reward': episode_reward / episode_steps if episode_steps > 0 else 0,
                'firing_rate': self._brain.getAverageFiringRate(),
                'novelty': self._agent.getNoveltyLevel(),
                'curiosity': self._agent.getCuriosityLevel()
            })
        
        # Calculate final metrics
        performance_metrics = {
            'final_avg_reward': learning_curve[-1]['avg_reward'] if learning_curve else 0,
            'total_reward': sum(ep['total_reward'] for ep in learning_curve),
            'convergence_score': self._calculate_convergence(learning_curve),
            'final_firing_rate': self._brain.getAverageFiringRate() if self._brain else 0,
            'final_novelty': self._agent.getNoveltyLevel() if self._agent else 0,
            'final_curiosity': self._agent.getCuriosityLevel() if self._agent else 0
        }
        
        return TrainingResult(
            performance_metrics=performance_metrics,
            experience_gained=len(learning_curve) * steps_per_episode,
            learning_curve=learning_curve,
            final_behavior={
                'avg_firing_rate': self._brain.getAverageFiringRate() if self._brain else 0,
                'total_spikes': self._brain.getTotalSpikeCount() if self._brain else 0,
                'novelty': self._agent.getNoveltyLevel() if self._agent else 0,
                'curiosity': self._agent.getCuriosityLevel() if self._agent else 0
            },
            convergence_score=self._calculate_convergence(learning_curve)
        )
    
    def run_experiment(self, experiment_name: str, max_steps: int = 1000) -> Dict[str, Any]:
        """
        Run a pre-configured experiment.
        
        Args:
            experiment_name: Name of the experiment
            max_steps: Maximum simulation steps
            
        Returns:
            Dictionary with experiment results
        """
        if not self._brain:
            raise ValueError("Brain not initialized.")
        
        # Configure for experiment
        if self._world:
            self._world.reset()
        
        # Run experiment
        total_reward = 0.0
        firing_rates = []
        
        for step in range(max_steps):
            if self._world:
                self._world.update(0.1)
                
                # Get observation
                percept = self._world.getSensoryPercept()
                if self._agent:
                    self._agent.processSensoryInput(percept)
                
                # Execute action
                if self._agent:
                    action = self._agent.decodeMotorCommand()
                    self._world.applyMotorCommand(action, self._world.getSimulationTime())
                    
                    reward = self._world.getLastReward()
                    total_reward += reward
                    
                    if self._agent:
                        self._agent.applyRewardModulation(reward, reward * 0.9)
            
            # Brain step
            self._brain.step(step)
            
            # Collect metrics
            if self._brain:
                firing_rates.append(self._brain.getAverageFiringRate())
            
            if self._agent:
                self._agent.updateDevelopment(0.1)
        
        return {
            'experiment_name': experiment_name,
            'steps_completed': max_steps,
            'total_reward': total_reward,
            'avg_reward': total_reward / max_steps,
            'avg_firing_rate': sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            'final_spike_count': self._brain.getTotalSpikeCount() if self._brain else 0,
            'final_developmental_stage': self._brain.getDevelopmentalStage() 
                if self._brain else None,
            'final_novelty': self._agent.getNoveltyLevel() if self._agent else 0,
            'final_curiosity': self._agent.getCuriosityLevel() if self._agent else 0,
            'metadata': {
                'experiment_type': 'standard',
                'brain_configured': self._brain is not None,
                'agent_configured': self._agent is not None,
                'world_configured': self._world is not None
            }
        }
    
    # ============================================================================
    # ANALYSIS AND VISUALIZATION HELPERS
    # ============================================================================
    
    def analyze_simulation_data(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Analyze simulation data and extract key metrics.
        
        Args:
            data: List of dictionaries with simulation data points
            
        Returns:
            Dictionary with analyzed metrics
        """
        if not data:
            return {}
        
        # Extract metrics
        total_rewards = [d.get('reward', 0) for d in data]
        firing_rates = [d.get('firing_rate', 0) for d in data]
        steps = list(range(len(data)))
        
        # Calculate statistics
        analysis = {
            'steps': steps,
            'total_rewards': total_rewards,
            'avg_reward': sum(total_rewards) / len(total_rewards) if total_rewards else 0,
            'max_reward': max(total_rewards) if total_rewards else 0,
            'min_reward': min(total_rewards) if total_rewards else 0,
            'firing_rates': firing_rates,
            'avg_firing_rate': sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            'stability_score': self._calculate_stability(firing_rates),
            'trend': self._calculate_trend(total_rewards),
            'peaks': self._find_peaks(total_rewards),
            'valleys': self._find_valleys(total_rewards)
        }
        
        return analysis
    
    def create_visualization_data(self, brain: pynlm.Brain, 
                                 world: Optional[pynlm.SimpleWorld] = None) -> Dict[str, Any]:
        """
        Create data ready for visualization.
        
        Args:
            brain: Brain object to analyze
            world: Optional world object to include in data
            
        Returns:
            Dictionary with visualization-ready data
        """
        viz_data = {
            'neural_activity': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getFiringNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            },
            'memory': {
                'working_memory_active_traces': brain.getWorkingMemory().getActiveTraces() 
                    if brain.getWorkingMemory() else 0,
                'episodic_memory_episodes': brain.getEpisodicMemory().getEpisodeCount()
                    if brain.getEpisodicMemory() else 0,
                'associative_memory_patterns': 0  # Not directly exposed
            },
            'neuromodulation': {
                'dopamine_level': brain.getDopamine() if brain.getDopamine() else 0,
                'curiosity_level': 0,  # Not directly exposed
                'novelty_level': 0  # Not directly exposed
            },
            'development': {
                'developmental_stage': brain.getDevelopmentalStage(),
                'plasticity_rate': 0.0  # Not directly exposed
            }
        }
        
        if world:
            viz_data['environment'] = {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'max_energy': world.getMaxEnergy(),
                'current_time': world.getSimulationTime()
            }
        
        return viz_data
    
    # ============================================================================
    # PRIVATE HELPER METHODS
    # ============================================================================
    
    def _random_action(self) -> pynlm.Action:
        """Generate a random action for exploration."""
        import random
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.MoveBackward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Look,
            pynlm.ActionType.Interact
        ]
        action_type = random.choice(action_types)
        return pynlm.Action(action_type)
    
    def _calculate_convergence(self, learning_curve: List[Dict[str, Any]]) -> float:
        """Calculate convergence score from learning curve."""
        if len(learning_curve) < 10:
            return 0.0
        
        # Use last 20% of episodes to calculate convergence
        recent_episodes = learning_curve[-len(learning_curve)//5:]
        if not recent_episodes:
            return 0.0
        
        recent_rewards = [ep['avg_reward'] for ep in recent_episodes]
        max_reward = max(recent_rewards)
        min_reward = min(recent_rewards)
        
        # Lower variance in recent rewards indicates convergence
        variance = sum((r - (max_reward + min_reward) / 2) ** 2 for r in recent_rewards) / len(recent_rewards)
        
        return max(0.0, 1.0 - variance)
    
    def _calculate_stability(self, values: List[float]) -> float:
        """Calculate stability score based on value variance."""
        if len(values) < 2:
            return 1.0
        
        mean = sum(values) / len(values)
        variance = sum((v - mean) ** 2 for v in values) / len(values)
        std_dev = variance ** 0.5
        
        # Normalize to 0-1 range (lower variance = higher stability)
        return max(0.0, 1.0 - min(std_dev / 10.0, 1.0))
    
    def _calculate_trend(self, values: List[float]) -> str:
        """Calculate trend direction of values."""
        if len(values) < 5:
            return 'insufficient_data'
        
        # Simple linear regression
        n = len(values)
        x_mean = (n - 1) / 2
        y_mean = sum(values) / n
        
        numerator = sum((i - x_mean) * (v - y_mean) for i, v in enumerate(values))
        denominator = sum((i - x_mean) ** 2 for i in range(n))
        
        if denominator == 0:
            return 'stable'
        
        slope = numerator / denominator
        
        if abs(slope) < 0.01:
            return 'stable'
        elif slope > 0:
            return 'increasing'
        else:
            return 'decreasing'
    
    def _find_peaks(self, values: List[float], threshold: float = 0.1) -> List[int]:
        """Find peaks in value sequence."""
        peaks = []
        for i in range(1, len(values) - 1):
            if values[i] > values[i-1] and values[i] > values[i+1]:
                # Check if peak is significant
                max_val = max(values)
                min_val = min(values)
                if max_val > min_val:  # Avoid division by zero
                    peak_height = (values[i] - min_val) / (max_val - min_val)
                    if peak_height > threshold:
                        peaks.append(i)
        return peaks
    
    def _find_valleys(self, values: List[float], threshold: float = 0.1) -> List[int]:
        """Find valleys in value sequence."""
        valleys = []
        for i in range(1, len(values) - 1):
            if values[i] < values[i-1] and values[i] < values[i+1]:
                # Check if valley is significant
                max_val = max(values)
                min_val = min(values)
                if max_val > min_val:  # Avoid division by zero
                    valley_depth = (max_val - values[i]) / (max_val - min_val)
                    if valley_depth > threshold:
                        valleys.append(i)
        return valleys


# ============================================================================
# CONVENIENCE FACTORY FUNCTIONS
# ============================================================================

def create_simple_neural_simulation(neuron_count: int = 1000, 
                                    steps: int = 100) -> SimulationResult:
    """
    Create and run a simple neural simulation with minimal setup.
    
    This is the easiest way to run a basic NLM simulation.
    
    Args:
        neuron_count: Number of neurons in the brain
        steps: Number of simulation steps
        
    Returns:
        SimulationResult with performance metrics
    """
    api = NLMAPI.create_simple_brain(neuron_count)
    return api.run_basic_simulation(steps)


def create_agent_training_simulation(neuron_count: int = 2000,
                                   episodes: int = 50,
                                   steps_per_episode: int = 100) -> TrainingResult:
    """
    Create and run an agent training simulation.
    
    This is the easiest way to train an agent using reinforcement learning.
    
    Args:
        neuron_count: Number of neurons in the brain
        episodes: Number of training episodes
        steps_per_episode: Steps per training episode
        
    Returns:
        TrainingResult with training metrics and learning curve
    """
    api = NLMAPI.create_agent_training_brain(neuron_count)
    return api.train_agent(episodes, steps_per_episode)


def create_experiment_simulation(experiment_config: Dict[str, Any],
                                experiment_name: str,
                                max_steps: int = 1000) -> Dict[str, Any]:
    """
    Create and run a pre-configured experiment.
    
    Args:
        experiment_config: Dictionary with experiment parameters
        experiment_name: Name of the experiment
        max_steps: Maximum simulation steps
        
    Returns:
        Dictionary with experiment results
    """
    api = NLMAPI.create_experiment_setup(experiment_config)
    return api.run_experiment(experiment_name, max_steps)


# ============================================================================
# BACKWARD COMPATIBILITY WRAPPERS
# ============================================================================

def run_legacy_simulation(brain: pynlm.Brain, steps: int = 100) -> SimulationResult:
    """
    Legacy simulation wrapper for backward compatibility.
    
    Args:
        brain: Existing Brain object
        steps: Number of simulation steps
        
    Returns:
        SimulationResult with performance metrics
    """
    api = NLMAPI()
    api._brain = brain
    return api.run_basic_simulation(steps)


def run_legacy_agent_simulation(brain: pynlm.Brain,
                               world: pynlm.SimpleWorld,
                               agent: pynlm.AgentBrain,
                               steps: int = 1000) -> SimulationResult:
    """
    Legacy agent simulation wrapper for backward compatibility.
    
    Args:
        brain: Existing Brain object
        world: Existing World object
        agent: Existing AgentBrain object
        steps: Number of simulation steps
        
    Returns:
        SimulationResult with performance metrics
    """
    api = NLMAPI()
    api._brain = brain
    api._world = world
    api._agent = agent
    return api.run_agent_simulation(steps, enable_world=True)


# ============================================================================
# EXAMPLE FUNCTIONS
# ============================================================================

def basic_simulation_example():
    """Example of basic neural simulation."""
    print("=== Basic Neural Simulation Example ===")
    result = create_simple_neural_simulation(neuron_count=500, steps=50)
    print(f"Simulation completed: {result.steps_completed} steps")
    print(f"Average firing rate: {result.avg_firing_rate:.3f}")
    print(f"Total spikes: {result.total_spikes}")
    print()


def agent_training_example():
    """Example of agent training."""
    print("=== Agent Training Example ===")
    result = create_agent_training_simulation(neuron_count=1000, 
                                            episodes=10, 
                                            steps_per_episode=50)
    print(f"Training completed: {result.experience_gained} experience points")
    print(f"Final performance: {result.performance_metrics['final_avg_reward']:.3f}")
    print(f"Convergence score: {result.convergence_score:.3f}")
    print()


def experiment_example():
    """Example of experiment setup."""
    print("=== Experiment Example ===")
    experiment_config = {
        'neuron_count': 1500,
        'max_steps': 200,
        'enable_development': True
    }
    result = create_experiment_simulation(experiment_config,
                                        "memory_test",
                                        max_steps=100)
    print(f"Experiment '{result['experiment_name']}' completed")
    print(f"Steps completed: {result['steps_completed']}")
    print(f"Total reward: {result['total_reward']:.3f}")
    print()


if __name__ == "__main__":
    import time
    import random
    
    print("NLM High-Level API Examples\n")
    
    # Run examples
    basic_simulation_example()
    agent_training_example()
    experiment_example()
    
    print("All examples completed successfully!")
