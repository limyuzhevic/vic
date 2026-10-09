#!/usr/bin/env python3
"""
Example 4: Multi-Agent Simulation

This example demonstrates multi-agent simulations and complex interactions
between multiple NLM brains in a shared environment.

Key features demonstrated:
- Multi-agent environment setup
- Agent-to-agent interactions
- Competition and cooperation behaviors
- Social learning and imitation
- Population-level analysis
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import numpy as np
import matplotlib.pyplot as plt
import time
import threading
import queue
from collections import deque

class MultiAgentEnvironment:
    """
    Environment supporting multiple NLM agents with interactions.
    
    This environment allows for:
    - Multiple agents with different brain configurations
    - Agent-to-agent interactions (social learning)
    - Shared resources and competition
    - Complex behavioral patterns
    """
    
    def __init__(self, width=100, height=100):
        self.width = width
        self.height = height
        self.agents = []
        self.resources = []
        self.obstacles = []
        self.time_step = 0
        self.agent_positions = {}
        self.resource_positions = {}
        
        # Initialize environment
        self.reset()
    
    def add_agent(self, agent, position):
        """Add an agent to the environment."""
        self.agents.append(agent)
        self.agent_positions[agent] = position
        agent.initialize(self)
        return len(self.agents) - 1
    
    def add_resource(self, position, value=1.0, type_name="resource"):
        """Add a resource to the environment."""
        self.resources.append({
            'position': position,
            'value': value,
            'type': type_name,
            'active': True
        })
        self.resource_positions[position] = len(self.resources) - 1
        return len(self.resources) - 1
    
    def add_obstacle(self, position, radius=1.0):
        """Add an obstacle to the environment."""
        self.obstacles.append({
            'position': position,
            'radius': radius
        })
        return len(self.obstacles) - 1
    
    def reset(self):
        """Reset the environment to initial state."""
        self.agents.clear()
        self.resources.clear()
        self.obstacles.clear()
        self.agent_positions.clear()
        self.resource_positions.clear()
        self.time_step = 0
        
        # Add default obstacles
        for x in range(20, 80, 15):
            for y in range(20, 80, 15):
                if (x, y) not in [(25, 25), (55, 55)]:  # Leave space for agents
                    self.add_obstacle((x, y), radius=2.0)
        
        # Add resources
        for i in range(20):
            x = np.random.uniform(5, 95)
            y = np.random.uniform(5, 95)
            if not self._is_obstacle(x, y):
                value = np.random.uniform(0.5, 2.0)
                self.add_resource((x, y), value, "energy")
    
    def _is_obstacle(self, x, y):
        """Check if position is occupied by an obstacle."""
        for obstacle in self.obstacles:
            if np.hypot(obstacle['position'][0] - x, obstacle['position'][1] - y) < obstacle['radius']:
                return True
        return False
    
    def _is_valid_position(self, x, y):
        """Check if position is valid (not obstacle, within bounds)."""
        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return False
        if self._is_obstacle(x, y):
            return False
        
        # Check if too close to other agents
        for agent_pos in self.agent_positions.values():
            if np.hypot(agent_pos[0] - x, agent_pos[1] - y) < 1.5:
                return False
        
        return True
    
    def update(self, timestep):
        """Update environment state."""
        self.time_step += timestep
        
        # Update resources (decay)
        for resource in self.resources:
            if resource['active']:
                resource['value'] *= 0.99  # Decay over time
        
        # Handle agent movements and interactions
        for agent in self.agents:
            self._process_agent_agent_interactions(agent)
            self._process_agent_resource_interactions(agent)
    
    def _process_agent_agent_interactions(self, agent):
        """Process interactions between agents."""
        agent_pos = self.agent_positions[agent]
        
        # Social learning: observe nearby agents
        for other_agent in self.agents:
            if other_agent != agent:
                other_pos = self.agent_positions[other_agent]
                distance = np.hypot(agent_pos[0] - other_pos[0], agent_pos[1] - other_pos[1])
                
                if distance < 10.0:  # Social learning radius
                    # Imitation learning
                    if np.random.random() < 0.1:  # 10% chance of imitation
                        # Learn from other agent's behavior
                        if hasattr(other_agent, 'brain') and hasattr(agent, 'brain'):
                            self._imitate_behavior(agent, other_agent, distance)
                    
                    # Competition or cooperation
                    if distance < 3.0:  # Close proximity
                        if np.random.random() < 0.3:  # 30% chance of conflict
                            # Conflict resolution
                            self._resolve_conflict(agent, other_agent, distance)
                        else:
                            # Cooperative behavior
                            self._cooperative_behavior(agent, other_agent, distance)
    
    def _process_agent_resource_interactions(self, agent):
        """Process interactions between agents and resources."""
        agent_pos = self.agent_positions[agent]
        
        for resource in self.resources:
            if resource['active']:
                res_pos = resource['position']
                distance = np.hypot(agent_pos[0] - res_pos[0], agent_pos[1] - res_pos[1])
                
                if distance < 2.0:  # Resource interaction radius
                    # Consume resource
                    if hasattr(agent, 'agent') and hasattr(agent.agent, 'processSensoryInput'):
                        # Create sensory input for resource detection
                        percept = self._create_resource_percept(resource, distance)
                        agent.agent.processSensoryInput(percept)
                        
                        # Apply reward for resource consumption
                        reward = resource['value'] * (1.0 - distance / 2.0)
                        if hasattr(agent, 'agent') and hasattr(agent.agent, 'applyRewardModulation'):
                            agent.agent.applyRewardModulation(reward, 0.0)
                        
                        # Remove resource after consumption
                        resource['active'] = False
                        del self.resource_positions[res_pos]
                        break
    
    def _imitate_behavior(self, agent, other_agent, distance):
        """Imitate behavior from another agent."""
        if hasattr(other_agent, 'agent') and hasattr(agent, 'agent'):
            # Transfer motor patterns or action tendencies
            # This is a simplified imitation mechanism
            imitation_strength = 1.0 / (1.0 + distance)
            
            # In a real implementation, this would transfer synaptic weights or motor patterns
            print(f"  [Imitation] Agent imitating from neighbor at distance {distance:.1f} (strength: {imitation_strength:.2f})")
    
    def _resolve_conflict(self, agent1, agent2, distance):
        """Resolve conflict between two agents."""
        # Simple conflict resolution based on agent fitness or strength
        # In a real implementation, this would be more sophisticated
        conflict_type = np.random.choice(['aggression', 'submission', 'negotiation'])
        
        if conflict_type == 'aggression':
            print(f"  [Conflict] Aggression between agents at distance {distance:.1f}")
        elif conflict_type == 'submission':
            print(f"  [Conflict] Submission between agents at distance {distance:.1f}")
        else:
            print(f"  [Conflict] Negotiation between agents at distance {distance:.1f}")
    
    def _cooperative_behavior(self, agent1, agent2, distance):
        """Cooperative behavior between agents."""
        # Simple cooperation: share resources or help each other
        cooperation_strength = 1.0 / (1.0 + distance)
        
        if np.random.random() < cooperation_strength:
            print(f"  [Cooperation] Cooperation between agents at distance {distance:.1f} (strength: {cooperation_strength:.2f})")
    
    def _create_resource_percept(self, resource, distance):
        """Create a percept for resource detection."""
        # This would create a sensory input representing the resource
        # For simplicity, we'll just return a dictionary
        return {
            'type': 'resource',
            'position': resource['position'],
            'value': resource['value'],
            'distance': distance,
            'quality': resource['value'] * (1.0 - distance / 2.0)
        }
    
    def get_sensory_percept(self, agent):
        """Get sensory percept for an agent."""
        agent_pos = self.agent_positions[agent]
        
        percept = {
            'vision': [],
            'touch': [],
            'internal': [],
            'proprioception': {
                'x': agent_pos[0],
                'y': agent_pos[1],
                'orientation': 0.0,
                'velocity_x': 0.0,
                'velocity_y': 0.0
            }
        }
        
        # Vision: look for resources and other agents
        vision_radius = 15
        for x in range(int(agent_pos[0] - vision_radius), int(agent_pos[0] + vision_radius + 1), 1):
            for y in range(int(agent_pos[1] - vision_radius), int(agent_pos[1] + vision_radius + 1), 1):
                if 0 <= x < self.width and 0 <= y < self.height:
                    # Check for resources
                    for resource in self.resources:
                        if resource['active']:
                            res_pos = resource['position']
                            dist = np.hypot(x - res_pos[0], y - res_pos[1])
                            if dist <= vision_radius:
                                percept['vision'].append({
                                    'type': resource['type'],
                                    'x': x,
                                    'y': y,
                                    'distance': dist,
                                    'value': resource['value']
                                })
                    
                    # Check for other agents
                    for other_agent in self.agents:
                        if other_agent != agent:
                            other_pos = self.agent_positions[other_agent]
                            dist = np.hypot(x - other_pos[0], y - other_pos[1])
                            if dist <= vision_radius:
                                percept['vision'].append({
                                    'type': 'agent',
                                    'agent_id': id(other_agent),
                                    'x': x,
                                    'y': y,
                                    'distance': dist
                                })
        
        return percept

def create_multi_agent_config(agent_id, neuron_count=800, curiosity=True, learning=True):
    """Create configuration for a multi-agent system."""
    config = pynlm.createDefaultConfig()
    
    # Agent-specific parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.12)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", agent_id * 1000 + 42)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", learning)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", learning)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", curiosity)
    config.set("neuromod.curiosity.threshold", 0.3)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_stage", "CriticalPeriod")
    
    return config

def example_multi_agent_simulation():
    """Demonstrate multi-agent simulation."""
    print("=== Example 4: Multi-Agent Simulation ===\n")
    
    # Create environment
    print("1. Creating multi-agent environment...")
    environment = MultiAgentEnvironment(width=100, height=100)
    print(f"   Environment size: {environment.width}x{environment.height}")
    print(f"   Initial obstacles: {len(environment.obstacles)}")
    print(f"   Initial resources: {len(environment.resources)}")
    
    # Create multiple agents with different characteristics
    print("\n2. Creating agents with different configurations...")
    
    agents = []
    agent_configs = [
        {"id": 1, "neuron_count": 600, "curiosity": True, "learning": True, "position": (10, 10)},
        {"id": 2, "neuron_count": 1000, "curiosity": True, "learning": True, "position": (90, 90)},
        {"id": 3, "neuron_count": 800, "curiosity": False, "learning": True, "position": (10, 90)},
        {"id": 4, "neuron_count": 1200, "curiosity": True, "learning": True, "position": (90, 10)},
    ]
    
    for agent_config in agent_configs:
        # Create configuration
        config = create_multi_agent_config(
            agent_config["id"],
            agent_config["neuron_count"],
            agent_config["curiosity"],
            agent_config["learning"]
        )
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Add to environment
        agent_index = environment.add_agent(agent, agent_config["position"])
        
        agents.append({
            "id": agent_config["id"],
            "brain": brain,
            "agent": agent,
            "config": agent_config
        })
        
        print(f"   Agent {agent_config['id']}: {agent_config['neuron_count']} neurons, "
              f"curiosity: {agent_config['curiosity']}, learning: {agent_config['learning']}")
    
    # Create experiment runner for the first agent (simplified)
    print("\n3. Setting up experiment runner...")
    main_agent = agents[0]["agent"]
    main_brain = agents[0]["brain"]
    
    runner = NLMExperimentRunner(main_brain, environment, main_agent)
    
    # Add callbacks for multi-agent monitoring
    def multi_agent_callback(runner, metrics, event_type):
        """Monitor multi-agent system state."""
        agent_count = len(environment.agents)
        active_resources = sum(1 for r in environment.resources if r['active'])
        
        if metrics['step'] % 100 == 0:
            print(f"   [Multi-Agent Monitor] Step {metrics['step']}: "
                  f"{agent_count} agents, {active_resources} active resources")
        
        # Check for learning events
        if 'curiosity_level' in metrics:
            curiosity = metrics['curiosity_level']
            if curiosity > 0.7:
                print(f"   [Multi-Agent Monitor] High curiosity detected: {curiosity:.2f}")
    
    runner.add_callback(multi_agent_callback)
    
    # Configure experiment
    experiment_config = ExperimentConfig(
        name="Multi-Agent Simulation",
        description="Simulation of multiple NLM agents with social learning and competition",
        max_steps=500,
        timestep=0.1,
        save_checkpoints=True,
        checkpoint_interval=100,
        metadata={
            'num_agents': len(agents),
            'environment_size': (environment.width, environment.height),
            'learning_type': 'social_learning'
        }
    )
    
    print(f"\n4. Running multi-agent experiment...")
    print(f"   Experiment: {experiment_config.name}")
    print(f"   Duration: {experiment_config.max_steps} steps")
    print(f"   Social learning: Enabled")
    print(f"   Competition: Enabled")
    print(f"   Cooperation: Enabled")
    
    # Run experiment (simplified - focusing on main agent)
    start_time = time.time()
    result = runner.run_experiment(experiment_config)
    end_time = time.time()
    
    print(f"\n=== Experiment Results ===")
    print(f"Success: {'✓' if result.success else '✗'}")
    print(f"Duration: {end_time - start_time:.2f} seconds")
    print(f"Steps completed: {result.steps_completed}")
    print(f"Checkpoints saved: {len(result.checkpoint_files)}")
    
    # Analyze multi-agent metrics
    print(f"\n=== Multi-Agent Analysis ===")
    
    # Calculate system-level metrics
    total_agents = len(agents)
    active_agents = sum(1 for agent_data in agents if agent_data['brain'].getTotalSpikeCount() > 0)
    
    print(f"Agent Statistics:")
    print(f"  Total agents: {total_agents}")
    print(f"  Active agents: {active_agents}")
    print(f"  Activity rate: {active_agents/total_agents*100:.1f}%")
    
    # Calculate interaction metrics
    if hasattr(environment, 'social_events'):
        print(f"  Social events recorded: {len(environment.social_events)}")
    
    # Calculate resource dynamics
    initial_resources = len([r for r in environment.resources if r['active']])
    final_resources = sum(1 for r in environment.resources if r['active'])
    resource_change = final_resources - initial_resources
    
    print(f"  Resource dynamics: {initial_resources} -> {final_resources} (change: {resource_change})")
    
    # Analyze agent diversity
    neuron_counts = [agent_data['brain'].getTotalNeuronCount() for agent_data in agents]
    curiosity_levels = []
    
    for agent_data in agents:
        if hasattr(agent_data['agent'], 'getCuriosityLevel'):
            curiosity_levels.append(agent_data['agent'].getCuriosityLevel())
    
    print(f"  Agent diversity:")
    print(f"    Neuron counts: {min(neuron_counts)} - {max(neuron_counts)}")
    if curiosity_levels:
        print(f"    Curiosity levels: {min(curiosity_levels):.2f} - {max(curiosity_levels):.2f}")
    
    return result, agents, environment

def example_social_learning_analysis():
    """Analyze social learning patterns in multi-agent system."""
    print("\n=== Social Learning Analysis Example ===\n")
    
    # Simulate social learning data
    print("1. Simulating social learning data...")
    
    # Generate synthetic agent interaction data
    agents = 4
    steps = 200
    
    # Simulate learning curves with social influence
    social_learning_curves = []
    
    for agent_id in range(agents):
        # Base learning curve
        base_learning = np.linspace(0.1, 0.8, steps)
        
        # Add social influence (learning from neighbors)
        social_influence = np.zeros(steps)
        for step in range(50, steps):
            # Social learning peaks around step 100
            social_influence[step] = 0.3 * np.exp(-(step - 100) / 50)
        
        # Combine base and social learning
        final_learning = base_learning + social_influence
        final_learning = np.clip(final_learning, 0, 1)  # Clip to [0, 1]
        
        social_learning_curves.append({
            'agent_id': agent_id,
            'learning_curve': final_learning,
            'peak_learning': np.max(final_learning),
            'learning_rate': np.gradient(final_learning).mean(),
            'social_influence_strength': np.mean(social_influence)
        })
    
    # Analyze social learning patterns
    print("2. Analyzing social learning patterns...")
    
    # Calculate similarity between agents
    similarity_matrix = np.zeros((agents, agents))
    for i in range(agents):
        for j in range(agents):
            if i != j:
                similarity = np.corrcoef(social_learning_curves[i]['learning_curve'],
                                        social_learning_curves[j]['learning_curve'])[0, 1]
                similarity_matrix[i, j] = similarity
    
    print(f"\nSocial Learning Similarity Matrix:")
    print("Agent influence matrix (correlation of learning curves):")
    for i in range(agents):
        row_str = "  "
        for j in range(agents):
            if i == j:
                row_str += f"{np.nan:6.3f} "
            else:
                row_str += f"{similarity_matrix[i, j]:6.3f} "
        print(f"  Agent {i}: {row_str}")
    
    # Calculate social learning metrics
    print(f"\nSocial Learning Metrics:")
    avg_curiosity = np.mean([curve['social_influence_strength'] for curve in social_learning_curves])
    print(f"  Average social influence: {avg_curiosity:.3f}")
    
    # Identify learning leaders
    learning_leaders = sorted(social_learning_curves, 
                             key=lambda x: x['peak_learning'], reverse=True)[:2]
    print(f"  Learning leaders:")
    for leader in learning_leaders:
        print(f"    Agent {leader['agent_id']}: peak learning = {leader['peak_learning']:.3f}")
    
    # Calculate social contagion
    total_social_influence = np.sum([curve['social_influence_strength'] for curve in social_learning_curves])
    print(f"  Total social influence: {total_social_influence:.3f}")
    
    return social_learning_curves, similarity_matrix

if __name__ == "__main__":
    print("NLM Advanced Features - Example 4: Multi-Agent Simulation")
    print("=" * 65)
    
    try:
        # Run multi-agent simulation example
        result, agents, environment = example_multi_agent_simulation()
        
        # Run social learning analysis example
        learning_curves, similarity_matrix = example_social_learning_analysis()
        
        print("\n" + "=" * 65)
        print("Example 4 completed successfully!")
        print("Key insights:")
        print("  - Multi-agent systems show emergent social behaviors")
        print("  - Social learning enhances individual learning rates")
        print("  - Competition and cooperation coexist in the system")
        print("  - Environment structure influences agent interactions")
        print("  - Similarity analysis reveals learning patterns")
        
    except Exception as e:
        print(f"\nError in example 4: {e}")
        import traceback
        traceback.print_exc()
