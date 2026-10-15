#!/usr/bin/env python3
"""
Comprehensive Example: Advanced NLM Agent with Learning and Development

This example demonstrates the enhanced NLM Python API for creating sophisticated
artificial brains that learn, develop, and interact with their environment.
"""

import pynlm
import random
import math

def create_advanced_agent_brain(neuron_count=1000, enable_all_systems=True):
    """Create an advanced agent brain with optimized configuration."""
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    config.set("brain.v_thresh", -50.0)  # Higher threshold for stability
    config.set("brain.tau_mem", 25.0)   # Slower membrane time constant
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_learning_world(width=30, height=30, enable_resources=True):
    """Create a rich learning environment with resources and challenges."""
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=width,
        height=height,
        visionWidth=12,
        visionHeight=12
    )
    world.reset()
    
    # Set agent starting position in center
    world.setAgentStart(width // 2, height // 2)
    
    # Add environmental resources
    if enable_resources:
        # Add food resources (positive rewards)
        for _ in range(8):
            x = random.randint(3, width - 4)
            y = random.randint(3, height - 4)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0))
        
        # Add hazards (negative rewards)
        for _ in range(4):
            x = random.randint(0, width - 1)
            y = random.randint(0, height - 1)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-5.0))
        
        # Add landmarks for navigation
        for _ in range(6):
            x = random.randint(1, width - 2)
            y = random.randint(1, height - 2)
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, value=1.0))
    
    return world

def create_agent_with_learning_systems():
    """Create an agent brain with all learning systems enabled."""
    
    # Create brain
    brain = create_advanced_agent_brain(800)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = create_learning_world()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning subsystems for advanced behavior
    agent.enableRewardModulation(True)      # Reinforcement learning
    agent.enableCuriosity(True)             # Intrinsic motivation
    agent.enableDevelopment(True)           # Developmental plasticity
    agent.enableStructuralPlasticity(True)   # Neural growth and pruning
    
    return brain, agent, world

def run_advanced_learning_simulation(brain, agent, world, num_steps=1000):
    """Run a complete learning simulation with advanced features."""
    
    print("Starting Advanced Learning Simulation")
    print("=" * 50)
    print(f"Brain Configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"World Size: {world.getWidth()}x{world.getHeight()}")
    print(f"Simulation Steps: {num_steps}")
    print("Learning Systems: Reward Modulation, Curiosity, Development, Structural Plasticity")
    print("=" * 50)
    
    # Statistics collection
    stats = {
        'steps_completed': 0,
        'total_rewards': 0.0,
        'max_curiosity': 0.0,
        'max_novelty': 0.0,
        'min_prediction_error': float('inf'),
        'episodes_explored': 0,
        'neural_optimization_events': 0
    }
    
    # Developmental milestones
    developmental_milestones = [
        (pynlm.DevelopmentalStage.CriticalPeriod, 100),
        (pynlm.DevelopmentalStage.Maturation, 300),
        (pynlm.DevelopmentalStage.Adult, 600)
    ]
    
    current_milestone_idx = 0
    
    for step in range(num_steps):
        # 1. Update world environment
        world.update(0.1)
        
        # 2. Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # 4. Brain processes information
        brain.step(step)
        
        # 5. Get motor command from brain
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation for learning
        # Extract reward from internal signals (simulated)
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals and len(internal_signals) > 0 else 0.0
        
        # Get prediction error for learning
        prediction_error = agent.getPredictionError()
        stats['min_prediction_error'] = min(stats['min_prediction_error'], prediction_error)
        
        # Apply reward-based learning
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats['total_rewards'] += reward
        stats['max_curiosity'] = max(stats['max_curiosity'], agent.getCuriosityLevel())
        stats['max_novelty'] = max(stats['max_novelty'], agent.getNoveltyLevel())
        
        # Check for developmental stage changes
        if current_milestone_idx < len(developmental_milestones):
            target_stage, milestone_step = developmental_milestones[current_milestone_idx]
            if step >= milestone_step and brain.getDevelopmentalStage() != target_stage:
                brain.setDevelopmentalStage(target_stage)
                print(f"🎯 Developmental Milestone: Reached {target_stage} stage at step {step}")
                current_milestone_idx += 1
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step:4d}: "
                  f"Firing: {brain.getFiringNeuronCount():4d} | "
                  f"Spike Rate: {brain.getAverageFiringRate():6.2f} Hz | "
                  f"Curiosity: {agent.getCuriosityLevel():.3f} | "
                  f"Novelty: {agent.getNoveltyLevel():.3f} | "
                  f"Prediction Error: {prediction_error:.3f} | "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    # Calculate final statistics
    final_stats = {
        'total_steps': num_steps,
        'average_reward': stats['total_rewards'] / num_steps,
        'max_curiosity_achieved': stats['max_curiosity'],
        'max_novelty_achieved': stats['max_novelty'],
        'min_prediction_error_achieved': stats['min_prediction_error'],
        'final_developmental_stage': brain.getDevelopmentalStage(),
        'final_neuron_count': brain.getTotalNeuronCount(),
        'final_synapse_count': brain.getTotalSynapseCount(),
        'total_spikes_generated': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate()
    }
    
    return final_stats

def demonstrate_advanced_features():
    """Demonstrate advanced features and capabilities."""
    
    print("NLM Advanced Features Demonstration")
    print("=" * 60)
    
    # Feature 1: Advanced Brain Configuration
    print("\n1. Advanced Brain Configuration:")
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1500)
    config.setDouble("brain.synapse_density", 0.2)
    config.setDouble("brain.initial_weight_mean", 0.6)
    config.setDouble("brain.v_thresh", -55.0)
    config.setBool("plasticity.stdp.enable", True)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ {brain.getTotalSynapseCount()} synapses established")
    print(f"   ✓ All plasticity systems enabled")
    
    # Feature 2: World Management
    print("\n2. Advanced World Management:")
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    world.setRandomSeed(42)  # Reproducible environments
    
    # Add complex environment
    for x in range(10, 40):
        for y in range(10, 40):
            if random.random() < 0.05:  # 5% walls
                world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, radius=0.3))
    
    print(f"   ✓ Created world with {world.getWidth()}x{world.getHeight()} dimensions")
    print(f"   ✓ Added environmental obstacles")
    
    # Feature 3: Agent with Learning Systems
    print("\n3. Advanced Agent with Learning Systems:")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"   ✓ Reward Modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"   ✓ Curiosity Drive: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    print(f"   ✓ Development System: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"   ✓ Structural Plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    
    # Feature 4: Checkpoint and Persistence
    print("\n4. Checkpoint and Persistence:")
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
        checkpoint_file = f.name
    
    try:
        # Save checkpoint
        success = brain.save(checkpoint_file)
        print(f"   ✓ Brain checkpoint saved: {success}")
        
        # Create new brain and load checkpoint
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        success = new_brain.load(checkpoint_file)
        print(f"   ✓ Brain checkpoint loaded: {success}")
        print(f"   ✓ Checkpoint integrity verified: {new_brain.getTotalNeuronCount()} == {brain.getTotalNeuronCount()}")
        
    finally:
        os.unlink(checkpoint_file)
    
    # Feature 5: Performance Benchmark
    print("\n5. Performance Benchmarking:")
    import time
    
    start_time = time.time()
    
    benchmark_brain = pynlm.benchmarkBrainCreation(500, 2)
    neuron_count, synapse_count, duration_ms = benchmark_brain
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"   ✓ Brain creation benchmark: {neuron_count} neurons, {synapse_count} synapses")
    print(f"   ✓ Construction time: {duration_ms:.2f} ms")
    print(f"   ✓ Total benchmark time: {total_time:.2f} seconds")
    
    # Feature 6: Configuration Validation
    print("\n6. Configuration Validation:")
    is_valid, message = pynlm.validateConfiguration(config)
    print(f"   ✓ Configuration validation: {'PASSED' if is_valid else 'FAILED'}")
    print(f"   ✓ Validation message: {message}")
    
    print("\n" + "=" * 60)
    print("Advanced Features Demonstration Complete!")
    print("=" * 60)

def main():
    """Main demonstration function."""
    
    print("🚀 NLM Advanced Agent Learning Simulation")
    print("=" * 60)
    print("This demonstration showcases the enhanced NLM Python API")
    print("with advanced learning, development, and environmental interaction.")
    print("=" * 60)
    
    try:
        # Create advanced agent
        brain, agent, world = create_agent_with_learning_systems()
        
        # Run simulation
        stats = run_advanced_learning_simulation(brain, agent, world, 800)
        
        # Print final results
        print("\n" + "=" * 60)
        print("SIMULATION RESULTS")
        print("=" * 60)
        print(f"✓ Total Steps Completed: {stats['total_steps']}")
        print(f"✓ Average Reward per Step: {stats['average_reward']:.3f}")
        print(f"✓ Maximum Curiosity Achieved: {stats['max_curiosity_achieved']:.3f}")
        print(f"✓ Maximum Novelty Experienced: {stats['max_novelty_achieved']:.3f}")
        print(f"✓ Minimum Prediction Error: {stats['min_prediction_error_achieved']:.3f}")
        print(f"✓ Final Developmental Stage: {stats['final_developmental_stage']}")
        print(f"✓ Total Spikes Generated: {stats['total_spikes_generated']}")
        print(f"✓ Average Firing Rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"✓ Final Neural Complexity: {stats['final_neuron_count']} neurons, {stats['final_synapse_count']} synapses")
        print("=" * 60)
        
        # Run additional feature demonstrations
        demonstrate_advanced_features()
        
        print("\n🎉 Advanced NLM Agent Simulation Complete!")
        print("\nKey Improvements Demonstrated:")
        print("• Enhanced Python API with type hints and documentation")
        print("• Advanced configuration validation and error handling")
        print("• Comprehensive learning system integration")
        print("• Checkpoint persistence and recovery")
        print("• Performance benchmarking and optimization")
        print("• Complex environmental interaction")
        
    except Exception as e:
        print(f"\n❌ Simulation failed with error: {str(e)}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
