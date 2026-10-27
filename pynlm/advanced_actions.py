"""
pynlm - Advanced Actions and Motor Control

Advanced action handling, motor command generation, and behavior control
systems for the NLM Python interface.
"""

import numpy as np
from typing import List, Dict, Any, Optional, Callable
from enum import Enum
import time
class ActionType(Enum):
    """Advanced action type enumeration."""
    # Basic actions
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    
    # Environmental actions
    INTERACT = "interact"
    PICKUP = "pickup"
    DROP = "drop"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    
    # Advanced actions
    EXPLORE = "explore"
    AVOID = "avoid"
    FOLLOW = "follow"
    HIDE = "hide"
    GUARD = "guard"
    BUILD = "build"
    DIG = "dig"
    FLY = "fly"
    SWIM = "swim"
    CLIMB = "climb"
    
    # Social actions
    SHARE = "share"
    TRADE = "trade"
    ATTACK = "attack"
    DEFEND = "defend"
    
    # Meta actions
    REPEAT = "repeat"
    PAUSE = "pause"
    RESUME = "resume"
    RESET = "reset"
    CUSTOM = "custom"
class ActionPriority(Enum):
    """Action priority levels."""
    LOW = 1
    MEDIUM = 2
    HIGH = 3
    CRITICAL = 4
class ActionContext:
    """Context information for action selection."""
    
    def __init__(self, world: Any, brain: Any, agent: Any, step: int):
        self.world = world
        self.brain = brain
        self.agent = agent
        self.step = step
        self.time = world.getSimulationTime()
        self.position = world.getAgentBody()
        self.percept = world.getSensoryPercept()
        
    def get_vision_data(self) -> np.ndarray:
        """Get vision data as numpy array."""
        # This would extract and format vision data for easier processing
        vision = self.percept.getVision()
        if vision:
            return np.array(vision.getData()).reshape(
                vision.getHeight(), vision.getWidth(), vision.getChannels()
            )
        return np.zeros((0, 0, 0))
    
    def get_nearby_objects(self, radius: float = 5.0) -> List[Any]:
        """Get objects within a certain radius of the agent."""
        # This would scan the world for nearby objects
        # Simplified implementation
        nearby = []
        
        # Check agent body position
        agent_x, agent_y = self.position.x, self.position.y
        
        # This would iterate through world objects
        # For now, return empty list
        return nearby
    
    def is_safe_to_move(self, x: float, y: float) -> bool:
        """Check if a position is safe to move to."""
        # Check for walls
        if not self.world.isValidPosition(x, y):
            return False
        
        # Check for hazards
        # This would check if the position contains hazards
        
        return True
class ActionSequence:
    """Manage sequences of actions with conditional execution."""
    
    def __init__(self, actions: List[Dict[str, Any]]):
        self.actions = actions
        self.current_index = 0
        self.repetition_count = 0
        self.max_repetitions = 1
        self.loop_forever = False
        self.conditions_met = True
        
    def add_action(self, action: ActionType, parameters: Dict[str, Any] = None,
                   condition: Callable[[ActionContext], bool] = None,
                   delay: float = 0.0):
        """Add an action to the sequence."""
        action_data = {
            'type': action,
            'parameters': parameters or {},
            'condition': condition,
            'delay': delay
        }
        self.actions.append(action_data)
    
    def add_loop(self, actions: List[Dict[str, Any]], times: int = 1):
        """Add a loop of actions."""
        loop_action = {
            'type': ActionType.CUSTOM,
            'parameters': {'actions': actions, 'times': times},
            'condition': None,
            'delay': 0.0,
            'is_loop': True
        }
        self.actions.append(loop_action)
    
    def get_next_action(self, context: ActionContext) -> Optional[Dict[str, Any]]:
        """Get the next action to execute."""
        if self.current_index >= len(self.actions):
            # Check if we should loop
            if self.loop_forever:
                self.current_index = 0
                return self.get_next_action(context)
            elif self.repetition_count < self.max_repetitions:
                self.current_index = 0
                self.repetition_count += 1
                return self.get_next_action(context)
            else:
                return None
        
        action_data = self.actions[self.current_index]
        
        # Check condition if present
        if action_data['condition'] and not action_data['condition'](context):
            self.current_index += 1
            return self.get_next_action(context)
        
        # Wait for delay if specified
        if action_data['delay'] > 0:
            time.sleep(action_data['delay'])
        
        action_info = {
            'action': action_data['type'],
            'parameters': action_data['parameters'],
            'index': self.current_index
        }
        
        self.current_index += 1
        return action_info
    
    def reset(self):
        """Reset the action sequence."""
        self.current_index = 0
        self.repetition_count = 0
class ActionPlanner:
    """Plan sequences of actions using neural network and heuristic methods."""
    
    def __init__(self, agent: Any, world: Any, brain: Any):
        self.agent = agent
        self.world = world
        self.brain = brain
        self.action_history: List[Dict[str, Any]] = []
        self.planning_horizon = 10  # Number of steps to plan ahead
        
    def create_sequence_from_vision(self, vision_data: np.ndarray) -> List[ActionType]:
        """Create action sequence based on vision data using heuristics."""
        
        # Analyze vision data to determine likely actions
        height, width, channels = vision_data.shape
        
        # Simple heuristics based on visual features
        actions = []
        
        # Check for obstacles (dark areas in vision)
        obstacle_pixels = np.mean(vision_data, axis=2) < 0.3
        if np.any(obstacle_pixels):
            actions.append(ActionType.AVOID)
        
        # Check for resources (bright colored areas)
        if channels >= 3:
            red_pixels = np.mean(vision_data[:, :, 0], axis=1) > 0.7
            green_pixels = np.mean(vision_data[:, :, 1], axis=1) > 0.7
            blue_pixels = np.mean(vision_data[:, :, 2], axis=1) > 0.7
            
            if np.any(red_pixels):
                actions.append(ActionType.EAT)
            if np.any(green_pixels):
                actions.append(ActionType.REST)
            if np.any(blue_pixels):
                actions.append(ActionType.INTERACT)
        
        # Check for empty space (uniform brightness)
        brightness = np.mean(vision_data, axis=2)
        uniform_pixels = np.abs(brightness - np.mean(brightness)) < 0.1
        if np.any(uniform_pixels):
            actions.append(ActionType.EXPLORE)
        
        # Default action if no clear direction
        if not actions:
            actions.append(ActionType.WAIT)
        
        return actions
    
    def plan_path(self, start_x: float, start_y: float, 
                   target_x: float, target_y: float) -> List[tuple]:
        """Plan a path to a target using simple heuristics."""
        
        path = []
        current_x, current_y = start_x, start_y
        
        # Simple A* like pathfinding
        max_steps = 50
        step_count = 0
        
        while step_count < max_steps:
            if abs(current_x - target_x) < 1.0 and abs(current_y - target_y) < 1.0:
                break
            
            # Calculate direction to target
            dx = target_x - current_x
            dy = target_y - current_y
            distance = np.sqrt(dx**2 + dy**2)
            
            if distance == 0:
                break
            
            # Move in direction of target
            step_x = dx / distance
            step_y = dy / distance
            
            # Choose best step based on cost function
            best_step = self._evaluate_step(current_x + step_x, current_y + step_y)
            
            current_x += best_step[0]
            current_y += best_step[1]
            path.append((current_x, current_y))
            
            step_count += 1
        
        return path
    
    def _evaluate_step(self, x: float, y: float) -> tuple:
        """Evaluate a potential step based on cost function."""
        
        # Check if step is valid
        if not self.world.isValidPosition(x, y):
            return (0, 0)  # Stay in place
        
        # Simple cost: prefer moving closer to agent goal
        # This would be enhanced with neural network predictions
        return (x, y)
    
    def record_action(self, action: ActionType, context: ActionContext, 
                     success: bool, reward: float):
        """Record an action for learning and planning."""
        
        action_record = {
            'action': action,
            'context': {
                'position': (context.position.x, context.position.y),
                'time': context.time,
                'step': context.step
            },
            'success': success,
            'reward': reward,
            'timestamp': time.time()
        }
        
        self.action_history.append(action_record)
        
        # Limit history size
        if len(self.action_history) > 1000:
            self.action_history = self.action_history[-1000:]
    
    def get_action_statistics(self) -> Dict[str, Any]:
        """Get statistics of recorded actions."""
        
        if not self.action_history:
            return {}
        
        stats = {
            'total_actions': len(self.action_history),
            'unique_actions': len(set(record['action'] for record in self.action_history)),
            'success_rate': sum(1 for record in self.action_history if record['success']) / len(self.action_history),
            'average_reward': sum(record['reward'] for record in self.action_history) / len(self.action_history),
            'most_frequent_action': max(set(record['action'] for record in self.action_history), 
                                      key=lambda a: sum(1 for record in self.action_history if record['action'] == a))
        }
        
        return stats
class BehaviorTree:
    """Behavior tree for complex action selection and control."""
    
    def __init__(self):
        self.root: Optional[Node] = None
        self.current_node: Optional[Node] = None
        
    def add_root(self, node: 'Node'):
        """Set the root node of the behavior tree."""
        self.root = node
        self.current_node = node
    
    def update(self, context: ActionContext) -> Optional[ActionType]:
        """Update the behavior tree and return action if one is selected."""
        if not self.root:
            return None
            
        if self.current_node is None:
            self.current_node = self.root
        
        result = self.current_node.execute(context)
        
        if result == NodeStatus.COMPLETED:
            self.current_node = self.current_node.get_next_node()
            if self.current_node is None:
                self.current_node = self.root
        elif result == NodeStatus.FAILED:
            self.current_node = self.current_node.get_parent() or self.root
        
        return self.current_node.get_action()
    
    def reset(self):
        """Reset the behavior tree."""
        self.current_node = self.root
class Node:
    """Base class for behavior tree nodes."""
    
    def __init__(self, name: str):
        self.name = name
        self.parent: Optional['Node'] = None
        self.children: List['Node'] = []
    
    def add_child(self, child: 'Node'):
        """Add a child node."""
        child.parent = self
        self.children.append(child)
    
    def get_next_node(self) -> Optional['Node']:
        """Get the next node in sequence."""
        if self.children:
            return self.children[0]
        return self.parent.get_next_node() if self.parent else None
    
    def get_parent(self) -> Optional['Node']:
        """Get parent node."""
        return self.parent
    
    def get_action(self) -> Optional[ActionType]:
        """Get action for this node."""
        return None
    
    def execute(self, context: ActionContext) -> 'NodeStatus':
        """Execute the node."""
        raise NotImplementedError()
class ActionNode(Node):
    """Node that executes an action."""
    
    def __init__(self, name: str, action: ActionType, parameters: Dict[str, Any] = None):
        super().__init__(name)
        self.action = action
        self.parameters = parameters or {}
    
    def execute(self, context: ActionContext) -> 'NodeStatus':
        """Execute action node."""
        # This would apply the action to the world
        # For now, just return completed
        return NodeStatus.COMPLETED
    
    def get_action(self) -> ActionType:
        return self.action
class CompositeNode(Node):
    """Node with children nodes."""
    
    def __init__(self, name: str, child_type: str = 'sequence'):
        super().__init__(name)
        self.child_type = child_type  # 'sequence', 'selector', 'parallel'
    
    def execute(self, context: ActionContext) -> 'NodeStatus':
        """Execute composite node."""
        if self.child_type == 'sequence':
            for child in self.children:
                result = child.execute(context)
                if result == NodeStatus.FAILED:
                    return NodeStatus.FAILED
                elif result == NodeStatus.RUNNING:
                    return NodeStatus.RUNNING
            return NodeStatus.COMPLETED
        elif self.child_type == 'selector':
            for child in self.children:
                result = child.execute(context)
                if result == NodeStatus.COMPLETED:
                    return NodeStatus.COMPLETED
                elif result == NodeStatus.RUNNING:
                    return NodeStatus.RUNNING
            return NodeStatus.FAILED
        else:  # parallel
            running_children = [child for child in self.children if child.execute(context) == NodeStatus.RUNNING]
            if not running_children:
                return NodeStatus.COMPLETED
            elif all(child.execute(context) == NodeStatus.COMPLETED for child in self.children):
                return NodeStatus.COMPLETED
            else:
                return NodeStatus.RUNNING
class NodeStatus(Enum):
    """Node execution status."""
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
def create_advanced_behavior_tree() -> BehaviorTree:
    """Create an advanced behavior tree for the agent."""
    
    tree = BehaviorTree()
    
    # Root sequence node
    root = CompositeNode("root", 'sequence')
    
    # Check for obstacles first
    avoid_obstacles = CompositeNode("avoid_obstacles", 'selector')
    avoid_obstacles.add_child(ActionNode("check_vision", ActionType.AVOID))
    avoid_obstacles.add_child(ActionNode("plan_path", ActionType.MOVE_FORWARD))
    root.add_child(avoid_obstacles)
    
    # Then explore for resources
    explore = CompositeNode("explore", 'selector')
    explore.add_child(ActionNode("look_for_resources", ActionType.EAT))
    explore.add_child(ActionNode("rest_if_needed", ActionType.REST))
    explore.add_child(ActionNode("interact_with_object", ActionType.INTERACT))
    root.add_child(explore)
    
    # Finally, random wander
    wander = CompositeNode("wander", 'selector')
    wander.add_child(ActionNode("move_forward", ActionType.MOVE_FORWARD))
    wander.add_child(ActionNode("turn_randomly", ActionType.TURN_LEFT))
    wander.add_child(ActionNode("wait", ActionType.WAIT))
    root.add_child(wander)
    
    tree.add_root(root)
    return tree
def create_action_sequence_from_context(context: ActionContext) -> List[Dict[str, Any]]:
    """Create an action sequence based on context analysis."""
    
    sequence = []
    
    # Analyze context to determine actions
    vision = context.get_vision_data()
    
    # Simple rule-based action selection
    if vision.size > 0:
        # Check for obstacles in vision
        if np.any(vision < 0.2):
            sequence.append({
                'action': ActionType.AVOID,
                'parameters': {'intensity': 0.8},
                'condition': lambda ctx: ctx.get_vision_data().mean() < 0.3
            })
        
        # Check for resources
        if vision[..., 0].mean() > 0.7:  # Red channel dominant
            sequence.append({
                'action': ActionType.EAT,
                'parameters': {'value': 10},
                'condition': lambda ctx: ctx.get_vision_data().shape[0] > 0
            })
        
        # Check for empty space
        if np.std(vision) < 0.2:  # Low contrast = empty space
            sequence.append({
                'action': ActionType.EXPLORE,
                'parameters': {'direction': 'forward'},
                'condition': lambda ctx: ctx.get_vision_data().shape[1] > 0
            })
    
    # Add default action if sequence is empty
    if not sequence:
        sequence.append({
            'action': ActionType.WAIT,
            'parameters': {},
            'condition': lambda ctx: True
        })
    
    return sequence
# Re-export commonly used classes and functions
__all__ = [
    'ActionType', 'ActionPriority', 'ActionContext', 'ActionSequence', 
    'ActionPlanner', 'BehaviorTree', 'Node', 'ActionNode', 'CompositeNode',
    'NodeStatus', 'create_advanced_behavior_tree', 'create_action_sequence_from_context'
]
