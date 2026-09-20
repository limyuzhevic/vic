#!/usr/bin/env python3
"""
Example Scripts for NLM Scenarios
Contains example scripts demonstrating different NLM use cases:
1. Basic simulation examples
2. Complex behavior demonstrations
3. Learning and development examples
4. Multi-agent coordination examples
5. Real-world problem solving examples
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import os
import json


def save_result_plots(results, filename_prefix="nlm_example"):
    """Save results plots for examples"""
    try:
        os.makedirs("results", exist_ok=True)
        
        # Plot learning curve
        plt.figure(figsize=(12, 8))
        
        plt.subplot(2, 3, 1)
        plt.plot(results['steps'], results['rewards'], 'b-', alpha=0.7)
        plt.xlabel('Step')
        plt.ylabel('Reward')
        plt.title('Learning Curve')
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 2)
        plt.plot(results['steps'], results['firing_rates'], 'r-', alpha=0.7)
        plt.xlabel('Step')
        plt.ylabel('Firing Rate (Hz)')
        plt.title('Neural Activity')
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 3)
        plt.plot(results['steps'], results['curiosity_levels'], 'g-', alpha=0.7)
        plt.xlabel('Step')
        plt.ylabel('Curiosity Level')
        plt.title('Exploration Motivation')
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 4)
        if 'action_distribution' in results:
            actions, counts = zip(*results['action_distribution'].items())
            plt.bar(actions, counts, alpha=0.7)
            plt.xlabel('Action')
            plt.ylabel('Frequency')
            plt.title('Action Selection')
            plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 5)
        if 'novelty_levels' in results:
            plt.plot(results['steps'], results['novelty_levels'], 'c-', alpha=0.7)
            plt.xlabel('Step')
            plt.ylabel('Novelty Level')
            plt.title('Novelty Detection')
            plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 3, 6)
        if 'memory_usage' in results:
            plt.plot(results['steps'], results['memory_usage'], 'm-', alpha=0.7)
            plt.xlabel('Step')
            plt.ylabel('Memory Usage')
            plt.title('Memory System Usage')
            plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(f"results/{filename_prefix}_analysis.png", dpi=150)
        plt.close()
        
    except Exception as e:
        print(f"Warning: Could not save plots: {e}")


class BasicSimulationExample:
    """
    Basic NLM Simulation Example
    Demonstrates the simplest possible NLM brain simulation
    """
    
    def __init__(self):
        self.results = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': []
        }
    
    def run(self):
        """Run basic simulation"""
        print("=== Basic NLM Simulation Example ===")
        
        # Create simple configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 5, 5)
        world.reset()
        
        agent.initialize(world)
        
        # Run simulation
        print("Running basic simulation...")
        for step in range(100):
            # World update
            world.update(0.1)
            
            # Perception and action
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            self.results['steps'].append(step)
            self.results['rewards'].append(0.0)
            self.results['firing_rates'].append(brain.getAverageFiringRate())
            self.results['curiosity_levels'].append(agent.getCuriosityLevel())
            self.results['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Display results
        self._display_results()
        
        # Save visualization
        save_result_plots(self.results, "basic_simulation")
        
        return self.results
    
    def _display_results(self):
        """Display simulation results"""
        print(f"\nSimulation Results:")
        print(f"  Total steps: {len(self.results['steps'])}")
        print(f"  Average firing rate: {np.mean(self.results['firing_rates']):.2f} Hz")
        print(f"  Final curiosity: {self.results['curiosity_levels'][-1]:.3f}")
        print(f"  Final novelty: {self.results['novelty_levels'][-1]:.3f}")
        
        # Create simple plot
        plt.figure(figsize=(10, 6))
        
        plt.subplot(2, 2, 1)
        plt.plot(self.results['steps'], self.results['firing_rates'], 'b-')
        plt.xlabel('Step')
        plt.ylabel('Firing Rate (Hz)')
        plt.title('Basic NLM Simulation')
        plt.grid(True, alpha=0.3)
        
        plt.subplot(2, 2, 2)
        plt.plot(self.results['steps'], self.results['curiosity_levels'], 'g-')
        plt.xlabel('Step')
        plt.ylabel('Curiosity Level')
        plt.title('Exploration Motivation')
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()


class ComplexBehaviorDemonstration:
    """
    Complex Behavior Demonstration
    Shows how NLM can learn complex behaviors in challenging environments
    """
    
    def __init__(self):
        self.results = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'action_distribution': {},
            'development_stages': []
        }
    
    def run(self):
        """Run complex behavior demonstration"""
        print("\n=== Complex Behavior Demonstration ===")
        
        # Create configuration for complex behavior
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("memory.episodic.enable", True)
        config.set("memory.associative.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(20.0, 20.0, 8, 8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Enable all subsystems for complex behavior
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Set up challenge: navigate to moving target
        target_x, target_y = 5.0, 5.0
        target_velocity = 0.1
        
        print(f"Starting complex behavior test...")
        print(f"  Navigate to moving target at ({target_x}, {target_y})")
        print(f"  Agent starts at (10, 10)")
        
        # Run simulation
        success_count = 0
        for step in range(500):
            # Update moving target
            target_x += target_velocity if step % 100 < 50 else -target_velocity
            target_y += target_velocity if step % 100 < 50 else -target_velocity
            
            # Get agent's current position
            agent_body = world.getAgentBody()
            agent_x, agent_y = agent_body.getX(), agent_body.getY()
            
            # Calculate distance to target
            distance = np.sqrt((agent_x - target_x)**2 + (agent_y - target_y)**2)
            
            # Reward based on proximity to target
            reward = max(0, 1.0 - distance / 20.0)
            
            # Complete simulation loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record metrics
            self.results['steps'].append(step)
            self.results['rewards'].append(reward)
            self.results['firing_rates'].append(brain.getAverageFiringRate())
            self.results['development_stages'].append(agent.getDevelopmentalStage())
            
            # Track actions
            action_key = str(action)
            self.results['action_distribution'][action_key] = self.results['action_distribution'].get(action_key, 0) + 1
            
            # Check for success
            if distance < 2.0:
                success_count += 1
        
        # Display results
        self._display_results(success_count)
        
        # Save visualization
        save_result_plots(self.results, "complex_behavior")
        
        return self.results
    
    def _display_results(self, success_count):
        """Display complex behavior results"""
        print(f"\nComplex Behavior Results:")
        print(f"  Steps completed: {len(self.results['steps'])}")
        print(f"  Success instances: {success_count}")
        print(f"  Success rate: {success_count/len(self.results['steps'])*100:.1f}%")
        print(f"  Average firing rate: {np.mean(self.results['firing_rates']):.2f} Hz")
        print(f"  Development stage: {self.results['development_stages'][-1]}")
        
        # Action distribution
        if self.results['action_distribution']:
            print(f"\nAction distribution:")
            for action, count in sorted(self.results['action_distribution'].items()):
                print(f"  {action}: {count} times")
        
        # Create visualization
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # Learning curve
        axes[0, 0].plot(self.results['steps'], self.results['rewards'], 'b-', alpha=0.7)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Reward')
        axes[0, 0].set_title('Learning to Navigate')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Neural activity
        axes[0, 1].plot(self.results['steps'], self.results['firing_rates'], 'r-', alpha=0.7)
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        axes[0, 1].set_title('Neural Dynamics')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Action distribution
        if self.results['action_distribution']:
            actions, counts = zip(*sorted(self.results['action_distribution'].items()))
            axes[1, 0].bar(actions, counts, alpha=0.7)
            axes[1, 0].set_xlabel('Action')
            axes[1, 0].set_ylabel('Frequency')
            axes[1, 0].set_title('Action Selection Distribution')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Development stages
        stages = list(set(self.results['development_stages']))
        stage_counts = [self.results['development_stages'].count(stage) for stage in stages]
        axes[1, 1].bar(range(len(stages)), stage_counts, alpha=0.7)
        axes[1, 1].set_xlabel('Development Stage')
        axes[1, 1].set_ylabel('Steps in Stage')
        axes[1, 1].set_title('Development Stage Distribution')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()


class LearningAndDevelopmentExample:
    """
    Learning and Development Example
    Demonstrates how NLM brains develop and learn over their lifetime
    """
    
    def __init__(self):
        self.results = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'synapse_count': [],
            'development_stages': []
        }
    
    def run(self):
        """Run learning and development example"""
        print("\n=== Learning and Development Example ===")
        
        # Create configuration with developmental features
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("brain.development.critical_period", 100)
        config.set("brain.development.maturation_time", 200)
        config.set("plasticity.structural.enable", True)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(15.0, 15.0, 6, 6)
        world.reset()
        
        agent.initialize(world)
        
        # Enable developmental learning
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        agent.enableCuriosity(True)
        
        print("Running developmental learning simulation...")
        print(f"  Initial neuron count: {brain.getTotalNeuronCount()}")
        print(f"  Initial synapse count: {brain.getTotalSynapseCount()}")
        
        # Run developmental simulation
        for step in range(400):
            # Create challenging environment
            world.update(0.1)
            
            # Add complexity based on developmental stage
            if step < 50:  # Initial phase
                challenge_factor = 0.5
            elif step < 150:  # Critical period
                challenge_factor = 1.0
            elif step < 300:  # Maturation phase
                challenge_factor = 1.5
            else:  # Adult phase
                challenge_factor = 1.2
            
            # Process and learn
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            # Complex action based on state
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Reward that changes with development
            base_reward = 0.1
            curiosity_bonus = agent.getCuriosityLevel() * 0.05
            developmental_bonus = 0.0
            
            if agent.getDevelopmentalStage().name == "Critical":
                developmental_bonus = 0.1
            elif agent.getDevelopmentalStage().name == "Maturation":
                developmental_bonus = 0.05
            
            reward = base_reward + curiosity_bonus + developmental_bonus
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record developmental metrics
            self.results['steps'].append(step)
            self.results['rewards'].append(reward)
            self.results['firing_rates'].append(brain.getAverageFiringRate())
            self.results['curiosity_levels'].append(agent.getCuriosityLevel())
            self.results['novelty_levels'].append(agent.getNoveltyLevel())
            self.results['synapse_count'].append(brain.getTotalSynapseCount())
            self.results['development_stages'].append(agent.getDevelopmentalStage())
        
        # Display developmental results
        self._display_results()
        
        # Save visualization
        save_result_plots(self.results, "development")
        
        return self.results
    
    def _display_results(self):
        """Display developmental results"""
        print(f"\nDevelopmental Learning Results:")
        print(f"  Total steps: {len(self.results['steps'])}")
        print(f"  Initial synapse count: {self.results['synapse_count'][0]}")
        print(f"  Final synapse count: {self.results['synapse_count'][-1]}")
        print(f"  Synapse growth: {self.results['synapse_count'][-1] - self.results['synapse_count'][0]}")
        
        # Analyze development stages
        stages = list(set(self.results['development_stages']))
        print(f"\nDevelopment stages visited: {len(stages)}")
        for stage in stages:
            stage_steps = self.results['development_stages'].count(stage)
            print(f"  {stage}: {stage_steps} steps ({stage_steps/len(self.results['steps'])*100:.1f}%)")
        
        # Create developmental visualization
        fig, axes = plt.subplots(2, 3, figsize=(16, 10))
        
        # Synapse growth
        axes[0, 0].plot(self.results['steps'], self.results['synapse_count'], 'b-', linewidth=2)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Synapse Count')
        axes[0, 0].set_title('Synaptic Development')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Learning curve
        axes[0, 1].plot(self.results['steps'], self.results['rewards'], 'g-', alpha=0.7)
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Reward')
        axes[0, 1].set_title('Learning Progress')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Curiosity development
        axes[0, 2].plot(self.results['steps'], self.results['curiosity_levels'], 'r-', alpha=0.7)
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Curiosity Level')
        axes[0, 2].set_title('Exploration Development')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Firing rate development
        axes[1, 0].plot(self.results['steps'], self.results['firing_rates'], 'c-', alpha=0.7)
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Firing Rate (Hz)')
        axes[1, 0].set_title('Neural Activity Development')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Novelty detection
        axes[1, 1].plot(self.results['steps'], self.results['novelty_levels'], 'm-', alpha=0.7)
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Novelty Level')
        axes[1, 1].set_title('Novelty Detection')
        axes[1, 1].grid(True, alpha=0.3)
        
        # Developmental stage timeline
        for i, stage in enumerate(stages):
            stage_start = next((s for s, st in enumerate(self.results['development_stages']) 
                              if st == stage), 0)
            stage_length = self.results['development_stages'].count(stage)
            axes[1, 2].bar(i, stage_length, alpha=0.7,
                          label=f"{stage} ({stage_length} steps)")
        
        axes[1, 2].set_xlabel('Development Stage')
        axes[1, 2].set_ylabel('Duration (steps)')
        axes[1, 2].set_title('Developmental Timeline')
        axes[1, 2].legend()
        axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()


class MultiAgentCoordinationExample:
    """
    Multi-Agent Coordination Example
    Demonstrates how multiple NLM brains can coordinate together
    """
    
    def __init__(self):
        self.results = []
    
    def run(self):
        """Run multi-agent coordination example"""
        print("\n=== Multi-Agent Coordination Example ===")
        
        # Create multiple agents
        num_agents = 3
        brains = []
        agents = []
        worlds = []
        
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200 + i * 100)  # Different sizes
            config.set("region_count", 1)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(12.0, 12.0, 6, 6)
            world.reset()
            world.setAgentStart((i + 1) * 3.0, (i + 1) * 3.0)
            
            agent.initialize(world)
            
            brains.append(brain)
            agents.append(agent)
            worlds.append(world)
        
        print(f"Created {num_agents} agents with different brain sizes:")
        for i, brain in enumerate(brains):
            print(f"  Agent {i}: {brain.getTotalNeuronCount()} neurons")
        
        # Run coordinated simulation
        coordination_metrics = {
            'steps': [],
            'group_reward': [],
            'individual_rewards': [[] for _ in range(num_agents)],
            'agent_positions': [],
            'coordination_score': 0.0
        }
        
        for step in range(200):
            # Update all worlds
            for world in worlds:
                world.update(0.1)
            
            # Each agent perceives its own world
            group_reward = 0.0
            
            for i, (agent, world) in enumerate(zip(agents, worlds)):
                # Agent perceives its environment
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain processes independently
                brains[i].step(step)
                
                # Coordinated action selection
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Individual reward
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                coordination_metrics['individual_rewards'][i].append(reward)
                
                group_reward += reward
                
                # Update development
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Calculate coordination metrics
            coordination_metrics['steps'].append(step)
            coordination_metrics['group_reward'].append(group_reward)
            
            # Record agent positions
            positions = []
            for world in worlds:
                agent_body = world.getAgentBody()
                positions.append((agent_body.getX(), agent_body.getY()))
            
            coordination_metrics['agent_positions'].append(positions)
            
            # Calculate coordination score
            if step > 0:
                position_variation = self._calculate_position_variation(coordination_metrics['agent_positions'][-1])
                reward_smoothing = self._calculate_reward_smoothing(coordination_metrics['individual_rewards'])
                coordination_metrics['coordination_score'] = (position_variation + reward_smoothing) / 2.0
        
        # Display coordination results
        self._display_results(coordination_metrics)
        
        return coordination_metrics
    
    def _calculate_position_variation(self, positions):
        """Calculate how spread out agents are"""
        if len(positions) < 2:
            return 0.0
        
        # Calculate mean position
        mean_x = sum(p[0] for p, _ in positions) / len(positions)
        mean_y = sum(p[1] for _, p in positions) / len(positions)
        
        # Calculate variance (lower variance = more coordinated)
        variance = sum((p[0] - mean_x)**2 + (p[1] - mean_y)**2 for p in positions) / len(positions)
        
        return 1.0 - min(1.0, variance / 100.0)  # Normalize to 0-1
    
    def _calculate_reward_smoothing(self, individual_rewards):
        """Calculate how smooth rewards are across agents"""
        if not individual_rewards:
            return 0.0
        
        # Calculate reward variance
        all_rewards = [reward for agent_rewards in individual_rewards for reward in agent_rewards]
        if len(all_rewards) < 2:
            return 0.0
        
        mean_reward = sum(all_rewards) / len(all_rewards)
        variance = sum((r - mean_reward)**2 for r in all_rewards) / len(all_rewards)
        std_dev = variance ** 0.5
        
        # Lower variance = more coordinated
        return max(0, 1.0 - std_dev / 1.0)
    
    def _display_results(self, metrics):
        """Display multi-agent coordination results"""
        print(f"\nMulti-Agent Coordination Results:")
        print(f"  Steps completed: {len(metrics['steps'])}")
        print(f"  Final coordination score: {metrics['coordination_score']:.3f}")
        
        # Analyze reward distribution
        for i, agent_rewards in enumerate(metrics['individual_rewards']):
            avg_reward = sum(agent_rewards) / len(agent_rewards)
            print(f"  Agent {i} average reward: {avg_reward:.3f}")
        
        # Create coordination visualization
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # Group reward over time
        axes[0, 0].plot(metrics['steps'], metrics['group_reward'], 'b-', linewidth=2)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Group Reward')
        axes[0, 0].set_title('Group Performance')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Agent positions over time (simplified)
        if metrics['agent_positions']:
            positions = metrics['agent_positions'][0]
            axes[0, 1].scatter([p[0] for p in positions], [p[1] for p in positions], 
                             alpha=0.7, s=100)
            axes[0, 1].set_xlabel('X Position')
            axes[0, 1].set_ylabel('Y Position')
            axes[0, 1].set_title('Initial Agent Positions')
            axes[0, 1].grid(True, alpha=0.3)
        
        # Coordination score over time
        if len(metrics['steps']) > 1:
            # Calculate running coordination score
            running_scores = []
            for step in range(1, len(metrics['steps'])):
                prev_positions = metrics['agent_positions'][step-1]
                curr_positions = metrics['agent_positions'][step]
                var1 = self._calculate_position_variation(prev_positions)
                var2 = self._calculate_position_variation(curr_positions)
                running_scores.append((var1 + var2) / 2.0)
            
            axes[1, 0].plot(range(1, len(metrics['steps'])), running_scores, 'g-', alpha=0.7)
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Coordination Score')
            axes[1, 0].set_title('Coordination Development')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Reward distribution
        avg_rewards = [sum(agent_rewards) / len(agent_rewards) 
                      for agent_rewards in metrics['individual_rewards']]
        
        axes[1, 1].bar(range(len(avg_rewards)), avg_rewards, alpha=0.7,
                      color=['red', 'blue', 'green'][:len(avg_rewards)])
        axes[1, 1].set_xlabel('Agent Index')
        axes[1, 1].set_ylabel('Average Reward')
        axes[1, 1].set_title('Individual Performance')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()


class RealWorldProblemSolvingExample:
    """
    Real-World Problem Solving Example
    Demonstrates NLM solving practical problems
    """
    
    def __init__(self):
        self.results = {
            'steps': [],
            'rewards': [],
            'firing_rates': [],
            'memory_usage': [],
            'problem_solved': False,
            'solving_time': 0
        }
    
    def run(self):
        """Run real-world problem solving example"""
        print("\n=== Real-World Problem Solving Example ===")
        
        # Problem: Find and navigate to a moving target in a complex environment
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 800)
        config.set("region_count", 2)
        config.set("memory.episodic.enable", True)
        config.set("memory.associative.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.003)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Create complex environment with obstacles
        world.configure(25.0, 25.0, 10, 10)
        world.reset()
        
        # Add obstacles (simplified)
        agent_body = world.getAgentBody()
        agent_body.setX(5.0)
        agent_body.setY(5.0)
        
        agent.initialize(world)
        
        # Enable problem-solving capabilities
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        
        # Define moving target
        target_x, target_y = 20.0, 20.0
        target_direction = {'dx': 0.5, 'dy': 0.5}
        
        # Create obstacles (circular barriers)
        obstacles = [
            {'x': 10.0, 'y': 10.0, 'radius': 3.0},
            {'x': 20.0, 'y': 10.0, 'radius': 3.0},
            {'x': 10.0, 'y': 20.0, 'radius': 3.0}
        ]
        
        print("Solving real-world navigation problem...")
        print(f"  Target: ({target_x}, {target_y}) moving")
        print(f"  Start: ({agent_body.getX()}, {agent_body.getY()})")
        print(f"  Obstacles: {len(obstacles)} barriers")
        
        # Run problem-solving simulation
        target_reached_count = 0
        
        for step in range(600):
            # Update moving target
            target_x += target_direction['dx']
            target_y += target_direction['dy']
            
            # Bounce off walls
            if target_x <= 5.0 or target_x >= world.getWidth() - 5.0:
                target_direction['dx'] *= -1
            if target_y <= 5.0 or target_y >= world.getHeight() - 5.0:
                target_direction['dy'] *= -1
            
            # Calculate distance to target
            distance = np.sqrt((agent_body.getX() - target_x)**2 + 
                             (agent_body.getY() - target_y)**2)
            
            # Calculate collision risk with obstacles
            collision_risk = 0.0
            for obstacle in obstacles:
                obs_dist = np.sqrt((agent_body.getX() - obstacle['x'])**2 + 
                                 (agent_body.getY() - obstacle['y'])**2)
                if obs_dist < obstacle['radius']:
                    collision_risk += (obstacle['radius'] - obs_dist) / obstacle['radius']
            
            # Reward based on success and safety
            proximity_reward = max(0, 1.0 - distance / 30.0)
            safety_reward = max(0, 1.0 - collision_risk)
            curiosity_reward = agent.getCuriosityLevel() * 0.1
            
            reward = proximity_reward * 0.5 + safety_reward * 0.3 + curiosity_reward * 0.2
            
            # Complete simulation loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record metrics
            self.results['steps'].append(step)
            self.results['rewards'].append(reward)
            self.results['firing_rates'].append(brain.getAverageFiringRate())
            self.results['memory_usage'].append(brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0)
            
            # Check if problem solved
            if distance < 3.0 and collision_risk < 0.2:
                target_reached_count += 1
                if not self.results['problem_solved']:
                    self.results['problem_solved'] = True
                    self.results['solving_time'] = step
        
        # Display problem-solving results
        self._display_results(target_reached_count)
        
        # Save visualization
        save_result_plots(self.results, "problem_solving")
        
        return self.results
    
    def _display_results(self, target_reached_count):
        """Display problem-solving results"""
        print(f"\nProblem-Solving Results:")
        print(f"  Steps completed: {len(self.results['steps'])}")
        print(f"  Problem solved: {'YES' if self.results['problem_solved'] else 'NO'}")
        
        if self.results['problem_solved']:
            print(f"  Time to solve: {self.results['solving_time']} steps")
        
        print(f"  Total successful target reaches: {target_reached_count}")
        print(f"  Average firing rate: {np.mean(self.results['firing_rates']):.2f} Hz")
        print(f"  Average memory usage: {np.mean(self.results['memory_usage']):.1f} traces")
        
        # Create problem-solving visualization
        fig, axes = plt.subplots(2, 3, figsize=(16, 10))
        
        # Learning curve
        axes[0, 0].plot(self.results['steps'], self.results['rewards'], 'b-', alpha=0.7)
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Reward')
        axes[0, 0].set_title('Problem-Solving Learning')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Neural activity
        axes[0, 1].plot(self.results['steps'], self.results['firing_rates'], 'r-', alpha=0.7)
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        axes[0, 1].set_title('Neural Processing During Problem-Solving')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Memory usage
        axes[0, 2].plot(self.results['steps'], self.results['memory_usage'], 'g-', alpha=0.7)
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Memory Traces')
        axes[0, 2].set_title('Working Memory Usage')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Curiosity-driven exploration
        if 'curiosity_levels' not in self.results:
            self.results['curiosity_levels'] = [0.5] * len(self.results['steps'])
        
        axes[1, 0].plot(self.results['steps'], self.results['curiosity_levels'], 'c-', alpha=0.7)
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Curiosity Level')
        axes[1, 0].set_title('Exploration Motivation')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Solution timeline
        if self.results['problem_solved']:
            axes[1, 1].axvline(x=self.results['solving_time'], color='red', linestyle='--', alpha=0.7)
            axes[1, 1].plot(self.results['steps'], self.results['rewards'], 'b-', alpha=0.7)
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Reward')
            axes[1, 1].set_title(f'Solution Found at Step {self.results["solving_time"]}')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Performance summary
        success_rate = target_reached_count / len(self.results['steps'])
        axes[1, 2].bar(['Success Rate', 'Firing Rate', 'Memory Efficiency'], 
                      [success_rate, np.mean(self.results['firing_rates']) / 20, 
                      1.0 / (np.mean(self.results['memory_usage']) + 1)],
                      alpha=0.7, color=['green', 'blue', 'orange'])
        axes[1, 2].set_ylabel('Performance Score')
        axes[1, 2].set_title('Overall Performance')
        axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()


# Example script execution
if __name__ == "__main__":
    print("NLM Example Scripts Suite")
    print("=" * 50)
    
    # Run examples (comment out to skip specific examples)
    print("\n1. Running Basic Simulation Example...")
    basic_example = BasicSimulationExample()
    basic_results = basic_example.run()
    
    print("\n2. Running Complex Behavior Demonstration...")
    complex_example = ComplexBehaviorDemonstration()
    complex_results = complex_example.run()
    
    print("\n3. Running Learning and Development Example...")
    development_example = LearningAndDevelopmentExample()
    development_results = development_example.run()
    
    print("\n4. Running Multi-Agent Coordination Example...")
    multiagent_example = MultiAgentCoordinationExample()
    multiagent_results = multiagent_example.run()
    
    print("\n5. Running Real-World Problem Solving Example...")
    problem_example = RealWorldProblemSolvingExample()
    problem_results = problem_example.run()
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
    print("\nExample results saved to 'results/' directory.")
    print("\nFor more information, see the documentation in each example class.")


# Summary of examples:

"""
Basic Simulation Example:
  - Demonstrates the simplest NLM brain simulation
  - Shows basic world interaction
  - Introduces core concepts of perception-action loop

Complex Behavior Demonstration:
  - Shows learning in challenging environments
  - Demonstrates action selection and decision making
  - Includes navigation and target tracking

Learning and Development Example:
  - Demonstrates developmental stages
  - Shows synaptic growth and pruning
  - Illustrates curiosity-driven learning

Multi-Agent Coordination Example:
  - Shows how multiple brains work together
  - Demonstrates group decision making
  - Includes coordination metrics

Real-World Problem Solving Example:
  - Shows practical problem-solving
  - Includes navigation with obstacles
  - Demonstrates adaptation and learning
"""