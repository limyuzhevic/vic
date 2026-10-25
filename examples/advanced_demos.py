#!/usr/bin/env python3
"""
Advanced NLM Demo Scripts

This module provides advanced demonstration scripts for the NLM (Neural Learning Machine)
framework, showcasing various use cases and advanced features.

Author: NLM Development Team
License: MIT
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time
from typing import List, Tuple, Optional, Dict, Any

class AdvancedBrainTrainer:
    """
    Advanced brain training class that demonstrates sophisticated NLM usage patterns.
    """
    
    def __init__(self, config_file: str = None):
        """
        Initialize advanced brain trainer.
        
        Args:
            config_file: Path to configuration file (optional)
        """
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Training statistics
        self.training_history: List[Dict[str, Any]] = []
        self.performance_metrics: Dict[str, float] = {}
    
    def setup_environment(self, width: int = 20, height: int = 20, 
                         vision_width: int = 8, vision_height: int = 8):
        """
        Setup the training environment.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
        """
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable all learning capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, max_steps: int, seed: int = None) -> Dict[str, Any]:
        """
        Run a single training episode.
        
        Args:
            max_steps: Maximum number of steps per episode
            seed: Random seed for reproducibility
            
        Returns:
            Dictionary containing episode results
        """
        if seed is not None:
            self.world.setRandomSeed(seed)
        
        episode_stats = {
            'steps': 0,
            'total_spikes': 0,
            'firing_neurons': 0,
            'rewards': [],
            'actions': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'developmental_stages': []
        }
        
        print(f"Starting episode (max steps: {max_steps})...")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action from agent
            action = self.agent.decodeMotorCommand()
            episode_stats['actions'].append(action)
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = self._extract_reward(percept)
            episode_stats['rewards'].append(reward)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect statistics
            episode_stats['steps'] += 1
            episode_stats['total_spikes'] += self.brain.getTotalSpikeCount()
            episode_stats['firing_neurons'] += self.brain.getFiringNeuronCount()
            episode_stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
            episode_stats['novelty_levels'].append(self.agent.getNoveltyLevel())
            episode_stats['developmental_stages'].append(self.brain.getDevelopmentalStage())
            
            # Progress reporting
            if step % 100 == 0:
                print(f"  Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Spikes: {self.brain.getTotalSpikeCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {self.agent.getNoveltyLevel():.3f}")
        
        # Calculate episode statistics
        episode_stats['avg_firing_rate'] = (
            episode_stats['firing_neurons'] / max_steps
        )
        episode_stats['avg_spike_rate'] = (
            episode_stats['total_spikes'] / max_steps
        )
        episode_stats['total_reward'] = sum(episode_stats['rewards'])
        episode_stats['avg_reward'] = (
            episode_stats['total_reward'] / len(episode_stats['rewards'])
        )
        
        return episode_stats
    
    def _extract_reward(self, percept) -> float:
        """
        Extract reward signal from sensory percept.
        
        Args:
            percept: Sensory percept data
            
        Returns:
            Reward value
        """
        # Check for resource in vision
        vision = percept.getVision()
        if vision:
            width, height = vision.getWidth(), vision.getHeight()
            for x in range(width):
                for y in range(height):
                    # Simple reward logic: resources are positive
                    if vision.getType() == pynlm.WorldObjectType.Resource:
                        return 1.0
                    # Hazards are negative
                    elif vision.getType() == pynlm.WorldObjectType.Hazard:
                        return -1.0
        
        # Check internal signals for encoded rewards
        internal = percept.getInternal()
        if internal and not internal.empty():
            return internal.front()
        
        return 0.0
    
    def train(self, num_episodes: int, steps_per_episode: int, 
              save_interval: int = 100, seed: int = None) -> Dict[str, Any]:
        """
        Run training for specified number of episodes.
        
        Args:
            num_episodes: Number of training episodes
            steps_per_episode: Steps per episode
            save_interval: Save model every N episodes
            seed: Random seed for reproducibility
            
        Returns:
            Dictionary containing training results
        """
        print(f"Starting training: {num_episodes} episodes, "
              f"{steps_per_episode} steps each")
        
        total_start_time = time.time()
        
        for episode in range(num_episodes):
            print(f"\nEpisode {episode + 1}/{num_episodes}")
            
            # Run episode
            episode_stats = self.run_episode(steps_per_episode, seed)
            
            # Store episode statistics
            self.training_history.append(episode_stats)
            
            # Print episode summary
            self._print_episode_summary(episode + 1, episode_stats)
            
            # Save model periodically
            if save_interval > 0 and (episode + 1) % save_interval == 0:
                checkpoint_file = f"checkpoint_episode_{episode + 1}.bin"
                self.brain.save(checkpoint_file)
                print(f"Model saved to {checkpoint_file}")
            
            # Update random seed for next episode if specified
            if seed is not None:
                seed += 1
        
        total_time = time.time() - total_start_time
        
        # Calculate performance metrics
        self._calculate_performance_metrics()
        
        training_results = {
            'total_episodes': num_episodes,
            'total_steps': num_episodes * steps_per_episode,
            'total_time': total_time,
            'avg_episode_time': total_time / num_episodes,
            'performance_metrics': self.performance_metrics,
            'training_history': self.training_history
        }
        
        print(f"\nTraining completed in {total_time:.2f} seconds")
        self._print_training_summary(training_results)
        
        return training_results
    
    def _print_episode_summary(self, episode_num: int, stats: Dict[str, Any]):
        """Print summary of a single episode."""
        print(f"  Episode {episode_num} Summary:")
        print(f"    Steps: {stats['steps']}")
        print(f"    Avg Firing Rate: {stats['avg_firing_rate']:.2f}")
        print(f"    Avg Spike Rate: {stats['avg_spike_rate']:.2f}")
        print(f"    Total Reward: {stats['total_reward']:.2f}")
        print(f"    Average Reward: {stats['avg_reward']:.3f}")
        print(f"    Final Curiosity: {stats['curiosity_levels'][-1]:.3f}")
        print(f"    Final Novelty: {stats['novelty_levels'][-1]:.3f}")
        print(f"    Final Development Stage: {stats['developmental_stages'][-1]}")
    
    def _calculate_performance_metrics(self):
        """Calculate overall performance metrics from training history."""
        if not self.training_history:
            return
        
        num_episodes = len(self.training_history)
        
        # Calculate averages across all episodes
        self.performance_metrics = {
            'avg_firing_rate': sum(
                ep['avg_firing_rate'] for ep in self.training_history
            ) / num_episodes,
            'avg_spike_rate': sum(
                ep['avg_spike_rate'] for ep in self.training_history
            ) / num_episodes,
            'avg_total_reward': sum(
                ep['total_reward'] for ep in self.training_history
            ) / num_episodes,
            'avg_episode_steps': sum(
                ep['steps'] for ep in self.training_history
            ) / num_episodes,
            'final_curiosity': self.training_history[-1]['curiosity_levels'][-1],
            'final_novelty': self.training_history[-1]['novelty_levels'][-1],
            'final_development_stage': self.training_history[-1]['developmental_stages'][-1]
        }
    
    def _print_training_summary(self, results: Dict[str, Any]):
        """Print training summary and performance metrics."""
        print("\n" + "="*60)
        print("TRAINING SUMMARY")
        print("="*60)
        print(f"Total Episodes: {results['total_episodes']}")
        print(f"Total Steps: {results['total_steps']}")
        print(f"Total Time: {results['total_time']:.2f} seconds")
        print(f"Average Episode Time: {results['avg_episode_time']:.2f} seconds")
        
        print("\nPerformance Metrics:")
        metrics = results['performance_metrics']
        print(f"  Average Firing Rate: {metrics['avg_firing_rate']:.2f}")
        print(f"  Average Spike Rate: {metrics['avg_spike_rate']:.2f}")
        print(f"  Average Total Reward: {metrics['avg_total_reward']:.2f}")
        print(f"  Average Steps per Episode: {metrics['avg_episode_steps']:.0f}")
        print(f"  Final Curiosity Level: {metrics['final_curiosity']:.3f}")
        print(f"  Final Novelty Level: {metrics['final_novelty']:.3f}")
        print(f"  Final Development Stage: {metrics['final_development_stage']}")
class AdvancedSimulationRunner:
    """
    Advanced simulation runner that demonstrates complex multi-agent and 
    multi-environment scenarios.
    """
    
    def __init__(self):
        """Initialize advanced simulation runner."""
        self.agents: List[AdvancedBrainTrainer] = []
        self.worlds: List[pynlm.SimpleWorld] = []
        self.scenarios: List[Dict[str, Any]] = []
    
    def create_competitive_scenario(self, num_agents: int = 3) -> Dict[str, Any]:
        """
        Create a competitive multi-agent scenario.
        
        Args:
            num_agents: Number of competing agents
            
        Returns:
            Scenario configuration
        """
        scenario = {
            'type': 'competitive',
            'num_agents': num_agents,
            'world_width': 30,
            'world_height': 30,
            'vision_range': 6,
            'agent_positions': []
        }
        
        # Calculate agent positions evenly around the map
        for i in range(num_agents):
            angle = (i * 360 / num_agents) * (3.14159 / 180)
            x = 15 + 10 * math.cos(angle)
            y = 15 + 10 * math.sin(angle)
            scenario['agent_positions'].append((x, y))
        
        return scenario
    
    def run_competitive_simulation(self, num_episodes: int = 100, 
                                   steps_per_episode: int = 200) -> Dict[str, Any]:
        """
        Run competitive multi-agent simulation.
        
        Args:
            num_episodes: Number of simulation episodes
            steps_per_episode: Steps per episode
            
        Returns:
            Simulation results
        """
        print("Setting up competitive multi-agent scenario...")
        
        # Create scenario
        scenario = self.create_competitive_scenario()
        
        # Initialize agents
        for i in range(scenario['num_agents']):
            trainer = AdvancedBrainTrainer()
            trainer.setup_environment(
                scenario['world_width'], 
                scenario['world_height'],
                scenario['vision_range'],
                scenario['vision_range']
            )
            self.agents.append(trainer)
        
        print(f"Running {num_episodes} episodes with {scenario['num_agents']} agents...")
        
        # Run simulation episodes
        all_results = []
        for episode in range(num_episodes):
            print(f"\nEpisode {episode + 1}/{num_episodes}")
            
            episode_results = []
            for agent_idx, trainer in enumerate(self.agents):
                episode_stats = trainer.run_episode(steps_per_episode, episode)
                episode_stats['agent_id'] = agent_idx
                episode_stats['agent_position'] = scenario['agent_positions'][agent_idx]
                episode_results.append(episode_stats)
            
            all_results.append(episode_results)
            
            # Print episode comparison
            print("  Episode Rankings:")
            rankings = sorted(episode_results, 
                            key=lambda x: x['avg_total_reward'], 
                            reverse=True)
            for rank, result in enumerate(rankings):
                agent_id = result['agent_id']
                reward = result['avg_total_reward']
                print(f"    #{rank + 1} Agent {agent_id}: {reward:.2f} avg reward")
        
        # Calculate overall statistics
        simulation_results = {
            'scenario': scenario,
            'episodes': all_results,
            'total_episodes': num_episodes,
            'agents': len(self.agents)
        }
        
        self._print_simulation_summary(simulation_results)
        return simulation_results
    
    def _print_simulation_summary(self, results: Dict[str, Any]):
        """Print simulation summary."""
        print("\n" + "="*60)
        print("SIMULATION SUMMARY")
        print("="*60)
        
        # Calculate aggregate statistics
        all_episodes = results['episodes']
        all_agents = []
        
        for episode in all_episodes:
            for agent_result in episode:
                all_agents.append(agent_result)
        
        # Agent performance rankings
        agent_performance = {}
        for agent_result in all_agents:
            agent_id = agent_result['agent_id']
            if agent_id not in agent_performance:
                agent_performance[agent_id] = []
            agent_performance[agent_id].append(agent_result['avg_total_reward'])
        
        print("\nAgent Performance Rankings:")
        sorted_agents = sorted(agent_performance.items(),
                              key=lambda x: sum(x[1]) / len(x[1]),
                              reverse=True)
        for rank, (agent_id, rewards) in enumerate(sorted_agents):
            avg_reward = sum(rewards) / len(rewards)
            print(f"  #{rank + 1} Agent {agent_id}: {avg_reward:.2f} avg reward "
                  f"({len(rewards)} episodes)")
    
    def cleanup(self):
        """Clean up simulation resources."""
        self.agents.clear()
        self.worlds.clear()
        self.scenarios.clear()
        print("Simulation cleanup completed.")
# Convenience functions for common use cases
def create_reactive_agent():
    """
    Create a reactive agent that responds quickly to environmental stimuli.
    
    Returns:
        Configured AdvancedBrainTrainer instance
    """
    trainer = AdvancedBrainTrainer()
    trainer.setup_environment(15, 15, 5, 5)
    
    # Configure for fast reactions
    config = trainer.config
    config.set("brain.neuron_count", 500)
    config.set("brain.v_thresh", -40.0)  # Higher threshold for faster firing
    config.set("brain.tau_mem", 15.0)   # Shorter membrane time constant
    
    return trainer
def create_creative_agent():
    """
    Create a creative agent that explores and generates novel behaviors.
    
    Returns:
        Configured AdvancedBrainTrainer instance
    """
    trainer = AdvancedBrainTrainer()
    trainer.setup_environment(25, 25, 8, 8)
    
    # Enable all creative capabilities
    trainer.agent.enableCuriosity(True)
    trainer.agent.enableNoveltyDetection(True)
    trainer.agent.enableStructuralPlasticity(True)
    trainer.agent.enableDevelopment(True)
    
    # Configure for exploration
    config = trainer.config
    config.set("brain.neuron_count", 1000)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    
    return trainer
def create_cooperative_agent():
    """
    Create a cooperative agent designed to work with other agents.
    
    Returns:
        Configured AdvancedBrainTrainer instance
    """
    trainer = AdvancedBrainTrainer()
    trainer.setup_environment(20, 20, 6, 6)
    
    # Configure for cooperation
    config = trainer.config
    config.set("brain.neuron_count", 800)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    # Set social learning parameters
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    return trainer
def run_demo_sequence():
    """
    Run a comprehensive demo sequence showcasing different agent types.
    
    Returns:
        Dictionary containing demo results
    """
    print("="*60)
    print("NLM ADVANCED DEMOS")
    print("="*60)
    
    demos = [
        ("Reactive Agent", create_reactive_agent),
        ("Creative Agent", create_creative_agent),
        ("Cooperative Agent", create_cooperative_agent),
    ]
    
    results = {}
    
    for demo_name, agent_factory in demos:
        print(f"\n{'='*20} {demo_name} Demo {'='*20}")
        
        # Create and configure agent
        agent = agent_factory()
        
        # Run demonstration
        if demo_name == "Reactive Agent":
            stats = agent.run_episode(100, seed=42)
        elif demo_name == "Creative Agent":
            stats = agent.train(20, 50, save_interval=10)
        else:  # Cooperative Agent
            stats = agent.run_episode(150, seed=123)
        
        results[demo_name] = stats
        
        # Print demo summary
        print(f"\n{demo_name} Demo Summary:")
        print(f"  Total Steps: {stats['steps']}")
        print(f"  Average Reward: {stats.get('avg_reward', stats.get('avg_total_reward', 0)):.3f}")
        print(f"  Final Curiosity: {stats.get('curiosity_levels', [0])[-1]:.3f}")
    
    print(f"\n{'='*60}")
    print("ALL DEMOS COMPLETED")
    print(f"{'='*60}")
    
    return results
if __name__ == "__main__":
    import math
    
    print("Advanced NLM Demo Scripts")
    print("Available demonstrations:")
    print("  1. Run comprehensive demo sequence")
    print("  2. Run competitive multi-agent simulation")
    print("  3. Create custom agent configurations")
    print("  4. Exit")
    
    # Uncomment to run demos
    # run_demo_sequence()
    
    print("\nTo run demos, uncomment the appropriate function call in main()")
    print("Or run specific demos by importing and calling functions directly.")
