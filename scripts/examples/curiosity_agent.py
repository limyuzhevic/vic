#!/usr/bin/env python3
"""
Curiosity Agent Example

Demonstrates curiosity-driven learning in NLM agents.
This example shows how agents explore environments based on curiosity signals.

Usage: python scripts/examples/curiosity_agent.py [config_file]
"""

import sys
import os
from pathlib import Path
import json
import random
import numpy as np
import time

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm import NeuralLearningMachine
from nlm.neuromodulation import DopamineSystem, CuriositySystem, NoveltyDetector
from nlm.memory import WorkingMemory


class CuriosityDrivenAgent:
    """Agent that learns driven by curiosity and intrinsic motivation."""
    
    def __init__(self, config_path: str = None):
        """Initialize curiosity-driven agent.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = config_path
        self.nlm = None
        self.dopamine = None
        self.curiosity = None
        self.novelty = None
        self.working_memory = None
        self.exploration_log = []
        self.curiosity_history = []
        self.exploration_efficiency = 0.0
        
    def initialize(self):
        """Initialize the curiosity-driven agent."""
        print("Initializing Curiosity-Driven Agent...")
        
        # Create NLM with neuromodulation capabilities
        if self.config_path and Path(self.config_path).exists():
            self.nlm = NeuralLearningMachine.load_config(self.config_path)
        else:
            from nlm.config import get_default_config
            config = get_default_config()
            self.nlm = NeuralLearningMachine(config)
        
        # Initialize neuromodulation systems
        neuromod_config = self.nlm.config.get('neuromodulation', {})
        
        self.dopamine = DopamineSystem(
            weight=neuromod_config.get('dopamine', {}).get('weight', 1.0),
            learning_rate=neuromod_config.get('dopamine', {}).get('learning_rate', 0.01),
            adaptation_rate=neuromod_config.get('dopamine', {}).get('adaptation_rate', 0.001)
        )
        
        self.curiosity = CuriositySystem(
            weight=neuromod_config.get('curiosity', {}).get('weight', 0.5),
            novelty_threshold=neuromod_config.get('curiosity', {}).get('novelty_threshold', 0.3),
            history_size=neuromod_config.get('curiosity', {}).get('history_size', 100)
        )
        
        self.novelty = NoveltyDetector(
            sensitivity=neuromod_config.get('novelty', {}).get('sensitivity', 0.7),
            novelty_history=[]
        )
        
        # Initialize working memory
        self.working_memory = WorkingMemory(
            capacity=self.nlm.config.get('memory', {}).get(
                'working_memory', {}).get('working_memory_capacity', 100)
        )
        
        print("✓ Neuromodulation systems initialized successfully")
    
    def get_curiosity_signal(self, sensory_input: Dict) -> float:
        """Calculate curiosity signal for given sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Curiosity signal value
        """
        # Calculate prediction error
        prediction_error = self.calculate_prediction_error(sensory_input)
        
        # Calculate novelty
        novelty_score = self.novelty.calculate_novelty(sensory_input)
        
        # Update curiosity system
        curiosity_signal = self.curiosity.calculate_curiosity(
            prediction_error, novelty_score
        )
        
        # Update novelty detector
        self.novelty.update_history(sensory_input)
        
        return curiosity_signal
    
    def calculate_prediction_error(self, sensory_input: Dict) -> float:
        """Calculate prediction error for sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Prediction error value
        """
        # Get current prediction from working memory
        current_prediction = self.working_memory.get_prediction(sensory_input)
        
        # Calculate actual input
        actual_input = self.extract_features(sensory_input)
        
        # Calculate error (simplified)
        if current_prediction is not None:
            error = np.linalg.norm(np.array(actual_input) - np.array(current_prediction))
        else:
            error = np.linalg.norm(np.array(actual_input))
        
        return min(error, 1.0)  # Normalize to 0-1
    
    def extract_features(self, sensory_input: Dict) -> List[float]:
        """Extract features from sensory input.
        
        Args:
            sensory_input: Current sensory observations
            
        Returns:
            Feature vector
        """
        features = []
        
        # Extract position features
        if 'position' in sensory_input:
            pos = sensory_input['position']
            features.extend([pos[0], pos[1]])
        
        # Extract color features
        if 'color' in sensory_input:
            color_map = {'red': [1, 0, 0], 'green': [0, 1, 0], 'blue': [0, 0, 1]}
            color = sensory_input['color']
            features.extend(color_map.get(color, [0.5, 0.5, 0.5]))
        
        # Extract texture features
        if 'texture' in sensory_input:
            texture_map = {'smooth': 0.0, 'rough': 0.5, 'bumpy': 1.0}
            features.append(texture_map.get(sensory_input['texture'], 0.0))
        
        # Extract sound features
        if 'sound' in sensory_input:
            sound_map = {'silence': 0.0, 'noise': 0.5, 'tone': 1.0}
            features.append(sound_map.get(sensory_input['sound'], 0.0))
        
        # Add random feature for exploration
        features.append(random.random())
        
        return features
    
    def explore(self, current_position: Tuple[float, float]) -> Dict:
        """Perform exploration based on curiosity.
        
        Args:
            current_position: Current agent position
            
        Returns:
            Exploration action and metrics
        """
        # Generate candidate actions
        candidate_actions = self.generate_candidate_actions(current_position)
        
        # Evaluate each action's curiosity value
        action_scores = []
        
        for action in candidate_actions:
            # Simulate action
            new_position = self.apply_action(current_position, action)
            
            # Get sensory input from new position
            sensory_input = self.get_sensory_input(new_position)
            
            # Calculate curiosity for this action
            curiosity = self.get_curiosity_signal(sensory_input)
            
            # Calculate dopamine signal
            reward = self.evaluate_reward(sensory_input, action)
            dopamine = self.dopamine.calculate_dopamine(reward, curiosity)
            
            action_scores.append({
                'action': action,
                'position': new_position,
                'curiosity': curiosity,
                'dopamine': dopamine,
                'sensory_input': sensory_input,
                'reward': reward
            })
        
        # Select best action based on curiosity
        best_action = max(action_scores, key=lambda x: x['curiosity'] * 0.7 + x['dopamine'] * 0.3)
        
        # Store in working memory
        self.working_memory.store(
            best_action['sensory_input'],
            best_action['action'],
            best_action['reward']
        )
        
        # Log exploration
        self.exploration_log.append({
            'time': len(self.exploration_log),
            'action': best_action['action'],
            'position': current_position,
            'new_position': best_action['position'],
            'curiosity': best_action['curiosity'],
            'dopamine': best_action['dopamine'],
            'reward': best_action['reward']
        })
        
        self.curiosity_history.append(best_action['curiosity'])
        
        return best_action
    
    def generate_candidate_actions(self, position: Tuple[float, float]) -> List[Dict]:
        """Generate candidate actions for exploration.
        
        Args:
            position: Current agent position
            
        Returns:
            List of candidate actions
        """
        actions = []
        
        # Basic movement actions
        for direction in ['north', 'south', 'east', 'west', 'northeast', 'northwest', 
                         'southeast', 'southwest']:
            action = {
                'type': 'move',
                'direction': direction,
                'step_size': 1.0,
                'efficiency': self.calculate_action_efficiency(position, direction)
            }
            actions.append(action)
        
        # Exploration actions
        exploration_actions = [
            {'type': 'scan', 'range': 5.0, 'resolution': 'high'},
            {'type': 'listen', 'duration': 2.0, 'range': 3.0},
            {'type': 'smell', 'range': 2.0, 'sensitivity': 'high'},
            {'type': 'touch', 'area': 'surroundings', 'detail': 'fine'}
        ]
        
        actions.extend(exploration_actions)
        
        return actions
    
    def apply_action(self, position: Tuple[float, float], action: Dict) -> Tuple[float, float]:n"""Apply action to get new position.
        
        Args:
            position: Current position
            action: Action to apply
            
        Returns:
            New position
        """
        if action['type'] == 'move':
            dx, dy = 0.0, 0.0
            direction = action['direction']
            
            if 'north' in direction:
                dy += action['step_size']
            elif 'south' in direction:
                dy -= action['step_size']
            if 'east' in direction:
                dx += action['step_size']
            elif 'west' in direction:
                dx -= action['step_size']
            
            new_x = max(0, min(10, position[0] + dx))
            new_y = max(0, min(10, position[1] + dy))
            return (new_x, new_y)
        
        # For non-movement actions, return current position
        return position
    
    def get_sensory_input(self, position: Tuple[float, float]) -> Dict:
        """Get sensory input from environment.
        
        Args:
            position: Agent position
            
        Returns:
            Sensory input dictionary
        """
        # Generate random sensory data
        sensory_input = {
            'position': position,
            'color': random.choice(['red', 'green', 'blue', 'yellow', 'purple']),
            'texture': random.choice(['smooth', 'rough', 'bumpy', 'soft', 'hard']),
            'sound': random.choice(['silence', 'noise', 'tone', 'music', 'silence']),
            'light': random.uniform(0.3, 1.0),
            'temperature': random.uniform(20.0, 30.0),
            'humidity': random.uniform(30.0, 80.0)
        }
        
        return sensory_input
    
    def evaluate_reward(self, sensory_input: Dict, action: Dict) -> float:
        """Evaluate reward for sensory input and action.
        
        Args:
            sensory_input: Sensory observations
            action: Action taken
            
        Returns:
            Reward value
        """
        reward = 0.0
        
        # Reward based on proximity to center
        position = sensory_input['position']
        center_distance = np.sqrt((position[0] - 5.0)**2 + (position[1] - 5.0)**2)
        reward += max(0, 5.0 - center_distance) / 5.0 * 0.3
        
        # Reward based on interesting colors
        color_rewards = {'red': 0.8, 'blue': 0.7, 'green': 0.6, 'yellow': 0.9, 'purple': 0.8}
        reward += color_rewards.get(sensory_input['color'], 0.0) * 0.2
        
        # Reward based on action type
        if action['type'] == 'scan':
            reward += 0.5
        elif action['type'] == 'move':
            reward += 0.3
        
        return min(reward, 1.0)
    
    def calculate_action_efficiency(self, position: Tuple[float, float], direction: str) -> float:
        """Calculate efficiency of moving in a direction.
        
        Args:
            position: Current position
            direction: Direction to move
            
        Returns:
            Efficiency score (0-1)
        """
        # Calculate distance to center
        center_distance = np.sqrt((position[0] - 5.0)**2 + (position[1] - 5.0)**2)
        
        # Moving toward center is more efficient
        if 'north' in direction and position[1] < 5.0:
            efficiency = 1.0 - center_distance / 5.0
        elif 'south' in direction and position[1] > 5.0:
            efficiency = 1.0 - center_distance / 5.0
        elif 'east' in direction and position[0] > 5.0:
            efficiency = 1.0 - center_distance / 5.0
        elif 'west' in direction and position[0] < 5.0:
            efficiency = 1.0 - center_distance / 5.0
        else:
            efficiency = 0.5
        
        return max(0.0, min(1.0, efficiency))
    
    def simulate_exploration(self, num_steps: int = 100):
        """Simulate exploration session.
        
        Args:
            num_steps: Number of exploration steps
        """
        print(f"Starting {num_steps}-step exploration session...")
        print("(Agent exploring based on curiosity and intrinsic motivation)")
        
        # Start at random position
        position = (random.uniform(1.0, 9.0), random.uniform(1.0, 9.0))
        
        for step in range(num_steps):
            # Explore based on curiosity
            exploration = self.explore(position)
            
            # Update position
            position = exploration['position']
            
            if (step + 1) % 20 == 0:
                print(f"Step {step + 1}/{num_steps} completed")
                print(f"  Current position: ({position[0]:.1f}, {position[1]:.1f})")
                print(f"  Curiosity level: {exploration['curiosity']:.2f}")
                print(f"  Dopamine level: {exploration['dopamine']:.2f}")
        
        print("✓ Exploration session completed")
        self.print_summary()
    
    def print_summary(self):
        """Print exploration session summary."""
        print("\n" + "=" * 60)
        print("CURIOSITY-DRIVEN EXPLORATION SUMMARY")
        print("=" * 60)
        print(f"Total exploration steps: {len(self.exploration_log)}")
        
        if self.exploration_log:
            avg_curiosity = np.mean([e['curiosity'] for e in self.exploration_log])
            avg_dopamine = np.mean([e['dopamine'] for e in self.exploration_log])
            avg_reward = np.mean([e['reward'] for e in self.exploration_log])
            
            print(f"Average curiosity level: {avg_curiosity:.3f}")
            print(f"Average dopamine level: {avg_dopamine:.3f}")
            print(f"Average reward: {avg_reward:.3f}")
            
            # Calculate exploration efficiency
            visited_positions = set()
            for e in self.exploration_log:
                visited_positions.add(tuple(e['position']))
            
            self.exploration_efficiency = len(visited_positions) / len(self.exploration_log)
            print(f"Exploration efficiency: {self.exploration_efficiency:.3f}")
            
            # Find most explored areas
            position_counts = {}
            for e in self.exploration_log:
                pos = tuple(e['position'])
                position_counts[pos] = position_counts.get(pos, 0) + 1
            
            if position_counts:
                most_explored = max(position_counts.items(), key=lambda x: x[1])
                print(f"Most explored position: {most_explored[0]} (visited {most_explored[1]} times)")
        
        print("=" * 60)


def main():
    """Main entry point for curiosity agent example."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Curiosity-Driven Learning Example")
    parser.add_argument("config", nargs="?", help="Path to configuration file (optional)")
    parser.add_argument("-s", "--steps", type=int, default=100,
                       help="Number of exploration steps")
    parser.add_argument("--no-run", action="store_true",
                       help="Don't run exploration, just initialize agent")
    parser.add_argument("--visualize", action="store_true",
                       help="Create visualization of exploration")
    
    args = parser.parse_args()
    
    # Create and initialize agent
    agent = CuriosityDrivenAgent(args.config)
    agent.initialize()
    
    if not args.no_run:
        # Run exploration session
        agent.simulate_exploration(args.steps)
        
        # Create visualization if requested
        if args.visualize:
            print("\nCreating exploration visualization...")
            # In a real implementation, this would create plots
            print("✓ Visualization created (placeholder)")
    
    print("\n✓ Curiosity-driven learning example completed successfully")


if __name__ == "__main__":
    main()
