#!/usr/bin/env python3
"""
World Navigation Example

Demonstrates agent navigation and interaction with environment.
This example shows pathfinding, obstacle avoidance, and goal-directed behavior.

Usage: python scripts/examples/world_navigation.py [config_file]
"""

import sys
import os
from pathlib import Path
import json
import random
import numpy as np
from queue import PriorityQueue
from typing import List, Tuple, Dict, Optional
import time
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm import NeuralLearningMachine
from nlm.memory import WorkingMemory
from nlm.novelty import NoveltyDetector


class Environment:
    """Environment for agent navigation and learning."""
    
    def __init__(self, width: float = 20.0, height: float = 20.0, 
                 obstacle_density: float = 0.3):
        """Initialize environment.
        
        Args:
            width: Environment width
            height: Environment height
            obstacle_density: Density of obstacles (0-1)
        """
        self.width = width
        self.height = height
        self.obstacle_density = obstacle_density
        self.obstacles = []
        self.goal_position = (width * 0.8, height * 0.8)
        self.agent_position = (width * 0.2, height * 0.2)
        
        # Generate random obstacles
        self._generate_obstacles()
    
    def _generate_obstacles(self):
        """Generate random obstacles in the environment."""
        num_obstacles = int(self.width * self.height * self.obstacle_density / 10)
        
        for _ in range(num_obstacles):
            size = random.uniform(1.0, 3.0)
            x = random.uniform(size/2, self.width - size/2)
            y = random.uniform(size/2, self.height - size/2)
            
            obstacle = {
                'position': (x, y),
                'size': size,
                'type': random.choice(['static', 'moving', 'dynamic']),
                'traversable': random.random() > 0.1  # 10% chance of being non-traversable
            }
            
            if obstacle['traversable']:
                self.obstacles.append(obstacle)
    
    def is_position_valid(self, x: float, y: float) -> bool:
        """Check if position is valid (not inside obstacle).
        
        Args:
            x: X position
            y: Y position
            
        Returns:
            True if position is valid
        """
        if x < 0 or x > self.width or y < 0 or y > self.height:
            return False
        
        for obstacle in self.obstacles:
            if obstacle['traversable']:
                obstacle_size = obstacle['size']
                if (abs(x - obstacle['position'][0]) < obstacle_size/2 and
                    abs(y - obstacle['position'][1]) < obstacle_size/2):
                    return False
        
        return True
    
    def get_sensory_input(self, position: Tuple[float, float]) -> Dict:
        """Get sensory input for given position.
        
        Args:
            position: Agent position
            
        Returns:
            Sensory input dictionary
        """
        x, y = position
        
        # Distance to goal
        goal_dist = np.sqrt((x - self.goal_position[0])**2 + (y - self.goal_position[1])**2)
        
        # Proximity to obstacles
        proximity_to_obstacle = float('inf')
        nearest_obstacle = None
        
        for obstacle in self.obstacles:
            if obstacle['traversable']:
                dist = np.sqrt((x - obstacle['position'][0])**2 + (y - obstacle['position'][1])**2)
                if dist < proximity_to_obstacle:
                    proximity_to_obstacle = dist
                    nearest_obstacle = obstacle
        
        # Line of sight to goal
        clear_path = self._check_line_of_sight(position, self.goal_position)
        
        # Environmental features
        sensory_input = {
            'position': position,
            'distance_to_goal': goal_dist,
            'proximity_to_obstacle': proximity_to_obstacle,
            'clear_path_to_goal': clear_path,
            'goal_position': self.goal_position,
            'obstacle_count': len(self.obstacles),
            'nearest_obstacle_type': nearest_obstacle['type'] if nearest_obstacle else None,
            'field_of_view': self._get_field_of_view(position)
        }
        
        return sensory_input
    
    def _check_line_of_sight(self, pos1: Tuple[float, float], pos2: Tuple[float, float]) -> bool:
        """Check if line of sight exists between two positions.
        
        Args:
            pos1: First position
            pos2: Second position
            
        Returns:
            True if line of sight exists
        """
        x1, y1 = pos1
        x2, y2 = pos2
        
        # Check if line intersects any obstacle
        steps = max(int(np.sqrt((x2-x1)**2 + (y2-y1)**2)), 1)
        
        for i in range(steps + 1):
            t = i / steps
            x = x1 + t * (x2 - x1)
            y = y1 + t * (y2 - y1)
            
            if not self.is_position_valid(x, y):
                return False
        
        return True
    
    def _get_field_of_view(self, position: Tuple[float, float]) -> Dict:
        """Get field of view information around position.
        
        Args:
            position: Agent position
            
        Returns:
            Field of view dictionary
        """
        x, y = position
        fov = {
            'forward': {},
            'left': {},
            'right': {},
            'backward': {}
        }
        
        # Check for obstacles in different directions
        directions = {
            'forward': (1, 0),
            'left': (0, 1),
            'right': (0, -1),
            'backward': (-1, 0)
        }
        
        for dir_name, (dx, dy) in directions.items():
            ray_distance = 5.0
            blocked = False
            
            for distance in np.linspace(0.1, ray_distance, 10):
                check_x = x + dx * distance
                check_y = y + dy * distance
                
                if not self.is_position_valid(check_x, check_y):
                    blocked = True
                    break
                
                # Check for goal in this direction
                goal_dist = np.sqrt((check_x - self.goal_position[0])**2 + 
                                  (check_y - self.goal_position[1])**2)
                if goal_dist < 1.0:
                    fov[dir_name]['goal'] = True
                    break
            
            fov[dir_name]['blocked'] = blocked
        
        return fov


class PathfindingAgent:
    """Agent that navigates through environment using pathfinding."""
    
    def __init__(self, environment: Environment, config_path: str = None):
        """Initialize pathfinding agent.
        
        Args:
            environment: Navigation environment
            config_path: Path to configuration file
        """
        self.environment = environment
        self.config_path = config_path
        self.nlm = None
        self.working_memory = None
        self.novelty = None
        self.position = environment.agent_position
        self.goal_position = environment.goal_position
        self.path = []
        self.movement_history = []
        self.novelty_history = []
        self.rewards = []
        
        # Initialize systems
        self.initialize()
    
    def initialize(self):
        """Initialize the agent and its systems."""
        print("Initializing Pathfinding Agent...")
        
        # Create NLM if config available
        if self.config_path and Path(self.config_path).exists():
            self.nlm = NeuralLearningMachine.load_config(self.config_path)
        else:
            from nlm.config import get_default_config
            config = get_default_config()
            self.nlm = NeuralLearningMachine(config)
        
        # Initialize memory and novelty systems
        self.working_memory = WorkingMemory(
            capacity=self.nlm.config.get('memory', {}).get(
                'working_memory', {}).get('working_memory_capacity', 100)
        )
        
        self.novelty = NoveltyDetector(
            sensitivity=self.nlm.config.get('neuromodulation', {}).get(
                'novelty', {}).get('sensitivity', 0.7),
            novelty_history=[]
        )
        
        print("✓ Agent systems initialized successfully")
    
    def find_path(self, start: Tuple[float, float], goal: Tuple[float, float]) -> List[Tuple[float, float]]:
        """Find path from start to goal using A* algorithm.
        
        Args:
            start: Starting position
            goal: Goal position
            
        Returns:
            List of positions representing path
        """
        # A* pathfinding implementation
        open_set = PriorityQueue()
        open_set.put((0, start))
        
        came_from = {start: None}
        g_score = {start: 0}
        f_score = {start: self.heuristic(start, goal)}
        
        while not open_set.empty():
            current = open_set.get()[1]
            
            if current == goal:
                return self.reconstruct_path(came_from, current)
            
            neighbors = self.get_neighbors(current)
            
            for neighbor in neighbors:
                tentative_g = g_score[current] + self.distance(current, neighbor)
                
                if neighbor not in g_score or tentative_g < g_score[neighbor]:
                    came_from[neighbor] = current
                    g_score[neighbor] = tentative_g
                    f_score[neighbor] = tentative_g + self.heuristic(neighbor, goal)
                    open_set.put((f_score[neighbor], neighbor))
        
        return []  # No path found
    
    def get_neighbors(self, position: Tuple[float, float]) -> List[Tuple[float, float]]:
        """Get valid neighboring positions.
        
        Args:
            position: Current position
            
        Returns:
            List of valid neighbor positions
        """
        x, y = position
        neighbors = []
        
        # Possible movements (8-direction)
        moves = [(-1, 0), (1, 0), (0, -1), (0, 1),
                (-1, -1), (-1, 1), (1, -1), (1, 1)]
        
        for dx, dy in moves:
            new_x = x + dx
            new_y = y + dy
            
            if self.environment.is_position_valid(new_x, new_y):
                neighbors.append((new_x, new_y))
        
        return neighbors
    
    def heuristic(self, pos1: Tuple[float, float], pos2: Tuple[float, float]) -> float:
        """Calculate heuristic (Euclidean distance) between two positions.
        
        Args:
            pos1: First position
            pos2: Second position
            
        Returns:
            Heuristic distance
        """
        return np.sqrt((pos1[0] - pos2[0])**2 + (pos1[1] - pos2[1])**2)
    
    def distance(self, pos1: Tuple[float, float], pos2: Tuple[float, float]) -> float:
        """Calculate distance between two positions.
        
        Args:
            pos1: First position
            pos2: Second position
            
        Returns:
            Distance
        """
        return self.heuristic(pos1, pos2)
    
    def reconstruct_path(self, came_from: Dict, current: Tuple[float, float]) -> List[Tuple[float, float]]:
        """Reconstruct path from came_from dictionary.
        
        Args:
            came_from: Dictionary mapping positions to previous positions
            current: Current position
            
        Returns:
            Reconstructed path
        """
        path = [current]
        
        while current in came_from and came_from[current] is not None:
            current = came_from[current]
            path.append(current)
        
        path.reverse()
        return path
    
    def get_sensory_input(self) -> Dict:
        """Get current sensory input.
        
        Returns:
            Sensory input dictionary
        """
        sensory_input = self.environment.get_sensory_input(self.position)
        return sensory_input
    
    def calculate_curiosity(self, sensory_input: Dict) -> float:
        """Calculate curiosity based on sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Curiosity score
        """
        # Calculate prediction error
        prediction_error = self.calculate_prediction_error(sensory_input)
        
        # Calculate novelty
        novelty_score = self.novelty.calculate_novelty(sensory_input)
        
        # Combine to get curiosity
        curiosity = 0.7 * prediction_error + 0.3 * novelty_score
        
        # Update novelty detector
        self.novelty.update_history(sensory_input)
        
        return curiosity
    
    def calculate_prediction_error(self, sensory_input: Dict) -> float:
        """Calculate prediction error for sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Prediction error value
        """
        # Get prediction from working memory
        prediction = self.working_memory.get_prediction(sensory_input)
        
        # Extract features
        features = self.extract_features(sensory_input)
        
        # Calculate error
        if prediction is not None:
            error = np.linalg.norm(np.array(features) - np.array(prediction))
        else:
            error = np.linalg.norm(np.array(features))
        
        return min(error, 1.0)
    
    def extract_features(self, sensory_input: Dict) -> List[float]:
        """Extract features from sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Feature vector
        """
        features = []
        
        # Extract position features
        pos = sensory_input['position']
        features.extend([pos[0] / self.environment.width, 
                        pos[1] / self.environment.height])
        
        # Extract goal distance
        features.append(sensory_input['distance_to_goal'] / (self.environment.width + self.environment.height))
        
        # Extract obstacle proximity
        if sensory_input['proximity_to_obstacle'] < float('inf'):
            features.append(1.0 - min(sensory_input['proximity_to_obstacle'], 10.0) / 10.0)
        else:
            features.append(0.0)
        
        # Extract field of view information
        fov = sensory_input['field_of_view']
        for direction in ['forward', 'left', 'right', 'backward']:
            features.append(1.0 if not fov[direction]['blocked'] else 0.0)
        
        # Add random exploration feature
        features.append(random.random())
        
        return features
    
    def move_toward_goal(self) -> Tuple[float, float]:
        """Move agent toward goal using pathfinding.
        
        Returns:
            New position
        """
        # Get current path
        if not self.path:
            self.path = self.find_path(self.position, self.goal_position)
        
        # If no path found, move randomly
        if not self.path or len(self.path) < 2:
            return self.random_move()
        
        # Get next position
        next_pos = self.path[1]  # Skip current position
        
        # Move toward next position
        move_vector = (next_pos[0] - self.position[0], 
                      next_pos[1] - self.position[1])
        distance = self.distance(self.position, next_pos)
        
        # Normalize move vector
        if distance > 0:
            move_vector = (move_vector[0] / distance, move_vector[1] / distance)
        
        # Apply movement with some randomness
        move_distance = distance * 0.8  # Don't move full step
        new_x = self.position[0] + move_vector[0] * move_distance
        new_y = self.position[1] + move_vector[1] * move_distance
        
        new_position = (new_x, new_y)
        
        # Update path if we've reached the next waypoint
        if distance < 0.5:
            self.path.pop(0)
        
        return new_position
    
    def random_move(self) -> Tuple[float, float]:
        """Move randomly when no path is available.
        
        Returns:
            New position
        """
        # Try 10 random movements
        for _ in range(10):
            new_x = self.position[0] + random.uniform(-1, 1)
            new_y = self.position[1] + random.uniform(-1, 1)
            
            if self.environment.is_position_valid(new_x, new_y):
                return (new_x, new_y)
        
        # If all random moves fail, stay in place
        return self.position
    
    def navigate(self, num_steps: int = 100) -> Dict:
        """Navigate through environment for given number of steps.
        
        Args:
            num_steps: Number of navigation steps
            
        Returns:
            Navigation metrics
        """
        print(f"Starting navigation for {num_steps} steps...")
        
        for step in range(num_steps):
            # Get current sensory input
            sensory_input = self.get_sensory_input()
            
            # Calculate curiosity
            curiosity = self.calculate_curiosity(sensory_input)
            
            # Decide action based on curiosity and proximity to goal
            distance_to_goal = sensory_input['distance_to_goal']
            
            if curiosity > 0.7 or distance_to_goal < 2.0:
                # High curiosity or close to goal - use pathfinding
                new_position = self.move_toward_goal()
            else:
                # Low curiosity - explore
                new_position = self.random_move()
            
            # Store in working memory
            self.working_memory.store(sensory_input, new_position, curiosity)
            
            # Calculate reward
            reward = self.calculate_reward(new_position, curiosity)
            self.rewards.append(reward)
            
            # Log movement
            self.movement_history.append({
                'step': step,
                'position': self.position,
                'new_position': new_position,
                'curiosity': curiosity,
                'reward': reward
            })
            
            self.novelty_history.append(curiosity)
            
            # Update position
            self.position = new_position
            
            # Check if goal reached
            distance_to_goal = self.environment.heuristic(self.position, self.goal_position)
            if distance_to_goal < 1.0:
                print(f"✓ Goal reached at step {step + 1}!")
                break
            
            if (step + 1) % 20 == 0:
                print(f"Step {step + 1}/{num_steps} - "
                      f"Position: ({self.position[0]:.1f}, {self.position[1]:.1f}), "
                      f"Distance to goal: {distance_to_goal:.1f}")
        
        print("✓ Navigation completed")
        self.print_summary()
        
        return {
            'final_position': self.position,
            'steps_taken': len(self.movement_history),
            'goal_reached': distance_to_goal < 1.0,
            'final_distance_to_goal': distance_to_goal,
            'avg_reward': np.mean(self.rewards) if self.rewards else 0,
            'avg_curiosity': np.mean(self.novelty_history) if self.novelty_history else 0
        }
    
    def calculate_reward(self, position: Tuple[float, float], curiosity: float) -> float:
        """Calculate reward for given position and curiosity.
        
        Args:
            position: Agent position
            curiosity: Current curiosity level
            
        Returns:
            Reward value
        """
        # Reward for approaching goal
        distance = self.environment.heuristic(position, self.goal_position)
        goal_reward = max(0, 10.0 - distance) / 10.0 * 0.6
        
        # Reward for exploration (high curiosity)
        exploration_reward = curiosity * 0.2
        
        # Reward for efficiency (less movement)
        efficiency_reward = 0.2
        
        return min(goal_reward + exploration_reward + efficiency_reward, 1.0)
    
    def create_visualization(self, output_path: str = None):
        """Create visualization of navigation path.
        
        Args:
            output_path: Output file path
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"navigation_path_{timestamp}.png"
        
        # Create plot
        fig, ax = plt.subplots(figsize=(10, 10))
        
        # Draw environment boundaries
        ax.plot([0, self.environment.width, self.environment.width, 0, 0],
               [0, 0, self.environment.height, self.environment.height, 0], 'k-', linewidth=2)
        
        # Draw obstacles
        for obstacle in self.environment.obstacles:
            if obstacle['traversable']:
                rect = Rectangle((obstacle['position'][0] - obstacle['size']/2,
                                obstacle['position'][1] - obstacle['size']/2),
                               obstacle['size'], obstacle['size'],
                               facecolor='gray', alpha=0.5)
                ax.add_patch(rect)
        
        # Draw goal
        goal_circle = plt.Circle(self.goal_position, 0.5, facecolor='green', alpha=0.7)
        ax.add_patch(goal_circle)
        ax.text(self.goal_position[0], self.goal_position[1] + 0.7, 'GOAL',
               fontsize=12, ha='center', color='green')
        
        # Draw agent path
        if self.movement_history:
            path_x = [log['position'][0] for log in self.movement_history]
            path_y = [log['position'][1] for log in self.movement_history]
            ax.plot(path_x, path_y, 'b-', alpha=0.5, linewidth=2, label='Path')
            
            # Draw agent current position
            current = self.movement_history[-1]['position']
            agent_circle = plt.Circle(current, 0.3, facecolor='red', alpha=0.8)
            ax.add_patch(agent_circle)
            ax.text(current[0], current[1] + 0.4, 'AGENT',
                   fontsize=10, ha='center', color='red')
        
        ax.set_xlim(-1, self.environment.width + 1)
        ax.set_ylim(-1, self.environment.height + 1)
        ax.set_aspect('equal')
        ax.set_title('Agent Navigation Path')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"✓ Navigation visualization saved to: {output_path}")
    
    def print_summary(self):
        """Print navigation session summary."""
        print("\n" + "=" * 60)
        print("WORLD NAVIGATION SUMMARY")
        print("=" * 60)
        
        if self.movement_history:
            final_pos = self.movement_history[-1]['position']
            print(f"Final position: ({final_pos[0]:.1f}, {final_pos[1]:.1f})")
            print(f"Steps taken: {len(self.movement_history)}")
            
            # Calculate efficiency
            start_pos = self.movement_history[0]['position']
            direct_distance = self.environment.heuristic(start_pos, self.goal_position)
            actual_distance = sum([
                self.environment.distance(
                    self.movement_history[i]['position'],
                    self.movement_history[i+1]['position']
                ) for i in range(len(self.movement_history) - 1)
            ]) if len(self.movement_history) > 1 else 0
            
            efficiency = direct_distance / actual_distance if actual_distance > 0 else 0
            print(f"Navigation efficiency: {efficiency:.3f}")
            
            # Calculate statistics
            if self.novelty_history:
                print(f"Average curiosity: {np.mean(self.novelty_history):.3f}")
                print(f"Peak curiosity: {np.max(self.novelty_history):.3f}")
                print(f"Curiosity std: {np.std(self.novelty_history):.3f}")
            
            if self.rewards:
                print(f"Average reward: {np.mean(self.rewards):.3f}")
                print(f"Best reward: {np.max(self.rewards):.3f}")
                print(f"Worst reward: {np.min(self.rewards):.3f}")
        
        print("=" * 60)


def main():
    """Main entry point for world navigation example."""
    import argparse
    
    parser = argparse.ArgumentParser(description="World Navigation Example")
    parser.add_argument("config", nargs="?", help="Path to configuration file (optional)")
    parser.add_argument("-s", "--steps", type=int, default=100,
                       help="Number of navigation steps")
    parser.add_argument("--no-plots", action="store_true",
                       help="Don't create visualization plots")
    parser.add_argument("--quick", action="store_true",
                       help="Run quick navigation (20 steps)")
    parser.add_argument("--width", type=float, default=20.0,
                       help="Environment width")
    parser.add_argument("--height", type=float, default=20.0,
                       help="Environment height")
    parser.add_argument("--density", type=float, default=0.3,
                       help="Obstacle density (0-1)")
    
    args = parser.parse_args()
    
    # Adjust steps for quick mode
    num_steps = 20 if args.quick else args.steps
    
    # Create environment
    environment = Environment(
        width=args.width,
        height=args.height,
        obstacle_density=args.density
    )
    
    # Create and initialize agent
    agent = PathfindingAgent(environment, args.config)
    agent.initialize()
    
    # Run navigation
    metrics = agent.navigate(num_steps)
    
    # Create visualization if requested
    if not args.no_plots:
        agent.create_visualization()
    
    print(f"\n✓ World navigation example completed successfully")
    print(f"✓ Agent reached goal: {'Yes' if metrics['goal_reached'] else 'No'}")
    print(f"✓ Final distance to goal: {metrics['final_distance_to_goal']:.1f}")


if __name__ == "__main__":
    main()
