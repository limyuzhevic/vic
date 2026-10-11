"""
Enhanced agent brain module for NLM with advanced features.

This module provides enhanced agent functionality with better memory management,
neuromodulation, and interaction patterns for more intelligent behavior.
"""

import time
from typing import Optional, Dict, List, Any, Callable
from enum import Enum
from dataclasses import dataclass

from src.pynlm.enhanced_api import EnhancedBrain, EnhancedBrainConfig, NLMBrainError

class AgentBehavior(Enum):
    """Different agent behaviors."""
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    REFLECTIVE = "reflective"
    CURIOUS = "curious"

class LearningMode(Enum):
    """Different learning modes."""
    SUPERVISED = "supervised"
    REINFORCE = "reinforce"
    POLICY_GRADIENT = "policy_gradient"
    Q_LEARNING = "q_learning"
    EPISODIC = "episodic"

class ActionSelector(Enum):
    """Different action selection strategies."""
    GREEDY = "greedy"
    EPSILON_GREEDY = "epsilon_greedy"
    SOFTMAX = "softmax"
    UCB = "ucb"
    Thompson_Sampling = "thompson_sampling"

@dataclass
class AgentState:
    """Agent's internal state."""
    step: int
    position: tuple
    energy: float
    memory_utilization: float
    curiosity_level: float
    novelty_encountered: bool
    last_reward: float
    action_history: List[Dict[str, Any]]

@dataclass
class LearningMetrics:
    """Learning performance metrics."""
    exploration_rate: float
    exploitation_rate: float
    reward_average: float
    reward_variance: float
    learning_curve: List[float]
    convergence_step: int
    success_rate: float

class EnhancedAgentBrain:
    """
    Enhanced agent brain with intelligent behavior and learning capabilities.
    
    This class extends the basic agent functionality with advanced features:
    - Multiple learning algorithms
    - Intelligent action selection
    - Memory-based decision making
    - Curiosity-driven exploration
    - Performance monitoring
    """
    
    def __init__(self, brain: EnhancedBrain):
        """
        Initialize enhanced agent brain.
        
        Args:
            brain: Enhanced brain instance
        """
        self._brain = brain
        self._state = AgentState(
            step=0,
            position=(0.0, 0.0),
            energy=100.0,
            memory_utilization=0.0,
            curiosity_level=0.0,
            novelty_encountered=False,
            last_reward=0.0,
            action_history=[]
        )
        
        # Learning parameters
        self._behavior = AgentBehavior.BALANCED
        self._learning_mode = LearningMode.EPISODIC
        self._action_selector = ActionSelector.EPSILON_GREEDY
        
        # Learning algorithm parameters
        self._epsilon = 0.1  # Exploration rate
        self._temperature = 1.0  # For softmax selection
        self._alpha = 0.01  # Learning rate
        self._gamma = 0.99  # Discount factor
        self._tau = 0.005  # Eligibility trace decay
        
        # Q-learning components
        self._q_values: Dict[str, Dict[str, float]] = {}
        self._action_counts: Dict[str, int] = {}
        
        # Value function approximation
        self._value_function: Dict[str, float] = {}
        
        # Memory for decision making
        self._action_preferences: Dict[str, float] = {}
        self._context_memory: List[Dict[str, Any]] = []
        
        # Performance tracking
        self._learning_metrics = LearningMetrics(
            exploration_rate=0.0,
            exploitation_rate=0.0,
            reward_average=0.0,
            reward_variance=0.0,
            learning_curve=[],
            convergence_step=0,
            success_rate=0.0
        )
        
        # Enable agent subsystems
        self._enable_advanced_features()
    
    def _enable_advanced_features(self):
        """Enable advanced agent features."""
        # Ensure the underlying brain has the required subsystems
        brain = self._brain._brain
        
        # Check and enable subsystems
        if brain.getDopamine() is None:
            # Try to enable dopamine system
            pass
        
        if brain.getCuriosity() is None:
            # Try to enable curiosity system
            pass
        
        if brain.getPredictionSystem() is None:
            # Try to enable prediction system
            pass
        
        # Enable structural plasticity for agent development
        if brain.getDevelopmentSystem():
            brain.getDevelopmentSystem().setPlasticityRate(0.01)
    
    def process_sensory_input(self, percept: Any) -> Dict[str, Any]:
        """
        Process sensory input and update agent state.
        
        Args:
            percept: Sensory perception data
        
        Returns:
            Dictionary with processed sensory information
        """
        # Extract sensory features
        features = self._extract_sensory_features(percept)
        
        # Update agent state
        self._state.step += 1
        self._state.memory_utilization = min(1.0, self._state.memory_utilization + 0.01)
        
        # Update curiosity based on novelty
        novelty = self._calculate_novelty(features)
        self._state.curiosity_level = min(1.0, self._state.curiosity_level + novelty * 0.1)
        
        # Store context in memory
        context = {
            "step": self._state.step,
            "features": features,
            "position": self._state.position,
            "energy": self._state.energy,
            "curiosity": self._state.curiosity_level,
            "novelty": novelty
        }
        self._context_memory.append(context)
        
        # Limit context memory
        if len(self._context_memory) > 1000:
            self._context_memory = self._context_memory[-1000:]
        
        return {
            "features": features,
            "novelty": novelty,
            "curiosity": self._state.curiosity_level,
            "step": self._state.step
        }
    
    def decide_action(self, state: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Decide action based on current state and learned policies.
        
        Args:
            state: Current state (auto-computed if None)
        
        Returns:
            Dictionary with selected action and metadata
        """
        if state is None:
            state = self._get_current_state()
        
        # Select action based on current strategy
        action_type, action_params = self._select_action(state)
        
        # Record action in history
        action_record = {
            "step": self._state.step,
            "action_type": action_type,
            "action_params": action_params,
            "state_features": state.get("features", {}),
            "curiosity": self._state.curiosity_level,
            "energy": self._state.energy
        }
        self._state.action_history.append(action_record)
        
        # Update action preferences
        self._update_action_preferences(action_type, state)
        
        return {
            "action_type": action_type,
            "action_params": action_params,
            "confidence": self._calculate_action_confidence(state, action_type),
            "exploration": self._state.curiosity_level > 0.5,
            "timestamp": time.time()
        }
    
    def learn_from_experience(self, reward: float, next_state: Dict[str, Any],
                            action: str, done: bool = False) -> Dict[str, Any]:
        """
        Learn from experience using selected learning algorithm.
        
        Args:
            reward: Reward received
            next_state: Next state
            action: Action taken
            done: Whether episode is complete
        
        Returns:
            Dictionary with learning metrics
        """
        # Update state
        self._state.last_reward = reward
        
        # Apply learning based on selected mode
        if self._learning_mode == LearningMode.SUPERVISED:
            metrics = self._supervised_learning(reward, next_state)
        elif self._learning_mode == LearningMode.REINFORCE:
            metrics = self._reinforce_learning(reward, next_state, action)
        elif self._learning_mode == LearningMode.POLICY_GRADIENT:
            metrics = self._policy_gradient_learning(reward, next_state, action)
        elif self._learning_mode == LearningMode.Q_LEARNING:
            metrics = self._q_learning(reward, next_state, action, done)
        elif self._learning_mode == LearningMode.EPISODIC:
            metrics = self._episodic_learning(reward, next_state, action)
        
        # Update learning metrics
        self._update_learning_metrics(metrics)
        
        return metrics
    
    def get_brain(self) -> EnhancedBrain:
        """Get the underlying enhanced brain."""
        return self._brain
    
    def get_action_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get action history.
        
        Args:
            limit: Maximum number of actions to return
        
        Returns:
            List of action records
        """
        if limit is None:
            return self._state.action_history.copy()
        else:
            return self._state.action_history[-limit:]
    
    def get_context_memory(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get context memory.
        
        Args:
            limit: Maximum number of context entries to return
        
        Returns:
            List of context entries
        """
        if limit is None:
            return self._context_memory.copy()
        else:
            return self._context_memory[-limit:]
    
    def get_learning_metrics(self) -> Dict[str, Any]:
        """Get learning performance metrics."""
        return {
            "behavior": self._behavior.value,
            "learning_mode": self._learning_mode.value,
            "action_selector": self._action_selector.value,
            "exploration_rate": self._learning_metrics.exploration_rate,
            "exploitation_rate": self._learning_metrics.exploitation_rate,
            "reward_average": self._learning_metrics.reward_average,
            "reward_variance": self._learning_metrics.reward_variance,
            "learning_curve": self._learning_metrics.learning_curve,
            "convergence_step": self._learning_metrics.convergence_step,
            "success_rate": self._learning_metrics.success_rate,
            "epsilon": self._epsilon,
            "temperature": self._temperature,
            "alpha": self._alpha,
            "gamma": self._gamma
        }
    
    def set_behavior(self, behavior: AgentBehavior):
        """Set agent behavior."""
        self._behavior = behavior
        return self
    
    def set_learning_mode(self, mode: LearningMode):
        """Set learning mode."""
        self._learning_mode = mode
        return self
    
    def set_action_selector(self, selector: ActionSelector):
        """Set action selection strategy."""
        self._action_selector = selector
        return self
    
    def set_exploration_rate(self, epsilon: float):
        """Set exploration rate (epsilon)."""
        self._epsilon = max(0.0, min(1.0, epsilon))
        return self
    
    def set_learning_rate(self, alpha: float):
        """Set learning rate."""
        self._alpha = max(0.0, min(1.0, alpha))
        return self
    
    def set_discount_factor(self, gamma: float):
        """Set discount factor."""
        self._gamma = max(0.0, min(1.0, gamma))
        return self
    
    def reset(self):
        """Reset agent state."""
        self._state = AgentState(
            step=0,
            position=(0.0, 0.0),
            energy=100.0,
            memory_utilization=0.0,
            curiosity_level=0.0,
            novelty_encountered=False,
            last_reward=0.0,
            action_history=[]
        )
        
        # Reset learning components
        self._q_values.clear()
        self._action_counts.clear()
        self._value_function.clear()
        self._action_preferences.clear()
        self._context_memory.clear()
        
        self._learning_metrics = LearningMetrics(
            exploration_rate=0.0,
            exploitation_rate=0.0,
            reward_average=0.0,
            reward_variance=0.0,
            learning_curve=[],
            convergence_step=0,
            success_rate=0.0
        )
        
        return self
    
    # === Private Methods ===
    
    def _extract_sensory_features(self, percept: Any) -> Dict[str, Any]:
        """Extract features from sensory perception."""
        features = {}
        
        # Handle different percept types
        if hasattr(percept, 'getInternal'):
            internal = percept.getInternal()
            if internal:
                features.update(internal)
        
        if hasattr(percept, 'getVisual'):
            visual = percept.getVisual()
            if visual:
                features['visual'] = visual
        
        if hasattr(percept, 'getAudio'):
            audio = percept.getAudio()
            if audio:
                features['audio'] = audio
        
        # Add position-based features
        x, y = self._state.position
        features['position_x'] = x
        features['position_y'] = y
        features['distance_from_center'] = ((x - 10.0) ** 2 + (y - 10.0) ** 2) ** 0.5
        
        # Add energy features
        features['energy_normalized'] = self._state.energy / 100.0
        features['energy_above_threshold'] = self._state.energy > 50.0
        
        return features
    
    def _calculate_novelty(self, features: Dict[str, Any]) -> float:
        """Calculate novelty of sensory input."""
        # Simple novelty calculation based on feature variance
        # In practice, this would use more sophisticated novelty detection
        if not self._context_memory:
            return 1.0
        
        # Compare with recent context
        recent_contexts = self._context_memory[-10:]
        if not recent_contexts:
            return 1.0
        
        # Calculate average feature values
        avg_features = {}
        for ctx in recent_contexts:
            for key, value in ctx['features'].items():
                if key in avg_features:
                    avg_features[key] += value
                else:
                    avg_features[key] = value
        
        for key in avg_features:
            avg_features[key] /= len(recent_contexts)
        
        # Calculate feature difference
        difference = 0.0
        for key, value in features.items():
            if key in avg_features:
                difference += abs(value - avg_features[key])
        
        # Normalize to [0, 1]
        return min(1.0, difference / max(1.0, len(features)))
    
    def _get_current_state(self) -> Dict[str, Any]:
        """Get current agent state as features."""
        state = {
            "step": self._state.step,
            "position_x": self._state.position[0],
            "position_y": self._state.position[1],
            "energy": self._state.energy,
            "curiosity": self._state.curiosity_level,
            "memory_utilization": self._state.memory_utilization,
            "last_reward": self._state.last_reward,
            "novelty_encountered": self._state.novelty_encountered
        }
        
        # Add brain statistics
        brain_stats = self._brain.get_statistics()
        state.update({
            "brain_neurons": brain_stats['total_neurons'],
            "brain_firing_rate": brain_stats['average_firing_rate'],
            "brain_memory_traces": brain_stats['memory_traces'],
            "brain_developmental_stage": brain_stats['development_stage']
        })
        
        return state
    
    def _select_action(self, state: Dict[str, Any]) -> tuple:
        """Select action based on current strategy."""
        if self._action_selector == ActionSelector.GREEDY:
            return self._greedy_action_selection(state)
        elif self._action_selector == ActionSelector.EPSILON_GREEDY:
            return self._epsilon_greedy_action_selection(state)
        elif self._action_selector == ActionSelector.SOFTMAX:
            return self._softmax_action_selection(state)
        elif self._action_selector == ActionSelector.UCB:
            return self._ucb_action_selection(state)
        elif self._action_selector == ActionSelector.Thompson_Sampling:
            return self._thompson_sampling_action_selection(state)
        else:
            return self._greedy_action_selection(state)
    
    def _greedy_action_selection(self, state: Dict[str, Any]) -> tuple:
        """Greedy action selection based on current preferences."""
        action_type = max(self._action_preferences.items(), 
                         key=lambda x: x[1], default=("MOVE_FORWARD", 0.0))[0]
        
        # Convert action type to parameters
        if action_type == "MOVE_FORWARD":
            params = {"distance": 1.0}
        elif action_type == "TURN_LEFT":
            params = {"angle": 45.0}
        elif action_type == "TURN_RIGHT":
            params = {"angle": 45.0}
        elif action_type == "INTERACT":
            params = {"object": "nearest"}
        elif action_type == "EAT":
            params = {"amount": 10.0}
        else:
            params = {}
        
        return action_type, params
    
    def _epsilon_greedy_action_selection(self, state: Dict[str, Any]) -> tuple:
        """Epsilon-greedy action selection."""
        import random
        
        if random.random() < self._epsilon:
            # Explore: random action
            actions = ["MOVE_FORWARD", "TURN_LEFT", "TURN_RIGHT", "INTERACT", "EAT"]
            action_type = random.choice(actions)
        else:
            # Exploit: greedy action
            action_type = max(self._action_preferences.items(),
                            key=lambda x: x[1], default=("MOVE_FORWARD", 0.0))[0]
        
        # Convert action type to parameters
        params = self._get_action_parameters(action_type, state)
        
        return action_type, params
    
    def _softmax_action_selection(self, state: Dict[str, Any]) -> tuple:
        """Softmax action selection."""
        import math
        
        # Get preferences (with exploration bonus)
        preferences = dict(self._action_preferences)
        for action in preferences:
            # Add curiosity bonus
            preferences[action] *= (1.0 + self._state.curiosity_level)
        
        # Convert to probabilities using softmax
        max_pref = max(preferences.values()) if preferences else 0.0
        exp_preferences = {action: math.exp((pref - max_pref) / self._temperature) 
                          for action, pref in preferences.items()}
        
        total_exp = sum(exp_preferences.values())
        if total_exp > 0:
            probabilities = {action: exp_pref / total_exp 
                           for action, exp_pref in exp_preferences.items()}
        else:
            probabilities = {action: 1.0 / len(preferences) for action in preferences}
        
        # Select action based on probabilities
        action_type = self._weighted_choice(list(probabilities.keys()), 
                                          list(probabilities.values()))
        
        params = self._get_action_parameters(action_type, state)
        
        return action_type, params
    
    def _ucb_action_selection(self, state: Dict[str, Any]) -> tuple:
        """Upper Confidence Bound action selection."""
        # Get action statistics
        import math
        
        ucb_values = {}
        for action, preference in self._action_preferences.items():
            count = self._action_counts.get(action, 1)
            ucb_value = preference + self._temperature * math.sqrt(math.log(10) / count)
            ucb_values[action] = ucb_value
        
        if ucb_values:
            action_type = max(ucb_values.items(), key=lambda x: x[1])[0]
        else:
            action_type = "MOVE_FORWARD"
        
        params = self._get_action_parameters(action_type, state)
        
        return action_type, params
    
    def _thompson_sampling_action_selection(self, state: Dict[str, Any]) -> tuple:
        """Thompson sampling action selection."""
        # Sample from posterior distributions
        action_type = self._weighted_choice(
            list(self._action_preferences.keys()),
            [max(0.0, pref) for pref in self._action_preferences.values()]
        )
        
        params = self._get_action_parameters(action_type, state)
        
        return action_type, params
    
    def _get_action_parameters(self, action_type: str, state: Dict[str, Any]) -> Dict[str, Any]:
        """Get action parameters based on action type and state."""
        import random
        
        if action_type == "MOVE_FORWARD":
            # Adjust distance based on energy and obstacles
            energy_factor = min(1.0, self._state.energy / 50.0)
            distance = 0.5 + energy_factor * 2.0
            return {"distance": distance}
        
        elif action_type == "TURN_LEFT":
            return {"angle": 30.0 + random.uniform(-10.0, 10.0)}
        
        elif action_type == "TURN_RIGHT":
            return {"angle": 30.0 + random.uniform(-10.0, 10.0)}
        
        elif action_type == "INTERACT":
            # Try to find something to interact with
            return {"object": "nearest"}
        
        elif action_type == "EAT":
            # Adjust amount based on energy needs
            energy_need = max(0.0, 50.0 - self._state.energy)
            amount = min(energy_need, 20.0)
            return {"amount": amount}
        
        else:
            return {}
    
    def _weighted_choice(self, choices: List[str], weights: List[float]) -> str:
        """Weighted random choice."""
        import random
        
        total_weight = sum(weights)
        if total_weight <= 0:
            return random.choice(choices)
        
        rand_val = random.uniform(0, total_weight)
        cumulative = 0.0
        
        for choice, weight in zip(choices, weights):
            cumulative += weight
            if rand_val <= cumulative:
                return choice
        
        return choices[-1]  # Fallback to last choice
    
    def _calculate_action_confidence(self, state: Dict[str, Any], action: str) -> float:
        """Calculate confidence in selected action."""
        preference = self._action_preferences.get(action, 0.0)
        
        # Normalize to [0, 1]
        max_pref = max(self._action_preferences.values(), default=1.0)
        confidence = preference / max_pref if max_pref > 0 else 0.0
        
        # Adjust based on curiosity
        confidence *= (1.0 + self._state.curiosity_level * 0.2)
        
        return min(1.0, confidence)
    
    def _update_action_preferences(self, action: str, state: Dict[str, Any]):
        """Update action preferences based on experience."""
        # Simple preference update
        current_pref = self._action_preferences.get(action, 0.0)
        
        # Add exploration bonus
        curiosity_bonus = self._state.curiosity_level * 0.5
        
        new_pref = current_pref + self._alpha * curiosity_bonus
        self._action_preferences[action] = min(10.0, new_pref)
        
        # Track action counts
        self._action_counts[action] = self._action_counts.get(action, 0) + 1
    
    # === Learning Algorithm Implementations ===
    
    def _supervised_learning(self, reward: float, next_state: Dict[str, Any]) -> Dict[str, Any]:
        """Supervised learning implementation."""
        # Update value function
        state_key = str(next_state)
        self._value_function[state_key] = (1.0 - self._alpha) * self._value_function.get(state_key, 0.0) + \
                                        self._alpha * reward
        
        return {"type": "supervised", "reward": reward}
    
    def _reinforce_learning(self, reward: float, next_state: Dict[str, Any], action: str) -> Dict[str, Any]:
        """REINFORCE (Policy Gradient) learning implementation."""
        # Simplified REINFORCE
        state_key = str(next_state)
        advantage = reward - self._value_function.get(state_key, 0.0)
        
        # Update action preferences
        self._action_preferences[action] += self._alpha * advantage
        
        return {"type": "reinforce", "reward": reward, "advantage": advantage}
    
    def _policy_gradient_learning(self, reward: float, next_state: Dict[str, Any], action: str) -> Dict[str, Any]:
        """Policy gradient learning implementation."""
        # Simplified policy gradient
        state_key = str(next_state)
        policy_gradient = reward - self._value_function.get(state_key, 0.0)
        
        self._action_preferences[action] += self._alpha * policy_gradient * self._temperature
        
        return {"type": "policy_gradient", "reward": reward, "gradient": policy_gradient}
    
    def _q_learning(self, reward: float, next_state: Dict[str, Any], action: str, done: bool) -> Dict[str, Any]:
        """Q-learning implementation."""
        # Simplified Q-learning
        next_state_key = str(next_state)
        current_state_key = str(self._get_current_state())
        
        # Get current Q-value
        current_q = self._q_values.get(current_state_key, {}).get(action, 0.0)
        
        # Calculate target Q-value
        max_next_q = max(self._q_values.get(next_state_key, {}).values(), default=0.0) if not done else 0.0
        target_q = reward + self._gamma * max_next_q
        
        # Update Q-value
        self._q_values.setdefault(current_state_key, {})[action] = \
            current_q + self._alpha * (target_q - current_q)
        
        return {"type": "q_learning", "reward": reward, "target_q": target_q, "current_q": current_q}
    
    def _episodic_learning(self, reward: float, next_state: Dict[str, Any], action: str) -> Dict[str, Any]:
        """Episodic memory learning implementation."""
        # Store episode memory
        episode_data = {
            "state": self._get_current_state(),
            "action": action,
            "reward": reward,
            "next_state": next_state,
            "step": self._state.step
        }
        
        # Update episodic memory
        if "episodes" not in self._brain._brain._episodic_memory:
            self._brain._brain._episodic_memory["episodes"] = []
        
        self._brain._brain._episodic_memory["episodes"].append(episode_data)
        
        # Limit episode memory size
        if len(self._brain._brain._episodic_memory["episodes"]) > 1000:
            self._brain._brain._episodic_memory["episodes"] = \
                self._brain._brain._episodic_memory["episodes"][-1000:]
        
        # Update action preferences based on episode
        self._update_action_preferences(action, next_state)
        
        return {"type": "episodic", "reward": reward, "episode_stored": True}
    
    def _update_learning_metrics(self, learning_result: Dict[str, Any]):
        """Update learning performance metrics."""
        # Update exploration/exploitation rates
        if self._behavior == AgentBehavior.EXPLORATION:
            self._learning_metrics.exploration_rate = 1.0
            self._learning_metrics.exploitation_rate = 0.0
        elif self._behavior == AgentBehavior.EXPLOITATION:
            self._learning_metrics.exploration_rate = 0.0
            self._learning_metrics.exploitation_rate = 1.0
        else:
            # Balanced based on epsilon
            self._learning_metrics.exploration_rate = self._epsilon
            self._learning_metrics.exploitation_rate = 1.0 - self._epsilon
        
        # Update reward statistics
        reward = learning_result.get("reward", 0.0)
        self._learning_metrics.reward_average = \
            (self._learning_metrics.reward_average * len(self._learning_metrics.learning_curve) + reward) / \
            (len(self._learning_metrics.learning_curve) + 1)
        
        self._learning_metrics.reward_variance = \
            (self._learning_metrics.reward_variance * (len(self._learning_metrics.learning_curve) - 1) + \
             (reward - self._learning_metrics.reward_average) ** 2) / len(self._learning_metrics.learning_curve)
        
        # Add to learning curve
        self._learning_metrics.learning_curve.append(reward)
        
        # Limit learning curve size
        if len(self._learning_metrics.learning_curve) > 1000:
            self._learning_metrics.learning_curve = self._learning_metrics.learning_curve[-1000:]
        
        # Update success rate
        if reward > 0.0:
            self._learning_metrics.success_rate = \
                (self._learning_metrics.success_rate * len(self._learning_metrics.learning_curve) + 1.0) / \
                (len(self._learning_metrics.learning_curve))

# === Enhanced World Module ===

class EnhancedSimpleWorld:
    """
    Enhanced simple world simulation with advanced features.
    
    This class extends the basic simple world with:
    - Advanced environment dynamics
    - Enhanced agent-world interaction
    - Performance monitoring
    - Visualization support
    - Simulation control
    """
    
    def __init__(self):
        """Initialize enhanced simple world."""
        self._width = 20
        self._height = 20
        self._vision_width = 8
        self._vision_height = 8
        
        # World state
        self._agent_position = (10.0, 10.0)
        self._agent_angle = 0.0
        self._simulation_time = 0.0
        self._max_energy = 100.0
        
        # Environment objects
        self._world_objects: Dict[str, Any] = {}
        self._resources = {}
        self._hazards = {}
        
        # Simulation state
        self._paused = False
        self._random_seed = 42
        
        # Performance monitoring
        self._simulation_stats = {
            "steps": 0,
            "agent_moves": 0,
            "objects_interacted": 0,
            "resources_collected": 0,
            "hazards_avoided": 0,
            "total_simulation_time": 0.0
        }
        
        # Events and callbacks
        self._event_handlers: Dict[str, List[Callable]] = {}
        
        # Initialize default objects
        self._initialize_world()
    
    def configure(self, width: int = 20, height: int = 20, 
                  vision_width: int = 8, vision_height: int = 8):
        """
        Configure world dimensions and vision.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
        # Adjust agent position to stay within bounds
        self._agent_position = (
            min(self._agent_position[0], width * 0.8),
            min(self._agent_position[1], height * 0.8)
        )
        
        return self
    
    def reset(self):
        """Reset world to initial state."""
        self._agent_position = (10.0, 10.0)
        self._agent_angle = 0.0
        self._simulation_time = 0.0
        self._simulation_stats = {
            "steps": 0,
            "agent_moves": 0,
            "objects_interacted": 0,
            "resources_collected": 0,
            "hazards_avoided": 0,
            "total_simulation_time": 0.0
        }
        
        # Clear temporary objects
        self._world_objects.clear()
        
        return self
    
    def update(self, timestep: float = 0.1):
        """
        Update world simulation.
        
        Args:
            timestep: Time step for update
        """
        if self._paused:
            return
        
        # Update simulation time
        self._simulation_time += timestep
        self._simulation_stats["steps"] += 1
        
        # Apply environmental effects
        self._apply_environmental_effects(timestep)
        
        # Update agent energy (natural decay)
        self._update_agent_energy(timestep)
        
        # Update resources
        self._update_resources(timestep)
        
        # Trigger events
        self._trigger_events("step_update")
    
    def get_sensory_percept(self):
        """
        Get sensory perception from current world state.
        
        Returns:
            EnhancedSensoryPercept with processed sensory information
        """
        # Extract visual information
        visual_percept = self._get_visual_percept()
        
        # Extract internal signals
        internal_percept = self._get_internal_percept()
        
        # Combine perceptions
        return EnhancedSensoryPercept(visual_percept, internal_percept)
    
    def apply_motor_command(self, motor_cmd: Any, current_time: float):
        """
        Apply motor command to world.
        
        Args:
            motor_cmd: Motor command to execute
            current_time: Current simulation time
        """
        if self._paused:
            return
        
        # Extract action from command
        action_type = getattr(motor_cmd, 'getActionType', lambda: 'MOVE_FORWARD')()
        action_params = self._extract_motor_params(motor_cmd)
        
        # Execute action
        success = self._execute_action(action_type, action_params)
        
        if success:
            self._simulation_stats["agent_moves"] += 1
            self._trigger_events("action_executed", action_type, action_params)
        
        # Update energy based on action
        self._update_energy_for_action(action_type, action_params)
    
    def add_object(self, obj_type: str, x: float, y: float, value: float = 1.0):
        """
        Add object to world.
        
        Args:
            obj_type: Type of object ('resource', 'hazard', 'marker')
            x: X position
            y: Y position
            value: Object value (for resources)
        """
        if obj_type == "resource":
            self._resources[(x, y)] = value
            self._trigger_events("resource_added", x, y, value)
        elif obj_type == "hazard":
            self._hazards[(x, y)] = value
            self._trigger_events("hazard_added", x, y, value)
        elif obj_type == "marker":
            self._world_objects[(x, y)] = {"type": "marker", "value": value}
            self._trigger_events("marker_added", x, y, value)
    
    def remove_object(self, x: float, y: float):
        """Remove object from world."""
        # Remove from all object types
        self._resources.pop((x, y), None)
        self._hazards.pop((x, y), None)
        self._world_objects.pop((x, y), None)
        
        self._trigger_events("object_removed", x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return (0.0 <= x < self._width and 0.0 <= y < self._height and
                not self._is_hazard(x, y))
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        if self.is_valid_position(x, y):
            self._agent_position = (x, y)
        else:
            raise ValueError(f"Invalid starting position: ({x}, {y})")
        
        return self
    
    def get_width(self) -> float:
        """Get world width."""
        return self._width
    
    def get_height(self) -> float:
        """Get world height."""
        return self._height
    
    def get_agent_position(self) -> tuple:
        """Get agent position."""
        return self._agent_position
    
    def get_agent_angle(self) -> float:
        """Get agent angle."""
        return self._agent_angle
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self._simulation_time
    
    def pause(self):
        """Pause world simulation."""
        self._paused = True
        self._trigger_events("world_paused")
        return self
    
    def resume(self):
        """Resume world simulation."""
        self._paused = False
        self._trigger_events("world_resumed")
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world simulation statistics."""
        return self._simulation_stats.copy()
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """
        Add event handler.
        
        Args:
            event_type: Type of event
            handler: Handler function
        """
        if event_type not in self._event_handlers:
            self._event_handlers[event_type] = []
        
        self._event_handlers[event_type].append(handler)
    
    def remove_event_handler(self, event_type: str, handler: Callable):
        """Remove event handler."""
        if event_type in self._event_handlers:
            try:
                self._event_handlers[event_type].remove(handler)
            except ValueError:
                pass
    
    # === Private Methods ===
    
    def _initialize_world(self):
        """Initialize world with default objects."""
        # Add some default resources
        for x in [2, 8, 15]:
            for y in [2, 8, 15]:
                if (x, y) != (10, 10):  # Not at agent start
                    self.add_object("resource", float(x), float(y), 10.0)
        
        # Add some hazards
        for x in [5, 12]:
            for y in [5, 12]:
                self.add_object("hazard", float(x), float(y), 5.0)
        
        # Add markers
        self.add_object("marker", 3.0, 3.0, 100.0)
        self.add_object("marker", 17.0, 17.0, 100.0)
    
    def _get_visual_percept(self) -> Dict[str, Any]:
        """Get visual perception from agent's perspective."""
        # Agent's field of view
        vision_center_x = self._agent_position[0]
        vision_center_y = self._agent_position[1]
        
        # Create vision grid
        vision_data = {
            "width": self._vision_width,
            "height": self._vision_height,
            "center_x": vision_center_x,
            "center_y": vision_center_y,
            "angle": self._agent_angle,
            "cells": []
        }
        
        # Populate vision grid
        cell_size_x = self._width / self._vision_width
        cell_size_y = self._height / self._vision_height
        
        for i in range(self._vision_width):
            for j in range(self._vision_height):
                cell_x = vision_center_x + (i - self._vision_width // 2) * cell_size_x
                cell_y = vision_center_y + (j - self._vision_height // 2) * cell_size_y
                
                cell_content = self._get_cell_content(cell_x, cell_y)
                vision_data["cells"].append({
                    "x": cell_x,
                    "y": cell_y,
                    "content": cell_content,
                    "distance": ((cell_x - vision_center_x) ** 2 + 
                                (cell_y - vision_center_y) ** 2) ** 0.5
                })
        
        return vision_data
    
    def _get_internal_percept(self) -> Dict[str, Any]:
        """Get internal perception (agent's internal state)."""
        return {
            "energy": self._state.energy,
            "curiosity": self._state.curiosity_level,
            "position": self._agent_position,
            "angle": self._agent_angle,
            "step": self._simulation_stats["steps"]
        }
    
    def _get_cell_content(self, x: float, y: float) -> Dict[str, Any]:
        """Get content of a vision cell."""
        content = {"type": "empty", "value": 0.0}
        
        # Check for hazards first
        for hazard_pos, hazard_value in self._hazards.items():
            if abs(hazard_pos[0] - x) < 0.5 and abs(hazard_pos[1] - y) < 0.5:
                content = {"type": "hazard", "value": hazard_value}
                break
        
        # Check for resources
        for resource_pos, resource_value in self._resources.items():
            if abs(resource_pos[0] - x) < 0.5 and abs(resource_pos[1] - y) < 0.5:
                content = {"type": "resource", "value": resource_value}
                break
        
        # Check for markers
        for marker_pos, marker_data in self._world_objects.items():
            if abs(marker_pos[0] - x) < 0.5 and abs(marker_pos[1] - y) < 0.5:
                content = {"type": "marker", "value": marker_data["value"]}
                break
        
        return content
    
    def _is_hazard(self, x: float, y: float) -> bool:
        """Check if position contains hazard."""
        for hazard_pos in self._hazards.keys():
            if abs(hazard_pos[0] - x) < 0.5 and abs(hazard_pos[1] - y) < 0.5:
                return True
        return False
    
    def _apply_environmental_effects(self, timestep: float):
        """Apply environmental effects."""
        # Natural resource regeneration
        for pos in list(self._resources.keys()):
            if self._simulation_time % 10.0 < timestep:  # Every 10 seconds
                x, y = pos
                if self._can_regenerate(x, y):
                    self._resources[pos] = min(10.0, self._resources[pos] + 0.5)
        
        # Hazard spreading (very slow)
        if self._simulation_time % 50.0 < timestep:
            self._spread_hazards()
    
    def _can_regenerate(self, x: float, y: float) -> bool:
        """Check if resource can regenerate."""
        # Resources near agent recover slower
        dist_from_agent = ((x - self._agent_position[0]) ** 2 + 
                          (y - self._agent_position[1]) ** 2) ** 0.5
        return dist_from_agent > 5.0
    
    def _spread_hazards(self):
        """Spread hazards to nearby cells."""
        for hazard_pos in list(self._hazards.keys()):
            x, y = hazard_pos
            # Spread to adjacent cells (rarely)
            if abs(self._simulation_time) % 2.0 < 0.1:  # 10% chance
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        if dx == 0 and dy == 0:
                            continue
                        new_x = x + dx
                        new_y = y + dy
                        if self.is_valid_position(new_x, new_y):
                            self.add_object("hazard", new_x, new_y, 2.0)
                            break
    
    def _update_agent_energy(self, timestep: float):
        """Update agent energy based on activity."""
        # Natural energy decay
        energy_decay = timestep * 2.0  # Lose 2 energy per second
        self._state.energy = max(0.0, self._state.energy - energy_decay)
        
        # Energy gain from nearby resources
        self._gain_energy_from_resources()
    
    def _gain_energy_from_resources(self):
        """Gain energy from nearby resources."""
        x, y = self._agent_position
        
        # Check resources in 3x3 area
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                resource_pos = (x + dx, y + dy)
                if resource_pos in self._resources:
                    # Gain energy proportional to resource value
                    energy_gain = self._resources[resource_pos] * 0.5
                    self._state.energy = min(self._max_energy, 
                                           self._state.energy + energy_gain)
                    
                    # Remove consumed resource
                    del self._resources[resource_pos]
                    self._simulation_stats["resources_collected"] += 1
                    
                    self._trigger_events("resource_collected", resource_pos[0], resource_pos[1])
    
    def _update_resources(self, timestep: float):
        """Update resource state."""
        # Simple resource dynamics
        pass
    
    def _execute_action(self, action_type: str, params: Dict[str, Any]) -> bool:
        """Execute action and update state."""
        import math
        
        if action_type == "MOVE_FORWARD":
            distance = params.get("distance", 1.0)
            angle_rad = math.radians(self._agent_angle)
            new_x = self._agent_position[0] + distance * math.cos(angle_rad)
            new_y = self._agent_position[1] + distance * math.sin(angle_rad)
            
            if self.is_valid_position(new_x, new_y):
                self._agent_position = (new_x, new_y)
                return True
        
        elif action_type == "TURN_LEFT":
            angle = params.get("angle", 45.0)
            self._agent_angle = (self._agent_angle - angle) % 360.0
            return True
        
        elif action_type == "TURN_RIGHT":
            angle = params.get("angle", 45.0)
            self._agent_angle = (self._agent_angle + angle) % 360.0
            return True
        
        elif action_type == "INTERACT":
            # Interact with nearby object
            return self._interact_with_nearby()
        
        elif action_type == "EAT":
            # Eat to gain energy
            amount = params.get("amount", 10.0)
            energy_gain = min(amount, self._max_energy - self._state.energy)
            self._state.energy = min(self._max_energy, self._state.energy + energy_gain)
            return True
        
        return False
    
    def _interact_with_nearby(self) -> bool:
        """Interact with nearby object."""
        x, y = self._agent_position
        
        # Check for nearby resources
        for resource_pos, resource_value in self._resources.items():
            dist = ((resource_pos[0] - x) ** 2 + (resource_pos[1] - y) ** 2) ** 0.5
            if dist < 1.0:
                # Collect resource
                self._state.energy = min(self._max_energy, 
                                       self._state.energy + resource_value * 0.5)
                del self._resources[resource_pos]
                self._simulation_stats["resources_collected"] += 1
                self._trigger_events("resource_collected", resource_pos[0], resource_pos[1])
                return True
        
        # Check for hazards
        for hazard_pos in self._hazards.keys():
            dist = ((hazard_pos[0] - x) ** 2 + (hazard_pos[1] - y) ** 2) ** 0.5
            if dist < 1.0:
                # Hazard damage
                self._state.energy = max(0.0, self._state.energy - 10.0)
                self._simulation_stats["hazards_avoided"] += 1
                self._trigger_events("hazard_avoided", hazard_pos[0], hazard_pos[1])
                return True
        
        return False
    
    def _update_energy_for_action(self, action_type: str, params: Dict[str, Any]):
        """Update energy cost for action."""
        # Energy costs for different actions
        energy_costs = {
            "MOVE_FORWARD": 1.0,
            "TURN_LEFT": 0.5,
            "TURN_RIGHT": 0.5,
            "INTERACT": 2.0,
            "EAT": 0.1
        }
        
        cost = energy_costs.get(action_type, 1.0) * params.get("multiplier", 1.0)
        self._state.energy = max(0.0, self._state.energy - cost)
    
    def _extract_motor_params(self, motor_cmd: Any) -> Dict[str, Any]:
        """Extract parameters from motor command."""
        params = {}
        
        if hasattr(motor_cmd, 'getDistance'):
            params["distance"] = motor_cmd.getDistance()
        
        if hasattr(motor_cmd, 'getAngle'):
            params["angle"] = motor_cmd.getAngle()
        
        if hasattr(motor_cmd, 'getActionType'):
            params["action_type"] = motor_cmd.getActionType()
        
        return params
    
    def _trigger_events(self, event_type: str, *args):
        """Trigger event handlers."""
        if event_type in self._event_handlers:
            for handler in self._event_handlers[event_type]:
                try:
                    handler(event_type, *args)
                except Exception as e:
                    print(f"Error in event handler for {event_type}: {e}")

# === Enhanced Sensory Percept ===

class EnhancedSensoryPercept:
    """
    Enhanced sensory perception with processed features.
    
    This class wraps the basic sensory percept with additional features
    extracted and processed for enhanced agent behavior.
    """
    
    def __init__(self, visual_percept: Dict[str, Any], internal_percept: Dict[str, Any]):
        """
        Initialize enhanced sensory perception.
        
        Args:
            visual_percept: Visual perception data
            internal_percept: Internal perception data
        """
        self._visual = visual_percept
        self._internal = internal_percept
        self._timestamp = time.time()
        
        # Process and enhance features
        self._features = self._extract_features()
    
    def _extract_features(self) -> Dict[str, Any]:
        """Extract enhanced features from perceptions."""
        features = {}
        
        # Visual features
        if self._visual:
            features.update(self._process_visual_features())
        
        # Internal features
        if self._internal:
            features.update(self._process_internal_features())
        
        # Add contextual features
        features.update(self._process_contextual_features())
        
        return features
    
    def _process_visual_features(self) -> Dict[str, Any]:
        """Process visual features."""
        features = {}
        
        # Cell statistics
        cells = self._visual.get("cells", [])
        if cells:
            resource_count = sum(1 for cell in cells if cell["content"]["type"] == "resource")
            hazard_count = sum(1 for cell in cells if cell["content"]["type"] == "hazard")
            marker_count = sum(1 for cell in cells if cell["content"]["type"] == "marker")
            
            features["visual_resource_count"] = resource_count
            features["visual_hazard_count"] = hazard_count
            features["visual_marker_count"] = marker_count
            
            # Proximity features
            nearest_resource = min((cell["distance"] for cell in cells 
                                 if cell["content"]["type"] == "resource"),
                                default=float('inf'))
            nearest_hazard = min((cell["distance"] for cell in cells 
                               if cell["content"]["type"] == "hazard"),
                              default=float('inf'))
            
            features["nearest_resource_distance"] = nearest_resource
            features["nearest_hazard_distance"] = nearest_hazard
        
        return features
    
    def _process_internal_features(self) -> Dict[str, Any]:
        """Process internal features."""
        return self._internal.copy() if self._internal else {}
    
    def _process_contextual_features(self) -> Dict[str, Any]:
        """Process contextual features."""
        features = {}
        
        # Time-based features
        current_time = time.time()
        features["time_since_last_step"] = current_time - self._timestamp
        
        # Add simulation stats if available
        # This would be populated by the world simulation
        
        return features
    
    def get_internal(self) -> Dict[str, Any]:
        """Get internal perception."""
        return self._internal.copy() if self._internal else {}
    
    def get_visual(self) -> Dict[str, Any]:
        """Get visual perception."""
        return self._visual.copy() if self._visual else {}
    
    def get_features(self) -> Dict[str, Any]:
        """Get processed features."""
        return self._features.copy()
    
    def get_feature(self, key: str, default: Any = None) -> Any:
        """Get specific feature."""
        return self._features.get(key, default)

# === Factory Functions ===

def create_enhanced_agent(brain: EnhancedBrain) -> EnhancedAgentBrain:
    """
    Create enhanced agent brain.
    
    Args:
        brain: Enhanced brain instance
    
    Returns:
        EnhancedAgentBrain instance
    """
    return EnhancedAgentBrain(brain)

def create_enhanced_world() -> EnhancedSimpleWorld:
    """Create enhanced simple world."""
    return EnhancedSimpleWorld()

# === Backward Compatibility ===

# Re-export original classes for backward compatibility
__all__ = [
    "EnhancedAgentBrain",
    "EnhancedSimpleWorld",
    "EnhancedSensoryPercept",
    "AgentBehavior",
    "LearningMode",
    "ActionSelector",
    "create_enhanced_agent",
    "create_enhanced_world",
]

if __name__ == "__main__":
    # Example usage
    print("Enhanced NLM Agent and World Modules")
    print("=" * 50)
    
    # Create enhanced world
    world = create_enhanced_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    print(f"Created enhanced world ({world.get_width()}x{world.get_height()})")
    
    # Create enhanced brain
    from .enhanced_api import create_enhanced_brain
    brain = create_enhanced_brain()
    print(f"Created enhanced brain with {brain.get_statistics()['total_neurons']} neurons")
    
    # Create enhanced agent
    agent = create_enhanced_agent(brain)
    agent.set_behavior(AgentBehavior.BALANCED)
    agent.set_exploration_rate(0.1)
    
    print(f"Created enhanced agent with behavior: {agent._behavior.value}")
    
    # Run a simple simulation
    for step in range(10):
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        processed = agent.process_sensory_input(percept)
        
        # Decide action
        action = agent.decide_action()
        
        # Apply action to world
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Get brain state
        brain.step(step, step * 0.1)
        
        # Get reward
        reward = world.get_simulation_time() / 100.0  # Simple reward
        
        # Learn from experience
        next_state = brain.get_statistics()
        agent.learn_from_experience(reward, next_state, action["action_type"])
    
    print(f"Simulation complete!")
    print(f"Final brain stats: {brain.get_statistics()}")
    print(f"Agent metrics: {agent.get_learning_metrics()}")
    print(f"World stats: {world.get_stats()}")
    
    print("\nEnhanced NLM agent and world modules ready for use!")
