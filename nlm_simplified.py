#!/usr/bin/env python3
"""
NLM Python Simplified Interface

This module provides high-level, simplified interfaces for common NLM use cases.
It abstracts away the complexity of the low-level bindings while maintaining
full access to advanced functionality when needed.

The goal is to make it easy for beginners to get started with NLM, while
still providing power users with the tools they need for complex scenarios.
"""

import nlm
from typing import List, Tuple, Optional, Callable
import math

class NLMScenario:
    """Base class for NLM scenarios with common functionality."""
    
    def __init__(self, width: int = 100, height: int = 100, 
                 vision_width: int = 32, vision_height: int = 32):
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Create NLM components
        self.config = nlm.createDefaultConfig()
        self.brain = None
        self.agent_brain = None
        self.world = None
        self.step_count = 0
        
        # Setup default configuration
        self._setup_default_config()
    
    def _setup_default_config(self):
        """Setup default configuration for the scenario."""
        self.config.setValue("neuron_count", 500)
        self.config.setValue("region_count", 1)
        self.config.setValue("connection_probability", 0.1)
        self.config.setValue("random_seed", 42)
        self.config.setValue("simulation_timestep", 0.001)
    
    def initialize(self):
        """Initialize all NLM components."""
        if self.brain is not None:
            self.brain.reset()
        else:
            self.brain = nlm.createBrain(self.config)
            self.agent_brain = nlm.createAgentBrain(self.brain)
            
            self.world = nlm.createSimpleWorld()
            self.world.configure(
                self.width, self.height, 
                self.vision_width, self.vision_height
            )
            self.world.setAgentStart(self.width // 2, self.height // 2)
            self.agent_brain.initialize(self.world)
        
        return self
    
    def step(self, reward: float = 0.0, predicted_reward: float = 0.0) -> dict:
        """Perform one simulation step."""
        if self.brain is None:
            raise RuntimeError("NLM not initialized. Call initialize() first.")
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent_brain.processSensoryInput(percept)
        
        # Generate action
        action = self.brain.produceAction()
        
        # Apply action in world
        self.world.applyMotorCommand(action, self.step_count)
        
        # Apply reward modulation
        self.agent_brain.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        self.agent_brain.updateDevelopment(0.001)
        
        # Get state information
        state = self._get_state_info()
        
        self.step_count += 1
        return state
    
    def _get_state_info(self) -> dict:
        """Get current state information."""
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        vision = percept.getVision()
        
        # Extract key features from vision
        vision_features = {
            'width': self.vision_width,
            'height': self.vision_height,
            'pixel_count': len(vision),
            'brightness_mean': sum(vision) / len(vision) if vision else 0.0,
            'brightness_min': min(vision) if vision else 0.0,
            'brightness_max': max(vision) if vision else 0.0,
        }
        
        return {
            'step': self.step_count,
            'position': (body.x, body.y),
            'orientation': body.orientation,
            'velocity': (body.velocityX, body.velocityY),
            'energy': body.energy,
            'health': body.health,
            'age': body.age,
            'vision_features': vision_features,
            'action_available': True,
        }
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file."""
        if self.brain is None:
            return False
        return self.brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file."""
        if self.brain is None:
            return False
        return self.brain.load(filepath)
class ExplorationScenario(NLMScenario):
    """Scenario for exploration and resource finding."""
    
    def __init__(self, width: int = 100, height: int = 100):
        super().__init__(width, height, 32, 32)
        self._setup_exploration_config()
    
    def _setup_exploration_config(self):
        """Setup configuration for exploration scenario."""
        self.config.setValue("neuron_count", 800)
        self.config.setValue("connection_probability", 0.15)
        self.config.setValue("stdp_ltp_weight", 0.015)
        self.config.setValue("stdp_ltd_weight", 0.018)
        self.config.setValue("enable_curiosity", True)
        self.config.setValue("enable_reward_modulation", True)
        
        # Add exploration-specific objects
        self._add_exploration_objects()
    
    def _add_exploration_objects(self):
        """Add exploration-specific objects to the world."""
        if not self.world:
            return
        
        # Add resources at random positions
        import random
        random.seed(42)  # For reproducibility
        
        for _ in range(5):
            x = random.uniform(10, self.width - 10)
            y = random.uniform(10, self.height - 10)
            self.world.addResource(x, y)
        
        # Add hazards
        for _ in range(3):
            x = random.uniform(0, self.width)
            y = random.uniform(0, self.height)
            self.world.addHazard(x, y)
        
        # Add walls to create structure
        wall_positions = [(20, 20), (80, 20), (20, 80), (80, 80)]
        for x, y in wall_positions:
            self.world.addWall(x, y, 2, 2)
class NavigationScenario(NLMScenario):
    """Scenario for navigation through complex environments."""
    
    def __init__(self, width: int = 200, height: int = 200):
        super().__init__(width, height, 64, 64)
        self._setup_navigation_config()
        self._create_maze()
    
    def _setup_navigation_config(self):
        """Setup configuration for navigation scenario."""
        self.config.setValue("neuron_count", 1200)
        self.config.setValue("region_count", 2)
        self.config.setValue("connection_probability", 0.12)
        self.config.setValue("stdp_ltp_weight", 0.012)
        self.config.setValue("stdp_ltd_weight", 0.015)
        self.config.setValue("enable_structural_plasticity", True)
        self.config.setValue("enable_development", True)
    
    def _create_maze(self):
        """Create a simple maze environment."""
        if not self.world:
            return
        
        # Create maze walls
        wall_width = 5
        # Horizontal walls
        for x in range(0, self.width, wall_width * 4):
            for y in range(0, self.height, wall_width * 2):
                self.world.addWall(x, y, wall_width, wall_width)
        
        # Vertical walls
        for x in range(0, self.width, wall_width * 2):
            for y in range(0, self.height, wall_width * 4):
                self.world.addWall(x, y, wall_width, wall_width)
        
        # Add resources in open areas
        self.world.addResource(30, 30)
        self.world.addResource(70, 30)
        self.world.addResource(30, 70)
        self.world.addResource(70, 70)
        
        # Add goal area
        self.world.addResource(150, 150, 5.0, 5.0)  # Larger resource
class LearningScenario(NLMScenario):
    """Scenario focused on learning and concept formation."""
    
    def __init__(self, width: int = 150, height: int = 150):
        super().__init__(width, height, 48, 48)
        self._setup_learning_config()
        self._add_learning_objects()
    
    def _setup_learning_config(self):
        """Setup configuration for learning scenario."""
        self.config.setValue("neuron_count", 600)
        self.config.setValue("connection_probability", 0.08)
        self.config.setValue("stdp_ltp_weight", 0.01)
        self.config.setValue("stdp_ltd_weight", 0.012)
        self.config.setValue("enable_planner", True)
        self.config.setValue("enable_concept_formation", True)
        self.config.setValue("enable_self_model", True)
    
    def _add_learning_objects(self):
        """Add objects that promote learning."""
        if not self.world:
            return
        
        # Add various object types to encourage concept learning
        # Resources (positive reinforcement)
        self.world.addResource(25, 25)
        self.world.addResource(125, 25)
        
        # Hazards (negative reinforcement)
        self.world.addHazard(75, 75)
        self.world.addHazard(125, 125)
        
        # Markers (neutral exploration targets)
        self.world.addObject(nlm.WorldObject(50, 50, nlm.WorldObjectType.Marker, 1.0, 0.5))
        self.world.addObject(nlm.WorldObject(100, 100, nlm.WorldObjectType.Marker, 1.0, 0.5))
class ReactiveAgent:
    """High-level agent that reacts to sensory input with simple policies."""
    
    def __init__(self, scenario: NLMScenario):
        self.scenario = scenario
        self.action_history = []
        self.reward_history = []
        self.resource_finder_active = False
        self.hazard_avoider_active = False
        self.exploration_mode = True
    
    def decide_action(self, state: dict) -> nlm.Action:
        """Decide action based on current state and active policies."""
        position = state['position']
        vision_features = state['vision_features']
        
        # Create default action
        action = nlm.Action.createMove(nlm.ActionType.Wait)
        
        # Apply reactive policies
        if self.hazard_avoider_active:
            action = self._avoid_hazards(state)
        
        if self.resource_finder_active:
            resource_action = self._find_resources(state)
            if resource_action.getType() != nlm.ActionType.Wait:
                action = resource_action
        
        if self.exploration_mode:
            exploration_action = self._explore(state)
            if exploration_action.getType() != nlm.ActionType.Wait:
                action = exploration_action
        
        self.action_history.append((state['step'], action))
        return action
    
    def _avoid_hazards(self, state: dict) -> nlm.Action:
        """Avoid detected hazards."""
        vision = state['vision_features']
        
        # Simple hazard avoidance logic
        if vision['brightness_min'] < 0.3:  # Dark area likely hazard
            position = state['position']
            body = self.scenario.world.getAgentBody()
            
            # Move away from dark areas
            if body.x > 50:
                return nlm.Action.createMove(nlm.ActionType.TurnLeft)
            else:
                return nlm.Action.createMove(nlm.ActionType.TurnRight)
        
        return nlm.Action.createMove(nlm.ActionType.Wait)
    
    def _find_resources(self, state: dict) -> nlm.Action:
        """Find bright resources."""
        vision = state['vision_features']
        
        # Look for bright areas (resources)
        if vision['brightness_max'] > 0.8:
            return nlm.Action.createMove(nlm.ActionType.Interact)
        
        return nlm.Action.createMove(nlm.ActionType.Wait)
    
    def _explore(self, state: dict) -> nlm.Action:
        """Simple exploration behavior."""
        # Move forward if possible
        body = self.scenario.world.getAgentBody()
        
        # Check if movement is safe
        future_x = body.x + body.velocityX * 0.1
        future_y = body.y + body.velocityY * 0.1
        
        if self.scenario.world.isValidPosition(future_x, future_y):
            return nlm.Action.createMove(nlm.ActionType.MoveForward)
        else:
            # Turn to find a safe path
            return nlm.Action.createMove(nlm.ActionType.TurnLeft)
class NLMEnvironment:
    """Environment wrapper that provides a clean interface for simulation."""
    
    def __init__(self, scenario_class, **kwargs):
        self.scenario_class = scenario_class
        self.scenario = None
        self.agent = None
        self.kwargs = kwargs
        self.is_running = False
    
    def reset(self) -> dict:
        """Reset the environment."""
        self.scenario = self.scenario_class(**self.kwargs)
        self.agent = ReactiveAgent(self.scenario)
        self.scenario.initialize()
        self.is_running = True
        return self._get_state()
    
    def step(self, action: nlm.Action) -> Tuple[dict, float, bool]:
        """Take a step in the environment."""
        if not self.is_running or not self.scenario:
            raise RuntimeError("Environment not initialized. Call reset() first.")
        
        # Apply agent's decision to world
        self.scenario.world.applyMotorCommand(action, self.scenario.step_count)
        
        # Get reward (simplified - in real usage this would come from world)
        reward = self._calculate_reward()
        
        # Advance simulation
        state = self.scenario.step(reward, 0.0)
        
        # Check if episode is done
        done = self._is_episode_done(state)
        
        return state, reward, done
    
    def _get_state(self) -> dict:
        """Get current state."""
        if not self.scenario:
            raise RuntimeError("Scenario not initialized.")
        return self.scenario._get_state_info()
    
    def _calculate_reward(self) -> float:
        """Calculate reward based on current state."""
        state = self._get_state()
        
        # Simple reward calculation
        reward = 0.0
        
        # Resource found
        if state['vision_features']['brightness_max'] > 0.7:
            reward += 10.0
        
        # Hazard encountered
        if state['vision_features']['brightness_min'] < 0.3:
            reward -= 5.0
        
        # Movement cost
        reward -= 0.1
        
        return reward
    
    def _is_episode_done(self, state: dict) -> bool:
        """Check if episode is done."""
        # Simple termination conditions
        position = state['position']
        
        # Check if agent is out of bounds
        if (position[0] < 0 or position[0] > self.scenario.width or
            position[1] < 0 or position[1] > self.scenario.height):
            return True
        
        # Check if agent has taken too many steps
        if state['step'] > 1000:
            return True
        
        return False
    
    def close(self):
        """Clean up environment."""
        self.scenario = None
        self.agent = None
        self.is_running = False
def create_exploration_agent(**kwargs) -> NLMEnvironment:
    """Create an exploration agent environment."""
    return NLMEnvironment(ExplorationScenario, **kwargs)
def create_navigation_agent(**kwargs) -> NLMEnvironment:
    """Create a navigation agent environment."""
    return NLMEnvironment(NavigationScenario, **kwargs)
def create_learning_agent(**kwargs) -> NLMEnvironment:
    """Create a learning agent environment."""
    return NLMEnvironment(LearningScenario, **kwargs)
class NLMTraining:
    """Training utilities for NLM agents."""
    
    @staticmethod
    def run_episode(scenario: NLMScenario, 
                   max_steps: int = 1000,
                   verbose: bool = False) -> dict:
        """Run a single training episode."""
        scenario.initialize()
        
        episode_stats = {
            'steps': 0,
            'total_reward': 0.0,
            'final_position': (0.0, 0.0),
            'resources_found': 0,
            'hazards_avoided': 0,
            'episode_length': 0,
        }
        
        for step in range(max_steps):
            # Simple policy: move toward bright areas, avoid dark
            state = scenario._get_state_info()
            
            # Choose action based on simple heuristics
            action = NLMTraining._simple_policy(state)
            
            # Get reward based on action result
            reward = NLMTraining._calculate_simple_reward(state, action)
            
            # Step the scenario
            state = scenario.step(reward, 0.0)
            
            # Update statistics
            episode_stats['steps'] += 1
            episode_stats['total_reward'] += reward
            episode_stats['final_position'] = state['position']
            episode_stats['episode_length'] = step + 1
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: Position {state['position']}, "
                      f"Reward {reward:.2f}, Total {episode_stats['total_reward']:.2f}")
            
            # Check for termination conditions
            if NLMTraining._should_terminate(state):
                break
        
        return episode_stats
    
    @staticmethod
    def _simple_policy(state: dict) -> nlm.Action:
        """Simple policy for decision making."""
        position = state['position']
        vision = state['vision_features']
        
        # If bright area (likely resource), try to interact
        if vision['brightness_max'] > 0.8:
            return nlm.Action.createMove(nlm.ActionType.Interact)
        
        # If dark area (likely hazard), try to move away
        if vision['brightness_min'] < 0.3:
            if position[0] < 50:  # Move right if left side
                return nlm.Action.createMove(nlm.ActionType.TurnRight)
            else:  # Move left if right side
                return nlm.Action.createMove(nlm.ActionType.TurnLeft)
        
        # Otherwise, move forward
        return nlm.Action.createMove(nlm.ActionType.MoveForward)
    
    @staticmethod
    def _calculate_simple_reward(state: dict, action: nlm.Action) -> float:
        """Calculate reward based on state and action."""
        vision = state['vision_features']
        
        reward = 0.0
        
        # Reward for finding resources
        if vision['brightness_max'] > 0.8:
            reward += 10.0
            action_type = action.getType()
            if action_type == nlm.ActionType.Interact:
                reward += 5.0  # Bonus for interacting with resource
        
        # Penalty for encountering hazards
        if vision['brightness_min'] < 0.3:
            reward -= 5.0
        
        # Small movement cost
        reward -= 0.1
        
        return reward
    
    @staticmethod
    def _should_terminate(state: dict) -> bool:
        """Check if episode should terminate."""
        position = state['position']
        
        # Out of bounds
        if (position[0] < 0 or position[0] > 100 or
            position[1] < 0 or position[1] > 100):
            return True
        
        # Too many steps
        if state['step'] > 500:
            return True
        
        return False
    
    @staticmethod
    def train_agent(scenario_class, num_episodes: int = 100, **kwargs) -> dict:
        """Train an agent using simple reinforcement learning."""
        episode_stats = []
        
        for episode in range(num_episodes):
            print(f"Training episode {episode + 1}/{num_episodes}")
            
            stats = NLMTraining.run_episode(scenario_class(**kwargs), verbose=False)
            episode_stats.append(stats)
            
            # Print progress every 10 episodes
            if (episode + 1) % 10 == 0:
                avg_reward = sum(s['total_reward'] for s in episode_stats[-10:]) / 10
                print(f"Average reward last 10 episodes: {avg_reward:.2f}")
        
        # Calculate summary statistics
        summary = {
            'total_episodes': num_episodes,
            'average_reward': sum(s['total_reward'] for s in episode_stats) / num_episodes,
            'average_steps': sum(s['episode_length'] for s in episode_stats) / num_episodes,
            'max_reward': max(s['total_reward'] for s in episode_stats),
            'min_reward': min(s['total_reward'] for s in episode_stats),
            'final_position': episode_stats[-1]['final_position'],
            'episode_stats': episode_stats,
        }
        
        return summary
# Example usage and demonstration functions
def demo_exploration():
    """Demonstrate exploration scenario."""
    print("=== NLM Exploration Demo ===")
    
    env = create_exploration_agent(width=80, height=80)
    state = env.reset()
    
    print(f"Initial position: {state['position']}")
    print(f"Vision brightness: {state['vision_features']['brightness_mean']:.2f}")
    
    for step in range(20):
        action = NLMTraining._simple_policy(state)
        state, reward, done = env.step(action)
        
        print(f"Step {step}: Action {action.getType()}, "
              f"Position {state['position']}, Reward {reward:.2f}")
        
        if done:
            print("Episode ended!")
            break
    
    print("Exploration demo completed!\n")
def demo_navigation():
    """Demonstrate navigation scenario."""
    print("=== NLM Navigation Demo ===")
    
    env = create_navigation_agent(width=150, height=150)
    state = env.reset()
    
    print(f"Agent starting at: {state['position']}")
    print(f"Goal is at: (150, 150)")
    
    for step in range(30):
        action = NLMTraining._simple_policy(state)
        state, reward, done = env.step(action)
        
        distance = ((state['position'][0] - 150) ** 2 + 
                   (state['position'][1] - 150) ** 2) ** 0.5
        
        print(f"Step {step}: Action {action.getType()}, "
              f"Pos {state['position']}, Distance to goal: {distance:.1f}, "
              f"Reward {reward:.2f}")
        
        if done:
            print("Navigation completed or failed!")
            break
    
    print("Navigation demo completed!\n")
def demo_training():
    """Demonstrate training scenario."""
    print("=== NLM Training Demo ===")
    
    summary = NLMTraining.train_agent(
        LearningScenario, 
        num_episodes=5,
        width=100, 
        height=100
    )
    
    print(f"Training completed!")
    print(f"Average reward: {summary['average_reward']:.2f}")
    print(f"Average episode length: {summary['average_steps']:.1f}")
    print(f"Final position: {summary['final_position']}")
    print("Training demo completed!\n")
if __name__ == "__main__":
    print("NLM Python Simplified Interface Demo\n")
    
    # Run demonstrations
    demo_exploration()
    demo_navigation()
    demo_training()
    
    print("All demos completed successfully!")
