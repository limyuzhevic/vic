# AgentWorld.py - Complete Agent-Based World Simulation

This example demonstrates how to create and run a complete agent-based simulation with the NLM Python bindings. It shows real-world usage patterns for neural simulation with environment interaction.

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import json

class AgentWorldSimulation:
    """Complete agent world simulation with neural learning and environmental interaction."""
    
    def __init__(self, width=20, height=20, vision_width=8, vision_height=8):
        # Initialize simulation components
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
        
        # Create brain with learning-enabled configuration
        config = pynlm.createDefaultConfig()
        config.set_int("brain.neuron_count", 1500)
        config.set_float("brain.synapse_density", 0.12)
        config.set_float("plasticity.stdp.enable", True)
        config.set_float("plasticity.hebbian.enable", True)
        config.set_float("neuromod.dopamine.scale", 1.5)
        config.set_float("neuromod.curiosity.enable", True)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure world
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        
        # Place agent in world
        self.world.setAgentStart(width // 2, height // 2)
        self.agent.initialize(self.world)
        
        # Enable advanced learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Initialize tracking
        self.step_count = 0
        self.time_elapsed = 0.0
        self.agent_position_history = []
        self.brain_activity_history = []
        self.reward_history = []
        self.heat_map = np.zeros((height, width))
        
        # Set up goal and obstacles
        self.setupEnvironment()
        
    def setupEnvironment(self):
        """Set up the simulation environment with goals and obstacles."""
        # Add goal at the far corner
        goal_x, goal_y = self.width - 2, self.height - 2
        goal = pynlm.WorldObject(goal_x, goal_y, pynlm.WorldObjectType.Marker, 
                                value=1.0, radius=0.8)
        self.world.addObject(goal)
        
        # Add some obstacles (hazards)
        obstacles = [
            (3, 3), (3, 7), (7, 3), (7, 7),
            (12, 5), (15, 10), (8, 12)
        ]
        
        for x, y in obstacles:
            hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 
                                      value=0.5, radius=0.6)
            self.world.addObject(hazard)
        
        # Add resource nodes
        resources = [
            (5, 5), (10, 15), (15, 5)
        ]
        
        for x, y in resources:
            resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 
                                        value=2.0, radius=0.7)
            self.world.addObject(resource)
    
    def updateHeatMap(self):
        """Update heat map based on agent activity."""
        body = self.world.getAgentBody()
        x, y = int(body.x), int(body.y)
        
        if 0 <= x < self.width and 0 <= y < self.height:
            # Add activity to heat map
            self.heat_map[y, x] += 1.0
            
            # Apply decay
            self.heat_map *= 0.95
    
    def calculateReward(self):
        """Calculate reward based on agent actions and environment."""
        reward = 0.0
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Check proximity to objects
        objects = []
        for y in range(self.height):
            for x in range(self.width):
                if self.world.isValidPosition(x, y):
                    # This is a simplified check - actual implementation
                    # would need to iterate through world objects
                    pass
        
        # Reward for proximity to resources
        resource_proximity = percept.getInternal()[0] if percept.getInternal() else 0.0
        reward += resource_proximity * 0.5
        
        # Reward for movement (encourages exploration)
        if body.isMoving:
            reward += 0.1
        
        # Reward for turning (encourages navigation)
        if body.isTurning:
            reward += 0.1
        
        # Penalty for hazards
        hazard_proximity = percept.getTouch()[0] if percept.getTouch() else 0.0
        if hazard_proximity > 0.7:
            reward -= 0.5
        
        return reward
    
    def runEpisode(self, max_steps=500):
        """Run a single simulation episode."""
        print(f"Starting episode with max steps: {max_steps}")
        
        for step in range(max_steps):
            # Record state
            body = self.world.getAgentBody()
            self.agent_position_history.append((body.x, body.y))
            self.brain_activity_history.append({
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError()
            })
            
            # Get current simulation time
            current_time = self.world.getSimulationTime()
            
            # Update world
            self.world.update(0.1)
            self.time_elapsed += 0.1
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step, current_time)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            self.world.applyMotorCommand(motor_cmd, current_time)
            
            # Calculate and apply reward
            reward = self.calculateReward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Update heat map
            self.updateHeatMap()
            
            # Record reward
            self.reward_history.append(reward)
            
            # Check for episode termination
            if step % 100 == 0:
                # Print progress
                curiosity = self.agent.getCuriosityLevel()
                novelty = self.agent.getNoveltyLevel()
                firing_rate = self.brain.getFiringNeuronCount()
                
                print(f"  Step {step}: Pos=({body.x:.1f},{body.y:.1f}), "
                      f"Firing={firing_rate}, Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                      f"Reward={reward:.3f}")
                
                # Check if agent reached goal
                goal_proximity = percept.getInternal()[1] if len(percept.getInternal()) > 1 else 0.0
                if goal_proximity > 0.8:
                    print("Goal reached! Episode completed successfully.")
                    return step, True
                
                # Check for excessive inactivity
                if step > 100 and len(self.reward_history) > 50:
                    avg_reward = sum(self.reward_history[-50:]) / 50
                    if avg_reward < -0.1:
                        print("Agent is stuck in negative reward area. Ending episode.")
                        return step, False
        
        # Episode ended without reaching goal
        print("Episode completed without reaching goal.")
        return max_steps, False
    
    def visualizeResults(self, episode_step, episode_success):
        """Visualize simulation results."""
        try:
            # Create figure with subplots
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle(f"AgentWorld Simulation Results - Episode {self.step_count + 1}", fontsize=16)
            
            # Plot 1: Agent trajectory
            ax1 = axes[0, 0]
            if self.agent_position_history:
                x_coords, y_coords = zip(*self.agent_position_history)
                ax1.plot(x_coords, y_coords, 'b-', alpha=0.7, linewidth=2)
                ax1.scatter(x_coords[0], y_coords[0], color='green', s=100, label='Start')
                ax1.scatter(x_coords[-1], y_coords[-1], color='red', s=100, label='End')
                ax1.set_xlim(0, self.width)
                ax1.set_ylim(0, self.height)
                ax1.set_xlabel('X Position')
                ax1.set_ylabel('Y Position')
                ax1.set_title('Agent Trajectory')
                ax1.legend()
                ax1.grid(True, alpha=0.3)
            
            # Plot 2: Brain activity over time
            ax2 = axes[0, 1]
            if self.brain_activity_history:
                steps = [i for i in range(len(self.brain_activity_history))]
                firing_rates = [data['firing_neurons'] for data in self.brain_activity_history]
                ax2.plot(steps, firing_rates, 'r-', alpha=0.7)
                ax2.set_xlabel('Step')
                ax2.set_ylabel('Firing Neurons')
                ax2.set_title('Brain Activity Over Time')
                ax2.grid(True, alpha=0.3)
            
            # Plot 3: Reward over time
            ax3 = axes[1, 0]
            if self.reward_history:
                steps = [i for i in range(len(self.reward_history))]
                ax3.plot(steps, self.reward_history, 'g-', alpha=0.7)
                ax3.set_xlabel('Step')
                ax3.set_ylabel('Reward')
                ax3.set_title('Reward Over Time')
                ax3.grid(True, alpha=0.3)
                ax3.set_ylim(-1, 2)
            
            # Plot 4: Heat map
            ax4 = axes[1, 1]
            im = ax4.imshow(self.heat_map.T, origin='lower', extent=[0, self.width, 0, self.height],
                           cmap='hot', alpha=0.8)
            ax4.scatter(self.width // 2, self.height // 2, color='blue', s=50, label='Start')
            ax4.set_xlabel('X Position')
            ax4.set_ylabel('Y Position')
            ax4.set_title('Activity Heat Map')
            ax4.legend()
            plt.colorbar(im, ax=ax4)
            
            plt.tight_layout()
            
            # Save figure
            filename = f"agent_world_episode_{self.step_count + 1}_step_{episode_step}.png"
            plt.savefig(filename, dpi=150, bbox_inches='tight')
            print(f"Visualization saved to {filename}")
            
            # Close figure to free memory
            plt.close()
            
        except Exception as e:
            print(f"Visualization failed: {e}")
    
    def getEpisodeStats(self):
        """Get comprehensive episode statistics."""
        if not self.brain_activity_history:
            return {}
        
        total_steps = len(self.brain_activity_history)
        final_firing = self.brain.getFiringNeuronCount()
        final_total_spikes = self.brain.getTotalSpikeCount()
        final_curiosity = self.agent.getCuriosityLevel()
        final_novelty = self.agent.getNoveltyLevel()
        
        # Calculate statistics
        initial_firing = self.brain_activity_history[0]['firing_neurons']
        avg_firing = sum(data['firing_neurons'] for data in self.brain_activity_history) / total_steps
        avg_curiosity = sum(data['curiosity'] for data in self.brain_activity_history) / total_steps
        avg_novelty = sum(data['novelty'] for data in self.brain_activity_history) / total_steps
        avg_prediction_error = sum(abs(data['prediction_error']) for data in self.brain_activity_history) / total_steps
        
        # Calculate exploration metrics
        unique_positions = len(set(self.agent_position_history))
        exploration_ratio = unique_positions / min(total_steps, 1000)
        
        # Calculate reward statistics
        avg_reward = sum(self.reward_history) / len(self.reward_history) if self.reward_history else 0.0
        max_reward = max(self.reward_history) if self.reward_history else 0.0
        min_reward = min(self.reward_history) if self.reward_history else 0.0
        
        return {
            'total_steps': total_steps,
            'final_firing_rate': final_firing,
            'total_spikes': final_total_spikes,
            'avg_firing_rate': avg_firing,
            'initial_firing_rate': initial_firing,
            'firing_rate_change': final_firing - initial_firing,
            'final_curiosity': final_curiosity,
            'final_novelty': final_novelty,
            'avg_curiosity': avg_curiosity,
            'avg_novelty': avg_novelty,
            'avg_prediction_error': avg_prediction_error,
            'unique_positions': unique_positions,
            'exploration_ratio': exploration_ratio,
            'avg_reward': avg_reward,
            'max_reward': max_reward,
            'min_reward': min_reward,
            'developmental_stage': str(self.brain.getDevelopmentalStage()),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount()
        }
    
    def runMultipleEpisodes(self, num_episodes=5, max_steps_per_episode=500):
        """Run multiple episodes and return aggregated results."""
        print(f"Running {num_episodes} episodes...")
        
        all_stats = []
        total_steps_completed = 0
        successful_episodes = 0
        
        for episode in range(num_episodes):
            print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
            
            # Run episode
            steps_completed, success = self.runEpisode(max_steps_per_episode)
            
            # Get statistics
            stats = self.getEpisodeStats()
            stats['episode'] = episode + 1
            stats['steps_completed'] = steps_completed
            stats['episode_success'] = success
            
            all_stats.append(stats)
            total_steps_completed += steps_completed
            
            if success:
                successful_episodes += 1
            
            # Visualize results
            self.visualizeResults(steps_completed, success)
            
            # Brief pause between episodes
            if episode < num_episodes - 1:
                time.sleep(1)
        
        # Calculate overall statistics
        overall_stats = {
            'total_episodes': num_episodes,
            'successful_episodes': successful_episodes,
            'success_rate': successful_episodes / num_episodes,
            'total_steps_completed': total_steps_completed,
            'avg_steps_per_episode': total_steps_completed / num_episodes,
            'episode_stats': all_stats
        }
        
        print(f"\n=== Overall Results ===")
        print(f"Success Rate: {overall_stats['success_rate']:.2%}")
        print(f"Average Steps per Episode: {overall_stats['avg_steps_per_episode']:.1f}")
        
        return overall_stats

# Example usage
if __name__ == "__main__":
    # Create and run agent world simulation
    print("=== AgentWorld Simulation ===")
    
    # Create simulation
    simulation = AgentWorldSimulation(width=20, height=20)
    
    # Run single episode
    steps, success = simulation.runEpisode(max_steps=300)
    
    # Get and print statistics
    stats = simulation.getEpisodeStats()
    
    print(f"\n=== Episode Results ===")
    print(f"Steps completed: {steps}")
    print(f"Episode success: {success}")
    print(f"Final position: ({simulation.agent_position_history[-1][0]:.1f}, {simulation.agent_position_history[-1][1]:.1f})")
    print(f"Average curiosity: {stats['avg_curiosity']:.3f}")
    print(f"Average novelty: {stats['avg_novelty']:.3f}")
    print(f"Exploration ratio: {stats['exploration_ratio']:.3f}")
    print(f"Average reward: {stats['avg_reward']:.3f}")
    
    # Run multiple episodes
    print(f"\n=== Running Multiple Episodes ===")
    multi_stats = simulation.runMultipleEpisodes(num_episodes=3, max_steps_per_episode=400)
    
    # Save results
    with open("agent_world_results.json", 'w') as f:
        json.dump(multi_stats, f, indent=2, default=str)
    
    print(f"\nResults saved to agent_world_results.json")
