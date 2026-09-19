#!/usr/bin/env python3
"""
NLM Multi-Agent Environment Simulation Example
===============================================

This example demonstrates a multi-agent simulation where multiple brains
interact with each other and a shared environment. It showcases:

1. Multiple agent creation and initialization
2. Environment shared between agents
3. Inter-agent competition and cooperation
4. World object interaction
5. Learning through environmental feedback
"""

import pynlm
import numpy as np
import time

class NLMNAgent:
    """NLM Neural Agent wrapper for interacting with the environment."""
    
    def __init__(self, brain_config=None, agent_id=0):
        """
        Initialize a new NLM agent.
        
        Args:
            brain_config: Configuration for the brain. If None, uses default.
            agent_id: Unique identifier for this agent.
        """
        self.agent_id = agent_id
        self.config = brain_config or pynlm.createDefaultConfig()
        
        # Configure agent-specific parameters
        self.config.set("brain.neuron_count", 300)
        self.config.set("brain.connection_probability", 0.08)
        self.config.set("brain.initial_weight_mean", 0.3)
        self.config.set("brain.initial_weight_std", 0.15)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Performance tracking
        self.total_rewards = 0.0
        self.steps_taken = 0
        self.active_objects_found = 0
        
    def initialize(self, world):
        """
        Initialize the agent with a world.
        
        Args:
            world: The SimpleWorld instance to interact with.
        """
        # Initialize brain
        self.brain.initialize()
        
        # Initialize agent with world
        self.agent.initialize(world)
        
        # Enable learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
    def step(self, world, timestep):
        """
        Execute one simulation step for this agent.
        
        Args:
            world: The world instance.
            timestep: Current simulation timestep.
            
        Returns:
            dict: Agent's actions and statistics.
        """
        step_result = {
            'agent_id': self.agent_id,
            'step': self.steps_taken,
            'position': {'x': world.getAgentBody().x, 'y': world.getAgentBody().y},
            'action': None,
            'reward': 0.0,
            'curiosity': 0.0,
            'novelty': 0.0,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'spikes': self.brain.getTotalSpikeCount()
        }
        
        try:
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(self.steps_taken)
            
            # Decode and execute action
            action = self.agent.decodeMotorCommand()
            step_result['action'] = action.getName()
            
            # Apply action to world
            world.applyMotorCommand(action, timestep)
            step_result['reward'] = 0.1  # Basic reward for survival
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update statistics
            self.total_rewards += step_result['reward']
            self.steps_taken += 1
            
            # Get neuromodulation levels
            step_result['curiosity'] = self.agent.getCuriosityLevel()
            step_result['novelty'] = self.agent.getNoveltyLevel()
            
        except Exception as e:
            print(f"Agent {self.agent_id} error: {e}")
            step_result['error'] = str(e)
        
        return step_result
    
    def get_status(self):
        """Get agent's current status."""
        return {
            'agent_id': self.agent_id,
            'steps_taken': self.steps_taken,
            'total_rewards': self.total_rewards,
            'avg_reward': self.total_rewards / max(1, self.steps_taken),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }

class ResourceWorld(pynlm.SimpleWorld):
    """Extended world with resources and hazards for agent interaction."""
    
    def __init__(self):
        super().__init__()
        self.resources = []
        self.hazards = []
        
    def add_resource(self, x, y, value=1.0, radius=0.5):
        """Add a resource to the world."""
        resource = pynlm.WorldObject(
            x, y, 
            pynlm.WorldObjectType.Resource, 
            value=value, 
            radius=radius
        )
        resource.active = True
        self.resources.append(resource)
        self.addObject(resource)
        
    def add_hazard(self, x, y, radius=0.5):
        """Add a hazard to the world."""
        hazard = pynlm.WorldObject(
            x, y,
            pynlm.WorldObjectType.Hazard,
            value=-1.0,
            radius=radius
        )
        hazard.active = True
        self.hazards.append(hazard)
        self.addObject(hazard)
        
    def get_nearby_resources(self, x, y, radius=3.0):
        """Get resources within radius of position."""
        nearby = []
        for resource in self.resources:
            if resource.active:
                distance = np.sqrt((resource.x - x)**2 + (resource.y - y)**2)
                if distance <= radius:
                    nearby.append(resource)
        return nearby
        
    def collect_resource(self, x, y, radius=0.5):
        """Collect resource near agent's position."""
        resources = self.get_nearby_resources(x, y, radius)
        collected = []
        
        for resource in resources[:]:  # Copy list to avoid modification issues
            distance = np.sqrt((resource.x - x)**2 + (resource.y - y)**2)
            if distance <= resource.radius:
                self.removeObject(resource.x, resource.y)
                resource.active = False
                self.resources.remove(resource)
                collected.append(resource.value)
                
        return collected
        
    def get_nearby_hazards(self, x, y, radius=3.0):
        """Get hazards within radius of position."""
        nearby = []
        for hazard in self.hazards:
            if hazard.active:
                distance = np.sqrt((hazard.x - x)**2 + (hazard.y - y)**2)
                if distance <= radius:
                    nearby.append(hazard)
        return nearby

def create_resource_world(width=20, height=20):
    """Create a world with resources and hazards."""
    world = ResourceWorld()
    world.configure(width, height, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Set up initial resources
    import random
    num_resources = 15
    num_hazards = 8
    
    for i in range(num_resources):
        x = random.uniform(2, width - 2)
        y = random.uniform(2, height - 2)
        value = random.uniform(0.5, 2.0)
        world.add_resource(x, y, value, radius=0.4)
    
    for i in range(num_hazards):
        x = random.uniform(2, width - 2)
        y = random.uniform(2, height - 2)
        world.add_hazard(x, y, radius=0.6)
    
    # Set starting positions for agents
    for i in range(3):
        x = (i + 1) * (width - 2) // 4 + 1
        y = height // 2
        world.setAgentStart(x, y)
    
    return world

def run_multi_agent_simulation():
    """Run a multi-agent simulation with resource competition."""
    print("=== NLM Multi-Agent Resource Competition Simulation ===\n")
    
    # Create shared world
    print("1. Creating resource-rich world...")
    world = create_resource_world(width=25, height=25)
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    print(f"   Resources: {len(world.resources)}")
    print(f"   Hazards: {len(world.hazards)}")
    
    # Create agents
    print("\n2. Creating agents...")
    agents = []
    base_config = pynlm.createDefaultConfig()
    
    for i in range(3):
        config = base_config
        # Different agent personalities
        if i == 0:
            # Explorer: more curiosity, faster movement
            config.set("brain.neuron_count", 400)
            config.set("brain.connection_probability", 0.06)
        elif i == 1:
            # Optimizer: more careful, better resource use
            config.set("brain.neuron_count", 350)
            config.set("brain.connection_probability", 0.07)
        else:
            # Explorer: balanced approach
            config.set("brain.neuron_count", 300)
            config.set("brain.connection_probability", 0.08)
        
        agent = NLMNAgent(config, agent_id=i)
        agents.append(agent)
    
    # Initialize all agents
    print("\n3. Initializing agents...")
    for i, agent in enumerate(agents):
        agent.initialize(world)
        print(f"   Agent {i}: {agent.get_status()['firing_neurons']} neurons, "
              f"curiosity: {agent.get_status()['curiosity_level']:.2f}")
    
    # Run simulation
    print("\n4. Running simulation...")
    num_steps = 200
    simulation_step = 0
    
    agent_stats = {i: [] for i in range(len(agents))}
    world_resources_collected = [0] * len(agents)
    
    start_time = time.time()
    
    for step in range(num_steps):
        timestep = step * 0.1
        
        # Each agent takes a turn
        for i, agent in enumerate(agents):
            # Get agent's current position
            body = world.getAgentBody()
            
            # Check for hazards
            hazards = world.get_nearby_hazards(body.x, body.y, radius=1.0)
            hazard_count = len(hazards)
            
            # Agent's step
            step_result = agent.step(world, timestep)
            
            # Collect resources if agent is close to some
            collected = world.collect_resource(body.x, body.y, radius=0.6)
            if collected:
                world_resources_collected[i] += len(collected)
                step_result['resources_collected'] = len(collected)
                step_result['total_resources'] = sum(collected)
            
            # Add hazard penalty
            if hazard_count > 0:
                step_result['hazard_penalty'] = -hazard_count * 0.5
            
            # Store statistics
            agent_stats[i].append(step_result)
        
        # Update world timestep
        world.update(timestep)
        
        # Print progress every 50 steps
        if (step + 1) % 50 == 0:
            print(f"   Step {step + 1}/{num_steps}")
            for i, agent in enumerate(agents):
                status = agent.get_status()
                print(f"     Agent {i}: pos({body.x:.1f},{body.y:.1f}), "
                      f"reward({status['total_rewards']:.1f}), "
                      f"curiosity({status['curiosity_level']:.2f})")
    
    end_time = time.time()
    simulation_time = end_time - start_time
    
    print(f"\n5. Simulation completed in {simulation_time:.2f} seconds")
    print(f"   Average speed: {num_steps/simulation_time:.1f} steps/second")
    
    # Analyze results
    print("\n6. Analyzing results...")
    
    for i, agent in enumerate(agents):
        stats = agent.get_status()
        avg_firing = stats['firing_neurons'] / max(1, stats['steps_taken'])
        
        print(f"\n   Agent {i} Performance:")
        print(f"     - Steps taken: {stats['steps_taken']}")
        print(f"     - Total rewards: {stats['total_rewards']:.2f}")
        print(f"     - Average reward per step: {stats['avg_reward']:.3f}")
        print(f"     - Resources collected: {world_resources_collected[i]}")
        print(f"     - Final curiosity level: {stats['curiosity_level']:.2f}")
        print(f"     - Final novelty level: {stats['novelty_level']:.2f}")
        print(f"     - Final developmental stage: {stats['developmental_stage']}")
        
        # Calculate efficiency
        if stats['total_rewards'] > 0:
            resource_efficiency = world_resources_collected[i] / stats['total_rewards']
            print(f"     - Resource efficiency: {resource_efficiency:.2f} resources/reward")
    
    # Winner determination
    print("\n7. Competition Results:")
    winner_stats = []
    for i, agent in enumerate(agents):
        stats = agent.get_status()
        winner_stats.append((i, stats['total_rewards'], world_resources_collected[i]))
    
    # Sort by total reward
    winner_stats.sort(key=lambda x: x[1], reverse=True)
    
    for rank, (agent_id, total_reward, resources) in enumerate(winner_stats):
        medal = ["🥇", "🥈", "🥉"][rank] if rank < 3 else "  "
        print(f"   {medal} Agent {agent_id}: {total_reward:.2f} rewards, {resources} resources")
    
    # Summary
    print(f"\n=== SIMULATION SUMMARY ===")
    print(f"Total simulation time: {simulation_time:.2f} seconds")
    print(f"Total environment steps: {num_steps}")
    print(f"Total resources in world: {len([r for agent in agents for r in [world_resources_collected]])} initially")
    print(f"Total resources collected: {sum(world_resources_collected)}")
    
    return {
        'agents': agents,
        'agent_stats': agent_stats,
        'simulation_time': simulation_time,
        'total_steps': num_steps,
        'resources_collected': world_resources_collected,
        'winner': winner_stats[0][0] if winner_stats else -1
    }

def run_learning_competition():
    """Run a learning competition where agents learn from experience."""
    print("\n" + "="*60)
    print("LEARNING COMPETITION: Agent Learning Over Time")
    print("="*60 + "\n")
    
    # Create world
    world = create_resource_world(width=20, height=20)
    
    # Create agents with different learning strategies
    print("Creating agents with different learning strategies...")
    
    agents = []
    strategies = {
        'explorer': {'neurons': 400, 'connections': 0.06, 'curiosity': 1.0},
        'optimizer': {'neurons': 350, 'connections': 0.07, 'curiosity': 0.5},
        'balanced': {'neurons': 300, 'connections': 0.08, 'curiosity': 0.8}
    }
    
    for i, (strategy_name, params) in enumerate(strategies.items()):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params['neurons'])
        config.set("brain.connection_probability", params['connections'])
        
        agent = NLMNAgent(config, agent_id=i)
        agent.strategy = strategy_name
        agents.append(agent)
    
    # Initialize agents
    for agent in agents:
        agent.initialize(world)
        print(f"  Agent {agent.strategy}: {agent.get_status()['firing_neurons']} neurons")
    
    # Extended learning competition
    print(f"\nRunning extended learning competition ({300} steps)...")
    
    for step in range(300):
        timestep = step * 0.1
        
        for agent in agents:
            # Agent's turn
            step_result = agent.step(world, timestep)
            
            # Collect resources
            body = world.getAgentBody()
            collected = world.collect_resource(body.x, body.y, radius=0.6)
            
            # Update development (learning)
            agent.agent.updateDevelopment(timestep)
            
            # Store results
            agent.step_history.append({
                'step': step,
                'timestep': timestep,
                'reward': step_result['reward'],
                'curiosity': step_result['curiosity'],
                'resources_collected': len(collected),
                'firing_neurons': step_result['firing_neurons']
            })
        
        world.update(timestep)
        
        if step % 100 == 0:
            print(f"  Step {step}: Competition ongoing...")
    
    # Analyze learning curves
    print(f"\nLearning Analysis:")
    for agent in agents:
        if hasattr(agent, 'step_history'):
            rewards = [s['reward'] for s in agent.step_history]
            curiosities = [s['curiosity'] for s in agent.step_history]
            
            initial_avg_reward = np.mean(rewards[:50])
            final_avg_reward = np.mean(rewards[-50:])
            reward_improvement = final_avg_reward - initial_avg_reward
            
            initial_avg_curiosity = np.mean(curiosities[:50])
            final_avg_curiosity = np.mean(curiosities[-50:])
            curiosity_change = final_avg_curiosity - initial_avg_curiosity
            
            print(f"\n  {agent.strategy.capitalize()} Agent:")
            print(f"    - Reward improvement: {reward_improvement:+.3f}")
            print(f"    - Curiosity change: {curiosity_change:+.3f}")
            print(f"    - Final performance: {agent.get_status()['total_rewards']:.2f} total rewards")
    
    return agents

if __name__ == "__main__":
    print("NLM Multi-Agent Examples")
    print("======================\n")
    
    try:
        # Example 1: Multi-agent resource competition
        results = run_multi_agent_simulation()
        
        # Example 2: Learning competition
        learning_agents = run_learning_competition()
        
        print(f"\n" + "="*60)
        print("EXAMPLES COMPLETED SUCCESSFULLY")
        print("="*60)
        print("\nThe examples demonstrate:")
        print("  1. Multi-agent interaction in shared environments")
        print("  2. Resource competition and cooperation")
        print("  3. Different agent learning strategies")
        print("  4. Environmental adaptation and survival")
        print("  5. Competition for limited resources")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
