"""
NLM Custom Agent Implementation Examples

This example demonstrates how to implement custom agents using the enhanced
NLM Python bindings, extending the basic functionality for specialized use cases.
"""

from pynlm import AdvancedBrain, Config, World, AgentBrain, NLMTypes, Action, SensoryPercept
import numpy as np
from typing import List, Dict, Any, Optional
import time


class ReactiveAgent(AgentBrain):
    """Simple reactive agent that responds to sensory input."""
    
    def __init__(self, brain: AdvancedBrain):
        super().__init__(brain)
        self.action_history: List[Action] = []
        self.reward_history: List[float] = []
        self.observation_history: List[SensoryPercept] = []
        
        # Agent parameters
        self.exploration_rate = 0.1
        self.max_energy = 100.0
        self.energy_decay = 0.01
    
    def get_action(self, percept: SensoryPercept) -> Action:
        """
        Get action based on current percept.
        
        Args:
            percept: Current sensory percept
            
        Returns:
            Action for the agent to execute
        """
        # Extract sensory features
        vision = percept.vision
        
        # Simple reactive rules
        if vision and hasattr(vision, 'data'):
            # If there's high-value resource nearby, move toward it
            if np.random.random() < self.exploration_rate:
                return Action(NLMTypes.ActionType.EXPLORE)
            else:
                return Action(NLMTypes.ActionType.WAIT)
        
        return Action(NLMTypes.ActionType.WAIT)
    
    def learn_from_experience(self, reward: float):
        """Update agent parameters based on experience."""
        self.reward_history.append(reward)
        
        # Simple reinforcement learning update
        if len(self.reward_history) > 10:
            recent_avg = np.mean(self.reward_history[-10:])
            if recent_avg > 5.0:
                self.exploration_rate = max(0.01, self.exploration_rate * 0.95)
            else:
                self.exploration_rate = min(0.5, self.exploration_rate * 1.05)


class GoalDrivenAgent(AgentBrain):
    """Agent with specific goals and planning capabilities."""
    
    def __init__(self, brain: AdvancedBrain, goal_position: tuple):
        super().__init__(brain)
        self.goal_position = goal_position
        self.path_planner = PathPlanner()
        self.current_action: Optional[Action] = None
        self.steps_since_goal = 0
    
    def plan_path(self, current_pos: tuple) -> List[tuple]:
        """
        Plan a path to the goal.
        
        Args:
            current_pos: Current agent position (x, y)
            
        Returns:
            List of waypoints to goal
        """
        return self.path_planner.find_path(current_pos, self.goal_position)
    
    def get_action(self, percept: SensoryPercept) -> Action:
        """
        Get action based on planned path.
        
        Args:
            percept: Current sensory percept
            
        Returns:
            Action for the agent to execute
        """
        # Get current position from world (would need to be implemented)
        current_pos = (0.0, 0.0)  # Placeholder
        
        # Get planned path
        path = self.plan_path(current_pos)
        
        if path and len(path) > 0:
            next_waypoint = path[0]
            
            # Determine action based on relative position
            dx = next_waypoint[0] - current_pos[0]
            dy = next_waypoint[1] - current_pos[1]
            
            if abs(dx) > abs(dy):
                if dx > 0:
                    return Action(NLMTypes.ActionType.MOVE_RIGHT)
                else:
                    return Action(NLMTypes.ActionType.MOVE_LEFT)
            else:
                if dy > 0:
                    return Action(NLMTypes.ActionType.MOVE_UP)
                else:
                    return Action(NLMTypes.ActionType.MOVE_DOWN)
        
        return Action(NLMTypes.ActionType.WAIT)
    
    def is_goal_reached(self, current_pos: tuple) -> bool:
        """
        Check if goal has been reached.
        
        Args:
            current_pos: Current agent position (x, y)
            
        Returns:
            True if goal reached, False otherwise
        """
        distance = ((current_pos[0] - self.goal_position[0]) ** 2 + 
                   (current_pos[1] - self.goal_position[1]) ** 2) ** 0.5
        return distance < 5.0


class PathPlanner:
    """Simple path planning for agents."""
    
    def find_path(self, start: tuple, goal: tuple) -> List[tuple]:
        """
        Find a path from start to goal using simple A* algorithm.
        
        Args:
            start: Starting position (x, y)
            goal: Goal position (x, y)
            
        Returns:
            List of waypoints from start to goal
        """
        # Simplified A* implementation
        open_list = [start]
        closed_list = []
        came_from = {}
        
        g_score = {start: 0}
        f_score = {start: self.heuristic(start, goal)}
        
        while open_list:
            current = min(open_list, key=lambda x: f_score.get(x, float('inf')))
            
            if current == goal:
                return self.reconstruct_path(came_from, current)
            
            open_list.remove(current)
            closed_list.append(current)
            
            # Generate neighbors (simplified movement)
            neighbors = [
                (current[0] + 1, current[1]),
                (current[0] - 1, current[1]),
                (current[0], current[1] + 1),
                (current[0], current[1] - 1)
            ]
            
            for neighbor in neighbors:
                if neighbor in closed_list:
                    continue
                
                tentative_g = g_score[current] + 1
                
                if neighbor not in g_score or tentative_g < g_score[neighbor]:
                    came_from[neighbor] = current
                    g_score[neighbor] = tentative_g
                    f_score[neighbor] = tentative_g + self.heuristic(neighbor, goal)
                    
                    if neighbor not in open_list:
                        open_list.append(neighbor)
        
        return []  # No path found
    
    def heuristic(self, a: tuple, b: tuple) -> float:
        """Heuristic function (Euclidean distance)."""
        return ((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2) ** 0.5
    
    def reconstruct_path(self, came_from: Dict, current: tuple) -> List[tuple]:
        """Reconstruct path from start to goal."""
        path = [current]
        while current in came_from:
            current = came_from[current]
            path.append(current)
        
        path.reverse()
        return path


class LearningAgent(AgentBrain):
    """Agent that learns from experience using simple RL."""
    
    def __init__(self, brain: AdvancedBrain, state_size: int, action_size: int):
        super().__init__(brain)
        self.state_size = state_size
        self.action_size = action_size
        
        # Q-learning components
        self.q_table: Dict[str, List[float]] = {}
        self.exploration_rate = 1.0
        self.decay_rate = 0.995
        self.min_exploration = 0.01
        
        # Experience replay
        self.memory: List[tuple] = []
        self.max_memory_size = 1000
        
        # Learning parameters
        self.gamma = 0.99  # discount factor
        self.alpha = 0.1   # learning rate
    
    def get_state_key(self, percept: SensoryPercept) -> str:
        """Convert percept to state key for Q-table."""
        # Extract features from percept
        # This is a simplified version - would need to match C++ percept structure
        features = []
        
        if percept.vision:
            # Add vision features (simplified)
            features.append(len(percept.vision))
        
        # Add other sensory features as needed
        
        return str(tuple(features))
    
    def get_action(self, percept: SensoryPercept) -> Action:
        """
        Get action using epsilon-greedy policy.
        
        Args:
            percept: Current sensory percept
            
        Returns:
            Action for the agent to execute
        """
        state = self.get_state_key(percept)
        
        # Initialize Q-table for state if not exists
        if state not in self.q_table:
            self.q_table[state] = [0.0] * self.action_size
        
        # Epsilon-greedy action selection
        if np.random.random() < self.exploration_rate:
            action_idx = np.random.randint(self.action_size)
        else:
            action_idx = np.argmax(self.q_table[state])
        
        # Map action index to ActionType
        action_types = list(NLMTypes.ActionType)
        if action_idx < len(action_types):
            return Action(action_types[action_idx])
        else:
            return Action(NLMTypes.ActionType.WAIT)
    
    def learn(self, state: str, action: Action, reward: float, next_state: str, done: bool):
        """
        Update Q-table based on experience.
        
        Args:
            state: Current state
            action: Action taken
            reward: Reward received
            next_state: Next state
            done: Whether episode ended
        """
        # Get action index
        action_types = list(NLMTypes.ActionType)
        try:
            action_idx = action_types.index(action.type)
        except ValueError:
            action_idx = 0
        
        # Initialize Q-table entries
        if state not in self.q_table:
            self.q_table[state] = [0.0] * self.action_size
        
        if next_state not in self.q_table:
            self.q_table[next_state] = [0.0] * self.action_size
        
        # Q-learning update
        current_q = self.q_table[state][action_idx]
        
        if done:
            target_q = reward
        else:
            target_q = reward + self.gamma * max(self.q_table[next_state])
        
        # Update Q-value
        self.q_table[state][action_idx] += self.alpha * (target_q - current_q)
        
        # Decay exploration rate
        self.exploration_rate = max(self.min_exploration, 
                                   self.exploration_rate * self.decay_rate)
    
    def remember(self, state: str, action: Action, reward: float, next_state: str, done: bool):
        """Store experience in memory."""
        self.memory.append((state, action, reward, next_state, done))
        
        # Trim memory if too large
        if len(self.memory) > self.max_memory_size:
            self.memory.pop(0)
    
    def replay(self, batch_size: int = 32):
        """Replay experiences for learning."""
        if len(self.memory) < batch_size:
            return
        
        # Sample random batch from memory
        batch = np.random.choice(len(self.memory), batch_size, replace=False)
        
        for i in batch:
            state, action, reward, next_state, done = self.memory[i]
            self.learn(state, action, reward, next_state, done)


def custom_reactive_example():
    """Example of using custom reactive agent."""
    print("=== Custom Reactive Agent Example ===\n")
    
    # Create simulation
    brain, world, agent_brain = create_simulation()
    
    # Create custom reactive agent
    reactive_agent = ReactiveAgent(brain)
    
    # Setup world
    world.configure(width=50.0, height=50.0)
    
    print("Created Reactive Agent:")
    print(f"  Exploration rate: {reactive_agent.exploration_rate}")
    print(f"  Max energy: {reactive_agent.max_energy}")
    
    # Simulate some agent interactions
    print("\nSimulating agent interactions...")
    
    # Mock sensory input
    percept = SensoryPercept()
    percept.vision = "mock_vision_data"
    
    # Get action based on percept
    action = reactive_agent.get_action(percept)
    print(f"  Action from percept: {action.get_name()}")
    
    # Simulate reward and learning
    reward = 10.0
    reactive_agent.learn_from_experience(reward)
    print(f"  After reward {reward}, exploration rate: {reactive_agent.exploration_rate:.3f}")


def custom_goal_driven_example():
    """Example of using custom goal-driven agent."""
    print("\n=== Custom Goal-Driven Agent Example ===\n")
    
    # Create simulation
    brain, world, agent_brain = create_simulation()
    
    # Create custom goal-driven agent
    goal_position = (25.0, 25.0)  # Center of world
    goal_agent = GoalDrivenAgent(brain, goal_position)
    
    # Setup world
    world.configure(width=50.0, height=50.0)
    
    print("Created Goal-Driven Agent:")
    print(f"  Goal position: {goal_agent.goal_position}")
    
    # Simulate path planning
    print("\nSimulating path planning...")
    
    # Mock current position
    current_position = (0.0, 0.0)
    
    # Plan path
    path = goal_agent.plan_path(current_position)
    print(f"  Planned path: {path}")
    
    # Get action
    percept = SensoryPercept()
    action = goal_agent.get_action(percept)
    print(f"  Action based on path: {action.get_name()}")
    
    # Check if goal reached
    is_reached = goal_agent.is_goal_reached(current_position)
    print(f"  Goal reached: {is_reached}")


def custom_learning_agent_example():
    """Example of using custom learning agent."""
    print("\n=== Custom Learning Agent Example ===\n")
    
    # Create simulation
    brain, world, agent_brain = create_simulation()
    
    # Create custom learning agent
    learning_agent = LearningAgent(brain, state_size=10, action_size=5)
    
    print("Created Learning Agent:")
    print(f"  State size: {learning_agent.state_size}")
    print(f"  Action size: {learning_agent.action_size}")
    print(f"  Initial exploration rate: {learning_agent.exploration_rate}")
    
    # Simulate learning episodes
    print("\nSimulating learning episodes...")
    
    # Mock experience
    state = learning_agent.get_state_key(SensoryPercept())
    action = learning_agent.get_action(SensoryPercept())
    
    print(f"  Initial state: {state}")
    print(f"  Initial action: {action.get_name()}")
    
    # Simulate some learning
    for episode in range(5):
        # Get new percept
        percept = SensoryPercept()
        
        # Get action
        action = learning_agent.get_action(percept)
        
        # Simulate reward
        reward = np.random.uniform(0, 10)
        
        # Store experience
        next_state = learning_agent.get_state_key(percept)
        learning_agent.remember(state, action, reward, next_state, False)
        
        print(f"  Episode {episode + 1}: State={state[:20]}..., Action={action.get_name()}, Reward={reward:.2f}")
        
        # Learn from experience
        learning_agent.replay(batch_size=1)
        
        state = next_state
    
    print(f"\nFinal Q-table size: {len(learning_agent.q_table)} states")
    print(f"Final exploration rate: {learning_agent.exploration_rate:.3f}")


def agent_extensibility_example():
    """Example of agent extensibility and composition."""
    print("\n=== Agent Extensibility Example ===\n")
    
    # Create simulation
    brain, world, agent_brain = create_simulation()
    
    # Create base agent
    base_agent = ReactiveAgent(brain)
    
    # Extend agent with additional capabilities
    class EnhancedReactiveAgent(ReactiveAgent):
        """Enhanced reactive agent with memory and decision making."""
        
        def __init__(self, brain: AdvancedBrain):
            super().__init__(brain)
            self.decision_history: List[Dict[str, Any]] = []
            self.memory_decay = 0.9
        
        def make_decision(self, percept: SensoryPercept) -> Action:
            """Make decision based on percept and history."""
            # Store current decision
            decision = {
                'percept': percept,
                'timestamp': time.time(),
                'action': self.get_action(percept)
            }
            
            self.decision_history.append(decision)
            
            # Trim history if too large
            if len(self.decision_history) > 100:
                self.decision_history.pop(0)
            
            return decision['action']
        
        def get_action_with_context(self, percept: SensoryPercept) -> Action:
            """Get action considering recent decisions."""
            # Get basic action
            action = self.get_action(percept)
            
            # Consider recent successful actions
            if self.decision_history:
                recent_actions = [d['action'] for d in self.decision_history[-10:]]
                # Simple strategy: repeat successful action types
                # This is a simplified example
                
            return action
    
    # Create enhanced agent
    enhanced_agent = EnhancedReactiveAgent(brain)
    
    print("Created Enhanced Reactive Agent:")
    print(f"  Inherits from: ReactiveAgent")
    print(f"  Additional capabilities: Decision history, Context-aware actions")
    
    # Demonstrate extensibility
    print("\nDemonstrating extensibility:")
    
    percept = SensoryPercept()
    percept.vision = "enhanced_percept"
    
    action = enhanced_agent.get_action_with_context(percept)
    print(f"  Action with context: {action.get_name()}")
    
    decision = enhanced_agent.make_decision(percept)
    print(f"  Stored decision: {decision['action'].get_name()}")
    print(f"  Decision history size: {len(enhanced_agent.decision_history)}")


def main():
    """Run all custom agent examples."""
    print("NLM Custom Agent Implementation Examples")
    print("=" * 60)
    print()
    
    custom_reactive_example()
    custom_goal_driven_example()
    custom_learning_agent_example()
    agent_extensibility_example()
    
    print("\n" + "=" * 60)
    print("All custom agent examples completed successfully!")
    print("\nCustom agents allow you to:")
    print("  - Implement specialized behavior for different scenarios")
    print("  - Combine multiple agent capabilities")
    print("  - Create learning agents with reinforcement learning")
    print("  - Design agents with specific goals and planning")
    print("  - Extend agent functionality through inheritance")


if __name__ == "__main__":
    main()