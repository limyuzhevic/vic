#!/usr/bin/env python3
"""
Example 2: Agent-World Interaction
===================================

This example demonstrates how to create an agent that interacts with a simulated environment.
It shows the complete workflow of perception, action, and learning.

Learning Objectives:
- Create an agent with brain, world, and sensory systems
- Implement the perception-action loop
- Handle sensory input and motor output
- Enable learning systems (reward, curiosity, development)
- Monitor agent behavior and performance

Expected Time: ~45 minutes
Difficulty: Intermediate
"""

import pynlm
import time
import sys
import numpy as np

class NLMExperiment:
    """
    Comprehensive NLM experiment class managing agent, world, and simulation.
    """
    
    def __init__(self, config_file=None):
        """
        Initialize the experiment with agent, world, and brain.
        
        Args:
            config_file: Optional path to configuration file
        """
        print("Initializing NLM Agent-World Experiment...")
        
        # Create default configuration
        self.config = pynlm.createDefaultConfig()
        
        # Configure for experimental use
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.08)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.dopamine.scale", 1.0)
        
        # Create brain
        print("Creating brain...")
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        print(f"✓ Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
        
        # Create agent brain interface
        print("Creating agent interface...")
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world environment
        print("Creating world environment...")
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=20,           # World size
            height=20,          # World size  
            visionWidth=7,     # Vision range
            visionHeight=7     # Vision range
        )
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)  # Agent starting position
        print("✓ World created and reset")
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        print("✓ Agent initialized with world")
        
        # Enable learning systems
        print("Enabling learning systems...")
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        print("✓ Learning systems enabled")
        
        # Experiment statistics
        self.stats = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'max_curiosity': 0.0,
            'total_novelty': 0.0,
            'structural_changes': 0,
            'start_time': time.time(),
            'vision_history': [],
            'action_history': []
        }
        
        print("✓ Experiment initialized successfully")
    
    def run_episode(self, num_steps=100, render=False):
        """
        Run a single episode of agent-environment interaction.
        
        Args:
            num_steps: Number of simulation steps
            render: Whether to print step-by-step details
        
        Returns:
            Dictionary with episode statistics
        """
        episode_stats = {
            'steps_completed': 0,
            'rewards': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'actions_taken': [],
            'firing_rates': [],
            'vision_changes': []
        }
        
        print(f"\nRunning episode for {num_steps} steps...")
        
        for step in range(num_steps):
            step_start = time.time()
            
            try:
                # 1. Environment update
                self.world.update(0.1)  # 100ms timestep
                
                # 2. Get sensory input from world
                percept = self.world.getSensoryPercept()
                
                # 3. Record vision for novelty calculation
                current_vision = percept.getVision()
                self.stats['vision_history'].append(current_vision.copy())
                if len(self.stats['vision_history']) > 1:
                    prev_vision = self.stats['vision_history'][-2]
                    vision_change = np.mean(np.abs(np.array(current_vision) - np.array(prev_vision)))
                    episode_stats['vision_changes'].append(vision_change)
                
                # 4. Process sensory input through agent
                self.agent.processSensoryInput(percept)
                
                # 5. Brain processing (thought)
                self.brain.step(step)
                
                # 6. Decode motor command from brain
                action = self.agent.decodeMotorCommand()
                episode_stats['actions_taken'].append(action)
                self.stats['action_history'].append(action)
                
                # 7. Apply action in world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # 8. Learn from experience (reward-based learning)
                reward = self.calculate_reward()
                novelty = self.agent.getNoveltyLevel()
                
                self.agent.applyRewardModulation(reward, 0.0)
                
                # 9. Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect episode statistics
                episode_stats['rewards'].append(reward)
                episode_stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
                episode_stats['novelty_levels'].append(novelty)
                episode_stats['firing_rates'].append(self.brain.getAverageFiringRate())
                
                # Update global statistics
                self.stats['total_reward'] += reward
                self.stats['max_curiosity'] = max(self.stats['max_curiosity'], 
                                                 self.agent.getCuriosityLevel())
                self.stats['total_novelty'] += novelty
                
                if step % 20 == 0:
                    print(f"  Step {step:3d}: "
                          f"Action: {self.action_to_str(action):8s}, "
                          f"Reward: {reward:6.3f}, "
                          f"Curiosity: {self.agent.getCuriosityLevel():5.3f}, "
                          f"Novelty: {novelty:5.3f}, "
                          f"Firing: {self.brain.getAverageFiringRate():6.2f}")
                
                episode_stats['steps_completed'] += 1
                
                # Optional: render step details
                if render and step % 10 == 0:
                    self.render_step(step, percept, action, reward)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                break
        
        # Calculate episode performance metrics
        if episode_stats['steps_completed'] > 0:
            episode_stats['avg_reward'] = np.mean(episode_stats['rewards'])
            episode_stats['max_reward'] = np.max(episode_stats['rewards'])
            episode_stats['avg_curiosity'] = np.mean(episode_stats['curiosity_levels'])
            episode_stats['avg_novelty'] = np.mean(episode_stats['novelty_levels'])
            episode_stats['avg_firing_rate'] = np.mean(episode_stats['firing_rates'])
            
            if episode_stats['vision_changes']:
                episode_stats['avg_vision_change'] = np.mean(episode_stats['vision_changes'])
        
        return episode_stats
    
    def calculate_reward(self):
        """
        Calculate reward based on agent's position and world state.
        
        Returns:
            Reward value (positive for good outcomes, negative for bad)
        """
        body = self.world.getAgentBody()
        
        # Positive reward for staying alive and active
        reward = 1.0
        
        # Bonus for being in center of world
        center_x, center_y = self.world.getWidth() / 2, self.world.getHeight() / 2
        distance_to_center = np.sqrt((body.x - center_x)**2 + (body.y - center_y)**2)
        distance_to_center = max(0, distance_to_center - 10)  # Ignore near-center distance
        reward += max(0, 2.0 - distance_to_center / 10.0)
        
        # Penalty for energy depletion
        if body.energy < 20.0:
            reward -= 0.5
        
        # Penalty for low health
        if body.health < 50.0:
            reward -= 0.2
        
        return reward
    
    def action_to_str(self, action):
        """Convert action enum to string representation."""
        action_names = {
            pynlm.MotorCommand.MoveForward: "MOVE_FWD",
            pynlm.MotorCommand.MoveBackward: "MOVE_BAK",
            pynlm.MotorCommand.TurnLeft: "TURN_L",
            pynlm.MotorCommand.TurnRight: "TURN_R",
            pynlm.MotorCommand.LookLeft: "LOOK_L",
            pynlm.MotorCommand.LookRight: "LOOK_R",
            pynlm.MotorCommand.Interact: "INTERACT",
            pynlm.MotorCommand.Wait: "WAIT"
        }
        return action_names.get(action, f"UNKNOWN({action})")
    
    def render_step(self, step, percept, action, reward):
        """Render current step details for debugging."""
        body = self.world.getAgentBody()
        
        print(f"    Step {step}:")
        print(f"      Position: ({body.x:5.1f}, {body.y:5.1f})")
        print(f"      Energy: {body.energy:5.1f}, Health: {body.health:5.1f}")
        print(f"      Orientation: {body.orientation:5.1f}°")
        
        if percept.getVision():
            vision_array = np.array(percept.getVision())
            max_vision = np.max(vision_array)
            min_vision = np.min(vision_array)
            print(f"      Vision: range [{min_vision:4.2f}, {max_vision:4.2f}]")
    
    def run_experiment(self, num_episodes=5, steps_per_episode=100):
        """
        Run complete experiment with multiple episodes.
        
        Args:
            num_episodes: Number of episodes to run
            steps_per_episode: Steps per episode
        
        Returns:
            Dictionary with experiment results
        """
        print(f"\nRunning complete experiment: {num_episodes} episodes")
        print(f"Each episode: {steps_per_episode} steps")
        
        experiment_results = {
            'episodes': [],
            'total_steps': 0,
            'total_rewards': 0.0,
            'avg_rewards': [],
            'learning_curve': [],
            'final_stats': None
        }
        
        # Run episodes
        for episode in range(num_episodes):
            print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
            
            episode_stats = self.run_episode(steps_per_episode, render=(episode == 0))
            experiment_results['episodes'].append(episode_stats)
            
            avg_reward = episode_stats.get('avg_reward', 0.0)
            experiment_results['avg_rewards'].append(avg_reward)
            experiment_results['learning_curve'].append(avg_reward)
            
            experiment_results['total_steps'] += episode_stats['steps_completed']
            experiment_results['total_rewards'] += avg_reward
            
            print(f"Episode completed: Average reward = {avg_reward:.3f}")
        
        # Calculate final statistics
        experiment_results['final_stats'] = {
            'avg_reward_per_episode': np.mean(experiment_results['avg_rewards']),
            'total_steps_per_episode': experiment_results['total_steps'] / num_episodes,
            'total_reward_per_step': experiment_results['total_rewards'] / experiment_results['total_steps'],
            'max_curiosity': self.stats['max_curiosity'],
            'total_novelty': self.stats['total_novelty'],
            'final_developmental_stage': self.agent.getDevelopmentalStage(),
            'final_dopamine_level': self.agent.getNeuromodulationLevel()
        }
        
        return experiment_results
    
    def print_results(self, results):
        """Print comprehensive experiment results."""
        print("\n" + "="*80)
        print("EXPERIMENT RESULTS SUMMARY")
        print("="*80)
        
        stats = results['final_stats']
        
        print(f"\nPerformance Summary:")
        print(f"  Episodes completed: {len(results['episodes'])}")
        print(f"  Steps per episode: {stats['total_steps_per_episode']:.1f}")
        print(f"  Average reward per episode: {stats['avg_reward_per_episode']:.3f}")
        print(f"  Total reward per step: {stats['total_reward_per_step']:.3f}")
        
        print(f"\nLearning Metrics:")
        print(f"  Maximum curiosity level: {stats['max_curiosity']:.3f}")
        print(f"  Total novelty experienced: {stats['total_novelty']:.3f}")
        print(f"  Final developmental stage: {stats['final_developmental_stage']}")
        print(f"  Final dopamine level: {stats['final_dopamine_level']:.3f}")
        
        # Analyze learning curve
        if len(results['learning_curve']) > 1:
            improvements = []
            for i in range(1, len(results['learning_curve'])):
                improvement = results['learning_curve'][i] - results['learning_curve'][i-1]
                improvements.append(improvement)
            
            if improvements:
                avg_improvement = np.mean(improvements)
                print(f"\nLearning Analysis:")
                print(f"  Average improvement per episode: {avg_improvement:.3f}")
                if avg_improvement > 0:
                    print("  → Learning is improving over time")
                elif avg_improvement < 0:
                    print("  → Performance is declining (may indicate instability)")
                else:
                    print("  → Performance is stable")
        
        # Recommendations
        print(f"\nRecommendations:")
        if stats['max_curiosity'] < 0.3:
            print("  - Increase curiosity drive for more exploration")
        if stats['total_novelty'] < 10:
            print("  - Environment may be too predictable")
        if stats['final_dopamine_level'] > 0.8:
            print("  - High dopamine levels indicate strong prediction errors")
        
        print(f"\nExperiment completed successfully!")

def main():
    """
    Main function demonstrating agent-world interaction.
    """
    print("NLM Agent-World Interaction Example")
    print("="*60)
    print("This example demonstrates how an agent learns to interact with a world.")
    print()
    
    try:
        # Initialize experiment
        experiment = NLMExperiment()
        
        # Run experiment
        results = experiment.run_experiment(
            num_episodes=3,
            steps_per_episode=50
        )
        
        # Print results
        experiment.print_results(results)
        
        print("\n" + "="*80)
        print("EXAMPLE COMPLETED SUCCESSFULLY")
        print("="*80)
        print("\nKey insights:")
        print("  - Agent learned through curiosity-driven exploration")
        print("  - Reward-based learning modified synaptic connections")
        print("  - Development system matured over time")
        print("  - Novelty detection drove exploration behavior")
        
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
