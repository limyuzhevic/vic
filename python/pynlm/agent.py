"""
Agent management for NLM Python bindings.

This module provides enhanced agent functionality for NLM,
including agent brain interfaces, behavior management, and
exploration strategies.
"""

import random
import time
from typing import Optional, Dict, Any, List, Tuple, Enum
from enum import Enum as PythonEnum

from .exceptions import (
    SimulationError, BrainError, ValidationError, AgentError
)
from .types import (
    NeuronType, ActionType, MotorCommand, DevelopmentalStage
)

# Import C++ types
try:
    from ..bindings import (
        createAgentBrain, AgentBrain, createSimpleWorld, SimpleWorld,
        createBrain, createDefaultConfig
    )
except ImportError as e:
    raise ImportError(f"Failed to import NLM agent bindings: {e}")


class ExplorationStrategy(PythonEnum):
    """Enumeration of exploration strategies for agents."""
    NONE = "none"
    EPSILON_GREEDY = "epsilon_greedy"
    UCB = "ucb"
    BOLTZMANN = "boltzmann"
    NOISE = "noise"
    CURIOUSITY = "curiosity"
class BehaviorType(PythonEnum):
    """Enumeration of agent behavior types."""
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    RANDOM = "random"
class Goal:
    """
    Agent goal with priority and completion tracking.
    """
    
    def __init__(self, description: str, priority: float = 1.0):
        """
        Initialize goal.
        
        Args:
            description: Goal description
            priority: Goal priority (higher = more important)
        """
        self.description = description
        self.priority = priority
        self.completed = False
        self.progress = 0.0
        self.created_time = time.time()
        self.last_updated = self.created_time
    
    def update_progress(self, progress: float) -> None:
        """
        Update goal progress.
        
        Args:
            progress: Progress value (0-1)
        """
        if not 0.0 <= progress <= 1.0:
            raise ValidationError(f"Progress must be between 0.0 and 1.0, got {progress}")
        
        self.progress = progress
        self.last_updated = time.time()
        
        if progress >= 1.0:
            self.completed = True
    
    def is_completed(self) -> bool:
        """Check if goal is completed."""
        return self.completed
    
    def get_time_since_last_update(self) -> float:
        """Get time since last update."""
        return time.time() - self.last_updated
    
    def __repr__(self) -> str:
        status = "✓" if self.completed else "○"
        return f"Goal('{self.description}', priority={self.priority}, {status})"


class AgentState:
    """
    Agent state with history and management.
    """
    
    def __init__(self):
        """Initialize agent state."""
        self._state = {}
        self._history = []
        self._timestamp = 0.0
    
    def set(self, key: str, value: Any) -> None:
        """Set state value."""
        self._state[key] = value
        self._timestamp = time.time()
        self._history.append((self._timestamp, key, value))
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get state value."""
        return self._state.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if state has key."""
        return key in self._state
    
    def get_all(self) -> Dict[str, Any]:
        """Get all state values."""
        return self._state.copy()
    
    def get_history(self, limit: Optional[int] = None) -> List[Tuple[float, str, Any]]:
        """
        Get state change history.
        
        Args:
            limit: Maximum number of history entries to return
            
        Returns:
            List of (timestamp, key, value) tuples
        """
        if limit is not None:
            return self._history[-limit:]
        return self._history.copy()
    
    def clear_history(self) -> None:
        """Clear state history."""
        self._history.clear()
    
    def clear(self) -> None:
        """Clear all state."""
        self._state.clear()
        self.clear_history()
    
    def __repr__(self) -> str:
        return f"AgentState({len(self._state)} variables)"


class NLMAgentBrain:
    """
    Enhanced NLM agent brain with Pythonic interface.
    
    Provides agent functionality including behavior control,
    exploration strategies, and goal management.
    """
    
    def __init__(self, brain):
        """
        Initialize agent brain.
        
        Args:
            brain: Underlying NLM brain object
        """
        if hasattr(brain, '_brain'):
            # If brain is NLMAgentBrain wrapper, get underlying brain
            self._brain = brain._brain
        else:
            self._brain = brain
        
        self._agent = createAgentBrain(self._brain)
        self._is_initialized = False
        
        # Agent state and behavior
        self.state = AgentState()
        self.exploration_strategy = ExplorationStrategy.NONE
        self.behavior_type = BehaviorType.BALANCED
        self.goals = []
        
        # Exploration parameters
        self.epsilon = 0.1
        self.temperature = 1.0
        self.ucb_c = 2.0  # UCB exploration constant
        
        # Statistics
        self.stats = {
            'steps': 0,
            'exploration_actions': 0,
            'exploitation_actions': 0,
            'rewards': 0.0,
            'novelty': 0.0,
            'curiosity': 0.0,
        }
    
    def initialize(self, world) -> None:
        """
        Initialize agent brain with world.
        
        Args:
            world: World object
        """
        if hasattr(world, '_world'):
            self._world = world._world
        else:
            self._world = world
        
        self._agent.initialize(self._world)
        self._is_initialized = True
        
        # Initialize agent state
        self.state.set('world_width', self._world.getWidth())
        self.state.set('world_height', self._world.getHeight())
        self.state.set('initialized', True)
    
    def is_initialized(self) -> bool:
        """Check if agent brain is initialized."""
        return self._is_initialized
    
    def set_exploration_strategy(self, strategy: ExplorationStrategy) -> None:
        """
        Set exploration strategy.
        
        Args:
            strategy: Exploration strategy
        """
        if not isinstance(strategy, ExplorationStrategy):
            raise ValidationError(f"Strategy must be ExplorationStrategy, got {type(strategy)}")
        
        self.exploration_strategy = strategy
        self.state.set('exploration_strategy', strategy.value)
    
    def set_behavior_type(self, behavior_type: BehaviorType) -> None:
        """
        Set behavior type.
        
        Args:
            behavior_type: Behavior type
        """
        if not isinstance(behavior_type, BehaviorType):
            raise ValidationError(f"Behavior type must be BehaviorType, got {type(behavior_type)}")
        
        self.behavior_type = behavior_type
        self.state.set('behavior_type', behavior_type.value)
    
    def set_exploration_parameters(self, epsilon: float = None, 
                                   temperature: float = None,
                                   ucb_c: float = None) -> None:
        """
        Set exploration parameters.
        
        Args:
            epsilon: Epsilon for epsilon-greedy
            temperature: Temperature for Boltzmann exploration
            ucb_c: C constant for UCB exploration
        """
        if epsilon is not None:
            if not 0.0 <= epsilon <= 1.0:
                raise ValidationError(f"Epsilon must be between 0.0 and 1.0, got {epsilon}")
            self.epsilon = epsilon
            self.state.set('epsilon', epsilon)
        
        if temperature is not None:
            if temperature <= 0.0:
                raise ValidationError(f"Temperature must be positive, got {temperature}")
            self.temperature = temperature
            self.state.set('temperature', temperature)
        
        if ucb_c is not None:
            if ucb_c <= 0.0:
                raise ValidationError(f"UCB C must be positive, got {ucb_c}")
            self.ucb_c = ucb_c
            self.state.set('ucb_c', ucb_c)
    
    def add_goal(self, description: str, priority: float = 1.0) -> Goal:
        """
        Add agent goal.
        
        Args:
            description: Goal description
            priority: Goal priority
            
        Returns:
            Created Goal object
        """
        goal = Goal(description, priority)
        self.goals.append(goal)
        self.state.set('goal_count', len(self.goals))
        return goal
    
    def get_goals(self) -> List[Goal]:
        """Get all goals."""
        return self.goals.copy()
    
    def get_completed_goals(self) -> List[Goal]:
        """Get completed goals."""
        return [g for g in self.goals if g.is_completed()]
    
    def get_incomplete_goals(self) -> List[Goal]:
        """Get incomplete goals."""
        return [g for g in self.goals if not g.is_completed()]
    
    def get_average_goal_progress(self) -> float:
        """Get average goal progress."""
        if not self.goals:
            return 0.0
        return sum(g.progress for g in self.goals) / len(self.goals)
    
    def process_sensory_input(self, percept) -> None:
        """
        Process sensory input and inject into brain.
        
        Args:
            percept: Sensory percept data
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.processSensoryInput(percept)
        
        # Update state
        self.state.set('last_sensory_update', time.time())
        self.state.set('sensory_available', True)
    
    def decode_motor_command(self):
        """
        Decode brain motor activity into motor command with exploration.
        
        Returns:
            Motor command
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        # Get default command from brain
        command = self._agent.decodeMotorCommand()
        
        # Apply exploration strategy
        if self.exploration_strategy != ExplorationStrategy.NONE:
            command = self._apply_exploration_strategy(command)
        
        # Update statistics
        self.stats['steps'] += 1
        if command == MotorCommand.Wait:
            self.stats['exploration_actions'] += 1
        else:
            self.stats['exploitation_actions'] += 1
        
        # Update state
        self.state.set('last_action', time.time())
        self.state.set('last_action_type', command.value)
        
        return command
    
    def _apply_exploration_strategy(self, default_command) -> Any:
        """
        Apply exploration strategy to modify default command.
        
        Args:
            default_command: Default motor command
            
        Returns:
            Modified command with exploration
        """
        if self.exploration_strategy == ExplorationStrategy.EPSILON_GREEDY:
            # Epsilon-greedy: choose random action with probability epsilon
            if random.random() < self.epsilon:
                return self._get_random_action()
            return default_command
        
        elif self.exploration_strategy == ExplorationStrategy.BOLTZMANN:
            # Boltzmann exploration: sample from softmax probabilities
            return self._boltzmann_exploration(default_command)
        
        elif self.exploration_strategy == ExplorationStrategy.UCB:
            # UCB: use UCB1 algorithm for action selection
            return self._ucb_exploration(default_command)
        
        elif self.exploration_strategy == ExplorationStrategy.NOISE:
            # Add noise to decision
            return self._noisy_exploration(default_command)
        
        elif self.exploration_strategy == ExplorationStrategy.CURIOUSITY:
            # Curiosity-based exploration
            return self._curiosity_exploration(default_command)
        
        else:
            return default_command
    
    def _get_random_action(self):
        """Get random motor command."""
        # Get all possible motor commands
        actions = [mc for mc in MotorCommand if mc != MotorCommand.Custom]
        return random.choice(actions)
    
    def _boltzmann_exploration(self, default_command):
        """Apply Boltzmann exploration."""
        # Get activity of each action (simplified)
        actions = [mc for mc in MotorCommand if mc != MotorCommand.Custom]
        
        # Calculate softmax probabilities
        activities = []
        for action in actions:
            # In real implementation, this would use actual neural activity
            # For now, use simple heuristic
            if action == default_command:
                activities.append(self.temperature)
            else:
                activities.append(0.1 * self.temperature)
        
        # Sample from softmax distribution
        exp_values = [exp(a / self.temperature) for a in activities]
        total = sum(exp_values)
        probabilities = [e / total for e in exp_values]
        
        # Sample action
        return random.choices(actions, weights=probabilities)[0]
    
    def _ucb_exploration(self, default_command):
        """Apply UCB exploration."""
        # Simplified UCB: prefer less frequently used actions
        actions = [mc for mc in MotorCommand if mc != MotorCommand.Custom]
        
        # Get action statistics (simplified)
        action_counts = {action: 1 for action in actions}  # Initialize
        
        # UCB score calculation
        scores = []
        for action in actions:
            count = action_counts.get(action, 1)
            score = (1 / count) + self.ucb_c * random.random()
            scores.append(score)
        
        # Select action with highest score
        return actions[scores.index(max(scores))]
    
    def _noisy_exploration(self, default_command):
        """Apply noise-based exploration."""
        # Add noise to default command selection
        # In real implementation, this would use neural noise
        if random.random() < 0.1:  # 10% chance to explore
            return self._get_random_action()
        return default_command
    
    def _curiosity_exploration(self, default_command):
        """Apply curiosity-based exploration."""
        # Curiosity-based: explore when curiosity is high
        curiosity = self.get_curiosity_level()
        
        # Get novelty from agent's curiosity system
        # In real implementation, this would use actual novelty values
        novelty = curiosity * 0.5 + random.random() * 0.5
        
        if novelty > 0.7:  # High curiosity = high exploration
            return self._get_random_action()
        
        return default_command
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.applyRewardModulation(reward, predicted_reward)
        
        # Update statistics
        self.stats['rewards'] += reward
        self.state.set('last_reward', time.time())
        self.state.set('last_reward_value', reward)
    
    def update_development(self, timestep: float) -> None:
        """
        Update development system.
n        
        Args:
            timestep: Time step
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.updateDevelopment(timestep)
        
        # Update agent state
        self.state.set('developmental_stage', 
                      self.get_developmental_stage().value)
    
    # Information access methods
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._agent.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self._agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent.getPredictionError()
    
    def get_brain(self):
        """Get underlying brain object."""
        return self._brain
    
    def get_world(self):
        """Get world object."""
        return self._world
    
    # Configuration methods
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
        self._agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
        self._agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool) -> None:
        """Enable/disable development."""
        self._agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
        self._agent.enableCuriosity(enable)
    
    # Statistics methods
    def get_statistics(self) -> Dict[str, Any]:
        """Get agent statistics."""
        return self.stats.copy()
    
    def reset_statistics(self) -> None:
        """Reset agent statistics."""
        self.stats = {
            'steps': 0,
            'exploration_actions': 0,
            'exploitation_actions': 0,
            'rewards': 0.0,
            'novelty': 0.0,
            'curiosity': 0.0,
        }
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        self._agent.reset()
        self._is_initialized = False
        self.state.clear()
        
        # Reset goals
        for goal in self.goals:
            goal.completed = False
            goal.progress = 0.0
    
    # Behavior analysis methods
    def get_exploration_rate(self) -> float:
        """Get exploration rate."""
        if self.stats['steps'] == 0:
            return 0.0
        return self.stats['exploration_actions'] / self.stats['steps']
    
    def get_average_reward(self) -> float:
        """Get average reward."""
        if self.stats['steps'] == 0:
            return 0.0
        return self.stats['rewards'] / self.stats['steps']
    
    def get_behavior_summary(self) -> Dict[str, Any]:
        """Get behavior summary."""
        return {
            'exploration_strategy': self.exploration_strategy.value,
            'behavior_type': self.behavior_type.value,
            'exploration_rate': self.get_exploration_rate(),
            'average_reward': self.get_average_reward(),
            'goal_progress': self.get_average_goal_progress(),
            'completed_goals': len(self.get_completed_goals()),
            'total_goals': len(self.goals),
        }
    
    def __repr__(self) -> str:
        status = "initialized" if self.is_initialized() else "not initialized"
        return f"NLMAgentBrain(strategy={self.exploration_strategy.value}, status={status})"
# Alias for backward compatibility
AgentBrain = NLMAgentBrain


# Convenience constructors
def create_agent_brain(brain) -> NLMAgentBrain:
    """
    Create NLM agent brain.
n    
    Args:
        brain: NLM brain object
        
    Returns:
        NLMAgentBrain object
    """
    return NLMAgentBrain(brain)

# Type aliases for convenience
ExplorationStrategyType = ExplorationStrategy
BehaviorTypeType = BehaviorType
AgentStateType = AgentState
GoalType = Goal

# Export all main classes
__all__.extend([
    'ExplorationStrategy', 'BehaviorType', 'Goal', 'AgentState',
    'NLMAgentBrain', 'create_agent_brain',
    'ExplorationStrategyType', 'BehaviorTypeType', 'AgentStateType', 'GoalType'
])
