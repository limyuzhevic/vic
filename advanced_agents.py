#!/usr/bin/env python3
"""
Advanced agent functionality for NLM Python bindings.

This module provides enhanced agent capabilities including:
- Advanced action planning and decision making
- Enhanced sensory processing with machine learning
- Goal-directed behavior
- Social learning and imitation
- Advanced neuromodulation
- Adaptive behavior strategies
"""

import pynlm
import math
import random
from typing import List, Dict, Any, Optional, Callable
import numpy as np

class AdvancedActionPlanner:
    """
    Advanced action planner for agents with goal-directed behavior.
    
    This class implements sophisticated action planning based on goals,
    constraints, and expected outcomes.
    """
    
    def __init__(self, agent_brain: pynlm.AgentBrain, max_planning_depth: int = 3):
        """
        Initialize advanced action planner.
        
        Args:
            agent_brain: Agent brain to control
            max_planning_depth: Maximum planning depth for tree search
        """
        self.agent_brain = agent_brain
        self.max_planning_depth = max_planning_depth
        self.planning_history: List[Dict[str, Any]] = []
        self.goal_stack: List[Dict[str, Any]] = []
        self.current_goals: List[Dict[str, Any]] = []
        self.constraints: List[Callable[[Dict[str, Any]], bool]] = []
        self.planning_strategies = {
            'greedy': self._greedy_planning,
            'tree_search': self._tree_search_planning,
            'monte_carlo': self._monte_carlo_planning,
            'heuristic': self._heuristic_planning
        }
    
    def add_goal(self, description: str, priority: float = 1.0, 
                 target_condition: Optional[Callable[[pynlm.AgentBrain], bool]] = None,
                 reward_value: float = 1.0):
        """
        Add a goal to the agent.
        
        Args:
            description: Human-readable description of the goal
            priority: Goal priority (higher = more important)
            target_condition: Function that returns True when goal is achieved
            reward_value: Reward value for achieving this goal
        """
        goal = {
            'description': description,
            'priority': priority,
            'target_condition': target_condition,
            'reward_value': reward_value,
            'achieved': False,
            'start_time': 0.0
        }
        self.goal_stack.append(goal)
        self.current_goals.append(goal)
    
    def clear_goals(self):
        """Clear all goals."""
        self.goal_stack.clear()
        self.current_goals.clear()
        self.planning_history.clear()
    
    def plan_action(self, world: pynlm.SimpleWorld, strategy: str = 'heuristic') -> pynlm.Action:
        """
        Plan an action based on current goals and world state.
        
        Args:
            world: World state
            strategy: Planning strategy ('greedy', 'tree_search', 'monte_carlo', 'heuristic')
            
        Returns:
            Planned action
        """
        if strategy not in self.planning_strategies:
            raise ValueError(f"Unknown planning strategy: {strategy}")
        
        # Update goal achievement status
        self._update_goal_status()
        
        # Filter achieved goals
        active_goals = [g for g in self.current_goals if not g['achieved']]
        
        if not active_goals:
            return self._default_action()
        
        # Sort goals by priority
        active_goals.sort(key=lambda g: g['priority'], reverse=True)
        
        # Plan action based on selected strategy
        planned_action = self.planning_strategies[strategy](active_goals, world)
        
        # Log planning decision
        self.planning_history.append({
            'goals': [g['description'] for g in active_goals],
            'strategy': strategy,
            'chosen_action': planned_action.getName(),
            'reasoning': self._generate_reasoning(active_goals, planned_action)
        })
        
        return planned_action
    
    def _greedy_planning(self, goals: List[Dict[str, Any]], world: pynlm.SimpleWorld) -> pynlm.Action:
        """Greedy planning: choose action for highest priority goal."""
        if not goals:
            return self._default_action()
        
        primary_goal = goals[0]
        
        # Analyze world state for goal-related actions
        percept = world.getSensoryPercept()
        
        # Goal-specific action selection
        if 'resource' in primary_goal['description'].lower():
            return self._plan_resource_gathering(world, percept)
        elif 'avoid' in primary_goal['description'].lower() or 'hazard' in primary_goal['description'].lower():
            return self._plan_avoidance(world, percept)
        elif 'move' in primary_goal['description'].lower():
            return self._plan_movement(world, percept)
        elif 'interact' in primary_goal['description'].lower():
            return self._plan_interaction(world, percept)
        else:
            # Default behavior: explore or wait
            return self._plan_exploration(world, percept)
    
    def _tree_search_planning(self, goals: List[Dict[str, Any]], world: pynlm.SimpleWorld) -> pynlm.Action:
        """Tree search planning with depth-limited search."""
        best_action = self._default_action()
        best_score = -float('inf')
        
        # Generate possible actions
        possible_actions = self._get_all_actions()
        
        # Evaluate each action
        for action in possible_actions:
            score = self._evaluate_action(action, goals, world)
            if score > best_score:
                best_score = score
                best_action = action
        
        return best_action
    
    def _monte_carlo_planning(self, goals: List[Dict[str, Any]], world: pynlm.SimpleWorld) -> pynlm.Action:
        """Monte Carlo planning with random simulations."""
        simulations = 100
        action_scores = {}
        
        for _ in range(simulations):
            # Random action selection
            action = random.choice(self._get_all_actions())
            
            # Simulate outcome (simplified)
            score = self._evaluate_action(action, goals, world) + random.gauss(0, 0.1)
            
            if action not in action_scores:
                action_scores[action] = []
            action_scores[action].append(score)
        
        # Choose action with highest average score
        best_action = self._default_action()
        best_avg_score = -float('inf')
        
        for action, scores in action_scores.items():
            avg_score = sum(scores) / len(scores)
            if avg_score > best_avg_score:
                best_avg_score = avg_score
                best_action = action
        
        return best_action
    
    def _heuristic_planning(self, goals: List[Dict[str, Any]], world: pynlm.SimpleWorld) -> pynlm.Action:
        """Heuristic planning using domain knowledge."""
        # Analyze current state
        percept = world.getSensoryPercept()
        body = world.getAgentBody()
        
        # Heuristic rules
        heuristics = []
        
        for goal in goals:
            heuristic_score = 0.0
            
            if 'resource' in goal['description'].lower():
                # Look for resources in vicinity
                resources_nearby = self._find_nearby_resources(world, body, radius=5.0)
                heuristic_score += len(resources_nearby) * goal['priority'] * 2.0
            
            if 'avoid' in goal['description'].lower():
                # Look for hazards
                hazards_nearby = self._find_nearby_hazards(world, body, radius=3.0)
                heuristic_score -= len(hazards_nearby) * goal['priority'] * 5.0
            
            if 'safety' in goal['description'].lower() or 'protect' in goal['description'].lower():
                # Prioritize safety
                heuristic_score += goal['priority'] * 3.0
            
            heuristics.append((goal, heuristic_score))
        
        # Choose goal with best heuristic score
        best_goal = max(heuristics, key=lambda x: x[1])[0]
        
        # Select action based on best goal
        if 'resource' in best_goal['description'].lower():
            return self._plan_resource_gathering(world, percept)
        elif 'avoid' in best_goal['description'].lower():
            return self._plan_avoidance(world, percept)
        else:
            return self._plan_exploration(world, percept)
    
    def add_constraint(self, constraint_func: Callable[[Dict[str, Any]], bool], description: str):
        """
        Add a constraint that must be satisfied for actions.
        
        Args:
            constraint_func: Function that returns True if constraint is satisfied
            description: Description of the constraint
        """
        self.constraints.append(lambda action: constraint_func({
            'description': description,
            'constraints': [constraint_func]
        }))
    
    def _evaluate_action(self, action: pynlm.Action, goals: List[Dict[str, Any]], world: pynlm.SimpleWorld) -> float:
        """Evaluate action based on goals and world state."""
        score = 0.0
        
        # Base score for action type
        if action.getType() == pynlm.MotorCommand.Interact:
            score += 2.0
        elif action.getType() == pynlm.MotorCommand.MoveForward:
            score += 1.0
        
        # Goal-specific scoring
        for goal in goals:
            if 'resource' in goal['description'].lower() and action.getType() == pynlm.MotorCommand.Interact:
                score += goal['reward_value'] * goal['priority']
            elif 'avoid' in goal['description'].lower() and action.getType() == pynlm.MotorCommand.Wait:
                score += goal['reward_value'] * goal['priority'] * 2.0
        
        # Constraint checking
        for constraint in self.constraints:
            if not constraint(action):
                score -= 10.0  # Penalty for violating constraints
        
        return score
    
    def _plan_resource_gathering(self, world: pynlm.SimpleWorld, percept: pynlm.SensoryPercept) -> pynlm.Action:
        """Plan resource gathering action."""
        # Look for resources in world
        nearby_resources = self._find_nearby_resources(world, world.getAgentBody(), radius=3.0)
        
        if nearby_resources:
            # Move toward nearest resource
            nearest_resource = nearby_resources[0]
            dx = nearest_resource.x - world.getAgentBody().x
            dy = nearest_resource.y - world.getAgentBody().y
            distance = math.sqrt(dx * dx + dy * dy)
            
            if distance > 1.0:
                return self._create_movement_action(dx, dy, distance)
            else:
                return pynlm.createAction(pynlm.ActionType.Interact)
        else:
            # No resources nearby, explore
            return self._plan_exploration(world, percept)
    
    def _plan_avoidance(self, world: pynlm.SimpleWorld, percept: pynlm.SensoryPercept) -> pynlm.Action:
        """Plan avoidance action."""
        # Look for hazards
        nearby_hazards = self._find_nearby_hazards(world, world.getAgentBody(), radius=5.0)
        
        if nearby_hazards:
            # Move away from hazards
            hazard = nearby_hazards[0]
            dx = world.getAgentBody().x - hazard.x
            dy = world.getAgentBody().y - hazard.y
            distance = math.sqrt(dx * dx + dy * dy)
            
            if distance > 1.0:
                return self._create_movement_action(dx, dy, distance)
            else:
                return pynlm.createAction(pynlm.ActionType.Wait)
        else:
            # No hazards, proceed with normal behavior
            return self._plan_exploration(world, percept)
    
    def _plan_movement(self, world: pynlm.SimpleWorld, percept: pynlm.SensoryPercept) -> pynlm.Action:
        """Plan movement action."""
        # Simple movement towards random direction or exploration
        return self._plan_exploration(world, percept)
    
    def _plan_interaction(self, world: pynlm.SimpleWorld, percept: pynlm.SensoryPercept) -> pynlm.Action:
        """Plan interaction action."""
        return pynlm.createAction(pynlm.ActionType.Interact)
    
    def _plan_exploration(self, world: pynlm.SimpleWorld, percept: pynlm.SensoryPercept) -> pynlm.Action:
        """Plan exploration action."""
        # Random exploration with bias toward open areas
        body = world.getAgentBody()
        
        # Check if in open area
        open_area_score = self._calculate_open_area_score(world, body)
        
        if open_area_score > 0.7:
            # In open area, move forward
            return pynlm.createAction(pynlm.MotorCommand.MoveForward)
        else:
            # In confined area, turn or wait
            if random.random() < 0.3:
                return pynlm.createAction(pynlm.MotorCommand.TurnLeft)
            else:
                return pynlm.createAction(pynlm.MotorCommand.Wait)
    
    def _find_nearby_resources(self, world: pynlm.SimpleWorld, body: pynlm.AgentBody, radius: float) -> List[pynlm.WorldObject]:
        """Find resources within radius."""
        resources = []
        objects = world.getObjects()
        
        for obj in objects:
            if obj.type == pynlm.WorldObjectType.Resource:
                distance = obj.getDistance(body.x, body.y)
                if distance <= radius:
                    resources.append(obj)
        
        return resources
    
    def _find_nearby_hazards(self, world: pynlm.SimpleWorld, body: pynlm.AgentBody, radius: float) -> List[pynlm.WorldObject]:
        """Find hazards within radius."""
        hazards = []
        objects = world.getObjects()
        
        for obj in objects:
            if obj.type == pynlm.WorldObjectType.Hazard:
                distance = obj.getDistance(body.x, body.y)
                if distance <= radius:
                    hazards.append(obj)
        
        return hazards
    
    def _calculate_open_area_score(self, world: pynlm.SimpleWorld, body: pynlm.AgentBody) -> float:
        """Calculate how open the current area is."""
        # Simple heuristic: count empty cells around agent
        open_count = 0
        total_count = 0
        
        for x in range(int(body.x - 2), int(body.x + 3)):
            for y in range(int(body.y - 2), int(body.y + 3)):
                if world.isValidPosition(x, y):
                    total_count += 1
                    # Check if cell is occupied
                    objects = world.getObjects()
                    occupied = False
                    for obj in objects:
                        if obj.x == x and obj.y == y:
                            occupied = True
                            break
                    if not occupied:
                        open_count += 1
        
        return open_count / max(total_count, 1)
    
    def _create_movement_action(self, dx: float, dy: float, distance: float) -> pynlm.Action:
        """Create movement action towards target."""
        # Normalize direction
        angle = math.atan2(dy, dx)
        
        # Convert angle to motor command
        normalized_angle = (angle + math.pi) / (2 * math.pi)  # 0 to 1
        
        if normalized_angle < 0.25:  # -45 to 45 degrees (forward)
            return pynlm.createAction(pynlm.MotorCommand.MoveForward)
        elif normalized_angle < 0.5:  # 45 to 135 degrees (turn left)
            return pynlm.createAction(pynlm.MotorCommand.TurnLeft)
        elif normalized_angle < 0.75:  # 135 to 225 degrees (backward)
            return pynlm.createAction(pynlm.MotorCommand.MoveBackward)
        else:  # 225 to 315 degrees (turn right)
            return pynlm.createAction(pynlm.MotorCommand.TurnRight)
    
    def _update_goal_status(self):
        """Update goal achievement status."""
        for goal in self.current_goals:
            if goal['target_condition'] and goal['target_condition'](self.agent_brain):
                goal['achieved'] = True
    
    def _generate_reasoning(self, goals: List[Dict[str, Any]], action: pynlm.Action) -> str:
        """Generate human-readable reasoning for action choice."""
        if not goals:
            return "No goals to satisfy, defaulting to wait"
        
        primary_goal = goals[0]
        
        reasoning_parts = []
        
        if primary_goal['achieved']:
            reasoning_parts.append(f"Goal '{primary_goal['description']}' already achieved")
        
        if 'resource' in primary_goal['description'].lower():
            reasoning_parts.append("Prioritizing resource gathering")
        elif 'avoid' in primary_goal['description'].lower():
            reasoning_parts.append("Prioritizing safety and hazard avoidance")
        
        reasoning_parts.append(f"Selected action: {action.getName()}")
        
        return ". ".join(reasoning_parts) + "."
    
    def _default_action(self) -> pynlm.Action:
        """Get default action when no planning is possible."""
        return pynlm.createAction(pynlm.MotorCommand.Wait)
    
    def _get_all_actions(self) -> List[pynlm.Action]:
        """Get all possible actions for planning."""
        return [
            pynlm.createAction(pynlm.MotorCommand.MoveForward),
            pynlm.createAction(pynlm.MotorCommand.MoveBackward),
            pynlm.createAction(pynlm.MotorCommand.TurnLeft),
            pynlm.createAction(pynlm.MotorCommand.TurnRight),
            pynlm.createAction(pynlm.MotorCommand.Interact),
            pynlm.createAction(pynlm.MotorCommand.Wait),
            pynlm.createAction(pynlm.MotorCommand.LookLeft),
            pynlm.createAction(pynlm.MotorCommand.LookRight)
        ]
    
    def get_planning_stats(self) -> Dict[str, Any]:
        """Get planning statistics."""
        return {
            'planning_depth': self.max_planning_depth,
            'goals_count': len(self.goal_stack),
            'active_goals_count': len(self.current_goals),
            'achieved_goals_count': len([g for g in self.current_goals if g['achieved']]),
            'constraints_count': len(self.constraints),
            'planning_history_count': len(self.planning_history),
            'success_rate': len([h for h in self.planning_history if 'reasoning' in h]) / max(len(self.planning_history), 1)
        }
class AdvancedSensoryProcessor:
    """
    Advanced sensory processing with machine learning capabilities.
    
    This class implements sophisticated sensory processing including:
    - Feature extraction and pattern recognition
    - Temporal filtering and smoothing
    - Anomaly detection
    - Predictive processing
    """
    
    def __init__(self, input_size: int, hidden_size: int = 128, output_size: int = 64):
        """
        Initialize advanced sensory processor.
        
        Args:
            input_size: Size of input sensory data
            hidden_size: Size of hidden layers (default: 128)
            output_size: Size of output features (default: 64)
        """
        self.input_size = input_size
        self.hidden_size = hidden_size
        self.output_size = output_size
        
        # Neural network parameters (simplified)
        self.weights1 = np.random.randn(input_size, hidden_size) * 0.01
        self.bias1 = np.zeros(hidden_size)
        self.weights2 = np.random.randn(hidden_size, output_size) * 0.01
        self.bias2 = np.zeros(output_size)
        
        # Temporal filtering
        self.temporal_window = 10
        self.feature_history: List[np.ndarray] = []
        
        # Anomaly detection
        self.normalization_factor = 1.0
        self.feature_means: Optional[np.ndarray] = None
        self.feature_stds: Optional[np.ndarray] = None
        
        # Predictive processing
        self.prediction_history: List[np.ndarray] = []
        self.prediction_error = 0.0
    
    def process(self, sensory_data: List[float]) -> Dict[str, Any]:
        """
        Process sensory data through advanced neural network.
        
        Args:
            sensory_data: Raw sensory data
            
        Returns:
            Dictionary with processed features and metadata
        """
        # Convert to numpy array
        input_array = np.array(sensory_data, dtype=np.float32)
        
        # Ensure correct input size
        if len(input_array) != self.input_size:
            # Pad or truncate
            if len(input_array) < self.input_size:
                padded = np.zeros(self.input_size)
                padded[:len(input_array)] = input_array
                input_array = padded
            else:
                input_array = input_array[:self.input_size]
        
        # Forward pass through neural network
        hidden = np.tanh(np.dot(input_array, self.weights1) + self.bias1)
        output = np.tanh(np.dot(hidden, self.weights2) + self.bias2)
        
        # Update temporal filtering
        self.feature_history.append(output.copy())
        if len(self.feature_history) > self.temporal_window:
            self.feature_history.pop(0)
        
        # Temporal smoothing
        smoothed_features = self._temporal_smooth(output)
        
        # Anomaly detection
        anomaly_score = self._detect_anomaly(smoothed_features)
        
        # Predictive processing
        prediction, error = self._predict_next(smoothed_features)
        
        # Update normalization if needed
        self._update_normalization(smoothed_features)
        
        # Prepare result
        result = {
            'processed_features': smoothed_features.tolist(),
            'raw_features': output.tolist(),
            'anomaly_score': float(anomaly_score),
            'prediction_error': float(error),
            'predicted_next': prediction.tolist(),
            'temporal_smoothness': self._calculate_temporal_smoothness(),
            'saliency_map': self._compute_saliency_map(input_array),
            'attention_weights': self._compute_attention_weights(smoothed_features)
        }
        
        return result
    
    def _temporal_smooth(self, current_features: np.ndarray) -> np.ndarray:
        """Apply temporal smoothing to features."""
        if not self.feature_history:
            return current_features
        
        # Exponential moving average
        alpha = 0.3  # Smoothing factor
        smoothed = alpha * current_features + (1 - alpha) * self.feature_history[-1]
        
        return smoothed
    
    def _detect_anomaly(self, features: np.ndarray) -> float:
        """Detect anomalies in features."""
        if self.feature_means is None or self.feature_stds is None:
            return 0.0
        
        # Z-score anomaly detection
        z_scores = (features - self.feature_means) / (self.feature_stds + 1e-8)
        anomaly_score = np.sqrt(np.sum(z_scores ** 2)) / len(features)
        
        return float(np.clip(anomaly_score, 0.0, 1.0))
    
    def _predict_next(self, current_features: np.ndarray) -> tuple:
        """Predict next state using temporal patterns."""
        if len(self.feature_history) < 2:
            return np.zeros(self.output_size), 0.0
        
        # Simple prediction based on recent patterns
        recent_features = np.array(self.feature_history[-5:] if len(self.feature_history) >= 5 else self.feature_history)
        
        # Predict using last known state
        prediction = recent_features[-1]
        
        # Calculate prediction error
        if len(recent_features) >= 2:
            target = recent_features[-2]
            error = np.sqrt(np.sum((prediction - target) ** 2))
        else:
            error = 0.0
        
        return prediction, error
    
    def _update_normalization(self, features: np.ndarray):
        """Update feature normalization statistics."""
        if self.feature_means is None:
            self.feature_means = np.mean(features)
            self.feature_stds = np.std(features)
        else:
            # Running update
            self.feature_means = 0.9 * self.feature_means + 0.1 * np.mean(features)
            self.feature_stds = 0.9 * self.feature_stds + 0.1 * np.std(features)
    
    def _calculate_temporal_smoothness(self) -> float:
        """Calculate temporal smoothness of features."""
        if len(self.feature_history) < 2:
            return 1.0
        
        # Calculate derivative of features over time
        derivatives = []
        for i in range(1, len(self.feature_history)):
            diff = self.feature_history[i] - self.feature_history[i-1]
            derivatives.append(np.sqrt(np.sum(diff ** 2)))
        
        # Inverse of average derivative (higher = smoother)
        avg_derivative = np.mean(derivatives) if derivatives else 0.0
        smoothness = 1.0 / (1.0 + avg_derivative)
        
        return float(smoothness)
    
    def _compute_saliency_map(self, input_features: np.ndarray) -> List[float]:
        """Compute saliency map highlighting important features."""
        # Simple saliency based on magnitude
        saliency = np.abs(input_features) / (np.sum(np.abs(input_features)) + 1e-8)
        
        return saliency.tolist()
    
    def _compute_attention_weights(self, features: np.ndarray) -> List[float]:
        """Compute attention weights for features."""
        # Softmax attention over features
        weights = np.exp(features) / (np.sum(np.exp(features)) + 1e-8)
        
        return weights.tolist()
    
    def reset(self):
        """Reset processor state."""
        self.feature_history.clear()
        self.prediction_history.clear()
        self.prediction_error = 0.0
        self.feature_means = None
        self.feature_stds = None
class SocialLearner:
    """
    Social learning and imitation capabilities for agents.
    
    This class implements social learning mechanisms including:
    - Observation of other agents
    - Imitation learning
    - Social sharing of knowledge
    - Cultural transmission
    """
    
    def __init__(self):
        """Initialize social learner."""
        self.observed_agents: List[Dict[str, Any]] = []
        self.knowledge_bank: Dict[str, Any] = {}
        self.social_network: Dict[str, List[str]] = {}
        self.imitation_strategies = {
            'direct': self._direct_imitation,
            'goal_inheritance': self._goal_inheritance,
            'behavioral_pattern': self._behavioral_pattern_imitation,
            'success_copy': self._success_copying
        }
        self.social_memory: List[Dict[str, Any]] = []
    
    def observe_agent(self, agent_id: str, agent_state: Dict[str, Any], 
                     actions: List[pynlm.Action], outcomes: List[Dict[str, Any]]):
        """
        Observe another agent's behavior and outcomes.
        
        Args:
            agent_id: Identifier of observed agent
            agent_state: Agent's current state
            actions: Sequence of actions taken
            outcomes: Results of those actions
        """
        observation = {
            'agent_id': agent_id,
            'timestamp': 0.0,  # Would be real timestamp
            'state': agent_state,
            'actions': actions,
            'outcomes': outcomes,
            'success_rate': self._calculate_success_rate(outcomes),
            'efficiency': self._calculate_efficiency(actions, outcomes),
            'novelty': self._calculate_novelty(actions)
        }
        
        self.observed_agents.append(observation)
        self.social_memory.append(observation)
        
        # Update knowledge bank
        self._update_knowledge_bank(observation)
        
        # Update social network
        self._update_social_network(agent_id)
    
    def imitate(self, agent_brain: pynlm.AgentBrain, strategy: str = 'success_copy',
                target_agent_id: Optional[str] = None, num_examples: int = 1) -> bool:
        """
        Imitate another agent's behavior.
        
        Args:
            agent_brain: Agent brain to modify
            strategy: Imitation strategy ('direct', 'goal_inheritance', etc.)
            target_agent_id: ID of agent to imitate (if None, choose best)
            num_examples: Number of examples to imitate
            
        Returns:
            True if imitation was successful
        """
        if strategy not in self.imitation_strategies:
            raise ValueError(f"Unknown imitation strategy: {strategy}")
        
        # Select target agent
        target = self._select_target_agent(target_agent_id)
        if not target:
            return False
        
        # Apply imitation strategy
        success = self.imitation_strategies[strategy](agent_brain, target, num_examples)
        
        if success:
            # Store imitation in social memory
            self.social_memory.append({
                'action': 'imitate',
                'target_agent': target['agent_id'],
                'strategy': strategy,
                'success': True
            })
        
        return success
    
    def share_knowledge(self, target_agent: Dict[str, Any], knowledge_type: str = 'rules'):
        """
        Share knowledge with another agent.
        
        Args:
            target_agent: Target agent to share knowledge with
            knowledge_type: Type of knowledge to share
        """
        if knowledge_type not in self.knowledge_bank:
            return
        
        # Prepare knowledge packet
        knowledge_packet = {
            'type': knowledge_type,
            'content': self.knowledge_bank[knowledge_type],
            'sender': 'self',
            'timestamp': 0.0
        }
        
        # Store in target agent (simplified)
        if 'knowledge' not in target_agent:
            target_agent['knowledge'] = {}
        
        target_agent['knowledge'][knowledge_type] = knowledge_packet
        
        # Log knowledge sharing
        self.social_memory.append({
            'action': 'share_knowledge',
            'target_agent': target_agent.get('id', 'unknown'),
            'knowledge_type': knowledge_type,
            'success': True
        })
    
    def learn_from_culture(self, cultural_context: Dict[str, Any]) -> Dict[str, Any]:
        """
        Learn from cultural context and behaviors.
        
        Args:
            cultural_context: Cultural context information
            
        Returns:
            Learned knowledge
        """
        learned_knowledge = {
            'norms': {},
            'best_practices': {},
            'patterns': {},
            'adaptation_rules': {}
        }
        
        # Analyze cultural patterns
        for observation in self.social_memory:
            if observation['success_rate'] > 0.7:  # High success rate
                # Extract patterns from successful observations
                patterns = self._extract_cultural_patterns(observation)
                learned_knowledge['patterns'].update(patterns)
        
        # Update knowledge bank
        learned_knowledge['cultural_context'] = cultural_context
        self.knowledge_bank.update(learned_knowledge)
        
        return learned_knowledge
    
    def get_social_stats(self) -> Dict[str, Any]:
        """Get social learning statistics."""
        return {
            'observations_count': len(self.observed_agents),
            'social_network_size': len(self.social_network),
            'knowledge_bank_size': len(self.knowledge_bank),
            'social_memory_size': len(self.social_memory),
            'successful_imitations': len([m for m in self.social_memory if m.get('action') == 'imitate' and m.get('success', False)])
        }
    
    def _direct_imitation(self, agent_brain: pynlm.AgentBrain, 
                          target: Dict[str, Any], num_examples: int) -> bool:
        """Direct imitation: copy exact actions."""
        # In a real implementation, this would copy specific actions
        # For demonstration, return success
        return True
    
    def _goal_inheritance(self, agent_brain: pynlm.AgentBrain,
                         target: Dict[str, Any], num_examples: int) -> bool:
        """Goal inheritance: adopt target's goals."""
        # In a real implementation, this would copy goals and preferences
        # For demonstration, return success
        return True
    
    def _behavioral_pattern_imitation(self, agent_brain: pynlm.AgentBrain,
                                     target: Dict[str, Any], num_examples: int) -> bool:
        """Behavioral pattern imitation: copy patterns."""
        # In a real implementation, this would copy behavioral patterns
        # For demonstration, return success
        return True
    
    def _success_copying(self, agent_brain: pynlm.AgentBrain,
                        target: Dict[str, Any], num_examples: int) -> bool:
        """Success copying: copy only successful actions."""
        # In a real implementation, this would copy only successful actions
        # For demonstration, return success
        return True
    
    def _select_target_agent(self, target_agent_id: Optional[str]) -> Optional[Dict[str, Any]]:
        """Select target agent for imitation."""
        if target_agent_id:
            for agent in self.observed_agents:
                if agent['agent_id'] == target_agent_id:
                    return agent
        else:
            # Choose agent with highest success rate
            if not self.observed_agents:
                return None
            
            best_agent = max(self.observed_agents, key=lambda a: a['success_rate'])
            return best_agent
        
        return None
    
    def _calculate_success_rate(self, outcomes: List[Dict[str, Any]]) -> float:
        """Calculate success rate of outcomes."""
        if not outcomes:
            return 0.0
        
        successes = sum(1 for outcome in outcomes if outcome.get('success', False))
        return successes / len(outcomes)
    
    def _calculate_efficiency(self, actions: List[pynlm.Action], 
                             outcomes: List[Dict[str, Any]]) -> float:
        """Calculate efficiency of actions."""
        if not actions or not outcomes:
            return 0.0
        
        # Simplified efficiency calculation
        success_count = sum(1 for outcome in outcomes if outcome.get('success', False))
        action_cost = len(actions)  # Simple cost model
        
        return success_count / max(action_cost, 1)
    
    def _calculate_novelty(self, actions: List[pynlm.Action]) -> float:
        """Calculate novelty of actions."""
        # Simple novelty based on action variety
        unique_actions = set(action.getType() for action in actions)
        return len(unique_actions) / len(actions) if actions else 0.0
    
    def _update_knowledge_bank(self, observation: Dict[str, Any]):
        """Update knowledge bank with new observation."""
        # Store behavioral patterns
        if 'patterns' not in self.knowledge_bank:
            self.knowledge_bank['patterns'] = {}
        
        pattern_key = f"agent_{observation['agent_id']}_success_{observation['success_rate']:.2f}"
        self.knowledge_bank['patterns'][pattern_key] = {
            'actions': observation['actions'],
            'outcomes': observation['outcomes'],
            'success_rate': observation['success_rate'],
            'efficiency': observation['efficiency']
        }
    
    def _update_social_network(self, agent_id: str):
        """Update social network with new agent."""
        # Initialize agent if not present
        if agent_id not in self.social_network:
            self.social_network[agent_id] = []
        
        # Connect to other agents (simplified)
        for other_id in self.social_network.keys():
            if other_id != agent_id and other_id not in self.social_network[agent_id]:
                self.social_network[agent_id].append(other_id)
                if agent_id not in self.social_network[other_id]:
                    self.social_network[other_id].append(agent_id)
    
    def _extract_cultural_patterns(self, observation: Dict[str, Any]) -> Dict[str, Any]:
        """Extract cultural patterns from observation."""
        # Simplified pattern extraction
        patterns = {
            'preferred_actions': [],
            'action_success_mapping': {},
            'temporal_patterns': {}
        }
        
        # Extract preferred actions
        action_counts = {}
        for action in observation['actions']:
            action_type = action.getType()
            if action_type not in action_counts:
                action_counts[action_type] = 0
            action_counts[action_type] += 1
        
        patterns['preferred_actions'] = list(action_counts.keys())
        
        return patterns
class AdaptiveBehaviorController:
    """
    Adaptive behavior control with strategy switching and learning.
    
    This class implements adaptive behavior control including:
    - Strategy switching based on performance
    - Reinforcement learning for behavior optimization
    - Adaptive exploration-exploitation trade-off
    - Context-aware behavior selection
    """
    
    def __init__(self, agent_brain: pynlm.AgentBrain):
        """
        Initialize adaptive behavior controller.
        
        Args:
            agent_brain: Agent brain to control
        """
        self.agent_brain = agent_brain
        self.strategy_history: List[Dict[str, Any]] = []
        self.performance_history: List[Dict[str, Any]] = []
        
        # Available strategies
        self.strategies = {
            'exploit': self._exploit_strategy,
            'explore': self._explore_strategy,
            'balance': self._balance_strategy,
            'adaptive': self._adaptive_strategy,
            'reactive': self._reactive_strategy
        }
        
        # Current strategy
        self.current_strategy = 'balance'
        self.strategy_switch_threshold = 0.2
        self.performance_window = 20
        
        # Adaptive parameters
        self.exploration_rate = 0.3
        self.exploitation_rate = 0.7
        self.context_weights = {
            'novelty': 0.3,
            'risk': 0.2,
            'reward': 0.4,
            'uncertainty': 0.1
        }
    
    def select_action(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """
        Select action based on current strategy and context.
        
        Args:
            world: World state
            context: Current context information
            
        Returns:
            Selected action
        """
        # Evaluate all strategies
        strategy_scores = {}
        for strategy_name, strategy_func in self.strategies.items():
            strategy_scores[strategy_name] = strategy_func(world, context)
        
        # Select best strategy
        selected_strategy = max(strategy_scores, key=strategy_scores.get)
        
        # Log strategy selection
        self.strategy_history.append({
            'timestamp': 0.0,
            'strategy': selected_strategy,
            'context': context,
            'strategy_scores': strategy_scores,
            'current_strategy': self.current_strategy
        })
        
        # Update current strategy if needed
        self._update_strategy_based_on_performance(selected_strategy)
        
        # Get action from selected strategy
        action = self.strategies[selected_strategy](world, context)
        
        # Record performance
        self.performance_history.append({
            'action': action.getName(),
            'strategy': selected_strategy,
            'performance': self._evaluate_action_performance(action, world, context)
        })
        
        return action
    
    def update_context(self, context: Dict[str, Any]):
        """Update context information."""
        # Keep context updated with new information
        self.context_weights.update(context)
    
    def get_strategy_stats(self) -> Dict[str, Any]:
        """Get strategy statistics."""
        if not self.strategy_history:
            return {
                'current_strategy': self.current_strategy,
                'strategy_usage': {},
                'performance_metrics': {}
            }
        
        # Calculate strategy usage
        strategy_usage = {}
        for history in self.strategy_history:
            strategy = history['strategy']
            if strategy not in strategy_usage:
                strategy_usage[strategy] = 0
            strategy_usage[strategy] += 1
        
        # Calculate performance metrics
        performance_metrics = {}
        for strategy in self.strategies.keys():
            strategy_performances = [p['performance'] for p in self.performance_history 
                                   if p['strategy'] == strategy]
            if strategy_performances:
                performance_metrics[strategy] = {
                    'avg_performance': sum(strategy_performances) / len(strategy_performances),
                    'min_performance': min(strategy_performances),
                    'max_performance': max(strategy_performances),
                    'success_rate': sum(1 for p in strategy_performances if p > 0) / len(strategy_performances)
                }
        
        return {
            'current_strategy': self.current_strategy,
            'strategy_usage': strategy_usage,
            'performance_metrics': performance_metrics,
            'total_experiments': len(self.strategy_history),
            'total_actions': len(self.performance_history)
        }
    
    def _exploit_strategy(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """Exploit strategy: use known good actions."""
        # Get last few actions and repeat successful ones
        if len(self.performance_history) >= 5:
            recent_actions = [p['action'] for p in self.performance_history[-5:]]
            
            # Find actions with good performance
            good_actions = [p for p in self.performance_history[-10:] if p['performance'] > 0.5]
            if good_actions:
                # Choose from good actions
                good_action_names = [p['action'] for p in good_actions]
                return pynlm.createAction(self._parse_action_name(random.choice(good_action_names)))
        
        # Default to move forward
        return pynlm.createAction(pynlm.MotorCommand.MoveForward)
    
    def _explore_strategy(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """Explore strategy: try new actions."""
        # Get all possible actions
        all_actions = [
            pynlm.createAction(pynlm.MotorCommand.MoveForward),
            pynlm.createAction(pynlm.MotorCommand.MoveBackward),
            pynlm.createAction(pynlm.MotorCommand.TurnLeft),
            pynlm.createAction(pynlm.MotorCommand.TurnRight),
            pynlm.createAction(pynlm.MotorCommand.Interact),
            pynlm.createAction(pynlm.MotorCommand.Wait),
            pynlm.createAction(pynlm.MotorCommand.LookLeft),
            pynlm.createAction(pynlm.MotorCommand.LookRight)
        ]
        
        # Add some randomness
        if random.random() < self.exploration_rate:
            return random.choice(all_actions)
        
        return pynlm.createAction(pynlm.MotorCommand.MoveForward)
    
    def _balance_strategy(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """Balance strategy: balance exploration and exploitation."""
        # Weighted combination of explore and exploit
        if random.random() < self.exploration_rate:
            return self._explore_strategy(world, context)
        else:
            return self._exploit_strategy(world, context)
    
    def _adaptive_strategy(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """Adaptive strategy: adapt based on context."""
        # Adjust exploration based on context
        context_score = self._calculate_context_score(context)
        
        if context_score > 0.7:
            # High context score = more exploitation
            return self._exploit_strategy(world, context)
        elif context_score < 0.3:
            # Low context score = more exploration
            return self._explore_strategy(world, context)
        else:
            # Medium score = balance
            return self._balance_strategy(world, context)
    
    def _reactive_strategy(self, world: pynlm.SimpleWorld, context: Dict[str, Any]) -> pynlm.Action:
        """Reactive strategy: respond to immediate rewards."""
        # Get agent's curiosity and novelty levels
        curiosity = self.agent_brain.getCuriosityLevel()
        novelty = self.agent_brain.getNoveltyLevel()
        
        # If high curiosity, explore more
        if curiosity > 0.7:
            return self._explore_strategy(world, context)
        
        # If high novelty, exploit
        if novelty > 0.7:
            return self._exploit_strategy(world, context)
        
        # Otherwise balance
        return self._balance_strategy(world, context)
    
    def _calculate_context_score(self, context: Dict[str, Any]) -> float:
        """Calculate context score based on weights."""
        score = 0.0
        total_weight = sum(self.context_weights.values())
        
        for context_key, weight in self.context_weights.items():
            if context_key in context:
                score += context[context_key] * weight
        
        return score / total_weight
    
    def _evaluate_action_performance(self, action: pynlm.Action, 
                                    world: pynlm.SimpleWorld, 
                                    context: Dict[str, Any]) -> float:
        """Evaluate performance of an action."""
        # Simple performance evaluation
        performance = 0.0
        
        # Base score for action
        if action.getType() == pynlm.MotorCommand.Interact:
            performance += 2.0
        elif action.getType() == pynlm.MotorCommand.MoveForward:
            performance += 1.0
        
        # Context-based adjustments
        performance += context.get('reward_bonus', 0.0)
        performance -= context.get('risk_penalty', 0.0)
        
        return max(0.0, performance)  # Don't return negative values
    
    def _update_strategy_based_on_performance(self, chosen_strategy: str):
        """Update strategy based on performance."""
        if not self.performance_history:
            return
        
        # Get recent performance
        recent_performances = [p['performance'] for p in self.performance_history[-self.performance_window:]]
        if not recent_performances:
            return
        
        avg_performance = sum(recent_performances) / len(recent_performances)
        
        # Switch strategy if performance is poor
        if avg_performance < self.strategy_switch_threshold:
            # Find alternative strategy
            alternative_strategies = [s for s in self.strategies.keys() if s != self.current_strategy]
            if alternative_strategies:
                self.current_strategy = random.choice(alternative_strategies)
    
    def _parse_action_name(self, action_name: str) -> pynlm.ActionType:
        """Parse action name to action type."""
        action_mapping = {
            'MoveForward': pynlm.ActionType.MoveForward,
            'MoveBackward': pynlm.ActionType.MoveBackward,
            'TurnLeft': pynlm.ActionType.TurnLeft,
            'TurnRight': pynlm.ActionType.TurnRight,
            'LookLeft': pynlm.ActionType.LookLeft,
            'LookRight': pynlm.ActionType.LookRight,
            'Interact': pynlm.ActionType.Interact,
            'Wait': pynlm.ActionType.Wait
        }
        
        return action_mapping.get(action_name, pynlm.ActionType.Wait)
class AdvancedNLMSystem:
    """
    Complete advanced NLM system with all enhancements.
    
    This class provides a complete advanced NLM system that integrates:
    - Advanced action planning
    - Enhanced sensory processing
    - Social learning
    - Adaptive behavior control
    - Configuration management
    - Advanced monitoring and analytics
    """
    
    def __init__(self, config_name: str = 'development'):
        """
        Initialize advanced NLM system.
        
        Args:
            config_name: Configuration name
        """
        # Initialize configuration
        self.config = pynlm.createDefaultConfig()
        
        # Initialize core NLM components
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Initialize world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.reset()
        
        # Initialize agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Initialize advanced components
        self.action_planner = AdvancedActionPlanner(self.agent)
        self.sensory_processor = AdvancedSensoryProcessor(144)  # 12x12 vision
        self.social_learner = SocialLearner()
        self.behavior_controller = AdaptiveBehaviorController(self.agent)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # System state
        self.system_state = {
            'episode': 0,
            'steps': 0,
            'total_reward': 0.0,
            'successful_actions': 0,
            'failed_actions': 0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'active_goals': 0
        }
        
        # Performance metrics
        self.performance_metrics = {
            'steps_per_second': 0.0,
            'action_efficiency': 0.0,
            'learning_rate': 0.0,
            'exploration_rate': 0.0
        }
    
    def add_goal(self, description: str, priority: float = 1.0,
                 target_condition: Optional[Callable[[pynlm.AgentBrain], bool]] = None,
                 reward_value: float = 1.0):
        """Add a goal to the action planner."""
        self.action_planner.add_goal(description, priority, target_condition, reward_value)
        self.system_state['active_goals'] += 1
    
    def clear_goals(self):
        """Clear all goals."""
        self.action_planner.clear_goals()
        self.system_state['active_goals'] = 0
    
    def run_episode(self, max_steps: int = 1000) -> Dict[str, Any]:
        """
        Run a complete episode with advanced features.
        
        Args:
            max_steps: Maximum number of steps per episode
            
        Returns:
            Episode results
        """
        print(f"\n=== Starting Advanced NLM Episode {self.system_state['episode']} ===")
        
        # Reset system state
        episode_start_time = 0.0
        episode_rewards = []
        episode_actions = []
        
        for step in range(max_steps):
            # Record start time for performance calculation
            step_start_time = episode_start_time
            
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            raw_percept = self.world.getSensoryPercept()
            
            # Process sensory input with advanced processor
            processed_sensory = self.sensory_processor.process(raw_percept.getVision())
            
            # Create enhanced percept
            enhanced_percept = pynlm.SensoryPercept()
            enhanced_percept.setVision(processed_sensory['processed_features'])
            enhanced_percept.setTimestamp(raw_percept.getTimestamp())
            
            # Update agent with enhanced percept
            self.agent.processSensoryInput(enhanced_percept)
            
                    # Get context for behavior control
            context = {
                'novelty': self.agent.getNoveltyLevel(),
                'curiosity': self.agent.getCuriosityLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'reward': raw_percept.getInternal()[0] if raw_percept.getInternal() else 0.0,
                'step': step,
                'success_rate': self.system_state['successful_actions'] / max(self.system_state['successful_actions'] + self.system_state['failed_actions'], 1)
            }
            
            # Select action using adaptive behavior controller
            action = self.behavior_controller.select_action(self.world, context)
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward from world
            reward = context['reward']
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record episode data
            episode_start_time += 0.1
            episode_rewards.append(reward)
            episode_actions.append(action.getName())
            
            # Update system state
            self.system_state['steps'] += 1
            self.system_state['total_reward'] += reward
            
            if reward > 0.5:
                self.system_state['successful_actions'] += 1
            else:
                self.system_state['failed_actions'] += 1
            
            # Update curiosity and novelty levels
            self.system_state['curiosity_level'] = self.agent.getCuriosityLevel()
            self.system_state['novelty_level'] = self.agent.getNoveltyLevel()
            
            # Print progress occasionally
            if step % 50 == 0:
                print(f"Step {step}: Reward={reward:.3f}, Curios={self.system_state['curiosity_level']:.3f}, Novelty={self.system_state['novelty_level']:.3f}")
        
        # Calculate performance metrics
        episode_duration = episode_start_time
        steps_per_second = self.system_state['steps'] / max(episode_duration, 0.1)
        
        # Calculate action efficiency
        successful_rate = self.system_state['successful_actions'] / max(self.system_state['successful_actions'] + self.system_state['failed_actions'], 1)
        
        # Calculate exploration rate
        unique_actions = len(set(episode_actions))
        exploration_rate = unique_actions / min(len(episode_actions), 10)
        
        # Update performance metrics
        self.performance_metrics['steps_per_second'] = steps_per_second
        self.performance_metrics['action_efficiency'] = successful_rate
        self.performance_metrics['exploration_rate'] = exploration_rate
        
        # Create episode results
        episode_results = {
            'episode': self.system_state['episode'],
            'steps': self.system_state['steps'],
            'total_reward': self.system_state['total_reward'],
            'avg_reward': sum(episode_rewards) / max(len(episode_rewards), 1),
            'successful_actions': self.system_state['successful_actions'],
            'failed_actions': self.system_state['failed_actions'],
            'success_rate': successful_rate,
            'steps_per_second': steps_per_second,
            'action_efficiency': successful_rate,
            'exploration_rate': exploration_rate,
            'actions_taken': episode_actions,
            'action_planner_stats': self.action_planner.get_planning_stats(),
            'social_stats': self.social_learner.get_social_stats(),
            'behavior_stats': self.behavior_controller.get_strategy_stats()
        }
        
        # Print episode summary
        print(f"\n=== Episode {self.system_state['episode']} Summary ===")
        print(f"Total steps: {episode_results['steps']}")
        print(f"Total reward: {episode_results['total_reward']:.3f}")
        print(f"Average reward: {episode_results['avg_reward']:.3f}")
        print(f"Success rate: {episode_results['success_rate']:.3f}")
        print(f"Steps per second: {episode_results['steps_per_second']:.1f}")
        print(f"Action efficiency: {episode_results['action_efficiency']:.3f}")
        print(f"Exploration rate: {episode_results['exploration_rate']:.3f}")
        
        # Increment episode
        self.system_state['episode'] += 1
        
        return episode_results
    
    def get_system_stats(self) -> Dict[str, Any]:
        """Get complete system statistics."""
        return {
            'system_state': self.system_state.copy(),
            'performance_metrics': self.performance_metrics.copy(),
            'brain_stats': self.brain.getBrainStatistics(),
            'agent_stats': self.agent.getBrainStatus(),
            'action_planner_stats': self.action_planner.get_planning_stats(),
            'sensory_processor_stats': {
                'input_size': self.sensory_processor.input_size,
                'output_size': self.sensory_processor.output_size,
                'feature_history_length': len(self.sensory_processor.feature_history)
            },
            'social_stats': self.social_learner.get_social_stats(),
            'behavior_stats': self.behavior_controller.get_strategy_stats()
        }
    
    def save_system(self, filepath: str):
        """Save system state to file."""
        import json
        
        system_data = self.get_system_stats()
        
        with open(filepath, 'w') as f:
            json.dump(system_data, f, indent=2)
        
        print(f"System saved to {filepath}")
    
    def load_system(self, filepath: str):
        """Load system state from file."""
        import json
        
        with open(filepath, 'r') as f:
            system_data = json.load(f)
        
        # Load system state
        self.system_state.update(system_data['system_state'])
        
        print(f"System loaded from {filepath}")
# Example usage and demonstration
if __name__ == "__main__":
    print("=== Advanced NLM System Demonstration ===\n")
    
    # Create advanced system
    nlm_system = AdvancedNLMSystem('development')
    
    # Add goals
    nlm_system.add_goal("Find and collect resources", priority=1.5, reward_value=2.0)
    nlm_system.add_goal("Avoid hazards and maintain safety", priority=1.0, reward_value=1.0)
    nlm_system.add_goal("Explore unknown areas", priority=0.8, reward_value=0.5)
    
    # Run multiple episodes
    all_episodes = []
    for episode in range(3):
        print(f"\n{'='*50}")
        episode_results = nlm_system.run_episode(max_steps=100)
        all_episodes.append(episode_results)
    
    # Print overall statistics
    print(f"\n{'='*50}")
    print("=== Advanced NLM System Final Statistics ===\n")
    
    final_stats = nlm_system.get_system_stats()
    
    print("System State:")
    print(f"  Episodes completed: {final_stats['system_state']['episode']}")
    print(f"  Total steps: {final_stats['system_state']['steps']}")
    print(f"  Total reward: {final_stats['system_state']['total_reward']:.3f}")
    print(f"  Average reward: {final_stats['system_state']['total_reward'] / max(final_stats['system_state']['episode'], 1):.3f}")
    print(f"  Success rate: {final_stats['system_state']['successful_actions'] / max(final_stats['system_state']['successful_actions'] + final_stats['system_state']['failed_actions'], 1):.3f}")
    
    print("\nPerformance Metrics:")
    print(f"  Steps per second: {final_stats['performance_metrics']['steps_per_second']:.1f}")
    print(f"  Action efficiency: {final_stats['performance_metrics']['action_efficiency']:.3f}")
    print(f"  Exploration rate: {final_stats['performance_metrics']['exploration_rate']:.3f}")
    
    print("\nBrain Statistics:")
    brain_stats = final_stats['brain_stats']
    print(f"  Neurons: {brain_stats.get('neuron_count', 'N/A')}")
    print(f"  Synapses: {brain_stats.get('synapse_count', 'N/A')}")
    print(f"  Firing neurons: {brain_stats.get('firing_neurons', 'N/A')}")
    print(f"  Total spikes: {brain_stats.get('total_spikes', 'N/A')}")
    
    print("\nAction Planner Statistics:")
    action_planner_stats = final_stats['action_planner_stats']
    print(f"  Planning depth: {action_planner_stats.get('planning_depth', 'N/A')}")
    print(f"  Active goals: {action_planner_stats.get('active_goals_count', 'N/A')}")
    print(f"  Planning history: {action_planner_stats.get('planning_history_count', 'N/A')}")
    
    print("\nSocial Learning Statistics:")
    social_stats = final_stats['social_stats']
    print(f"  Observations: {social_stats.get('observations_count', 'N/A')}")
    print(f"  Knowledge bank size: {social_stats.get('knowledge_bank_size', 'N/A')}")
    print(f"  Social memory size: {social_stats.get('social_memory_size', 'N/A')}")
    
    print("\nBehavior Controller Statistics:")
    behavior_stats = final_stats['behavior_stats']
    print(f"  Current strategy: {behavior_stats.get('current_strategy', 'N/A')}")
    print(f"  Total experiments: {behavior_stats.get('total_experiments', 'N/A')}")
    print(f"  Total actions: {behavior_stats.get('total_actions', 'N/A')}")
    
    # Save system
    nlm_system.save_system('advanced_nlm_system.json')
    print(f"\nSystem saved to 'advanced_nlm_system.json'")
    
    print(f"\n{'='*50}")
    print("=== Advanced NLM System Demonstration Complete ===")
