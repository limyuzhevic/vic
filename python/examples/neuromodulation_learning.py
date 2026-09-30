"""
Neuromodulation and Learning Examples for NLM

This module demonstrates advanced neuromodulation mechanisms and learning strategies
including reward-based learning, curiosity-driven exploration, and novelty detection.
"""

import pynlm
import numpy as np
import time
from typing import Dict, List, Optional, Tuple
import matplotlib.pyplot as plt

class NeuromodulationExamples:
    """Neuromodulation and learning examples."""
    
    @staticmethod
    def create_reward_modulation_example():
        """
        Example: Reward-modulated learning.
        
        Demonstrates how dopamine-based reward modulation enhances learning efficiency
        and goal-directed behavior.
        """
        print("=== Reward-Modulation Learning Example ===")
        
        # Create brain with reward modulation enabled
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.12)
        
        # Configure neuromodulation for reward learning
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", False)  # Focus on reward
        
        # Configure learning mechanisms
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.02)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent for reward learning
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable reward modulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(False)
        
        print("Created reward-modulated learning system")
        print("  Dopamine scale: 1.5")
        print("  Reward modulation: Enabled")
        
        # Simulate reward-based learning
        print("\nSimulating reward-modulated learning...")
        
        # Define reward scenarios
        reward_scenarios = [
            {"action": "approach_resource", "reward": 1.0, "probability": 0.8},
            {"action": "avoid_hazard", "reward": -1.0, "probability": 0.9},
            {"action": "explore_new", "reward": 0.3, "probability": 0.6},
            {"action": "stay", "reward": 0.0, "probability": 1.0},
            {"action": "approach_resource", "reward": 1.0, "probability": 0.8},
            {"action": "avoid_hazard", "reward": -1.0, "probability": 0.9}
        ]
        
        reward_history = []
        learning_progress = []
        
        for step in range(150):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action (simulated - in real scenario this would come from brain)
            action = agent.decodeMotorCommand()
            
            # Apply action to world (simplified)
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            # Simulate reward based on action and environment
            current_reward = 0.0
            for scenario in reward_scenarios:
                if scenario["action"] == "approach_resource":
                    current_reward += scenario["reward"] * scenario["probability"]
                elif scenario["action"] == "avoid_hazard":
                    current_reward += scenario["reward"] * scenario["probability"]
                elif scenario["action"] == "explore_new":
                    current_reward += scenario["reward"] * scenario["probability"]
            
            # Apply reward modulation
            agent.applyRewardModulation(current_reward, 0.0)
            
            # Record reward and learning progress
            reward_history.append(current_reward)
            
            if step % 30 == 0:
                curiosity_level = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.5
                novelty_level = agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.5
                dopamine_level = config.get("neuromod.dopamine.scale") if hasattr(config, 'get') else 1.5
                
                learning_progress.append({
                    'step': step,
                    'total_reward': sum(reward_history),
                    'curiosity': curiosity_level,
                    'novelty': novelty_level,
                    'dopamine_scale': dopamine_level
                })
                
                print(f"  Step {step}: Reward = {current_reward:+.2f}, "
                      f"Total = {sum(reward_history):+.2f}, "
                      f"Curiosity = {curiosity_level:.3f}")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "reward_history": reward_history,
            "learning_progress": learning_progress,
            "config": config
        }
    
    @staticmethod
    def create_curiosity_driven_learning_example():
        """
        Example: Curiosity-driven exploration.
        
        Demonstrates how intrinsic motivation (curiosity) drives exploration
        and discovery of novel states in the environment.
        """
        print("\n=== Curiosity-Driven Learning Example ===")
        
        # Create brain with strong curiosity components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2500)
        config.set("brain.synapse_density", 0.1)
        
        # Configure for curiosity-driven learning
        config.set("neuromod.dopamine.scale", 0.8)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Configure learning mechanisms
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.015)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(12.5, 12.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable curiosity and novelty systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableNovelty(True)
        
        print("Created curiosity-driven learning system")
        print("  Curiosity: Enabled (exploration motivation)")
        print("  Novelty: Enabled (discovery motivation)")
        
        # Simulate curiosity-driven exploration
        print("\nSimulating curiosity-driven exploration...")
        
        exploration_patterns = {
            'random_walk': 0.4,
            'resource_seeking': 0.3,
            'hazard_avoidance': 0.2,
            'social_exploration': 0.1
        }
        
        novelty_history = []
        curiosity_driven_actions = []
        visited_locations = set()
        
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            
            # Apply curiosity-driven motivation
            curiosity_level = agent.getCuriosityLevel() if hasattr(agent, 'getCuriosityLevel') else 0.5
            novelty_level = agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.5
            
            # Add intrinsic motivation rewards
            reward += curiosity_level * 0.1
            reward += novelty_level * 0.2
            
            # Track novelty (visited new locations)
            agent_pos = world.getAgentBody() if hasattr(world, 'getAgentBody') else None
            if agent_pos:
                pos_key = f"{agent_pos[0]:.1f}_{agent_pos[1]:.1f}"
                if pos_key not in visited_locations:
                    visited_locations.add(pos_key)
                    reward += novelty_level * 0.5  # Bonus for new discoveries
            
            agent.applyRewardModulation(reward, 0.0)
            
            # Record exploration metrics
            novelty_history.append(reward)
            curiosity_driven_actions.append(curiosity_level)
            
            if step % 40 == 0:
                print(f"  Step {step}: Novelty = {novelty_level:.3f}, "
                      f"Curiosity = {curiosity_level:.3f}, "
                      f"Reward = {reward:+.2f}, "
                      f"Locations visited = {len(visited_locations)}")
        
        return {
            "brain": brain,
            "agent": agent,
            "world": world,
            "novelty_history": novelty_history,
            "curiosity_driven_actions": curiosity_driven_actions,
            "visited_locations": len(visited_locations),
            "config": config
        }
    
    @staticmethod
    def create_transfer_learning_example():
        """
        Example: Transfer learning between tasks.
        
        Demonstrates how NLM can transfer knowledge learned in one task
        to improve performance in a related task.
        """
        print("\n=== Transfer Learning Example ===")
        
        # Create brain optimized for transfer learning
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.08)  # Lower density for better transfer
        
        # Configure for transfer learning
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Configure plasticity for transfer learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)  # Moderate learning rate
        config.set("plasticity.stdp.tau_plus", 30.0)  # Longer time constant
        config.set("plasticity.stdp.tau_minus", 30.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.008)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Created transfer learning system")
        print("  Task A: Resource collection")
        print("  Task B: Hazard avoidance") 
        print("  Expected: Knowledge transfer from Task A to Task B")
        
        # Simulate transfer learning scenario
        print("\nSimulating transfer learning...")
        
        # Phase 1: Learn Task A (resource collection)
        print("Phase 1: Learning Task A (Resource Collection)")
        task_a_success = []
        task_a_steps = []
        
        for step in range(100):
            brain.step(step)
            
            # Simulate Task A performance
            # (In real scenario, this would involve specific task environment)
            success_rate = min(0.95, 0.5 + (step * 0.0045))  # Improving over time
            task_a_success.append(success_rate)
            task_a_steps.append(step)
            
            if step % 25 == 0:
                print(f"  Step {step}: Task A success rate = {success_rate:.2%}")
        
        # Phase 2: Transfer to Task B with modifications
        print("\nPhase 2: Transfer to Task B (Hazard Avoidance)")
        task_b_performance = []
        
        for step in range(100, 200):
            brain.step(step)
            
            # Task B performance starts lower but benefits from transfer
            # Early steps are poor (need to learn Task B specifics)
            # Later steps benefit from Task A knowledge
            if step <= 150:
                performance = 0.3 + ((step - 100) * 0.002)  # Basic learning of Task B
            else:
                # Transfer benefit kicks in after 150 steps
                base_performance = 0.6 + ((step - 150) * 0.002)
                transfer_benefit = 0.2 * min(1.0, (step - 150) / 50)  # Maximum transfer after 200 steps
                performance = min(0.95, base_performance + transfer_benefit)
            
            task_b_performance.append(performance)
            
            if step % 25 == 0:
                print(f"  Step {step}: Task B performance = {performance:.2%}")
        
        # Phase 3: Combined task performance
        print("\nPhase 3: Combined Task Performance")
        combined_performance = []
        
        for step in range(200, 300):
            brain.step(step)
            
            # Combined performance leveraging both task learnings
            task_a_contrib = task_a_success[-1] * 0.4
            task_b_contrib = task_b_performance[-1] * 0.6
            combined_performance.append(task_a_contrib + task_b_contrib)
            
            if step % 25 == 0:
                print(f"  Step {step}: Combined performance = {combined_performance[-1]:.2%}")
        
        return {
            "brain": brain,
            "task_a_performance": task_a_success,
            "task_b_performance": task_b_performance,
            "combined_performance": combined_performance,
            "config": config
        }

if __name__ == "__main__":
    print("Neuromodulation and Learning Examples")
    print("=" * 40)
    
    # Example 1: Reward modulation
    reward_result = NeuromodulationExamples.create_reward_modulation_example()
    print(f"\nReward modulation example: {len(reward_result['learning_progress'])} learning points recorded")
    
    # Example 2: Curiosity-driven learning
    curiosity_result = NeuromodulationExamples.create_curiosity_driven_learning_example()
    print(f"\nCuriosity-driven example: {curiosity_result['visited_locations']} locations visited")
    
    # Example 3: Transfer learning
    transfer_result = NeuromodulationExamples.create_transfer_learning_example()
    print(f"\nTransfer learning example: Performance tracking over 300 steps")
    
    print("\n=== All neuromodulation examples completed ===")
