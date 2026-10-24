"""
Motor module - Motor control functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import math
import time

from .pynlm import (
    MotorSystem as MotorSystemCore, Action as ActionCore, Agent as AgentCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    MembranePotential, SynapticWeight, FiringRate, FiringState, NeuronType,
    RegionId, PopulationId, ActionType, ObservationData, NeuralRegion
)

__all__ = [
    'MotorSystem', 'Action', 'Agent',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
    'NeuralRegion',
]

class Action:
    """Enhanced action with detailed effects and parameters"""
    
    def __init__(self, action_type: ActionType, parameters: Optional[Dict[str, float]] = None,
                 target: Optional[Tuple[float, float]] = None):
        """Initialize action
        
        Args:
            action_type: Type of action
            parameters: Optional action parameters
            target: Optional target position
        """
        self.action_type = action_type
        self.parameters = parameters or {}
        self.target = target
        self.timestamp = time.time()
        self.effects = self._calculate_effects()
        self.priority = self._calculate_priority()
        self.execution_time = self._calculate_execution_time()
    
    def _calculate_effects(self) -> Dict[str, float]:
        """Calculate effects of action
        
        Returns:
            Dictionary of action effects
        """
        effects = {}
        
        # Basic effects based on action type
        if self.action_type == ActionType::MoveForward:
            effects['translation_x'] = 1.0
            effects['translation_y'] = 0.0
            effects['energy_cost'] = 0.1
            effects['speed'] = 1.0
            effects['acceleration'] = 0.5
            effects['has_momentum'] = True
        elif self.action_type == ActionType::MoveBackward:
            effects['translation_x'] = -1.0
            effects['translation_y'] = 0.0
            effects['energy_cost'] = 0.1
            effects['speed'] = 0.8
            effects['acceleration'] = 0.5
            effects['has_momentum'] = True
        elif self.action_type == ActionType::MoveLeft:
            effects['translation_x'] = 0.0
            effects['translation_y'] = -1.0
            effects['energy_cost'] = 0.1
            effects['speed'] = 1.0
            effects['acceleration'] = 0.5
            effects['has_momentum'] = True
        elif self.action_type == ActionType::MoveRight:
            effects['translation_x'] = 0.0
            effects['translation_y'] = 1.0
            effects['energy_cost'] = 0.1
            effects['speed'] = 1.0
            effects['acceleration'] = 0.5
            effects['has_momentum'] = True
        elif self.action_type == ActionType::TurnLeft:
            effects['rotation'] = -90.0
            effects['angular_velocity'] = -1.0
            effects['energy_cost'] = 0.05
            effects['turn_speed'] = 1.0
            effects['has_momentum'] = True
        elif self.action_type == ActionType::TurnRight:
            effects['rotation'] = 90.0
            effects['angular_velocity'] = 1.0
            effects['energy_cost'] = 0.05
            effects['turn_speed'] = 1.0
            effects['has_momentum'] = True
        elif self.action_type == ActionType::Look:
            effects['view_range'] = 10.0
            effects['field_of_view'] = 90.0
            effects['energy_cost'] = 0.01
            effects['scan_speed'] = 1.0
            effects['has_momentum'] = False
        elif self.action_type == ActionType::Interact:
            effects['interaction_strength'] = 1.0
            effects['energy_cost'] = 0.2
            effects['interaction_range'] = 5.0
            effects['has_momentum'] = False
        elif self.action_type == ActionType::Wait:
            effects['no_change'] = True
            effects['energy_cost'] = 0.0
            effects['has_momentum'] = False
        
        # Scale effects based on parameters
        for key, value in self.parameters.items():
            if key in effects:
                effects[key] *= value
        
        # Adjust based on target
        if self.target is not None:
            tx, ty = self.target
            distance = math.sqrt(tx * tx + ty * ty)
            if distance > 0:
                effects['translation_x'] *= tx / distance
                effects['translation_y'] *= ty / distance
        
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
        
        # Adjust based on target distance
        if self.target is not None:
            tx, ty = self.target
            distance = math.sqrt(tx * tx + ty * ty)
            if distance > 10:
                priority *= 1.2  # More urgent for distant targets
        
        return max(0.0, min(1.0, priority))
    
    def _calculate_execution_time(self) -> float:
        """Calculate action execution time
        
        Returns:
            Execution time in seconds
        """
        base_time = 0.1  # Base execution time
        
        # Adjust based on action type
        if self.action_type == ActionType::Wait:
            return 0.0  # No time for waiting
        elif self.action_type == ActionType::Look:
            return base_time * 0.5  # Fast for looking
        
        # Adjust based on parameters
        if 'speed' in self.parameters:
            base_time /= self.parameters['speed']
        
        return base_time
    
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
    
    def get_execution_time(self) -> float:
        """Get action execution time
        
        Returns:
            Execution time
        """
        return self.execution_time
    
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
    
    def get_action_type(self) -> ActionType:
        """Get action type
        
        Returns:
            Action type
        """
        return self.action_type
    
    def get_target(self) -> Optional[Tuple[float, float]]:
        """Get target position
        
        Returns:
            Target position or None
        """
        return self.target

class MotorSystem:
    """Enhanced motor system with learning and adaptation"""
    
    def __init__(self, agent: Optional['Agent'] = None, learning_rate: float = 0.01):
        """Initialize motor system
        
        Args:
            agent: Optional agent instance
            learning_rate: Learning rate
        """
        self.agent = agent
        self.learning_rate = learning_rate
        self.action_history = []
        self.reward_history = []
        self.policy = {}
        self.value_function = {}
        self.exploration_rate = 0.1
        self.adaptation_rate = 0.01
    
    def select_action(self, percept: Optional[Dict[str, Any]] = None) -> Action:
        """Select action based on perception
        
        Args:
            percept: Optional perception
            
        Returns:
            Selected action
        """
        # Get available actions
        available_actions = self._get_available_actions()
        
        if not available_actions:
            return Action(ActionType::Wait)
        
        # Choose action based on policy
        if percept is not None and len(self.policy) > 0:
            # Use learned policy
            action_type = self._choose_from_policy(percept, available_actions)
        else:
            # Random choice for exploration
            action_type = self._choose_random_action(available_actions)
        
        # Get action parameters
        parameters = self._get_action_parameters(action_type, percept)
        
        # Get target if needed
        target = self._get_target(action_type, percept)
        
        action = Action(action_type, parameters, target)
        self.action_history.append((percept, action))
        
        return action
    
    def execute_action(self, action: Action) -> Tuple[float, Dict[str, Any]]:
        """Execute action
        
        Args:
            action: Action to execute
            
        Returns:
            Tuple of (reward, outcome)
        """
        # Check if action is valid
        if not self._is_action_valid(action):
            reward = -1.0
            outcome = {'success': False, 'reason': 'invalid_action'}
            return reward, outcome
        
        # Execute action
        effects = action.get_all_effects()
        outcome = self._apply_action_effects(action, effects)
        
        # Calculate reward
        reward = self._calculate_action_reward(action, outcome)
        
        self.reward_history.append(reward)
        
        return reward, outcome
    
    def learn(self, percept: Dict[str, Any], action: Action, reward: float,
              next_percept: Optional[Dict[str, Any]] = None):
        """Learn from experience
        
        Args:
            percept: Current perception
            action: Action taken
            reward: Reward received
            next_percept: Next perception
        """
        # Update policy
        self._update_policy(percept, action, reward, next_percept)
        
        # Update value function
        self._update_value_function(percept, action, reward)
    
    def _get_available_actions(self) -> List[ActionType]:
        """Get available actions
        
        Returns:
            List of available action types
        """
        return [
            ActionType::MoveForward, ActionType::MoveBackward,
            ActionType::MoveLeft, ActionType::MoveRight,
            ActionType::TurnLeft, ActionType::TurnRight,
            ActionType::Look, ActionType::Interact, ActionType::Wait
        ]
    
    def _choose_from_policy(self, percept: Dict[str, Any], available_actions: List[ActionType]) -> ActionType:
        """Choose action based on policy
        
        Args:
            percept: Current perception
            available_actions: Available actions
            
        Returns:
            Chosen action type
        """
        # Convert percept to feature vector
        features = self._percept_to_features(percept)
        
        # Get action values from policy
        action_values = []
        for action_type in available_actions:
            key = tuple(features.items())
            value = self.policy.get(key, {}).get(action_type, 0.0)
            action_values.append((action_type, value))
        
        # Choose best action or explore
        if action_values and len(action_values) > 0:
            # Sort by value
            action_values.sort(key=lambda x: x[1], reverse=True)
            
            # Choose with exploration
            if time.time() % (1.0 / self.exploration_rate) < (1.0 / 100.0):  # Exploration chance
                return self._choose_random_action(available_actions)
            
            return action_values[0][0]
        
        return self._choose_random_action(available_actions)
    
    def _choose_random_action(self, available_actions: List[ActionType]) -> ActionType:
        """Choose random action
        
        Args:
            available_actions: Available actions
            
        Returns:
            Random action type
        """
        import random
        return random.choice(available_actions)
    
    def _get_action_parameters(self, action_type: ActionType, percept: Optional[Dict[str, Any]]) -> Dict[str, float]:
        """Get action parameters
        
        Args:
            action_type: Action type
            percept: Current perception
            
        Returns:
            Action parameters
        """
        parameters = {}
        
        if percept is None:
            return parameters
        
        # Extract features
        features = self._percept_to_features(percept)
        
        # Set parameters based on action type and features
        if action_type in [ActionType::MoveForward, ActionType::MoveBackward,
                          ActionType::MoveLeft, ActionType::MoveRight]:
            # Movement parameters
            distance = abs(features.get('distance_to_target', 0.0))
            parameters['distance'] = min(distance, 2.0)
            parameters['urgency'] = 1.0 - abs(features.get('uncertainty', 0.0))
        
        elif action_type in [ActionType::TurnLeft, ActionType::TurnRight]:
            # Turning parameters
            angle = features.get('angle_to_target', 0.0)
            parameters['angle'] = angle
            parameters['urgency'] = 1.0 - abs(features.get('uncertainty', 0.0))
        
        elif action_type == ActionType::Look:
            # Look parameters
            parameters['range'] = features.get('view_range', 10.0)
            parameters['focus'] = features.get('interest_level', 0.5)
        
        return parameters
    
    def _get_target(self, action_type: ActionType, percept: Optional[Dict[str, Any]]) -> Optional[Tuple[float, float]]:
        """Get action target
        
        Args:
            action_type: Action type
            percept: Current perception
            
        Returns:
            Target position or None
        """
        if percept is None:
            return None
        
        features = self._percept_to_features(percept)
        
        if 'target_position' in features:
            return (features['target_position_x'], features['target_position_y'])
        
        return None
    
    def _is_action_valid(self, action: Action) -> bool:
        """Check if action is valid
        
        Args:
            action: Action to check
            
        Returns:
            True if action is valid
        """
        if self.agent is None:
            return True
        
        # Check agent energy
        energy_cost = action.get_effect('energy_cost', 0.0)
        if not self.agent.has_energy_for_action(action, energy_cost):
            return False
        
        # Check action constraints
        effects = action.get_all_effects()
        
        # Check if target is reachable
        if 'translation_x' in effects and 'translation_y' in effects:
            tx = effects['translation_x']
            ty = effects['translation_y']
            
            # Check if target is within world bounds
            if self.agent.world is not None:
                world_width = self.agent.world.get_width()
                world_height = self.agent.world.get_height()
                
                if abs(tx) > world_width or abs(ty) > world_height:
                    return False
        
        return True
    
    def _apply_action_effects(self, action: Action, effects: Dict[str, float]) -> Dict[str, Any]:
        """Apply action effects
        
        Args:
            action: Action
            effects: Action effects
            
        Returns:
            Outcome
        """
        outcome = {
            'success': True,
            'effects': effects,
            'position_change': (0.0, 0.0),
            'rotation_change': 0.0,
            'energy_change': -effects.get('energy_cost', 0.0)
        }
        
        # Apply position changes
        if 'translation_x' in effects and 'translation_y' in effects:
            tx = effects['translation_x']
            ty = effects['translation_y']
            outcome['position_change'] = (tx, ty)
        
        # Apply rotation changes
        if 'rotation' in effects:
            outcome['rotation_change'] = effects['rotation']
        
        return outcome
    
    def _calculate_action_reward(self, action: Action, outcome: Dict[str, Any]) -> float:
        """Calculate action reward
        
        Args:
            action: Action taken
            outcome: Action outcome
            
        Returns:
            Action reward
        """
        reward = 0.0
        
        # Base reward for successful action
        if outcome['success']:
            reward += 0.1
        
        # Add reward for position changes
        position_change = outcome['position_change']
        distance_moved = math.sqrt(position_change[0] ** 2 + position_change[1] ** 2)
        reward += distance_moved * 0.01
        
        # Add reward for approaching targets
        if self.agent is not None and self.agent.target is not None:
            current_pos = self.agent.position
            target_pos = self.agent.target
            current_dist = math.sqrt((current_pos[0] - target_pos[0]) ** 2 + 
                                   (current_pos[1] - target_pos[1]) ** 2)
            
            new_dist = math.sqrt((current_pos[0] + position_change[0] - target_pos[0]) ** 2 +
                               (current_pos[1] + position_change[1] - target_pos[1]) ** 2)
            
            if new_dist < current_dist:
                reward += (current_dist - new_dist) * 0.1
        
        return reward
    
    def _percept_to_features(self, percept: Dict[str, Any]) -> Dict[str, float]:
        """Convert perception to feature vector
        
        Args:
            percept: Perception dictionary
            
        Returns:
            Feature vector
        """
        features = {}
        
        if percept is None:
            return features
        
        # Extract features from perception
        for key, value in percept.items():
            if isinstance(value, (int, float)):
                features[key] = float(value)
            elif isinstance(value, dict):
                # Flatten nested dictionaries
                for nested_key, nested_value in value.items():
                    if isinstance(nested_value, (int, float)):
                        features[f"{key}_{nested_key}"] = float(nested_value)
        
        return features
    
    def _update_policy(self, percept: Dict[str, Any], action: Action, reward: float,
                       next_percept: Optional[Dict[str, Any]] = None):
        """Update policy based on experience
        
        Args:
            percept: Current perception
            action: Action taken
            reward: Reward received
            next_percept: Next perception
        """
        # Simple policy update (placeholder for actual RL algorithm)
        features = self._percept_to_features(percept)
        key = tuple(features.items())
        
        if key not in self.policy:
            self.policy[key] = {}
        
        if action.action_type not in self.policy[key]:
            self.policy[key][action.action_type] = 0.0
        
        # Update policy (simplified)
        old_value = self.policy[key][action.action_type]
        new_value = old_value + self.learning_rate * (reward - old_value)
        self.policy[key][action.action_type] = new_value
    
    def _update_value_function(self, percept: Dict[str, Any], action: Action, reward: float):
        """Update value function
        
        Args:
            percept: Current perception
            action: Action taken
            reward: Reward received
        """
        features = self._percept_to_features(percept)
        key = tuple(features.items())
        
        if key not in self.value_function:
            self.value_function[key] = 0.0
        
        # Simple value function update
        self.value_function[key] = self.value_function[key] + self.adaptation_rate * (reward - self.value_function[key])

class Agent:
    """Enhanced agent with sensory-motor capabilities"""
    
    def __init__(self, name: str, world: Optional['SimpleWorld'] = None,
                 position: Tuple[float, float] = (0.0, 0.0),
                 orientation: float = 0.0, energy: float = 100.0):
        """Initialize agent
        
        Args:
            name: Agent name
            world: Optional world instance
            position: Initial position
            orientation: Initial orientation
            energy: Initial energy
        """
        self.name = name
        self.world = world
        self.position = position
        self.orientation = orientation
        self.energy = energy
        self.max_energy = energy
        self.motor_system = MotorSystem(self)
        self.sensor = Sensor(range=10.0)
        self.memory = {}
        self.planning_horizon = 5
    
    def perceive(self) -> Dict[str, Any]:
        """Perceive environment
        
        Returns:
            Perception dictionary
        """
        # Update sensor
        self.sensor.scan(self.position, 
                        (self.world.width if self.world else 100.0,
                         self.world.height if self.world else 100.0))
        
        # Get sensor reading
        reading = self.sensor.get_reading()
        
        # Convert to perception
        percept = {
            'position': self.position,
            'orientation': self.orientation,
            'energy': self.energy,
            'max_energy': self.max_energy,
            'sensor_reading': reading.getData(),
            'timestamp': time.time()
        }
        
        return percept
    
    def decide(self, percept: Optional[Dict[str, Any]] = None) -> Action:
        """Decide on action
        
        Args:
            percept: Optional perception
            
        Returns:
            Decided action
        """
        if percept is None:
            percept = self.perceive()
        
        return self.motor_system.select_action(percept)
    
    def act(self, percept: Optional[Dict[str, Any]] = None) -> Tuple[Action, float]:
        """Perform action
        
        Args:
            percept: Optional perception
            
        Returns:
            Tuple of (action, reward)
        """
        if percept is None:
            percept = self.perceive()
        
        # Decide on action
        action = self.decide(percept)
        
        # Execute action
        reward, outcome = self.motor_system.execute_action(action)
        
        # Update agent state based on outcome
        self._update_from_action(action, outcome, reward)
        
        # Learn from experience
        next_percept = self.perceive()
        self.motor_system.learn(percept, action, reward, next_percept)
        
        return action, reward
    
    def _update_from_action(self, action: Action, outcome: Dict[str, Any], reward: float):
        """Update agent state from action
        
        Args:
            action: Action taken
            outcome: Action outcome
            reward: Reward received
        """
        # Update position
        position_change = outcome['position_change']
        self.position = (self.position[0] + position_change[0],
                        self.position[1] + position_change[1])
        
        # Update orientation
        if 'rotation_change' in outcome:
            self.orientation += outcome['rotation_change']
        
        # Update energy
        self.energy += outcome['energy_change']
        
        # Store in memory
        self.memory[f"action_{len(self.memory)}"] = {
            'action': action.action_type,
            'outcome': outcome,
            'reward': reward,
            'timestamp': time.time()
        }
    
    def has_energy_for_action(self, action: Action, energy_cost: float) -> bool:
        """Check if agent has enough energy for action
        
        Args:
            action: Action
            energy_cost: Energy cost
            
        Returns:
            True if agent has enough energy
        """
        return self.energy >= energy_cost
    
    def get_nearest_object(self) -> Optional[Dict[str, Any]]:
        """Get nearest object
        
        Returns:
            Nearest object or None
        """
        if self.world is None:
            return None
        
        nearest = None
        nearest_distance = float('inf')
        
        for obj in self.world.environment.objects:
            if 'position' in obj:
                obj_pos = obj['position']
                dist = math.sqrt((self.position[0] - obj_pos['x']) ** 2 +
                               (self.position[1] - obj_pos['y']) ** 2)
                
                if dist < nearest_distance:
                    nearest_distance = dist
                    nearest = obj
        
        return nearest
    
    def get_nearest_agent(self) -> Optional[Dict[str, Any]]:
        """Get nearest agent
        
        Returns:
            Nearest agent or None
        """
        if self.world is None:
            return None
        
        nearest = None
        nearest_distance = float('inf')
        
        for agent in self.world.environment.agents:
            if agent.get('id') != self.name and 'position' in agent:
                agent_pos = agent['position']
                dist = math.sqrt((self.position[0] - agent_pos['x']) ** 2 +
                               (self.position[1] - agent_pos['y']) ** 2)
                
                if dist < nearest_distance:
                    nearest_distance = dist
                    nearest = agent
        
        return nearest

# Convenience functions
def create_motor_system(agent: Optional['Agent'] = None, learning_rate: float = 0.01) -> MotorSystem:
    """Create motor system
    
    Args:
        agent: Optional agent instance
        learning_rate: Learning rate
        
    Returns:
        MotorSystem instance
    """
    return MotorSystem(agent, learning_rate)

def create_agent(name: str, world: Optional['SimpleWorld'] = None,
                position: Tuple[float, float] = (0.0, 0.0),
                orientation: float = 0.0, energy: float = 100.0) -> Agent:
    """Create agent
    
    Args:
        name: Agent name
        world: World instance
        position: Initial position
        orientation: Initial orientation
        energy: Initial energy
        
    Returns:
        Agent instance
    """
    return Agent(name, world, position, orientation, energy)

class MotorUtils:
    """Utility functions for motor operations"""
    
    @staticmethod
    def create_motor_system(agent: Optional['Agent'] = None, learning_rate: float = 0.01) -> MotorSystem:
        """Create motor system
        
        Args:
            agent: Optional agent instance
            learning_rate: Learning rate
            
        Returns:
            MotorSystem instance
        """
        return MotorSystem(agent, learning_rate)
    
    @staticmethod
    def create_agent(name: str, world: Optional['SimpleWorld'] = None,
                    position: Tuple[float, float] = (0.0, 0.0),
                    orientation: float = 0.0, energy: float = 100.0) -> Agent:
        """Create agent
        
        Args:
            name: Agent name
            world: World instance
            position: Initial position
            orientation: Initial orientation
            energy: Initial energy
            
        Returns:
            Agent instance
        """
        return Agent(name, world, position, orientation, energy)
    
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
    
    @staticmethod
    def simulate_action(agent: Agent, action: Action) -> Tuple[Agent, float]:
        """Simulate action
        
        Args:
            agent: Agent
            action: Action
            
        Returns:
            Tuple of (updated agent, reward)
        """
        # Create copy of agent for simulation
        simulated_agent = Agent(agent.name, agent.world, agent.position,
                               agent.orientation, agent.energy)
        
        # Perform action
        reward = 0.0  # Simplified reward calculation
        
        return simulated_agent, reward
