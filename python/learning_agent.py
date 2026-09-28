#!/usr/bin/env python3
"""
NLM Example: Learning Agent with Reward System

This example demonstrates an autonomous agent learning to navigate a world,
collect resources, and avoid hazards using NLM's brain simulation capabilities.
"""

import pynlm
import random
import sys
from typing import List, Tuple

class LearningAgent:
    """Autonomous agent that learns through interaction with the environment."""
    
    def __init__(self, width: int = 30, height: int = 30):
        # Configure brain with learning-optimized parameters
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 800)
        self.config.set("region_count", 3)
        self.config.set("connection_probability", 0.12)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world with agent-friendly settings
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, visionWidth=10, visionHeight=10)
        self.world.reset()
        self.world.setAgentStart(width//2, height//2)
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableAllFeatures()  # Enable learning and development
        
        # Track performance metrics
        self.reward_history: List[float] = []
        self.step_count = 0
        self.resources_collected = 0
        self.hazards_avoided = 0
        
        # Place resources and hazards in world
        self._setup_environment()
    
    def _setup_environment(self):
        """Place resources and hazards in the world."""
        world_size = min(self.world.getWidth(), self.world.getHeight())
        
        # Place resources (good for agent)
        for _ in range(8):
            x = random.randint(5, world_size - 5)
            y = random.randint(5, world_size - 5)
            if self.world.isValidPosition(x, y):
                resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 20.0)
                self.world.addObject(resource)
        
        # Place hazards (bad for agent)
        for _ in range(4):
            x = random.randint(5, world_size - 5)
            y = random.randint(5, world_size - 5)
            if self.world.isValidPosition(x, y):
                hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 5.0)
                self.world.addObject(hazard)
        
        # Place walls for structure
        for x in range(0, world_size, 8):
            for y in [0, world_size - 1]:
                if self.world.isValidPosition(x, y):
                    wall = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 1.0)
                    self.world.addObject(wall)
    
    def run_episode(self, max_steps: int = 500) -> Tuple[float, int]:
        """Run a single episode and return total reward and steps taken."""
        episode_reward = 0.0
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get current state
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(self.step_count)
            action = self.agent.decodeMotorCommand()
            
            # Execute action in world
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from result
            self.agent.applyRewardModulation(
                action_result.reward, 
                self.agent.getPredictionError()
            )
            
            # Track performance
            self.reward_history.append(action_result.reward)
            episode_reward += action_result.reward
            self.step_count += 1
            
            # Check for specific events
            if action_result.success and "Resource" in str(action):
                self.resources_collected += 1
            
            if action_result.success and "Hazard" in str(action):
                self.hazards_avoided += 1
            
            # Update development periodically
            if step % 50 == 0:
                self.agent.updateDevelopment(0.1)
                
                # Add novelty-driven exploration
                if self.step_count % 100 == 0:
                    # Inject novel stimulus to encourage exploration
                    internal = pynlm.InternalSignals()
                    internal.addSignal(0.8)  # High novelty signal
                    internal.setTimestamp(self.world.getSimulationTime())
                    self.agent.processSensoryInput(internal)
                    self.agent.applyRewardModulation(0.1, self.agent.getNoveltyLevel())
            
            # Check for episode termination
            if self.step_count >= max_steps:
                break
                
                # Early termination if agent reaches a goal
            if self.resources_collected >= 5:  # Collect 5 resources to win
                break
        
        return episode_reward, self.step_count
    
    def save_brain(self, filepath: str):
        """Save brain state to file."""
        self.brain.save(filepath)
    
    def load_brain(self, filepath: str):
        """Load brain state from file."""
        self.brain.load(filepath)
        self.agent.initialize(self.world)
    
    def get_performance_stats(self) -> dict:
        """Get comprehensive performance statistics."""
        if not self.reward_history:
            return {}
        
        return {
            "total_reward": sum(self.reward_history),
            "avg_reward_per_step": sum(self.reward_history) / len(self.reward_history),
            "max_reward": max(self.reward_history),
            "min_reward": min(self.reward_history),
            "std_reward": (sum((x - sum(self.reward_history)/len(self.reward_history))**2 
                              for x in self.reward_history) / len(self.reward_history))**0.5,
            "steps_taken": self.step_count,
            "resources_collected": self.resources_collected,
            "hazards_avoided": self.hazards_avoided,
            "learning_rate": self.agent.getNeuromodulationLevel(),
            "curiosity_level": self.agent.getCuriosityLevel(),
            "development_stage": self.agent.getDevelopmentalStage(),
        }
    
    def reset(self):
        """Reset agent for new episode."""
        self.brain.reset()
        self.brain.initialize()
        self.agent.reset()
        self.agent.initialize(self.world)
        self.reward_history.clear()
        self.step_count = 0
        self.resources_collected = 0
        self.hazards_avoided = 0


def run_learning_experiment(num_episodes: int = 10, width: int = 25, height: int = 25):
    """Run multiple learning episodes and return statistics."""
    print(f"Starting learning experiment: {num_episodes} episodes")
    print(f"World size: {width}x{height}")
    
    all_rewards = []
    all_stats = []
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Create new agent for each episode
        agent = LearningAgent(width, height)
        
        # Run episode
        reward, steps = agent.run_episode(max_steps=300)
        
        # Get statistics
        stats = agent.get_performance_stats()
        all_rewards.append(reward)
        all_stats.append(stats)
        
        print(f"Episode completed in {steps} steps")
        print(f"Total reward: {reward:.2f}")
        print(f"Resources collected: {stats['resources_collected']}")
        print(f"Hazards avoided: {stats['hazards_avoided']}")
        print(f"Avg reward/step: {stats['avg_reward_per_step']:.3f}")
        print(f"Learning rate: {stats['learning_rate']:.3f}")
        print(f"Curiosity level: {stats['curiosity_level']:.3f}")
        
        # Save best brain
        if episode == 0 or reward == max(all_rewards):
            agent.save_brain(f"best_brain_episode{episode + 1}.json")
            print(f"Saved best brain to: best_brain_episode{episode + 1}.json")
    
    # Overall statistics
    avg_reward = sum(all_rewards) / len(all_rewards)
    best_reward = max(all_rewards)
    
    print(f"\n=== LEARNING EXPERIMENT RESULTS ===")
    print(f"Total episodes: {num_episodes}")
    print(f"Average reward: {avg_reward:.2f}")
    print(f"Best reward: {best_reward:.2f}")
    print(f"Reward std deviation: {(sum((x - avg_reward)**2 for x in all_rewards) / len(all_rewards))**0.5:.2f}")
    
    # Analyze learning progress
    first_half_rewards = all_rewards[:num_episodes//2]
    second_half_rewards = all_rewards[num_episodes//2:]
    first_avg = sum(first_half_rewards) / len(first_half_rewards)
    second_avg = sum(second_half_rewards) / len(second_half_rewards)
    
    print(f"First half average: {first_avg:.2f}")
    print(f"Second half average: {second_avg:.2f}")
    print(f"Learning improvement: {second_avg - first_avg:.2f}")
    
    if second_avg > first_avg:
        print("✓ Learning detected: Agent improved over time!")
    else:
        print("⚠ No clear learning trend detected")
    
    return all_stats


def main():
    """Main function with command-line argument support."""
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help":
            print("Usage: python learning_agent.py [options]")
            print("Options:")
            print("  --episodes N     Number of episodes (default: 10)")
            print("  --width W         World width (default: 25)")
            print("  --height H        World height (default: 25)")
            print("  --maxsteps S      Max steps per episode (default: 300)")
            print("  --savebrain       Save best brain")
            return
        
        # Parse arguments
        episodes = 10
        width = 25
        height = 25
        
        for i in range(1, len(sys.argv), 2):
            if sys.argv[i] == "--episodes" and i + 1 < len(sys.argv):
                episodes = int(sys.argv[i + 1])
            elif sys.argv[i] == "--width" and i + 1 < len(sys.argv):
                width = int(sys.argv[i + 1])
            elif sys.argv[i] == "--height" and i + 1 < len(sys.argv):
                height = int(sys.argv[i + 1])
            elif sys.argv[i] == "--maxsteps" and i + 1 < len(sys.argv):
                max_steps = int(sys.argv[i + 1])
    else:
        # Default parameters
        episodes = 10
        width = 25
        height = 25
        max_steps = 300
    
    # Run learning experiment
    try:
        stats = run_learning_experiment(num_episodes=episodes, width=width, height=height)
        print(f"\nExperiment completed successfully!")
        print(f"Results saved to: best_brain_episode*.json")
    except Exception as e:
        print(f"Error during experiment: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    exit(main())
