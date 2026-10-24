"""
Agent module - Agent functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import time

from .pynlm import (
    AgentBrain as AgentBrainCore, AgentBody as AgentBodyCore, SensoryPercept as SensoryPerceptCore,
    Brain as BrainCore, Action as ActionCore, SimpleWorld as WorldCore, SensoryInput as SensoryInputCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    NeuronType, RegionId, PopulationId, ActionType, ObservationData, Neuron
)

__all__ = [
    'AgentBrain', 'AgentBody', 'SensoryPercept',
    'Brain', 'Action',
    'SimpleWorld', 'Environment', 'Observation',
    'SensoryInput',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'PlasticityFlags', 'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
    'Neuron',
]

class SensoryPercept:
    """Enhanced sensory perception with preprocessing and feature extraction"""
    
    def __init__(self, data: ObservationData, timestamp: Timestamp = 0):
        """Initialize sensory perception
        
        Args:
            data: Observed sensory data
            timestamp: Observation timestamp
        """
        self.data = data
        self.timestamp = timestamp
        self.features = self._extract_features(data)
        self.confidence = self._calculate_confidence(data)
    
    def _extract_features(self, data: ObservationData) -> Dict[str, float]:
        """Extract features from sensory data
        
        Args:
            data: Sensory data
            
        Returns:
            Dictionary of extracted features
        """
        features = {}
        
        # Basic statistics
        if data.size() > 0:
            values = data.getData()
            features['mean'] = sum(values) / len(values)
            features['std'] = (sum((x - features['mean']) ** 2 for x in values) / len(values)) ** 0.5
            features['min'] = min(values)
            features['max'] = max(values)
            features['range'] = features['max'] - features['min']
            
            # Higher-order features
            if features['std'] > 0:
                features['skew'] = sum((x - features['mean']) ** 3 for x in values) / (len(values) * features['std'] ** 3)
                features['kurtosis'] = sum((x - features['mean']) ** 4 for x in values) / (len(values) * features['std'] ** 4) - 3
            else:
                features['skew'] = 0.0
                features['kurtosis'] = -3.0
            
            # Temporal features
            features['trend'] = self._calculate_trend(values)
            features['variation'] = self._calculate_variation(values)
        else:
            # Default features for empty data
            for key in ['mean', 'std', 'min', 'max', 'range', 'skew', 'kurtosis', 'trend', 'variation']:
                features[key] = 0.0
        
        return features
    
    def _calculate_trend(self, values: List[float]) -> float:
        """Calculate trend in values
        
        Args:
            values: List of values
            
        Returns:
            Trend value (-1 to 1, negative to positive trend)
        """
        if len(values) < 2:
            return 0.0
        
        x = list(range(len(values)))
        n = len(values)
        
        numerator = n * sum(x[i] * values[i] for i in range(n)) - sum(x) * sum(values)
        denominator = n * sum(x[i] ** 2 for i in range(n)) - sum(x) ** 2
        
        if denominator == 0:
            return 0.0
        
        return numerator / denominator
    
    def _calculate_variation(self, values: List[float]) -> float:
        """Calculate variation in values
        
        Args:
            values: List of values
            
        Returns:
            Variation value
        """
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
    
    def _calculate_confidence(self, data: ObservationData) -> float:
        """Calculate confidence in sensory perception
        
        Args:
            data: Sensory data
            
        Returns:
            Confidence value (0 to 1)
        """
        values = data.getData()
        
        # Confidence based on data quality
        confidence = 1.0
        
        # Reduce confidence if data has outliers
        if values:
            mean = sum(values) / len(values)
            std = (sum((x - mean) ** 2 for x in values) / len(values)) ** 0.5
            
            # Outlier detection
            threshold = 3.0 * std if std > 0 else 1.0
            outliers = sum(1 for x in values if abs(x - mean) > threshold)
            
            if outliers > 0:
                confidence *= (1.0 - outliers / len(values)) ** 2
        
        return max(0.0, min(1.0, confidence))
    
    def get_feature(self, feature_name: str) -> float:
        """Get specific feature
        
        Args:
            feature_name: Name of feature
            
        Returns:
            Feature value or 0.0 if not found
        """
        return self.features.get(feature_name, 0.0)
    
    def get_all_features(self) -> Dict[str, float]:
        """Get all features
        
        Returns:
            Dictionary of all features
        """
        return self.features.copy()
    
    def get_data(self) -> ObservationData:
        """Get original sensory data
        
        Returns:
            Original observation data
        """
        return self.data

class Action:
    """Enhanced action with metadata and effects"""
    
    def __init__(self, action_type: ActionType, parameters: Optional[Dict[str, float]] = None):
        """Initialize action
        
        Args:
            action_type: Type of action
            parameters: Optional action parameters
        """
        self.action_type = action_type
        self.parameters = parameters or {}
        self.timestamp = time.time()
        self.effects = self._calculate_effects()
        self.priority = self._calculate_priority()
    
    def _calculate_effects(self) -> Dict[str, float]:
        """Calculate effects of action
        
        Returns:
            Dictionary of action effects
        """
        effects = {}
        
        # Basic effects based on action type
        if self.action_type == ActionType::MoveForward:
            effects['position_change'] = 1.0
            effects['energy_cost'] = 0.1
        elif self.action_type == ActionType::MoveBackward:
            effects['position_change'] = -1.0
            effects['energy_cost'] = 0.1
        elif self.action_type == ActionType::MoveLeft:
            effects['position_change'] = -1.0
            effects['energy_cost'] = 0.1
        elif self.action_type == ActionType::MoveRight:
            effects['position_change'] = 1.0
            effects['energy_cost'] = 0.1
        elif self.action_type == ActionType::TurnLeft:
            effects['orientation_change'] = -90.0
            effects['energy_cost'] = 0.05
        elif self.action_type == ActionType::TurnRight:
            effects['orientation_change'] = 90.0
            effects['energy_cost'] = 0.05
        elif self.action_type == ActionType::Look:
            effects['perception_range'] = 10.0
            effects['energy_cost'] = 0.01
        elif self.action_type == ActionType::Interact:
            effects['interaction_strength'] = 1.0
            effects['energy_cost'] = 0.2
        elif self.action_type == ActionType::Wait:
            effects['no_change'] = True
            effects['energy_cost'] = 0.0
        
        # Scale effects based on parameters
        for key, value in self.parameters.items():
            if key in effects:
                effects[key] *= value
        
        return effects
    
    def _calculate_priority(self) -> float:
        """Calculate action priority
        
        Returns:
            Priority value (0 to 1)
        """
        priority = 0.5  # Base priority
        
        # Adjust based on action type
        if self.action_type == ActionType::Wait:
            priority *= 0.1
        elif self.action_type == ActionType::Look:
            priority *= 0.8
        elif self.action_type == ActionType::Interact:
            priority *= 0.9
        
        # Adjust based on parameters
        if 'urgency' in self.parameters:
            priority *= self.parameters['urgency']
        
        return max(0.0, min(1.0, priority))
    
    def get_effect(self, effect_name: str) -> float:
        """Get specific effect
        
        Args:
            effect_name: Name of effect
            
        Returns:
            Effect value or 0.0 if not found
        """
        return self.effects.get(effect_name, 0.0)
    
    def get_all_effects(self) -> Dict[str, float]:
        """Get all effects
        
        Returns:
            Dictionary of all effects
        """
        return self.effects.copy()
    
    def get_priority(self) -> float:
        """Get action priority
        
        Returns:
            Priority value
        """
        return self.priority
    
    def get_parameter(self, param_name: str) -> float:
        """Get specific parameter
        
        Args:
            param_name: Name of parameter
            
        Returns:
            Parameter value or 0.0 if not found
        """
        return self.parameters.get(param_name, 0.0)
    
    def get_all_parameters(self) -> Dict[str, float]:
        """Get all parameters
        
        Returns:
            Dictionary of all parameters
        """
        return self.parameters.copy()

class AgentBody:
    """Agent body with physical properties and capabilities"""
    
    def __init__(self, position: Tuple[float, float] = (0.0, 0.0), 
                 orientation: float = 0.0, energy: float = 100.0):
        """Initialize agent body
        
        Args:
            position: Initial position (x, y)
            orientation: Initial orientation in degrees
            energy: Initial energy
        """
        self.position = position
        self.orientation = orientation
        self.energy = energy
        self.max_energy = energy
        self.velocity = (0.0, 0.0)
        self.acceleration = (0.0, 0.0)
        self.sensor_range = 10.0
        self.motor_range = 5.0
        self.inventory = {}
    
    def move(self, dx: float, dy: float, dt: float = 1.0):
        """Move agent
        
        Args:
            dx: Change in x position
            dy: Change in y position
            dt: Time step
        """
        # Update velocity based on acceleration
        vx = self.velocity[0] + self.acceleration[0] * dt
        vy = self.velocity[1] + self.acceleration[1] * dt
        self.velocity = (vx, vy)
        
        # Update position
        new_x = self.position[0] + dx + vx * dt
        new_y = self.position[1] + dy + vy * dt
        self.position = (new_x, new_y)
        
        # Calculate energy cost
        distance = (dx ** 2 + dy ** 2) ** 0.5
        energy_cost = distance * 0.1  # Energy cost per unit distance
        self.energy = max(0.0, self.energy - energy_cost)
    
    def turn(self, angle: float):
        """Turn agent
        
        Args:
            angle: Angle to turn in degrees
        """
        self.orientation += angle
        self.orientation %= 360.0
        
        # Energy cost for turning
        energy_cost = abs(angle) * 0.01
        self.energy = max(0.0, self.energy - energy_cost)
    
    def update_energy(self, delta: float):
        """Update agent energy
        
        Args:
            delta: Energy change
        """
        self.energy = max(0.0, min(self.max_energy, self.energy + delta))
    
    def has_energy(self, amount: float) -> bool:
        """Check if agent has enough energy
        
        Args:
            amount: Required energy
            
        Returns:
            True if agent has enough energy
        """
        return self.energy >= amount

class AgentBrain:
    """Enhanced agent brain with learning and decision making"""
    
    def __init__(self, brain: BrainCore, body: AgentBody, world: Optional['SimpleWorld'] = None):
        """Initialize agent brain
        
        Args:
            brain: Core brain instance
            body: Agent body instance
            world: Optional world instance
        """
        self.brain = brain
        self.body = body
        self.world = world
        self.sensory_percepts = []
        self.action_history = []
        self.reward_history = []
        self.memory = {}
        self.planning_horizon = 5
        self.exploration_rate = 0.1
        
        # Learning parameters
        self.learning_rate = 0.01
        self.discount_factor = 0.95
        self.epsilon = 0.1
    
    def perceive(self) -> SensoryPercept:
        """Perceive environment
        
        Returns:
            Sensory perception
        """
        if self.world:
            observation = self.world.get_observation(self.body.position, self.body.sensor_range)
            percept = SensoryPercept(observation)
            self.sensory_percepts.append(percept)
            return percept
        else:
            return SensoryPercept(ObservationData())  # Empty observation
    
    def decide(self, percept: Optional[SensoryPercept] = None) -> Action:
        """Decide on action
        
        Args:
            percept: Optional sensory perception
            
        Returns:
            Decided action
        """
        if percept is None:
            percept = self.perceive()
        
        # Simple decision making with exploration
        action_type = self._select_action( percept)
        parameters = self._get_action_parameters(action_type, percept)
        
        action = Action(action_type, parameters)
        self.action_history.append((percept, action))
        
        return action
    
    def _select_action(self, percept: SensoryPercept) -> ActionType:
        """Select action based on percept and learning
        
        Args:
            percept: Sensory perception
            
        Returns:
            Selected action type
        """
        # Simple policy: choose action based on features
        features = percept.get_all_features()
        
        # Extract relevant features for decision
        mean_val = features.get('mean', 0.0)
        trend = features.get('trend', 0.0)
        
        # Simple heuristic decision making
        if mean_val < -0.5:
            return ActionType::MoveForward if self.exploration_rate > 0.5 else ActionType::Wait
        elif mean_val > 0.5:
            return ActionType::MoveBackward if self.exploration_rate > 0.5 else ActionType::Wait
        elif trend < -0.3:
            return ActionType::TurnLeft if self.exploration_rate > 0.3 else ActionType::Wait
        elif trend > 0.3:
            return ActionType::TurnRight if self.exploration_rate > 0.3 else ActionType::Wait
        else:
            # Exploration
            return self._choose_random_action()
    
    def _choose_random_action(self) -> ActionType:
        """Choose random action (for exploration)
        
        Returns:
            Random action type
        """
        import random
        actions = [
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::MoveLeft, ActionType::MoveRight,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Look, ActionType::Interact, ActionType::Wait
        ]
        return random.choice(actions)
    
    def _get_action_parameters(self, action_type: ActionType, percept: SensoryPercept) -> Dict[str, float]:
        """Get action parameters based on percept
        
        Args:
            action_type: Action type
            percept: Sensory perception
            
        Returns:
            Action parameters
        """
        parameters = {}
        
        # Extract features for parameter determination
        features = percept.get_all_features()
        
        if action_type in [ActionType::MoveForward, ActionType::MoveBackward,
                          ActionType::MoveLeft, ActionType::MoveRight]:
            # Movement parameters
            distance = abs(features.get('mean', 0.0))
            parameters['distance'] = min(distance, 2.0)
            parameters['urgency'] = 1.0 - abs(features.get('trend', 0.0))
        
        elif action_type in [ActionType::TurnLeft, ActionType::TurnRight]:
            # Turning parameters
            angle = features.get('kurtosis', 0.0) * 45.0
            parameters['angle'] = angle
            parameters['urgency'] = 1.0 - abs(features.get('skew', 0.0))
        
        elif action_type == ActionType::Look:
            # Look parameters
            parameters['range'] = features.get('max', 10.0)
            parameters['focus'] = abs(features.get('mean', 0.0))
        
        return parameters
    
    def act(self, percept: Optional[SensoryPercept] = None) -> Tuple[Action, Optional[float]]:
        """Perform action
        
        Args:
            percept: Optional sensory perception
            
        Returns:
            Tuple of (action, reward)
        """
        # Decide on action
        action = self.decide(percept)
        
        # Perform action if possible
        reward = None
        
        if self.world:
            # Check if action is valid
            if self._is_action_valid(action):
                # Execute action in world
                reward = self._execute_action(action)
                self.reward_history.append(reward)
        
        return action, reward
    
    def _is_action_valid(self, action: Action) -> bool:
        """Check if action is valid
        
        Args:
            action: Action to check
            
        Returns:
            True if action is valid
        """
        # Check energy
        if not self.body.has_energy(action.get_effect('energy_cost', 0.0)):
            return False
        
        # Check other constraints based on action type
        if action.action_type == ActionType::MoveForward:
            # Check if movement is possible
            new_pos = (self.body.position[0] + 1.0, self.body.position[1])
            return self._is_position_valid(new_pos)
        
        elif action.action_type == ActionType::Look:
            return True  # Looking is always possible
        
        else:
            return True  # Other actions are assumed valid
    
    def _execute_action(self, action: Action) -> float:
        """Execute action in world
        
        Args:
            action: Action to execute
            
        Returns:
            Reward for action
        """
        if not self._is_action_valid(action):
            return -1.0  # Penalty for invalid action
        
        # Execute action effects
        energy_cost = action.get_effect('energy_cost', 0.0)
        self.body.update_energy(-energy_cost)
        
        # Apply action-specific effects
        if action.action_type == ActionType::MoveForward:
            self.body.move(1.0, 0.0)
            reward = self._calculate_movement_reward()
        elif action.action_type == ActionType::MoveBackward:
            self.body.move(-1.0, 0.0)
            reward = self._calculate_movement_reward()
        elif action.action_type == ActionType::MoveLeft:
            self.body.move(0.0, -1.0)
            reward = self._calculate_movement_reward()
        elif action.action_type == ActionType::MoveRight:
            self.body.move(0.0, 1.0)
            reward = self._calculate_movement_reward()
        elif action.action_type == ActionType::TurnLeft:
            self.body.turn(-90.0)
            reward = 0.0  # No reward for turning
        elif action.action_type == ActionType::TurnRight:
            self.body.turn(90.0)
            reward = 0.0  # No reward for turning
        elif action.action_type == ActionType::Look:
            reward = self._calculate_perception_reward()
        elif action.action_type == ActionType::Interact:
            reward = self._calculate_interaction_reward()
        elif action.action_type == ActionType::Wait:
            reward = self._calculate_wait_reward()
        
        return reward
    
    def _calculate_movement_reward(self) -> float:
        """Calculate reward for movement
        
        Returns:
            Movement reward
        """
        # Simple reward: positive for being in good region
        percept = self.sensory_percepts[-1] if self.sensory_percepts else SensoryPercept(ObservationData())
        mean_val = percept.get_feature('mean')
        
        return mean_val * 0.1  # Scale based on perceptual mean
    
    def _calculate_perception_reward(self) -> float:
        """Calculate reward for perception
        
        Returns:
            Perception reward
        """
        # Reward for interesting observations
        percept = self.sensory_percepts[-1] if self.sensory_percepts else SensoryPercept(ObservationData())
        features = percept.get_all_features()
        
        # Reward for variance and trends
        std = features.get('std', 0.0)
        trend = abs(features.get('trend', 0.0))
        
        return (std + trend) * 0.05
    
    def _calculate_interaction_reward(self) -> float:
        """Calculate reward for interaction
        
        Returns:
            Interaction reward
        """
        return 0.5  # Base reward for interaction
    
    def _calculate_wait_reward(self) -> float:
        """Calculate reward for waiting
        
        Returns:
            Wait reward
        """
        return 0.0  # Neutral reward for waiting
    
    def learn(self, percept: SensoryPercept, action: Action, reward: float):
        """Learn from experience
        
        Args:
            percept: Sensory perception
            action: Action taken
            reward: Reward received
        """
        # Store in memory
        self.memory[(tuple(percept.get_all_features().items()), action.action_type)] = reward
        
        # Simple learning update
        # In a real implementation, this would use actual learning algorithms
        # For now, it's a placeholder for future implementation
    
    def get_experience(self) -> List[Tuple[SensoryPercept, Action, float]]:
        """Get recent experience
        
        Returns:
            List of experiences (percept, action, reward)
        """
        return list(zip(self.sensory_percepts, 
                       [action for _, action in self.action_history],
                       self.reward_history))
    
    def reset(self):
        """Reset agent state"""
        self.sensory_percepts.clear()
        self.action_history.clear()
        self.reward_history.clear()
        self.memory.clear()

# Convenience functions
def create_agent(brain: BrainCore, body: Optional[AgentBody] = None, 
                world: Optional['SimpleWorld'] = None) -> AgentBrain:
    """Create an agent
    
    Args:
        brain: Core brain instance
        body: Agent body instance
        world: World instance
        
    Returns:
        AgentBrain instance
    """
    if body is None:
        body = AgentBody()
    
    return AgentBrain(brain, body, world)

class AgentUtils:
    """Utility functions for agent operations"""
    
    @staticmethod
    def create_body(position: Tuple[float, float] = (0.0, 0.0), 
                    orientation: float = 0.0, energy: float = 100.0) -> AgentBody:
        """Create agent body
        
        Args:
            position: Initial position (x, y)
            orientation: Initial orientation in degrees
            energy: Initial energy
            
        Returns:
            AgentBody instance
        """
        return AgentBody(position, orientation, energy)
    
    @staticmethod
    def create_percept(data: ObservationData, timestamp: Timestamp = 0) -> SensoryPercept:
        """Create sensory perception
        
        Args:
            data: Observation data
            timestamp: Observation timestamp
            
        Returns:
            SensoryPercept instance
        """
        return SensoryPercept(data, timestamp)
    
    @staticmethod
    def create_action(action_type: ActionType, parameters: Optional[Dict[str, float]] = None) -> Action:
        """Create action
        
        Args:
            action_type: Action type
            parameters: Action parameters
            
        Returns:
            Action instance
        """
        return Action(action_type, parameters)
    
    @staticmethod
    def evaluate_action(action: Action, context: Dict[str, Any]) -> float:
        """Evaluate action in given context
        
        Args:
            action: Action to evaluate
            context: Evaluation context
            
        Returns:
            Action evaluation score
        """
        score = 0.5  # Base score
        
        # Adjust based on action effects
        effects = action.get_all_effects()
        for effect_name, effect_value in effects.items():
            if effect_name in context.get('preferred_effects', {}):
                score += effect_value * context['preferred_effects'][effect_name] * 0.1
        
        # Adjust based on action parameters
        parameters = action.get_all_parameters()
        for param_name, param_value in parameters.items():
            if param_name in context.get('preferred_parameters', {}):
                score += param_value * context['preferred_parameters'][param_name] * 0.1
        
        return max(0.0, min(1.0, score))
