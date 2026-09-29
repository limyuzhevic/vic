#!/usr/bin/env python3
"""
Example 3: Curiosity-Driven Learning Demo
===========================================

This example demonstrates curiosity-driven learning, a key mechanism in NLM's
developmental learning system. The agent explores environments based on
novelty detection and intrinsic motivation.

Learning Objectives:
- Understand curiosity-driven exploration
- Implement novelty detection
- Monitor curiosity levels
- Analyze exploration vs. exploitation trade-offs
- Visualize learning dynamics

Expected Time: ~60 minutes
Difficulty: Intermediate
"""

import pynlm
import time
import sys
import numpy as np
import matplotlib.pyplot as plt

class CuriosityLearner:
    """
    Agent that uses curiosity-driven learning to explore environments.
    
    This demonstrates how NLM's curiosity system drives exploration based on
    novelty detection and prediction errors, rather than just external rewards.
    """
    
    def __init__(self, curiosity_strength=1.0, novelty_threshold=0.5):
        """
        Initialize the curiosity learner.
        
        Args:
            curiosity_strength: Strength of curiosity drive (0.0-2.0)
            novelty_threshold: Novelty level required to trigger exploration
        """
        print("Initializing Curiosity-Driven Learner...")
        
        # Create configuration optimized for curiosity learning
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1500)
        self.config.set("brain.synapse_density", 0.08)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.curiosity.exploration_bonus", 0.5)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.novelty.sensitivity", 1.2)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=5, visionHeight=5)
        self.world.reset()
        self.world.setAgentStart(7.5, 7.5)
        
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Curiosity parameters
        self.curiosity_strength = curiosity_strength
        self.novelty_threshold = novelty_threshold
        
        # Learning statistics
        self.stats = {
            'steps': [],
            'novelty_levels': [],
            'curiosity_levels': [],
            'rewards': [],
            'actions': [],
            'firing_rates': [],
            'exploration_indices': [],
            'prediction_errors': []
        }
        
        # Environment history for novelty calculation
        self.vision_history = []
        self.last_positions = []
        
        print("✓ Curiosity learner initialized")
    
    def calculate_novelty(self, current_vision):
        """
        Calculate novelty based on changes in visual input.
        
        Args:
            current_vision: Current visual perception
        
        Returns:
            Novelty score (0.0-1.0)
        """
        if not self.vision_history:
            self.vision_history.append(current_vision.copy())
            return 0.0
        
        if len(self.vision_history) == 0:
            return 0.0
        
        # Get previous vision
        prev_vision = self.vision_history[-1]
        
        # Calculate mean absolute difference
        if len(current_vision) == len(prev_vision):
            diff = np.mean(np.abs(np.array(current_vision) - np.array(prev_vision)))
        else:
            # Handle different array sizes
            min_len = min(len(current_vision), len(prev_vision))
            if min_len == 0:
                return 0.0
            diff = np.mean(np.abs(np.array(current_vision[:min_len]) - 
                                 np.array(prev_vision[:min_len])))
        
        # Normalize to 0-1 range
        novelty = min(1.0, diff * 2.0)  # Scale factor for sensitivity
        
        # Decay over time
        novelty *= 0.9
        
        # Update history
        self.vision_history.append(current_vision.copy())
        
        # Keep history manageable
        if len(self.vision_history) > 10:
            self.vision_history.pop(0)
        
        return novelty
    
    def calculate_exploration_index(self, curiosity, novelty):
        """
        Calculate exploration index based on curiosity and novelty.
        
        Args:
            curiosity: Current curiosity level
            novelty: Current novelty level
        
        Returns:
            Exploration index (0.0-1.0)
        """
        # Weighted combination of curiosity and novelty
        exploration = (curiosity * 0.6 + novelty * 0.4) / self.curiosity_strength
        
        # Apply threshold for meaningful exploration
        if exploration < self.novelty_threshold:
            exploration = 0.0
        
        return min(1.0, exploration)
    
    def run_curiosity_session(self, num_steps=200):
        """
        Run a curiosity-driven learning session.
        
        Args:
            num_steps: Number of simulation steps
        
        Returns:
            Dictionary with session statistics
        """
        print(f"\nRunning curiosity session for {num_steps} steps...")
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            current_vision = percept.getVision()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Get current state
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.calculate_novelty(current_vision)
            reward = self.calculate_curiosity_reward(novelty, curiosity)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation (curiosity is both reward and prediction error)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record statistics
            exploration_idx = self.calculate_exploration_index(curiosity, novelty)
            
            self.stats['steps'].append(step)
            self.stats['novelty_levels'].append(novelty)
            self.stats['curiosity_levels'].append(curiosity)
            self.stats['rewards'].append(reward)
            self.stats['actions'].append(action)
            self.stats['firing_rates'].append(self.brain.getAverageFiringRate())
            self.stats['exploration_indices'].append(exploration_idx)
            self.stats['prediction_errors'].append(self.agent.getPredictionError())
            
            # Print progress
            if step % 50 == 0:
                print(f"  Step {step:3d}: "
                      f"Novelty: {novelty:5.3f}, "
                      f"Curiosity: {curiosity:5.3f}, "
                      f"Exploration: {exploration_idx:5.3f}, "
                      f"Reward: {reward:6.3f}")
        
        return self._analyze_session_stats()
    
    def calculate_curiosity_reward(self, novelty, curiosity):
        """
        Calculate curiosity-based reward.
        
        Args:
            novelty: Novelty level
            curiosity: Curiosity level
        
        Returns:
            Curiosity reward
        """
        # Base reward from novelty detection
        reward = novelty * 0.5
        
        # Bonus for curiosity-driven exploration
        if curiosity > 0.3:
            reward += curiosity * 0.3
        
        # Time-based exploration bonus
        if novelty > 0.7:
            reward += 0.2  # Significant exploration bonus
        
        return reward
    
    def _analyze_session_stats(self):
        """Analyze and return session statistics."""
        if not self.stats['steps']:
            return {}
        
        # Calculate key statistics
        stats = {
            'total_steps': len(self.stats['steps']),
            'avg_novelty': np.mean(self.stats['novelty_levels']),
            'max_novelty': np.max(self.stats['novelty_levels']),
            'avg_curiosity': np.mean(self.stats['curiosity_levels']),
            'max_curiosity': np.max(self.stats['curiosity_levels']),
            'avg_reward': np.mean(self.stats['rewards']),
            'avg_firing_rate': np.mean(self.stats['firing_rates']),
            'avg_exploration_index': np.mean(self.stats['exploration_indices']),
            'exploration_steps': sum(1 for idx in self.stats['exploration_indices'] if idx > 0),
            'prediction_error': np.mean(self.stats['prediction_errors']),
            'learning_efficiency': self._calculate_learning_efficiency()
        }
        
        return stats
    
    def _calculate_learning_efficiency(self):
        """Calculate learning efficiency score."""
        if len(self.stats['curiosity_levels']) < 2:
            return 0.0
        
        # Calculate improvement in curiosity over time
        first_half = self.stats['curiosity_levels'][:len(self.stats['curiosity_levels'])//2]
        second_half = self.stats['curiosity_levels'][len(self.stats['curiosity_levels'])//2:]
        
        avg_first = np.mean(first_half)
        avg_second = np.mean(second_half)
        
        improvement = (avg_second - avg_first) / max(avg_first, 0.01)
        
        # Calculate novelty discovery rate
        novelty_improvement = (np.max(self.stats['novelty_levels']) - 
                             np.min(self.stats['novelty_levels']))
        
        # Combine metrics
        efficiency = (improvement + novelty_improvement) / 2.0
        return max(0.0, efficiency)
    
    def visualize_learning(self, save_plots=False):
        """
        Create visualizations of the learning process.
        
        Args:
            save_plots: Whether to save plots to files
        """
        try:
            # Create figure with subplots
            fig, axes = plt.subplots(2, 3, figsize=(15, 10))
            fig.suptitle('Curiosity-Driven Learning Analysis', fontsize=16)
            
            steps = self.stats['steps']
            
            # 1. Novelty levels over time
            axes[0, 0].plot(steps, self.stats['novelty_levels'], 'b-', alpha=0.7)
            axes[0, 0].set_title('Novelty Levels')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Novelty (0-1)')
            axes[0, 0].grid(True, alpha=0.3)
            
            # 2. Curiosity levels over time
            axes[0, 1].plot(steps, self.stats['curiosity_levels'], 'r-', alpha=0.7)
            axes[0, 1].set_title('Curiosity Levels')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Curiosity (0-1)')
            axes[0, 1].grid(True, alpha=0.3)
            
            # 3. Exploration index over time
            axes[0, 2].plot(steps, self.stats['exploration_indices'], 'g-', alpha=0.7)
            axes[0, 2].set_title('Exploration Index')
            axes[0, 2].set_xlabel('Step')
            axes[0, 2].set_ylabel('Exploration (0-1)')
            axes[0, 2].grid(True, alpha=0.3)
            
            # 4. Rewards over time
            axes[1, 0].plot(steps, self.stats['rewards'], 'c-', alpha=0.7)
            axes[1, 0].set_title('Curiosity Rewards')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Reward')
            axes[1, 0].grid(True, alpha=0.3)
            
            # 5. Firing rates over time
            axes[1, 1].plot(steps, self.stats['firing_rates'], 'm-', alpha=0.7)
            axes[1, 1].set_title('Average Firing Rate')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Firing Rate')
            axes[1, 1].grid(True, alpha=0.3)
            
            # 6. Action distribution
            action_counts = {}
            for action in self.stats['actions']:
                action_name = self._action_to_str(action)
                action_counts[action_name] = action_counts.get(action_name, 0) + 1
            
            if action_counts:
                axes[1, 2].bar(action_counts.keys(), action_counts.values(), alpha=0.7)
                axes[1, 2].set_title('Action Distribution')
                axes[1, 2].set_xlabel('Action')
                axes[1, 2].set_ylabel('Count')
                axes[1, 2].tick_params(axis='x', rotation=45)
            
            plt.tight_layout()
            
            if save_plots:
                plt.savefig('curiosity_learning_analysis.png', dpi=150, bbox_inches='tight')
                print("Plots saved as 'curiosity_learning_analysis.png'")
            
            plt.show()
            
        except Exception as e:
            print(f"Visualization error: {e}")
            print("Please install matplotlib for plotting capabilities")
    
    def _action_to_str(self, action):
        """Convert action enum to string."""
        action_names = {
            pynlm.MotorCommand.MoveForward: "MoveF",
            pynlm.MotorCommand.MoveBackward: "MoveB",
            pynlm.MotorCommand.TurnLeft: "TurnL",
            pynlm.MotorCommand.TurnRight: "TurnR",
            pynlm.MotorCommand.LookLeft: "LookL",
            pynlm.MotorCommand.LookRight: "LookR",
            pynlm.MotorCommand.Interact: "Inter",
            pynlm.MotorCommand.Wait: "Wait"
        }
        return action_names.get(action, f"Action({action})")
    
    def print_summary(self, stats):
        """Print comprehensive learning summary."""
        print("\n" + "="*80)
        print("CURIOSITY-LEARNING ANALYSIS SUMMARY")
        print("="*80)
        
        print(f"\nLearning Performance:")
        print(f"  Total steps: {stats['total_steps']}")
        print(f"  Average novelty: {stats['avg_novelty']:.3f}")
        print(f"  Maximum novelty: {stats['max_novelty']:.3f}")
        print(f"  Average curiosity: {stats['avg_curiosity']:.3f}")
        print(f"  Maximum curiosity: {stats['max_curiosity']:.3f}")
        
        print(f"\nExploration Behavior:")
        print(f"  Exploration steps: {stats['exploration_steps']} "
              f"({stats['exploration_steps']/stats['total_steps']*100:.1f}%)")
        print(f"  Average exploration index: {stats['avg_exploration_index']:.3f}")
        print(f"  Average curiosity reward: {stats['avg_reward']:.3f}")
        
        print(f"\nNeural Activity:")
        print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"  Average prediction error: {stats['prediction_error']:.3f}")
        
        print(f"\nLearning Quality:")
        print(f"  Learning efficiency score: {stats['learning_efficiency']:.3f}")
        
        if stats['learning_efficiency'] > 0.5:
            print("  → Excellent learning performance")
        elif stats['learning_efficiency'] > 0.2:
            print("  → Good learning performance")
        elif stats['learning_efficiency'] > 0.0:
            print("  → Moderate learning performance")
        else:
            print("  → Limited learning observed")
        
        # Recommendations
        print(f"\nRecommendations:")
        if stats['avg_novelty'] < 0.3:
            print("  - Environment may be too predictable. Increase complexity.")
        if stats['avg_curiosity'] < 0.2:
            print("  - Curiosity drive is low. Adjust curiosity parameters.")
        if stats['exploration_steps'] < stats['total_steps'] * 0.1:
            print("  - Exploration is limited. Consider reducing novelty threshold.")
        if stats['learning_efficiency'] < 0.1:
            print("  - Learning is inefficient. Review configuration parameters.")
        
        print(f"\nKey Insights:")
        if stats['max_novelty'] > 0.7:
            print("  ✓ Agent discovered highly novel experiences")
        if stats['avg_curiosity'] > 0.3:
            print("  ✓ Curiosity-driven exploration is active")
        if stats['exploration_steps'] > stats['total_steps'] * 0.3:
            print("  ✓ Agent explores environment effectively")

def main():
    """
    Main function demonstrating curiosity-driven learning.
    """
    print("NLM Curiosity-Driven Learning Example")
    print("="*60)
    print("This example demonstrates how NLM's curiosity system drives exploration")
    print("based on novelty detection and intrinsic motivation.")
    print()
    
    try:
        # Create curiosity learner
        learner = CuriosityLearner(
            curiosity_strength=1.0,
            novelty_threshold=0.4
        )
        
        # Run curiosity session
        stats = learner.run_curiosity_session(num_steps=200)
        
        # Print summary
        learner.print_summary(stats)
        
        # Visualize learning (if matplotlib available)
        print("\nGenerating visualizations...")
        learner.visualize_learning(save_plots=True)
        
        print("\n" + "="*80)
        print("EXAMPLE COMPLETED SUCCESSFULLY")
        print("="*80)
        print("\nKey insights:")
        print("  - Agent learned through curiosity-driven exploration")
        print("  - Novelty detection guided exploration behavior")
        print("  - Intrinsic motivation drove learning")
        print("  - Curiosity system maintained engagement")
        
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
