# Advanced NLM API Examples and Tests

This file contains comprehensive examples and tests for advanced NLM Python API usage, covering complex scenarios and edge cases.

import pynlm
import time
import numpy as np
from typing import Dict, List, Tuple
import unittest.mock as mock

def example_1_advanced_brain_setup():
    """
    Example 1: Advanced brain setup with custom configuration and optimizations
    """
    print("=== Example 1: Advanced Brain Setup ===")
    
    # Create brain with detailed configuration
    config = pynlm.createDefaultConfig()
    
    # Optimize for learning performance
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.15)
    config.set("brain.v_thresh", -45.0)  # Higher threshold for more focused firing
    config.set("brain.tau_mem", 25.0)    # Slower decay for memory
    
    # Configure plasticity for faster learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.stdp.tau_plus", 30.0)
    config.set("plasticity.stdp.tau_minus", 30.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    
    # Configure neuromodulation
    config.set("neuromod.dopamine.scale", 2.0)  # Stronger dopamine effect
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")
    
    return brain


def example_2_agent_with_advanced_features():
    """
    Example 2: Advanced agent with all features enabled
    """
    print("\n=== Example 2: Advanced Agent with All Features ===")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create agent with all advanced features
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure advanced parameters
    brain.getConfig().set("brain.initial_weight_mean", 0.3)
    brain.getConfig().set("brain.initial_weight_std", 0.05)
    brain.getConfig().set("neuromod.dopamine.scale", 1.5)
    brain.getConfig().set("neuromod.curiosity.threshold", 0.3)
    
    # Run sophisticated simulation
    print("Running advanced simulation with curiosity-driven exploration...")
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decodes motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log advanced metrics every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    
    return brain, agent, world


def example_3_custom_sensory_processing():
    """
    Example 3: Custom sensory processing and feature extraction
    """
    print("\n=== Example 3: Custom Sensory Processing ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Custom sensory processing function
    def process_custom_features(percept):
        """
        Custom feature extraction beyond default vision/touch/internal signals.
        """
        # Extract custom features from percept
        vision_data = percept.getVision()
        touch_data = percept.getTouch()
        internal_data = percept.getInternal()
        
        # Create enhanced features
        enhanced_features = {}
        
        # Vision features
        enhanced_features['vision_avg'] = np.mean(vision_data) if vision_data else 0.0
        enhanced_features['vision_std'] = np.std(vision_data) if vision_data else 0.0
        enhanced_features['vision_max'] = np.max(vision_data) if vision_data else 0.0
        
        # Touch features
        enhanced_features['touch_avg'] = np.mean(touch_data) if touch_data else 0.0
        enhanced_features['touch_range'] = np.max(touch_data) - np.min(touch_data) if touch_data else 0.0
        
        # Internal features
        if internal_data:
            enhanced_features['energy_level'] = internal_data[0]
            enhanced_features['health_level'] = internal_data[1]
            enhanced_features['movement_state'] = internal_data[2]
            enhanced_features['turning_state'] = internal_data[3]
        
        # Body state features
        body = world.getAgentBody()
        enhanced_features['position_x'] = body.x
        enhanced_features['position_y'] = body.y
        enhanced_features['orientation'] = body.orientation
        enhanced_features['velocity_x'] = body.velocityX
        enhanced_features['velocity_y'] = body.velocityY
        
        return enhanced_features
    
    # Simulate with custom processing
    for step in range(100):
        world.update(0.1)
        
        # Get standard percept
        percept = world.getSensoryPercept()
        
        # Process with custom features
        custom_features = process_custom_features(percept)
        print(f"Step {step}: Custom features: {custom_features}")
        
        # Inject custom features into brain (using internal signals)
        internal_signals = percept.getInternal()
        enhanced_internal = internal_signals.copy() if internal_signals else []
        
        # Add custom features as additional internal signals
        enhanced_internal.extend([
            custom_features.get('vision_avg', 0.0),
            custom_features.get('touch_avg', 0.0),
            custom_features.get('energy_level', 0.0)
        ])
        
        # Create enhanced percept (hacky but demonstrates the concept)
        # In a real implementation, you'd extend SensoryPercept to support custom features
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Brain stats: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain, agent, world


def example_4_memory_system_analysis():
    """
    Example 4: Analysis of memory system integration
    """
    print("\n=== Example 4: Memory System Analysis ===")
    
    # Create brain with memory-optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.v_thresh", -50.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Memory systems available:")
    print(f"  Working memory: {working_memory is not None}")
    print(f"  Episodic memory: {episodic_memory is not None}")
    
    if working_memory:
        print(f"  Working memory active traces: {working_memory.getActiveTraces()}")
        print(f"  Working memory capacity: {working_memory.getCapacity()}")
        
    if episodic_memory:
        print(f"  Episodic memory episode count: {episodic_memory.getEpisodeCount()}")
        print(f"  Episodic memory capacity: {episodic_memory.getCapacity()}")
    
    # Run simulation to populate memory
    print("\nRunning simulation to populate memory systems...")
    
    for step in range(500):
        # Create simple input to stimulate memory
        vision_data = np.random.rand(16) * 0.5  # Low intensity input
        vision = pynlm.Vision(8, 8, 3)
        vision.setData(list(vision_data))
        
        # Inject sensory input
        brain.receiveSensoryInput(vision)
        brain.step(step, step * 0.001)
        
        # Check memory status periodically
        if step % 100 == 0 and working_memory:
            traces = working_memory.getActiveTraces()
            print(f"Step {step}: Working memory traces: {traces}")
            
        if step % 200 == 0 and episodic_memory:
            episodes = episodic_memory.getEpisodeCount()
            print(f"Step {step}: Episodic memory episodes: {episodes}")
    
    # Analyze memory effectiveness
    print("\nMemory System Analysis:")
    if working_memory:
        final_traces = working_memory.getActiveTraces()
        print(f"  Working memory final state: {final_traces}")
        
    if episodic_memory:
        final_episodes = episodic_memory.getEpisodeCount()
        print(f"  Episodic memory final state: {final_episodes} episodes")
        
        # Test replay if available
        if hasattr(episodic_memory, 'getEpisodesForReplay'):
            replay_episodes = episodic_memory.getEpisodesForReplay(3)
            print(f"  Replay available: {len(replay_episodes)} episodes for replay")
    
    return brain


def example_5_plasticity_learning_curves():
    """
    Example 5: Analyzing plasticity learning curves
    """
    print("\n=== Example 5: Plasticity Learning Curves ===")
    
    # Create brain with strong plasticity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable plasticity on all synapses
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            for synapse in region.getSynapses():
                synapse.enablePlasticity(true, true, false)  # Enable STDP and Hebbian
    
    print("Running plasticity learning simulation...")
    
    # Track synaptic weight changes
    initial_weights = []
    final_weights = []
    firing_rates = []
    
    # Get initial weights from first region
    if brain.getRegionCount() > 0:
        region = brain.getRegion(brain.getRegionIds()[0])
        if region:
            for synapse in region.getSynapses():
                initial_weights.append(synapse.getWeight())
    
    # Run learning simulation
    for step in range(1000):
        # Apply learning stimuli
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
        
        # Run brain step
        brain.step(step, step * 0.001)
        
        # Record metrics
        firing_rates.append(brain.getAverageFiringRate())
        
        # Record final weights
        if step == 999 and brain.getRegionCount() > 0:
            region = brain.getRegion(brain.getRegionIds()[0])
            if region:
                for synapse in region.getSynapses():
                    final_weights.append(synapse.getWeight())
        
        # Log progress
        if step % 200 == 0:
            print(f"Step {step}: Firing rate: {firing_rates[-1]:.3f}")
    
    # Analyze learning curves
    if initial_weights and final_weights:
        initial_mean = np.mean(initial_weights)
        final_mean = np.mean(final_weights)
        weight_change = final_mean - initial_mean
        
        print(f"\nPlasticity Analysis:")
        print(f"  Initial mean synaptic weight: {initial_mean:.3f}")
        print(f"  Final mean synaptic weight: {final_mean:.3f}")
        print(f"  Weight change: {weight_change:.3f}")
        print(f"  Learning rate: {(weight_change / initial_mean) * 100:.1f}%")
        
        # Determine if learning occurred
        if abs(weight_change) > 0.001:
            print(f"  ✓ Learning detected: Synaptic weights changed significantly")
        else:
            print(f"  ! Minimal learning: Weights changed little")
        
        # Plot firing rate curve (if matplotlib available)
        try:
            import matplotlib.pyplot as plt
            plt.figure(figsize=(10, 6))
            plt.plot(range(0, len(firing_rates) * 10, 10), firing_rates)
            plt.xlabel('Step')
            plt.ylabel('Average Firing Rate')
            plt.title('Firing Rate During Learning')
            plt.grid(True, alpha=0.3)
            plt.savefig('learning_curve.png')
            print("  Learning curve saved as 'learning_curve.png'")
            plt.close()
        except ImportError:
            print("  Matplotlib not available, skipping plot generation")
    
    return brain


def example_6_advanced_world_scenarios():
    """
    Example 6: Advanced world scenarios and complex environments
    """
    print("\n=== Example 6: Advanced World Scenarios ===")
    
    # Scenario 1: Resource collection world
    print("Scenario 1: Resource collection world")
    
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world1.reset()
    
    # Add many resources and hazards
    for _ in range(10):
        # Add resource
        resource = pynlm.WorldObject(
            x=np.random.uniform(5, 45),
            y=np.random.uniform(5, 45),
            type=pynlm.WorldObjectType.Resource,
            value=10.0,
            radius=0.8
        )
        world1.addObject(resource)
        
    for _ in range(3):
        # Add hazard
        hazard = pynlm.WorldObject(
            x=np.random.uniform(5, 45),
            y=np.random.uniform(5, 45),
            type=pynlm.WorldObjectType.Hazard,
            value=-15.0,
            radius=0.6
        )
        world1.addObject(hazard)
    
    # Create agent and simulate
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world1)
    
    # Enable curiosity for exploration
    agent1.enableCuriosity(True)
    agent1.enableRewardModulation(True)
    
    print(f"  World created with {world1.getWidth()}x{world1.getHeight()} dimensions")
    print(f"  Resources and hazards placed")
    print(f"  Agent starting position: ({world1.getAgentBody().x:.1f}, {world1.getAgentBody().y:.1f})")
    
    # Simulate resource collection
    resource_collected = 0
    hazard_avoided = 0
    
    for step in range(200):
        world1.update(0.1)
        
        percept = world1.getSensoryPercept()
        agent1.processSensoryInput(percept)
        brain1.step(step)
        action = agent1.decodeMotorCommand()
        
        # Check what was collected/interacted with
        action_result = world1.applyMotorCommand(action, world1.getSimulationTime())
        
        if action_result.success and "consumed_resource" in action_result.message:
            resource_collected += 1
        if action_result.success and "hit_hazard" in action_result.message:
            hazard_avoided += 1
    
    print(f"  Resources collected: {resource_collected}")
    print(f"  Hazards avoided: {hazard_avoided}")
    
    # Scenario 2: Complex navigation with obstacles
    print("\nScenario 2: Complex navigation with dynamic obstacles")
    
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world2.reset()
    
    # Add walls for navigation challenge
    for x in range(10, 30):
        world2.addObject(pynlm.WorldObject(
            x=x, y=20, type=pynlm.WorldObjectType.Wall, radius=0.4
        ))
    
    # Create agent
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world2)
    
    # Set specific starting position
    world2.setAgentStart(5.0, 5.0)
    
    print(f"  World with navigation obstacles created")
    print(f"  Agent starting at: ({world2.getAgentBody().x:.1f}, {world2.getAgentBody().y:.1f})")
    
    # Navigate through obstacles
    navigation_success = 0
    for step in range(150):
        world2.update(0.1)
        
        percept = world2.getSensoryPercept()
        agent2.processSensoryInput(percept)
        brain2.step(step)
        action = agent2.decodeMotorCommand()
        
        # Check if action was successful (didn't hit wall)
        result = world2.applyMotorCommand(action, world2.getSimulationTime())
        if result.success and "hitting wall" not in result.message:
            navigation_success += 1
    
    print(f"  Successful navigation actions: {navigation_success}/150")
    
    return brain1, agent1, world1


def example_7_reinforcement_learning():
    """
    Example 7: Simple reinforcement learning with agent
    """
    print("\n=== Example 7: Simple Reinforcement Learning ===")
    
    # Create RL environment (world) and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # RL training parameters
    total_episodes = 10
    max_steps_per_episode = 100
    
    # Training loop
    episode_rewards = []
    episode_lengths = []
    curiosity_levels = []
    
    for episode in range(total_episodes):
        episode_reward = 0.0
        episode_steps = 0
        
        # Reset environment for new episode
        world.reset()
        
        for step in range(max_steps_per_episode):
            # Environment update
            world.update(0.1)
            
            # Get state
            percept = world.getSensoryPercept()
            
            # Agent processes state
            agent.processSensoryInput(percept)
            
            # Agent takes action
            action = agent.decodeMotorCommand()
            
            # Environment executes action
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = result.reward
            episode_reward += reward
            episode_steps += 1
            
            # Agent learns from reward
            agent.applyRewardModulation(reward, 0.0)  # Simple RL with no predicted reward
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Store episode metrics
            if step % 20 == 0:
                curiosity_levels.append(agent.getCuriosityLevel())
        
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_steps)
        
        print(f"Episode {episode + 1}/{total_episodes}: Reward={episode_reward:.2f}, Steps={episode_steps}")
    
    # Analyze learning
    print(f"\nReinforcement Learning Analysis:")
    print(f"  Average reward per episode: {np.mean(episode_rewards):.2f}")
    print(f"  Average episode length: {np.mean(episode_lengths):.1f}")
    print(f"  Average curiosity level: {np.mean(curiosity_levels):.3f}")
    
    # Check if learning improved over time
    if len(episode_rewards) > 5:
        first_half = episode_rewards[:len(episode_rewards)//2]
        second_half = episode_rewards[len(episode_rewards)//2:]
        improvement = np.mean(second_half) - np.mean(first_half)
        
        if improvement > 0:
            print(f"  ✓ Learning detected: Reward improved by {improvement:.2f}")
        else:
            print(f"  ! No significant improvement detected")
    
    return brain, agent, world, {
        'episode_rewards': episode_rewards,
        'episode_lengths': episode_lengths,
        'curiosity_levels': curiosity_levels
    }


if __name__ == "__main__":
    print("Advanced NLM API Examples")
    print("=" * 50)
    
    # Run all examples
    try:
        example_1_advanced_brain_setup()
        example_2_agent_with_advanced_features()
        example_3_custom_sensory_processing()
        example_4_memory_system_analysis()
        example_5_plasticity_learning_curves()
        example_6_advanced_world_scenarios()
        example_7_reinforcement_learning()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
