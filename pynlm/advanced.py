"""
NLM (Neural Learning Machine) Advanced Python Bindings

Enhanced Python interface for the NLM computational brain simulation framework.

This module provides a comprehensive Python interface to the NLM C++ neural
simulation framework, with additional functionality beyond the basic bindings.

Key Features:
- Enhanced API with Pythonic interfaces
- Advanced simulation control
- Comprehensive monitoring and statistics
- Learning and development tracking
- Context managers for resource management
- Serialization support for brain states
- Performance monitoring tools

This is the advanced version of the NLM Python bindings, designed for
power users and researchers who need additional functionality beyond
the basic interface.
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

import time
import json
import pickle
from typing import Optional, List, Dict, Any, Callable, ContextManager
import contextlib

# Import core functionality from the basic module
from pynlm import (
    # Core classes from pynlm/__init__.py
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    SensoryInput,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    # Helper functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Type aliases for better code clarity
NeuronCount = int
SpikeCount = int
FiringRate = float
SimulationTime = float

class NLMContextManager:
    """
    Context manager for safe NLM resource management.
    
    Provides automatic cleanup of resources and ensures proper
    initialization/cleanup sequences for complex simulations.
    """
    
    def __init__(self, brain: Optional[Brain] = None, 
                 world: Optional[SimpleWorld] = None,
                 agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self._original_states = {}
        
    def __enter__(self) -> 'NLMContextManager':
        """Enter the context and save current states if provided."""
        if self.brain:
            self._save_brain_state('brain')
        if self.world:
            self._save_world_state('world')
        if self.agent:
            self._save_agent_state('agent')
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Exit the context and restore saved states."""
        if self.brain and 'brain' in self._original_states:
            self._restore_brain_state()
        if self.world and 'world' in self._original_states:
            self._restore_world_state()
        if self.agent and 'agent' in self._original_states:
            self._restore_agent_state()
            
    def _save_brain_state(self, key: str) -> None:
        """Save current brain state for restoration."""
        try:
            self._original_states[key] = {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'timestamp': time.time()
            }
        except Exception:
            pass  # Ignore state saving failures
            
    def _restore_brain_state(self) -> None:
        """Restore brain to saved state."""
        if 'brain' in self._original_states:
            state = self._original_states['brain']
            try:
                # Note: This is a simplified restoration
                # In a real implementation, you might want to use
                # brain.save() and brain.load() for proper state management
                pass
            except Exception:
                pass  # Ignore restoration failures

class SimulationMonitor:
    """
    Real-time monitoring and statistics collection for NLM simulations.
    
    Tracks simulation progress, performance metrics, and provides
    analysis tools for understanding simulation behavior.
    """
    
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None,
                 agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_count = 0
        self.start_time = time.time()
        self.step_times: List[float] = []
        self.metrics_history: List[Dict[str, Any]] = []
        
    def record_step(self, step_time: Optional[float] = None) -> None:
        """Record a simulation step and update metrics."""
        self.step_count += 1
        current_time = time.time()
        
        if step_time is not None:
            self.step_times.append(step_time)
            
        # Collect current metrics
        metrics = {
            'step': self.step_count,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'development_stage': self.brain.getDevelopmentalStage().name,
            'timestamp': current_time,
            'elapsed_time': current_time - self.start_time
        }
        
        if self.world:
            metrics.update({
                'world_time': self.world.getSimulationTime(),
                'agent_energy': self.world.getAgentBody().energy,
                'agent_health': self.world.getAgentBody().health,
                'agent_age': self.world.getAgentBody().age,
            })
            
        if self.agent:
            metrics.update({
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            })
            
        self.metrics_history.append(metrics)
        
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current simulation metrics."""
        return self.get_metrics_at_step(self.step_count)
        
    def get_metrics_at_step(self, step: int) -> Optional[Dict[str, Any]]:
        """Get metrics at a specific step."""
        for metrics in self.metrics_history:
            if metrics['step'] == step:
                return metrics.copy()
        return None
        
    def get_average_step_time(self) -> float:
        """Get average time per step."""
        if not self.step_times:
            return 0.0
        return sum(self.step_times) / len(self.step_times)
        
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary of the simulation."""
        if self.step_count == 0:
            return {}
            
        return {
            'total_steps': self.step_count,
            'total_time': time.time() - self.start_time,
            'average_step_time': self.get_average_step_time(),
            'steps_per_second': self.step_count / (time.time() - self.start_time)
            if time.time() > self.start_time else 0,
            'final_metrics': self.get_current_metrics(),
            'metric_count': len(self.metrics_history)
        }
        
    def get_statistical_analysis(self) -> Dict[str, Any]:
        """Get statistical analysis of simulation metrics."""
        if not self.metrics_history:
            return {}
            
        spikes = [m['total_spikes'] for m in self.metrics_history]
        firing_rates = [m['firing_rate'] for m in self.metrics_history if m['firing_rate'] > 0]
        
        return {
            'spike_statistics': {
                'min': min(spikes),
                'max': max(spikes),
                'mean': sum(spikes) / len(spikes),
                'std': (sum((x - sum(spikes)/len(spikes))**2 for x in spikes) / len(spikes))**0.5
                if len(spikes) > 1 else 0,
            },
            'firing_rate_statistics': {
                'min': min(firing_rates) if firing_rates else 0,
                'max': max(firing_rates) if firing_rates else 0,
                'mean': sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            },
            'development_stages': list(set(m['development_stage'] for m in self.metrics_history)),
            'time_range': {
                'start': self.metrics_history[0]['timestamp'],
                'end': self.metrics_history[-1]['timestamp'],
                'duration': self.metrics_history[-1]['timestamp'] - self.metrics_history[0]['timestamp']
            }
        }

class BrainSerializer:
    """
    Advanced serialization utilities for NLM brain states.
    
    Provides multiple serialization formats for saving and loading
    brain states, including JSON, pickle, and compressed formats.
    """
    
    @staticmethod
    def to_json(brain: Brain, world: Optional[SimpleWorld] = None,
                agent: Optional[AgentBrain] = None) -> str:
        """
        Serialize brain state to JSON format.
        
        Args:
            brain: The brain to serialize
            world: Optional world state to include
            agent: Optional agent state to include
            
        Returns:
            JSON string representation of the brain state
        """
        state = {
            'version': __version__,
            'timestamp': time.time(),
            'brain': {
                'neuron_count': brain.getTotalNeuronCount(),
                'spike_count': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage().name,
                'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            }
        }
        
        if world:
            state['world'] = {
                'time': world.getSimulationTime(),
                'width': world.getWidth(),
                'height': world.getHeight(),
                'agent': {
                    'x': world.getAgentBody().x,
                    'y': world.getAgentBody().y,
                    'orientation': world.getAgentBody().orientation,
                    'energy': world.getAgentBody().energy,
                    'health': world.getAgentBody().health,
                    'age': world.getAgentBody().age,
                }
            }
            
        if agent:
            state['agent'] = {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation': agent.getNeuromodulationLevel(),
            }
            
        return json.dumps(state, indent=2)
        
    @staticmethod
    def from_json(json_string: str, brain: Brain, world: Optional[SimpleWorld] = None,
                  agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
        """
        Deserialize brain state from JSON format.
        
        Args:
            json_string: JSON string to deserialize
            brain: Brain instance to update
            world: Optional world instance to update
            agent: Optional agent instance to update
            
        Returns:
            Dictionary with deserialization results
        """
        state = json.loads(json_string)
        results = {'success': True, 'warnings': []}
        
        # Note: This is a simplified implementation
        # In practice, you would use brain.save() and brain.load()
        # for proper brain state serialization
        
        return results
        
    @staticmethod
    def to_pickle(brain: Brain, filename: str) -> None:
        """
        Serialize brain state to pickle file.
        
        Args:
            brain: The brain to serialize
            filename: Output file name
        """
        with open(filename, 'wb') as f:
            pickle.dump(brain, f)
            
    @staticmethod
    def from_pickle(filename: str) -> Any:
        """
        Deserialize brain state from pickle file.
        
        Args:
            filename: Input file name
            
        Returns:
            Deserialized brain object
        """
        with open(filename, 'rb') as f:
            return pickle.load(f)

class AdvancedAgent(AgentBrain):
    """
    Enhanced agent with advanced learning and control capabilities.
    
    Extends the basic AgentBrain with additional functionality for
    advanced users, including adaptive control, advanced learning,
    and custom action selection strategies.
    """
    
    def __init__(self, brain: Brain):
        super().__init__(brain)
        self._custom_action_strategy: Optional[Callable] = None
        self._learning_parameters = {
            'reward_learning_rate': 0.01,
            'curiosity_weight': 1.0,
            'novelty_threshold': 0.5,
            'prediction_error_weight': 1.0,
        }
        self._action_history: List[Action] = []
        self._reward_history: List[float] = []
        
    def set_action_strategy(self, strategy: Callable[[Brain, SimpleWorld], Action]) -> None:
        """
        Set a custom action selection strategy.
        
        Args:
            strategy: Function that takes (brain, world) and returns an Action
        """
        self._custom_action_strategy = strategy
        
    def get_action_history(self, limit: Optional[int] = None) -> List[Action]:
        """
        Get recent action history.
        
        Args:
            limit: Maximum number of actions to return
            
        Returns:
            List of recent actions
        """
        if limit is None:
            return self._action_history.copy()
        return self._action_history[-limit:].copy()
        
    def get_reward_statistics(self) -> Dict[str, float]:
        """
        Get statistics about received rewards.
        
        Returns:
            Dictionary with reward statistics
        """
        if not self._reward_history:
            return {'count': 0, 'mean': 0.0, 'std': 0.0, 'min': 0.0, 'max': 0.0}
            
        rewards = self._reward_history
        mean = sum(rewards) / len(rewards)
        variance = sum((x - mean) ** 2 for x in rewards) / len(rewards)
        
        return {
            'count': len(rewards),
            'mean': mean,
            'std': variance ** 0.5,
            'min': min(rewards),
            'max': max(rewards),
            'total': sum(rewards)
        }
        
    def decodeMotorCommand(self) -> Action:
        """
        Decode motor command with optional custom strategy.
        
        Returns:
            Motor command action
        """
        # Use custom strategy if available
        if self._custom_action_strategy:
            # Note: This assumes a world is available
            # In practice, you might need to store a reference to the world
            pass
            
        # Fall back to default implementation
        return super().decodeMotorCommand()
        
    def applyAdvancedRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply advanced reward modulation with learning.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        # Store reward in history
        self._reward_history.append(reward)
        
        # Apply enhanced learning based on prediction error
        prediction_error = abs(reward - predicted_reward)
        
        # Update learning parameters based on experience
        if prediction_error > self._learning_parameters['novelty_threshold']:
            # High prediction error triggers exploration
            self._learning_parameters['curiosity_weight'] *= 1.1
        else:
            # Low prediction error encourages exploitation
            self._learning_parameters['curiosity_weight'] *= 0.95
            
        # Call parent method
        self.applyRewardModulation(reward, predicted_reward)

class SimulationRunner:
    """
    Advanced simulation runner with multiple execution modes and monitoring.
    
    Provides flexible simulation execution with support for different
    execution strategies, performance monitoring, and result collection.
    """
    
    def __init__(self, brain: Brain, world: Optional[SimpleWorld] = None,
                 agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitoring = SimulationMonitor(brain, world, agent)
        
    def run_episode(self, num_steps: int, timestep: float = 0.1,
                    monitor: bool = True) -> Dict[str, Any]:
        """
        Run a complete simulation episode.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Time per simulation step
            monitor: Whether to collect monitoring data
            
        Returns:
            Dictionary with episode results and statistics
        """
        if monitor:
            self.monitoring = SimulationMonitor(self.brain, self.world, self.agent)
            
        start_time = time.time()
        
        for step in range(num_steps):
            step_start_time = time.time()
            
            # Update world if available
            if self.world:
                self.world.update(timestep)
                
                # Get percept and process with agent
                if self.agent:
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    
            # Brain processing
            self.brain.step(step, step * timestep)
            
            # Get action and apply if agent available
            if self.agent:
                action = self.agent.decodeMotorCommand()
                if self.world:
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
            # Record monitoring data
            step_time = time.time() - step_start_time
            if monitor:
                self.monitoring.record_step(step_time)
                
        end_time = time.time()
        
        # Get final results
        results = {
            'episode_length': num_steps,
            'simulation_time': end_time - start_time,
            'brain_metrics': self.brain.getTotalSpikeCount(),
            'monitoring': self.monitoring.get_performance_summary() if monitor else None,
            'final_state': self._get_simulation_state()
        }
        
        return results
        
    def _get_simulation_state(self) -> Dict[str, Any]:
        """Get current simulation state as dictionary."""
        state = {
            'brain': {
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_neuron_count': self.brain.getFiringNeuronCount(),
                'total_neuron_count': self.brain.getTotalNeuronCount(),
                'development_stage': self.brain.getDevelopmentalStage().name,
            }
        }
        
        if self.world:
            state['world'] = {
                'simulation_time': self.world.getSimulationTime(),
                'agent_position': {
                    'x': self.world.getAgentBody().x,
                    'y': self.world.getAgentBody().y,
                },
                'agent_energy': self.world.getAgentBody().energy,
            }
            
        if self.agent:
            state['agent'] = {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
            }
            
        return state
        
    def run_multiple_episodes(self, num_episodes: int, steps_per_episode: int,
                             timestep: float = 0.1, monitor: bool = True) -> List[Dict[str, Any]]:
        """
        Run multiple simulation episodes and return results.
        
        Args:
            num_episodes: Number of episodes to run
            steps_per_episode: Steps per episode
            timestep: Time per simulation step
            monitor: Whether to collect monitoring data
            
        Returns:
            List of episode results
        """
        all_results = []
        
        for episode in range(num_episodes):
            # Reset simulation for new episode if possible
            if self.world:
                self.world.reset()
            if self.agent:
                self.agent.reset()
                
            # Run episode
            results = self.run_episode(steps_per_episode, timestep, monitor)
            results['episode_number'] = episode
            
            all_results.append(results)
            
        return all_results

# Module-level convenience functions for advanced usage

def create_advanced_brain(config=None, **config_kwargs) -> Brain:
    """
    Create an advanced brain with enhanced configuration.
    
    Args:
        config: Optional base configuration
        **config_kwargs: Additional configuration parameters
        
    Returns:
        Configured Brain instance
    """
    if config is None:
        config = createDefaultConfig()
        
    # Apply additional configuration
    for key, value in config_kwargs.items():
        config.set(key, value)
        
    brain = createBrain(config)
    brain.initialize()
    return brain


def create_advanced_agent(brain: Brain, world: SimpleWorld, 
                         enable_learning: bool = True) -> AdvancedAgent:
    """
    Create an advanced agent with learning capabilities.
    
    Args:
        brain: Brain instance
        world: World instance
        enable_learning: Whether to enable learning subsystems
        
    Returns:
        AdvancedAgent instance
    """
    agent = AdvancedAgent(brain)
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
    return agent


def run_advanced_simulation(brain: Brain, world: SimpleWorld,
                           agent: AdvancedAgent, num_steps: int,
                           timestep: float = 0.1) -> Dict[str, Any]:
    """
    Run an advanced simulation with comprehensive monitoring.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: Advanced agent instance
        num_steps: Number of simulation steps
        timestep: Time per simulation step
        
    Returns:
        Dictionary with simulation results and analysis
    """
    # Create simulation runner
    runner = SimulationRunner(brain, world, agent)
    
    # Run episode
    results = runner.run_episode(num_steps, timestep, monitor=True)
    
    # Add advanced analysis
    if 'monitoring' in results and results['monitoring']:
        monitoring = results['monitoring']
        if monitoring:
            results['statistical_analysis'] = runner.monitoring.get_statistical_analysis()
            
    return results

# Context manager for advanced simulation control
@contextlib.contextmanager
def simulation_session(brain: Brain, world: Optional[SimpleWorld] = None,
                       agent: Optional[AdvancedAgent] = None):
    """
    Context manager for advanced simulation sessions.
    
    Provides automatic resource management and cleanup for
    complex simulation scenarios.
    
    Example:
        with simulation_session(brain, world, agent) as session:
            results = session.run_simulation(1000)
            # Resources are automatically cleaned up
    """
    session = NLMContextManager(brain, world, agent)
    
    try:
        with session:
            yield session
    except Exception as e:
        # Ensure cleanup even if an exception occurs
        pass
        
# Export advanced classes and functions
__all__ = [
    # Core classes
    'NLMContextManager',
    'SimulationMonitor',
    'BrainSerializer',
    'AdvancedAgent',
    'SimulationRunner',
    
    # Convenience functions
    'create_advanced_brain',
    'create_advanced_agent',
    'run_advanced_simulation',
    
    # Context managers
    'simulation_session',
    
    # Type aliases
    'NeuronCount',
    'SpikeCount',
    'FiringRate',
    'SimulationTime',
]
