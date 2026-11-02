# Advanced NLM Python Examples

This file contains advanced examples for power users of the NLM (Neural Learning Machine) Python API.
These examples demonstrate complex usage patterns, advanced features, and real-world scenarios.

## Example 1: Advanced Multi-Agent System with Memory Integration

```python
"""
Advanced multi-agent system example demonstrating:
- Multiple brains with different specializations
- Complex world interactions
- Memory system integration
- Learning and adaptation over time
- Pattern recognition and concept formation
"""
import pynlm
import numpy as np
import time

def create_specialized_brains():
    """Create brains with different specializations"""
    brains = {}
    
    # Explorer brain - curiosity-driven exploration
    explorer_config = pynlm.createDefaultConfig()
    explorer_config.set("brain.neuron_count", 2000)
    explorer_config.set("brain.synapse_density", 0.05)
    explorer_config.set("plasticity.curiosity.enable", True)
    explorer_config.set("plasticity.novelty.enable", True)
    explorer_config.set("neuromod.dopamine.scale", 1.5f)
    
    brains["explorer"] = pynlm.createBrain(explorer_config)
    brains["explorer"].initialize()
    
    # Worker brain - task-focused efficiency
    worker_config = pynlm.createDefaultConfig()
    worker_config.set("brain.neuron_count", 1500)
    worker_config.set("brain.synapse_density", 0.08)
    worker_config.set("plasticity.stdp.enable", True)
    worker_config.set("plasticity.hebbian.enable", True)
    worker_config.set("plasticity.structural.enable", True)
    
    brains["worker"] = pynlm.createBrain(worker_config)
    brains["worker"].initialize()
    
    # Social brain - social learning and cooperation
    social_config = pynlm.createDefaultConfig()
    social_config.set("brain.neuron_count", 3000)
    social_config.set("brain.synapse_density", 0.03)
    social_config.set("cognition.self_model.enable", True)
    social_config.set("plasticity.social.enable", True)
    
    brains["social"] = pynlm.createBrain(social_config)
    brains["social"].initialize()
    
    return brains

def create_advanced_world():
    """Create a complex world with multiple object types and dynamics"""
    world = pynlm.createSimpleWorld()
    world.configure(
        width=50,
        height=50,
        visionWidth=16,
        visionHeight=16
    )
    
    # Set multiple agents starting positions
    world.setAgentStart(10.0, 10.0)
    world.setAgentStart(30.0, 30.0)
    world.setAgentStart(20.0, 20.0)
    
    # Add resources with different values
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0, 0.8))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, 5.0, 0.5))
    world.addObject(pynlm.WorldObject(35.0, 35.0, pynlm.WorldObjectType.Hazard, 3.0, 0.3))
    world.addObject(pynlm.WorldObject(45.0, 45.0, pynlm.WorldObjectType.Resource, 8.0, 0.6))
    
    # Add walls
    for x in range(0, 51, 5):
        world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, 0.0, 1.0))
        world.addObject(pynlm.WorldObject(x, 50, pynlm.WorldObjectType.Wall, 0.0, 1.0))
        world.addObject(pynlm.WorldObject(0, x, pynlm.WorldObjectType.Wall, 0.0, 1.0))
        world.addObject(pynlm.WorldObject(50, x, pynlm.WorldObjectType.Wall, 0.0, 1.0))
    
    return world

def run_advanced_multi_agent_simulation(num_agents=3, num_steps=1000):
    """Run advanced multi-agent simulation"""
    print("Starting Advanced Multi-Agent Simulation")
    print("=" * 50)
    
    # Create specialized brains
    brains = create_specialized_brains()
    
    # Create world
    world = create_advanced_world()
    
    # Create agent brains for each brain type
    agents = {}
    for brain_type, brain in brains.items():
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents[brain_type] = agent
    
    # Enable advanced features
    for agent in agents.values():
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
    
    # Run simulation
    agent_stats = {}
    for brain_type in brains.keys():
        agent_stats[brain_type] = {
            "steps": 0,
            "total_spikes": 0,
            "firing_neurons": 0,
            "reward": 0.0,
            "curiosity": 0.0,
            "novelty": 0.0,
            "patterns_learned": 0
        }
    
    print(f"Running {num_steps} simulation steps with {num_agents} agents")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process each agent's sensory input
        percepts = {}
        for brain_type, agent in agents.items():
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            percepts[brain_type] = percept
        
        # Each agent processes the world differently
        for brain_type, agent in agents.items():
            brain = agent.getBrain()
            
            # Agent-specific processing
            if brain_type == "explorer":
                # Explorer focuses on novel objects
                percept = percepts[brain_type]
                novelty = agent.getNoveltyLevel()
                if novelty > 0.7:
                    # High curiosity leads to exploration
                    pass
                    
            elif brain_type == "worker":
                # Worker focuses on resource gathering
                percept = percepts[brain_type]
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            elif brain_type == "social":
                # Social agent observes and learns from others
                percept = percepts[brain_type]
                brain.step(step)
                
                # Check for social interactions
                agent.applyRewardModulation(0.1, 0.05)  # Small social reward
        
        # Update development for all agents
        for agent in agents.values():
            agent.updateDevelopment(0.1)
        
        # Collect statistics
        for brain_type, agent in agents.items():
            brain = agent.getBrain()
            stats = agent_stats[brain_type]
            stats["steps"] += 1
            stats["total_spikes"] += brain.getTotalSpikeCount()
            stats["firing_neurons"] += brain.getFiringNeuronCount()
            stats["reward"] += agent.getNeuromodulationLevel()
            stats["curiosity"] += agent.getCuriosityLevel()
            stats["novelty"] += agent.getNoveltyLevel()
            
            # Print progress periodically
            if step % 100 == 0 and step > 0:
                print(f"Step {step}: {brain_type} brain - "
                      f"Spikes: {stats['total_spikes']}, "
                      f"Firing: {stats['firing_neurons']}, "
                      f"Curiosity: {stats['curiosity']/step:.3f}")
    
    # Print final statistics
    print("\n" + "=" * 50)
    print("Simulation Complete!")
    print("=" * 50)
    
    for brain_type, stats in agent_stats.items():
        print(f"\n{brain_type.upper()} Brain Performance:")
        print(f"  Steps: {stats['steps']}")
        print(f"  Total Spikes: {stats['total_spikes']}")
        print(f"  Average Firing: {stats['firing_neurons']/stats['steps']:.1f}")
        print(f"  Average Reward: {stats['reward']/stats['steps']:.3f}")
        print(f"  Average Curiosity: {stats['curiosity']/stats['steps']:.3f}")
        print(f"  Average Novelty: {stats['novelty']/stats['steps']:.3f}")
    
    return agent_stats

def run_pattern_learning_experiment():
    """Run pattern learning and concept formation experiment"""
    print("\nRunning Pattern Learning Experiment")
    print("=" * 50)
    
    # Create brain for pattern learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.05)
    config.set("memory.associative.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld())
    
    # Create patterns to learn
    patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.9, 0.8, 0.7, 0.6, 0.5],
        [0.5, 0.5, 0.5, 0.5, 0.5],
        [0.0, 0.1, 0.2, 0.3, 0.4],
        [0.4, 0.3, 0.2, 0.1, 0.0]
    ]
    
    # Train on patterns
    print("Training on patterns...")
    for iteration in range(100):
        # Present each pattern
        for pattern in patterns:
            # Create sensory input
            percept = pynlm.SensoryInput()
            vision = []
            for val in pattern:
                vision.append(val)
            while len(vision) < 16:
                vision.append(0.0)
            percept.setVision(vision)
            
            # Process input
            agent.processSensoryInput(percept)
            brain.step(iteration)
            
            # Apply some reward for successful learning
            reward = 0.1 if iteration % 10 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Monitor learning progress
        if iteration % 20 == 0:
            stats = brain.getBrainStatistics()
            print(f"Iteration {iteration}: Patterns learned = {stats.memory.associativeMemorySize}")
    
    # Test pattern recall
    print("\nTesting pattern recall...")
    test_patterns = patterns[:2]  # Test with first two patterns
    
    for i, pattern in enumerate(test_patterns):
        print(f"\nTesting pattern {i+1}:")
        print(f"  Original: {pattern}")
        
        # Present pattern
        percept = pynlm.SensoryInput()
        vision = []
        for val in pattern:
            vision.append(val)
        while len(vision) < 16:
            vision.append(0.0)
        percept.setVision(vision)
        
        agent.processSensoryInput(percept)
        brain.step(1000)
        
        # Get response
        action = agent.decodeMotorCommand()
        print(f"  Response: {action}")
    
    print("\nPattern learning experiment complete!")

if __name__ == "__main__":
    print("Advanced NLM Examples")
    print("=" * 60)
    
    # Run multi-agent simulation
    multi_agent_stats = run_advanced_multi_agent_simulation()
    
    # Run pattern learning experiment
    run_pattern_learning_experiment()
    
    print("\n" + "=" * 60)
    print("All advanced examples completed successfully!")
    print("=" * 60)
