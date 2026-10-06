"""
World simulation functionality for NLM Python bindings.

This module provides world simulation, environment interaction,
and sensory processing functionality.
"""

import numpy as np
from typing import Optional, Tuple, List, Dict, Any

# Re-export world bindings from pybind11
from ..core import *

class EnhancedSimpleWorld:
    """
    Enhanced world simulation with Pythonic interface.
    
    Provides world simulation with additional Python features including
    better error handling, progress tracking, and convenience methods.
    """
    
    def __init__(self, world):
        self._world = world
        self._simulation_time = 0.0
        self._step_history = []
    
    def configure_enhanced(self, width: int = 20, height: int = 20,
                          vision_width: int = 8, vision_height: int = 8,
                          vision_channels: int = 3, max_energy: float = 100.0,
                          energy_decay_rate: float = 0.01) -> None:
        """
        Enhanced world configuration with more options.
        
        Parameters
        ----------
        width : int
            World width in tiles.
        height : int
            World height in tiles.
        vision_width : int
            Vision width in tiles.
        vision_height : int
            Vision height in tiles.
        vision_channels : int
            Number of vision channels.
        max_energy : float
            Maximum agent energy.
        energy_decay_rate : float
            Energy decay rate per timestep.
        """
        self._world.configure(width, height, vision_width, vision_height)
        self._world.setMaxEnergy(max_energy)
        self._world.setEnergyDecayRate(energy_decay_rate)
    
    def add_resource(self, x: float, y: float, value: float = 1.0,
                    radius: float = 0.5) -> None:
        """
        Add a resource object to the world.
        
        Parameters
        ----------
        x, y : float
            Object coordinates.
        value : float
            Resource value.
        radius : float
            Object radius.
        """
        obj = WorldObject(x, y, WorldObjectType.Resource, value, radius)
        self._world.addObject(obj)
    
    def add_hazard(self, x: float, y: float, radius: float = 0.5) -> None:
        """
        Add a hazard object to the world.
        
        Parameters
        ----------
        x, y : float
            Object coordinates.
        radius : float
            Object radius.
        """
        obj = WorldObject(x, y, WorldObjectType.Hazard, 1.0, radius)
        self._world.addObject(obj)
    
    def get_vision_data(self) -> np.ndarray:
        """
        Get vision data as a numpy array.
        
        Returns
        -------
        numpy.ndarray
            Vision data array of shape (vision_height, vision_width, channels).
        """
        percept = self._world.getSensoryPercept()
        vision = percept.getVision()
        
        if not vision:
            width = self._world.getVisionWidth()
            height = self._world.getVisionHeight()
            return np.zeros((height, width, 3))
        
        # Reshape vision data to 2D array
        width = self._world.getVisionWidth()
        height = self._world.getVisionHeight()
        
        # Convert to numpy array
        vision_array = np.array(vision)
        if len(vision_array.shape) == 1:
            vision_array = vision_array.reshape(height, width)
            
        return vision_array
    
    def run_simulation_step(self, timestep: float = 0.1,
                           apply_action: Optional[Any] = None) -> Dict[str, Any]:
        """
        Run a single simulation step with automatic action application.
        
        Parameters
        ----------
        timestep : float
            Simulation timestep.
        apply_action : Action, optional
            Action to apply. If None, agent will decode its own action.
        
        Returns
        -------
        dict
            Step results including state before and after.
        """
        # Record state before step
        percept_before = self._world.getSensoryPercept()
        body_before = self._world.getAgentBody()
        
        # Update world
        self._world.update(timestep)
        
        # Apply action if provided, otherwise let agent decide
        if apply_action is not None:
            self._world.applyMotorCommand(apply_action, self._world.getSimulationTime())
        
        # Record state after step
        percept_after = self._world.getSensoryPercept()
        body_after = self._world.getAgentBody()
        
        # Calculate changes
        position_change = (
            body_after.x - body_before.x,
            body_after.y - body_before.y
        )
        energy_change = body_after.energy - body_before.energy
        
        # Store step history
        self._step_history.append({
            'step': len(self._step_history),
            'position_before': (body_before.x, body_before.y),
            'position_after': (body_after.x, body_after.y),
            'position_change': position_change,
            'energy_before': body_before.energy,
            'energy_after': body_after.energy,
            'energy_change': energy_change,
            'simulation_time': self._world.getSimulationTime(),
        })
        
        return {
            'percept_before': percept_before,
            'percept_after': percept_after,
            'body_before': body_before,
            'body_after': body_after,
            'position_change': position_change,
            'energy_change': energy_change,
        }
    
    def run_episode(self, num_steps: int, agent_brain=None,
                   record_sensory: bool = True,
                   callback: Optional[callable] = None) -> Dict[str, Any]:
        """
        Run a complete episode simulation.
        
        Parameters
        ----------
        num_steps : int
            Number of steps to run.
        agent_brain : AgentBrain, optional
            Agent brain to use. If None, uses agent's brain from world.
        record_sensory : bool
            Whether to record sensory data.
        callback : callable, optional
            Function called after each step.
        
        Returns
        -------
        dict
            Episode results and statistics.
        """
        import time
        
        results = {
            'steps_completed': num_steps,
            'total_time': 0.0,
            'final_position': (self._world.getAgentBody().x, self._world.getAgentBody().y),
            'final_energy': self._world.getAgentBody().energy,
            'sensory_data': [] if record_sensory else None,
            'step_times': [],
            'action_counts': {},
        }
        
        start_time = time.time()
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Get sensory input
            percept = self._world.getSensoryPercept()
            
            # Process sensory input if agent brain provided
            if agent_brain:
                agent_brain.processSensoryInput(percept)
            
            # Get action from agent brain or use None
            action = None
            if agent_brain:
                action = agent_brain.decodeMotorCommand()
                # Count actions
                action_type = action.getType() if hasattr(action, 'getType') else str(action)
                results['action_counts'][action_type] = results['action_counts'].get(action_type, 0) + 1
            
            # Run simulation step
            step_result = self.run_simulation_step(0.1, action)
            
            # Record sensory data if requested
            if record_sensory and step_result['percept_after']:
                results['sensory_data'].append({
                    'step': step,
                    'vision': step_result['percept_after'].getVision(),
                    'touch': step_result['percept_after'].getTouch(),
                    'internal': step_result['percept_after'].getInternal(),
                    'proprioception': step_result['percept_after'].getProprioception(),
                })
            
            # Call user callback
            if callback:
                callback(step, step_result)
            
            # Record step time
            step_time = time.time() - step_start
            results['step_times'].append(step_time)
        
        results['total_time'] = time.time() - start_time
        
        # Calculate statistics
        if results['step_times']:
            results['average_step_time'] = sum(results['step_times']) / len(results['step_times'])
            results['steps_per_second'] = num_steps / results['total_time'] if results['total_time'] > 0 else 0
        
        if results['action_counts']:
            total_actions = sum(results['action_counts'].values())
            results['action_distribution'] = {
                action: count / total_actions for action, count in results['action_counts'].items()
            }
        
        return results
    
    def get_world_state(self) -> Dict[str, Any]:
        """
        Get comprehensive world state information.
        
        Returns
        -------
        dict
            Complete world state.
        """
        body = self._world.getAgentBody()
        
        return {
            'width': self._world.getWidth(),
            'height': self._world.getHeight(),
            'vision_width': self._world.getVisionWidth(),
            'vision_height': self._world.getVisionHeight(),
            'agent_position': (body.x, body.y),
            'agent_orientation': body.orientation,
            'agent_energy': body.energy,
            'agent_health': body.health,
            'agent_age': body.age,
            'simulation_time': self._world.getSimulationTime(),
            'max_energy': self._world.getMaxEnergy(),
            'energy_decay_rate': self._world.getEnergyDecayRate(),
            'random_seed': self._world.getRandomSeed(),
            'agent_body_state': {
                'x': body.x,
                'y': body.y,
                'orientation': body.orientation,
                'velocityX': body.velocityX,
                'velocityY': body.velocityY,
                'angularVelocity': body.angularVelocity,
                'energy': body.energy,
                'health': body.health,
                'age': body.age,
                'isMoving': body.isMoving,
                'isTurning': body.isTurning,
                'lastActionTime': body.lastActionTime,
            }
        }

class AgentSimulator:
    """
    Complete agent simulation with brain and world interaction.
    
    Provides high-level interface for running agent simulations with
    common patterns and convenience methods.
    """
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.simulation_history = []
    
    def create_default_setup(self, config_params=None):
        """
        Create default agent setup with sensible defaults.
        
        Parameters
        ----------
        config_params : dict, optional
            Configuration parameters.
        """
        if config_params is None:
            config_params = {}
        
        # Create brain with config
        brain_config = createDefaultConfig()
        
        # Set brain parameters if provided
        if 'neuron_count' in config_params:
            brain_config.set("brain.neuron_count", config_params['neuron_count'])
        if 'learning_rate' in config_params:
            brain_config.set("plasticity.stdp.learning_rate", config_params['learning_rate'])
        
        self.brain = createBrain(brain_config)
        self.brain.initialize()
        
        # Create world
        self.world = createSimpleWorld()
        world_width = config_params.get('world_width', 20)
        world_height = config_params.get('world_height', 20)
        vision_width = config_params.get('vision_width', 8)
        vision_height = config_params.get('vision_height', 8)
        
        self.world.configure(world_width, world_height, vision_width, vision_height)
        self.world.reset()
        
        # Create agent
        self.agent = createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable subsystems if configured
        if config_params.get('enable_reward_modulation', True):
            self.agent.enableRewardModulation(True)
        if config_params.get('enable_curiosity', True):
            self.agent.enableCuriosity(True)
        if config_params.get('enable_development', True):
            self.agent.enableDevelopment(True)
        if config_params.get('enable_structural_plasticity', True):
            self.agent.enableStructuralPlasticity(True)
        
        return self
    
    def run_training_episode(self, num_steps: int, exploration_rate: float = 0.1,
                           record_metrics: bool = True) -> Dict[str, Any]:
        """
        Run a training episode with exploration and learning.
        
        Parameters
        ----------
        num_steps : int
            Number of training steps.
        exploration_rate : float
            Probability of random action (0-1).
        record_metrics : bool
            Whether to record training metrics.
        
        Returns
        -------
        dict
            Training results and metrics.
        """
        import time
        
        metrics = {
            'steps': num_steps,
            'total_spikes': 0,
            'max_firing': 0,
            'average_firing': 0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
            'rewards': [],
            'action_distribution': {},
            'step_times': [],
            'start_time': time.time(),
        }
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Get action with exploration
            action = self.agent.decodeMotorCommand()
            
            # Apply exploration
            if np.random.random() < exploration_rate:
                # Random action
                action_types = [
                    ActionType.MoveForward, ActionType.MoveBackward,
                    ActionType.TurnLeft, ActionType.TurnRight,
                    ActionType.Interact, ActionType.Wait
                ]
                action = Action(np.random.choice(action_types))
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            internal = percept.getInternal()
            reward = internal[0] if internal and len(internal) > 0 else 0.0
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record metrics
            if record_metrics:
                metrics['total_spikes'] += self.brain.getTotalSpikeCount()
                metrics['max_firing'] = max(metrics['max_firing'], self.brain.getFiringNeuronCount())
                metrics['curiosity_levels'].append(self.agent.getCuriosityLevel())
                metrics['novelty_levels'].append(self.agent.getNoveltyLevel())
                metrics['development_stages'].append(self.brain.getDevelopmentalStage())
                metrics['rewards'].append(reward)
                
                action_type = action.getType() if hasattr(action, 'getType') else str(action)
                metrics['action_distribution'][action_type] = metrics['action_distribution'].get(action_type, 0) + 1
            
            # Record step time
            step_time = time.time() - step_start
            metrics['step_times'].append(step_time)
            
            # Run brain step
            self.brain.step(step)
            
            # Store simulation state
            self.simulation_history.append({
                'step': step,
                'position': (self.world.getAgentBody().x, self.world.getAgentBody().y),
                'action': action,
                'reward': reward,
                'curiosity': self.agent.getCuriosityLevel(),
                'firing_rate': self.brain.getAverageFiringRate(),
            })
        
        # Calculate final statistics
        metrics['total_time'] = time.time() - metrics['start_time']
        
        if metrics['step_times']:
            metrics['average_step_time'] = sum(metrics['step_times']) / len(metrics['step_times'])
            metrics['steps_per_second'] = num_steps / metrics['total_time'] if metrics['total_time'] > 0 else 0
        
        if metrics['total_spikes'] > 0:
            metrics['average_firing'] = metrics['total_spikes'] / num_steps
        
        if metrics['action_distribution']:
            total_actions = sum(metrics['action_distribution'].values())
            metrics['action_distribution_normalized'] = {
                action: count / total_actions for action, count in metrics['action_distribution'].items()
            }
        
        return metrics
    
    def run_multiple_episodes(self, num_episodes: int, steps_per_episode: int,
                             exploration_rate: float = 0.1) -> List[Dict[str, Any]]:
        """
        Run multiple training episodes.
        
        Parameters
        ----------
        num_episodes : int
            Number of episodes to run.
        steps_per_episode : int
            Number of steps per episode.
        exploration_rate : float
            Exploration rate for all episodes.
        
        Returns
        -------
        list
            List of episode results.
        """
        episode_results = []
        
        for episode in range(num_episodes):
            print(f"Running episode {episode + 1}/{num_episodes}")
            episode_result = self.run_training_episode(
                steps_per_episode, exploration_rate
            )
            episode_result['episode'] = episode
            episode_results.append(episode_result)
        
        return episode_results

# Utility functions

def create_test_environment(width: int = 20, height: int = 20):
    """
    Create a test environment with resources and hazards.
    
    Parameters
    ----------
    width, height : int
        World dimensions.
    
    Returns
    -------
    SimpleWorld
        Configured world with test objects.
    """
    world = createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.reset()
    
    # Add some resources
    for i in range(5):
        x = np.random.uniform(2, width - 2)
        y = np.random.uniform(2, height - 2)
        world.addObject(WorldObject(x, y, WorldObjectType.Resource, 1.0))
    
    # Add some hazards
    for i in range(3):
        x = np.random.uniform(2, width - 2)
        y = np.random.uniform(2, height - 2)
        world.addObject(WorldObject(x, y, WorldObjectType.Hazard, 1.0))
    
    return world

def create_agent_with_features(brain_config=None, enable_features=None):
    """
    Create an agent brain with specific features enabled.
    
    Parameters
    ----------
    brain_config : dict, optional
        Configuration parameters.
    enable_features : list, optional
        List of features to enable.
    
    Returns
    -------
    tuple
        (brain, agent, world) - The created components.
    """
    brain = create_brain_with_config(brain_config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable specified features
    if enable_features:
        for feature in enable_features:
            if feature == 'reward_modulation':
                agent.enableRewardModulation(True)
            elif feature == 'curiosity':
                agent.enableCuriosity(True)
            elif feature == 'development':
                agent.enableDevelopment(True)
            elif feature == 'structural_plasticity':
                agent.enableStructuralPlasticity(True)
    
    return brain, agent, world

# Re-export commonly used classes and functions
__all__ = [
    'EnhancedSimpleWorld',
    'AgentSimulator',
    'create_test_environment',
    'create_agent_with_features',
    # Core types from bindings
    'WorldObject',
    'AgentBody', 
    'Action',
    'ActionResult',
    'SensoryPercept',
    'SimpleWorld',
    'ActionType',
    'WorldObjectType',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
]