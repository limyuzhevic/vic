"""
Agent system functionality for NLM Python bindings.

This module provides agent brain interfaces, action selection,
and behavior control functionality.
"""

import numpy as np
import random
from typing import Optional, List, Dict, Any, Tuple

# Re-export agent bindings from pybind11
from ..core import *

class EnhancedAgentBrain:
    """
    Enhanced agent brain with Pythonic interface and advanced features.
    
    Provides agent brain functionality with additional Python features
    including better error handling, progress tracking, and convenience methods.
    """
    
    def __init__(self, agent_brain):
        self._agent_brain = agent_brain
        self._action_history = []
        self._sensory_history = []
        self._neuromodulation_history = []
    
    def initialize_enhanced(self, world, enable_features: Dict[str, bool] = None):
        """
        Enhanced agent initialization with feature control.
        
        Parameters
        ----------
        world : SimpleWorld
            World to initialize with.
        enable_features : dict, optional
            Dictionary of features to enable/disable.
        """
        self._agent_brain.initialize(world)
        
        # Set default features if not specified
        if enable_features is None:
            enable_features = {
                'reward_modulation': True,
                'curiosity': True,
                'development': True,
                'structural_plasticity': True,
            }
        
        # Apply feature settings
        self._agent_brain.enableRewardModulation(enable_features['reward_modulation'])
        self._agent_brain.enableCuriosity(enable_features['curiosity'])
        self._agent_brain.enableDevelopment(enable_features['development'])
        self._agent_brain.enableStructuralPlasticity(enable_features['structural_plasticity'])
    
    def process_sensory_enhanced(self, percept, record_history: bool = True):
        """
        Enhanced sensory processing with history recording.
        
        Parameters
        ----------
        percept : SensoryPercept
            Sensory percept to process.
        record_history : bool
            Whether to record this percept in history.
        """
        if record_history:
            self._sensory_history.append({
                'step': len(self._sensory_history),
                'vision': percept.getVision(),
                'touch': percept.getTouch(),
                'internal': percept.getInternal(),
                'proprioception': percept.getProprioception(),
                'timestamp': percept.getTimestamp(),
            })
        
        self._agent_brain.processSensoryInput(percept)
    
    def decode_action_enhanced(self, exploration_rate: float = 0.0,
                              policy: str = 'argmax') -> Action:
        """
        Enhanced action decoding with exploration and policy options.
        
        Parameters
        ----------
        exploration_rate : float
            Probability of random action (0-1).
        policy : str
            Policy for action selection ('argmax', 'softmax', 'epsilon_greedy').
        
        Returns
        -------
        Action
            Selected action.
        """
        # Get default action from agent brain
        default_action = self._agent_brain.decodeMotorCommand()
        
        # Apply exploration based on policy
        if exploration_rate > 0 and random.random() < exploration_rate:
            # Random action
            action_types = [
                ActionType.MoveForward, ActionType.MoveBackward,
                ActionType.TurnLeft, ActionType.TurnRight,
                ActionType.LookLeft, ActionType.LookRight,
                ActionType.Interact, ActionType.Look,
                ActionType.LookUp, ActionType.LookDown,
                ActionType.Eat, ActionType.Drink, ActionType.Rest, ActionType.Wait
            ]
            return Action(random.choice(action_types))
        
        # Policy-based selection
        if policy == 'softmax':
            # Use curiosity to influence action selection
            curiosity = self._agent_brain.getCuriosityLevel()
            if curiosity > 0.5:
                # More exploration when curiosity is high
                return Action(random.choice([
                    ActionType.MoveForward, ActionType.MoveBackward,
                    ActionType.TurnLeft, ActionType.TurnRight,
                    ActionType.Interact, ActionType.Wait
                ]))
        
        return default_action
    
    def apply_reward_enhanced(self, reward: float, predicted_reward: float = 0.0,
                            record_history: bool = True):
        """
        Enhanced reward application with history recording.
        
        Parameters
        ----------
        reward : float
            Actual reward received.
        predicted_reward : float
            Predicted reward (for prediction error calculation).
        record_history : bool
            Whether to record this reward in history.
        """
        if record_history:
            self._neuromodulation_history.append({
                'step': len(self._neuromodulation_history),
                'reward': reward,
                'predicted_reward': predicted_reward,
                'prediction_error': reward - predicted_reward,
                'dopamine_level': self._agent_brain.getNeuromodulationLevel(),
                'curiosity_level': self._agent_brain.getCuriosityLevel(),
            })
        
        self._agent_brain.applyRewardModulation(reward, predicted_reward)
    
    def update_development_enhanced(self, timestep: float, development_speed: float = 1.0):
        """
        Enhanced development update with speed control.
        
        Parameters
        ----------
        timestep : float
            Simulation timestep.
        development_speed : float
            Development speed multiplier.
        """
        # Scale timestep by development speed
        scaled_timestep = timestep * development_speed
        self._agent_brain.updateDevelopment(scaled_timestep)
    
    def get_brain_state(self) -> Dict[str, Any]:
        """
        Get complete brain state information.
        
        Returns
        -------
        dict
            Complete brain state.
        """
        brain = self._agent_brain.getBrain()
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'neuromodulation_level': self._agent_brain.getNeuromodulationLevel(),
            'curiosity_level': self._agent_brain.getCuriosityLevel(),
            'novelty_level': self._agent_brain.getNoveltyLevel(),
            'prediction_error': self._agent_brain.getPredictionError(),
            'configuration': {
                'reward_modulation': self._agent_brain.isRewardModulationEnabled(),
                'curiosity': self._agent_brain.isCuriosityEnabled(),
                'development': self._agent_brain.isDevelopmentEnabled(),
                'structural_plasticity': self._agent_brain.isStructuralPlasticityEnabled(),
            }
        }
    
    def run_episode_enhanced(self, world, num_steps: int,
                           exploration_rate: float = 0.0,
                           record_metrics: bool = True) -> Dict[str, Any]:
        """
        Run enhanced episode simulation.
        
        Parameters
        ----------
        world : SimpleWorld
            World to simulate in.
        num_steps : int
            Number of steps to run.
        exploration_rate : float
            Probability of random action.
        record_metrics : bool
            Whether to record metrics.
        
        Returns
        -------
        dict
            Episode results and metrics.
        """
        import time
        
        metrics = {
            'steps_completed': num_steps,
            'actions_taken': [],
            'rewards_received': [],
            'sensory_inputs': [],
            'brain_states': [],
            'action_counts': {},
            'step_times': [],
            'start_time': time.time(),
        }
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            self.process_sensory_enhanced(percept)
            
            # Get action
            action = self.decode_action_enhanced(exploration_rate)
            
            # Record action
            action_type = action.getType() if hasattr(action, 'getType') else str(action)
            metrics['actions_taken'].append(action_type)
            metrics['action_counts'][action_type] = metrics['action_counts'].get(action_type, 0) + 1
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            internal = percept.getInternal()
            reward = internal[0] if internal and len(internal) > 0 else 0.0
            metrics['rewards_received'].append(reward)
            
            # Apply reward modulation
            self.apply_reward_enhanced(reward, 0.0)
            
            # Update development
            self.update_development_enhanced(0.1)
            
            # Record brain state
            if record_metrics:
                brain_state = self.get_brain_state()
                metrics['brain_states'].append(brain_state)
                metrics['sensory_inputs'].append({
                    'vision': percept.getVision(),
                    'touch': percept.getTouch(),
                    'internal': percept.getInternal(),
                    'proprioception': percept.getProprioception(),
                })
            
            # Record step time
            step_time = time.time() - step_start
            metrics['step_times'].append(step_time)
            
            # Run brain step
            brain = self._agent_brain.getBrain()
            brain.step(step)
        
        # Calculate statistics
        metrics['total_time'] = time.time() - metrics['start_time']
        
        if metrics['step_times']:
            metrics['average_step_time'] = sum(metrics['step_times']) / len(metrics['step_times'])
            metrics['steps_per_second'] = num_steps / metrics['total_time'] if metrics['total_time'] > 0 else 0
        
        if metrics['action_counts']:
            total_actions = sum(metrics['action_counts'].values())
            metrics['action_distribution'] = {
                action: count / total_actions for action, count in metrics['action_counts'].items()
            }
        
        if metrics['rewards_received']:
            metrics['total_reward'] = sum(metrics['rewards_received'])
            metrics['average_reward'] = metrics['total_reward'] / len(metrics['rewards_received'])
        
        return metrics

class BehavioralStrategy:
    """
    Behavioral strategy manager for agent behavior control.
    
    Provides high-level behavioral control with strategies, goals,
    and adaptive behavior modification.
    """
    
    def __init__(self, agent_brain, world):
        self.agent_brain = agent_brain
        self.world = world
        self.strategies = {}
        self.current_strategy = None
        self.goals = []
    
    def add_strategy(self, name: str, strategy_func, priority: int = 1):
        """
        Add a behavioral strategy.
        
        Parameters
        ----------
        name : str
            Strategy name.
        strategy_func : callable
            Function that returns action based on brain state.
        priority : int
            Strategy priority (higher = more important).
        """
        self.strategies[name] = {
            'func': strategy_func,
            'priority': priority,
        }
    
    def set_strategy(self, name: str) -> bool:
        """
        Set current behavioral strategy.
        
        Parameters
        ----------
        name : str
            Strategy name.
        
        Returns
        -------
        bool
            True if strategy set successfully.
        """
        if name in self.strategies:
            self.current_strategy = name
            return True
        return False
    
    def add_goal(self, goal_func, priority: int = 1):
        """
        Add a behavioral goal.
        
        Parameters
        ----------
        goal_func : callable
            Function that returns True if goal is met.
        priority : int
            Goal priority (higher = more important).
        """
        self.goals.append({
            'func': goal_func,
            'priority': priority,
            'completed': False,
        })
    
    def evaluate_goals(self) -> List[str]:
        """
        Evaluate all goals and return completed ones.
        
        Returns
        -------
        list
            List of completed goal names.
        """
        completed_goals = []
        
        for goal in self.goals:
            if not goal['completed'] and goal['func'](self.agent_brain, self.world):
                goal['completed'] = True
                completed_goals.append(goal['func'].__name__)
        
        return completed_goals
    
    def select_action_with_strategies(self) -> Action:
        """
        Select action based on current strategy and goals.
        
        Returns
        -------
        Action
            Selected action.
        """
        if self.current_strategy:
            strategy = self.strategies[self.current_strategy]
            action = strategy['func'](self.agent_brain, self.world)
            return action
        
        # Default action selection
        return self.agent_brain.decodeMotorCommand()

class LearningManager:
    """
    Learning manager for agent learning and adaptation.
    
    Provides learning curve tracking, performance evaluation,
    and adaptive parameter tuning.
    """
    
    def __init__(self, agent_brain):
        self.agent_brain = agent_brain
        self.learning_history = []
        self.performance_metrics = []
    
    def record_learning_event(self, event_type: str, parameters: Dict[str, Any]):
        """
        Record a learning event.
        
        Parameters
        ----------
        event_type : str
            Type of learning event.
        parameters : dict
            Parameters associated with the event.
        """
        self.learning_history.append({
            'timestamp': len(self.learning_history),
            'event_type': event_type,
            'parameters': parameters,
            'neuromodulation_level': self.agent_brain.getNeuromodulationLevel(),
            'curiosity_level': self.agent_brain.getCuriosityLevel(),
        })
    
    def get_learning_summary(self) -> Dict[str, Any]:
        """
        Get learning summary statistics.
        
        Returns
        -------
        dict
            Learning summary.
        """
        if not self.learning_history:
            return {}
        
        event_types = {}
        for event in self.learning_history:
            event_type = event['event_type']
            if event_type not in event_types:
                event_types[event_type] = []
            event_types[event_type].append(event)
        
        return {
            'total_events': len(self.learning_history),
            'event_types': list(event_types.keys()),
            'last_event': self.learning_history[-1] if self.learning_history else None,
            'neuromodulation_trend': self._calculate_trend(
                self.learning_history, 'neuromodulation_level'
            ),
            'curiosity_trend': self._calculate_trend(
                self.learning_history, 'curiosity_level'
            ),
        }
    
    def _calculate_trend(self, history: List[Dict], metric: str) -> Dict[str, float]:
        """Calculate trend in a metric over time."""
        if len(history) < 2:
            return {'trend': 'stable', 'change': 0.0}
        
        values = [event[metric] for event in history]
        if len(values) < 2:
            return {'trend': 'stable', 'change': 0.0}
        
        # Simple trend calculation
        start_val = values[0]
        end_val = values[-1]
        change = end_val - start_val
        trend = 'increasing' if change > 0.1 else 'decreasing' if change < -0.1 else 'stable'
        
        return {'trend': trend, 'change': change}

# Re-export commonly used classes and functions
__all__ = [
    'EnhancedAgentBrain',
    'BehavioralStrategy', 
    'LearningManager',
    # Core types from bindings
    'Action',
    'ActionType',
    'SensoryPercept',
    'WorldObject',
    'AgentBody',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
]