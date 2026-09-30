"""
NLM Agent Examples
==================

This module contains examples demonstrating agent behavior and world interaction
using the enhanced NLM Python API.
"""

from .pynlm import create_brain, create_simple_world, create_agent, load_config_template
def basic_agent_example():
    """Example 1: Basic agent behavior in a simple world.
    
    This shows how an agent navigates a 2D world using sensory input
    and motor output, learning through experience.
    """
    print("Example 1: Basic Agent Navigation")
    print("-" * 35)
    
    # Create agent with basic settings
    brain = create_brain({"neuron_count": 150, "connection_probability": 0.15})
    world = create_simple_world(width=15, height=15, vision_width=6, vision_height=6)
    agent = create_agent(brain, world)
    
    # Set agent starting position
    agent._agent.getBrain()->getRegions()[0].get()->setPosition(7.5, 7.5)
    
    # Enable basic learning
    agent.enable_feature('reward_modulation', True)
    
    print(f"Agent created at position ({agent._agent.getBrain()->getRegions()[0].get()->getPosition().x}, "
          f"{agent._agent.getBrain()->getRegions()[0].get()->getPosition().y})")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # Run navigation simulation
    steps = 100
    movement_actions = 0
    turn_actions = 0
    
    for step in range(steps):
        # Sense environment
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        
        # Make brain decision
        brain.step(step)
        action = agent.decode_action()
        
        # Execute action
        world.apply_action(action, world.get_simulation_time())
        
        # Track action types
        if action.getType() == "MoveForward":
            movement_actions += 1
        elif action.getType() in ["TurnLeft", "TurnRight"]:
            turn_actions += 1
        
        # Provide feedback based on action
        reward = 1.0 if action.getType() != "Wait" else 0.0
        agent.apply_reward(reward)
        
        if step % 20 == 0:
            print(f"Step {step}: Action={action.getType()}, Reward={reward:.1f}, "
                  f"Development={agent.get_development_stage()}")
    
    print(f"\nNavigation results ({steps} steps):")
    print(f"  - Movements: {movement_actions}")
    print(f"  - Turns: {turn_actions}")
    print(f"  - Action efficiency: {(movement_actions + turn_actions) / steps:.2%}")
    print()


def curiosity_driven_exploration_example():
    """Example 2: Curiosity-driven exploration.
    
    This demonstrates how curiosity drives the agent to explore
    unfamiliar environments and learn from novel experiences.
    """
    print("Example 2: Curiosity-Driven Exploration")
    print("-" * 35)
    
    # Create exploration-ready agent
    config = load_config_template('fast')
    brain = create_brain(config)
    world = create_simple_world(width=20, height=20, vision_width=8, vision_height=8)
    agent = create_agent(brain, world)
    
    # Enable curiosity and all learning features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('curiosity', True)
    agent.enable_feature('structural_plasticity', True)
    
    print(f"Exploration agent initialized with curiosity enabled")
    print(f"Initial curiosity level: {agent.get_curiosity_level():.2f}")
    
    # Run exploration simulation
    steps = 200
    exploration_events = []
    reward_events = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Compute reward and apply
        curiosity_bonus = agent.get_curiosity_level() * 0.3
        base_reward = 0.1
        reward = base_reward + curiosity_bonus
        
        agent.apply_reward(reward, 0.0)
        
        # Track exploration events
        if agent.get_novelty_level() > 0.7:
            exploration_events.append({
                'step': step,
                'novelty': agent.get_novelty_level(),
                'curiosity': agent.get_curiosity_level(),
                'action': action.getType()
            })
        
        reward_events.append({
            'step': step,
            'reward': reward,
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level()
        })
        
        if step % 40 == 0:
            print(f"Step {step}: Curios={agent.get_curiosity_level():.2f}, "
                  f"Novel={agent.get_novelty_level():.2f}, Action={action.getType()}")
    
    print(f"\nExploration results:")
    print(f"  - Total exploration events: {len(exploration_events)}")
    print(f"  - Average curiosity: {sum(d['curiosity'] for d in reward_events)/len(reward_events):.2f}")
    print(f"  - Average novelty: {sum(d['novelty'] for d in reward_events)/len(reward_events):.2f}")
    print(f"  - Exploration efficiency: {len(exploration_events)/steps:.2%}")
    
    # Show exploration highlights
    if exploration_events:
        print(f"\nExploration highlights (top {min(3, len(exploration_events))}):")
        for event in exploration_events[-min(3, len(exploration_events)):]:
            print(f"  Step {event['step']}: Novelty={event['novelty']:.2f}, "
                  f"Curiosity={event['curiosity']:.2f}, Action={event['action']}")
    print()


def multi_agent_coordination_example():
    """Example 3: Multiple agent coordination (simulated).
    
    This demonstrates how multiple agents could coordinate their actions
    through shared environment and learned social behaviors.
    """
    print("Example 3: Multi-Agent Coordination (Simulation)")
    print("-" * 35)
    
    # Create multiple agents that share the same brain
    config = load_config_template('rich')
    
    # Create shared brain and world
    brain = create_brain(config)
    world = create_simple_world(width=25, height=25, vision_width=10, vision_height=10)
    
    # Create multiple agents
    agents = []
    for i in range(3):
        agent = create_agent(brain, world)
        agent.enable_feature('reward_modulation', True)
        agent.enable_feature('curiosity', True)
        agent.enable_feature('structural_plasticity', True)
        
        # Set different starting positions
        pos_x = 5 + i * 6
        pos_y = 5 + i * 3
        agent._agent.getBrain()->getRegions()[0].get()->setPosition(pos_x, pos_y)
        
        agents.append(agent)
        print(f"Created agent {i+1} at position ({pos_x}, {pos_y})")
    
    # Run coordination simulation
    steps = 150
    coordination_events = []
    
    for step in range(steps):
        world.update(0.1)
        
        # Each agent senses the world
        perceptions = [world.get_sensory_percept() for _ in agents]
        
        # Each agent processes and acts independently
        for i, agent in enumerate(agents):
            agent.process_sensory(perceptions[i])
            brain.step(step)
            action = agent.decode_action()
            world.apply_action(action, world.get_simulation_time())
            
            # Cooperative behavior: reward for actions that move toward center
            center_x, center_y = world.getWidth() / 2, world.getHeight() / 2
            pos = agent._agent.getBrain()->getRegions()[0].get()->getPosition()
            distance_to_center = ((pos.x - center_x)**2 + (pos.y - center_y)**2)**0.5
            
            # Normalize distance to [0, 1] (closer to center = better)
            proximity_bonus = max(0, 1 - distance_to_center / 15)
            
            reward = 0.1 + proximity_bonus * 0.5
            agent.apply_reward(reward, 0.0)
            
            # Track coordination metrics
            if proximity_bonus > 0.5:  # Agent is close to center
                coordination_events.append({
                    'step': step,
                    'agent': i,
                    'proximity': proximity_bonus,
                    'action': action.getType()
                })
        
        if step % 30 == 0:
            print(f"Step {step}: Agents processed, "
                  f"Average proximity: {sum(e['proximity'] for e in coordination_events)/max(1, len(coordination_events)):.2f}")
    
    print(f"\nCoordination results:")
    print(f"  - Total coordination events: {len(coordination_events)}")
    print(f"  - Coordination rate: {len(coordination_events)/(steps*len(agents)):.2%}")
    print(f"  - Average proximity to center: {sum(e['proximity'] for e in coordination_events)/max(1, len(coordination_events)):.2f}")
    print()


def developmental_progression_example():
    """Example 4: Brain developmental progression.
    
    This demonstrates how the brain develops and matures over time,
    showing changes in behavior and capabilities.
    """
    print("Example 4: Developmental Progression")
    print("-" * 35)
    
    # Create developing brain
    brain = create_brain({"neuron_count": 200, "enable_development": True})
    world = create_simple_world(width=20, height=20, vision_width=8, vision_height=8)
    agent = create_agent(brain, world)
    
    # Enable all development features
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('structural_plasticity', True)
    agent.enable_feature('development', True)
    agent.enable_feature('curiosity', True)
    
    print(f"Development simulation with {brain.get_stats()['total_neurons']} neurons")
    
    # Track development over time
    developmental_data = []
    behavioral_data = []
    
    for stage in range(5):  # Simulate 5 development stages
        print(f"\nDevelopment Stage {stage + 1}:")
        stage_start = len(developmental_data)
        
        # Run stage for 50 steps
        for step in range(50):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory(percept)
            brain.step(stage * 50 + step)
            action = agent.decode_action()
            world.apply_action(action, world.get_simulation_time())
            
            # Get developmental stage
            dev_stage = agent.get_development_stage()
            
            # Record data
            developmental_data.append({
                'stage': stage + 1,
                'step': stage * 50 + step,
                'development_stage': str(dev_stage),
                'curiosity': agent.get_curiosity_level(),
                'novelty': agent.get_novelty_level(),
                'prediction_error': agent.get_prediction_error()
            })
            
            behavioral_data.append({
                'stage': stage + 1,
                'step': stage * 50 + step,
                'action': action.getType(),
                'reward': 0.1 + agent.get_curiosity_level() * 0.3
            })
            
            agent.apply_reward(0.1 + agent.get_curiosity_level() * 0.3)
            agent.update_development(0.1)
    
    # Analyze development
    stages = sorted(set(d['stage'] for d in developmental_data))
    
    print(f"\nDevelopment Analysis:")
    for stage in stages:
        stage_data = [d for d in developmental_data if d['stage'] == stage]
        if stage_data:
            avg_curiosity = sum(d['curiosity'] for d in stage_data) / len(stage_data)
            avg_novelty = sum(d['novelty'] for d in stage_data) / len(stage_data)
            print(f"  Stage {stage}: Average curiosity={avg_curiosity:.2f}, "
                  f"Novelty={avg_novelty:.2f}, Action frequency="
                  f"{max([list(behavioral_data).count({'stage': stage, 'action': a}) for a in ['MoveForward', 'TurnLeft', 'TurnRight']])}")
    
    print()


def neuroplasticity_example():
    """Example 5: Neuroplasticity and adaptation.
    
    This demonstrates how the brain's structure changes over time
    through synaptic plasticity and learning.
    """
    print("Example 5: Neuroplasticity and Adaptation")
    print("-" * 35)
    
    # Create plastic brain
    brain = create_brain({
        "neuron_count": 300,
        "connection_probability": 0.25,
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.04,
        "enable_development": True
    })
    world = create_simple_world(width=25, height=25, vision_width=10, vision_height=10)
    agent = create_agent(brain, world)
    
    # Enable plasticity
    agent.enable_feature('structural_plasticity', True)
    agent.enable_feature('reward_modulation', True)
    agent.enable_feature('curiosity', True)
    
    print(f"Plasticity simulation with {brain.get_stats()['total_neurons']} neurons and {brain.get_stats()['total_synapses']} synapses")
    
    # Track plasticity metrics
    plasticity_metrics = []
    
    for step in range(100):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory(percept)
        brain.step(step)
        action = agent.decode_action()
        world.apply_action(action, world.get_simulation_time())
        
        # Get current brain stats
        stats = brain.get_stats()
        
        # Record plasticity metrics
        plasticity_metrics.append({
            'step': step,
            'total_neurons': stats['total_neurons'],
            'total_synapses': stats['total_synapses'],
            'firing_neurons': stats['firing_neurons'],
            'average_firing_rate': stats['average_firing_rate'],
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level()
        })
        
        # Apply adaptive learning
        curiosity_bonus = agent.get_curiosity_level() * 0.5
        reward = 0.1 + curiosity_bonus
        agent.apply_reward(reward, 0.0)
    
    # Analyze plasticity
    initial = plasticity_metrics[0]
    final = plasticity_metrics[-1]
    
    print(f"\nPlasticity Results:")
    print(f"  - Initial state: {initial['total_neurons']} neurons, {initial['total_synapses']} synapses")
    print(f"  - Final state: {final['total_neurons']} neurons, {final['total_synapses']} synapses")
    print(f"  - Neuron change: {final['total_neurons'] - initial['total_neurons']} ({((final['total_neurons'] - initial['total_neurons'])/initial['total_neurons'])*100:.1f}%)")
    print(f"  - Synapse change: {final['total_synapses'] - initial['total_synapses']} ({((final['total_synapses'] - initial['total_synapses'])/initial['total_synapses'])*100:.1f}%)")
    print(f"  - Average firing rate change: {final['average_firing_rate'] - initial['average_firing_rate']:.3f} Hz")
    print(f"  - Curiosity progression: {initial['curiosity']:.2f} → {final['curiosity']:.2f}")
    print(f"  - Novelty progression: {initial['novelty']:.2f} → {final['novelty']:.2f}")
    
    # Check for learning indicators
    synapse_growth = (final['total_synapses'] - initial['total_synapses']) / initial['total_synapses']
    firing_change = abs(final['average_firing_rate'] - initial['average_firing_rate']) / initial['average_firing_rate']
    
    print(f"\nLearning Indicators:")
    print(f"  - Synaptic growth: {synapse_growth:.2%}")
    print(f"  - Firing rate change: {firing_change:.2%}")
    print(f"  - Overall plasticity: {'Strong' if synapse_growth > 0.1 or firing_change > 0.2 else 'Moderate' if synapse_growth > 0.05 or firing_change > 0.1 else 'Weak'}")
    print()


if __name__ == "__main__":
    print("NLM Agent Examples")
    print("==================")
    print()
    
    try:
        basic_agent_example()
        curiosity_driven_exploration_example()
        multi_agent_coordination_example()
        developmental_progression_example()
        neuroplasticity_example()
        
        print("All agent examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
