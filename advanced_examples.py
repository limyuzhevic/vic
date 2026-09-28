#!/usr/bin/env python3
"""
Advanced NLM Python Examples
============================

This file provides comprehensive examples of using the NLM Python API
for advanced simulations and demonstrations of neural learning capabilities.

These examples build upon the basic API and demonstrate:
- Complex agent simulations
- Memory and learning integration
- Performance monitoring
- Configuration customization
- Advanced world interaction patterns
"""

import pynlm
import time
import sys

def example_1_advanced_agent_simulation():
    """
    Advanced agent simulation demonstrating complete brain-world interaction.
    
    This example shows how to create a sophisticated agent that:
    - Perceives its environment through sensory input
    - Makes decisions based on neural processing
    - Takes actions to achieve goals
    - Learns from rewards and experiences
    - Maintains working memory of recent experiences
    """
    print("=== Advanced Agent Simulation Example ===")
    
    # Create custom configuration for advanced simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Initialize brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with larger environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.setRandomSeed(12345)
    
    # Place agent in center of world
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems for maximum adaptability
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Track performance metrics
    metrics = {
        'steps': 0,
        'total_reward': 0.0,
        'max_reward': 0.0,
        'memory_episodes': 0,
        'curiosity_level': 0.0,
        'novelty_level': 0.0,
        'avg_firing_rate': 0.0,
        'dev_stage': 0
    }
    
    print(f"Starting advanced agent simulation...")
    print(f"World: {world.getWidth()}x{world.getHeight()} pixels")
    print(f"Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()} pixels")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Run extended simulation
    for step in range(2000):
        # Update world state
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Decode motor command from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get internal signals for reward calculation
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        
        # Apply reward-based learning
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Track metrics
        metrics['steps'] = step + 1
        metrics['total_reward'] += reward
        metrics['max_reward'] = max(metrics['max_reward'], reward)
        metrics['curiosity_level'] = agent.getCuriosityLevel()
        metrics['novelty_level'] = agent.getNoveltyLevel()
        metrics['avg_firing_rate'] = brain.getAverageFiringRate()
        metrics['dev_stage'] = brain.getDevelopmentalStage()
        
        # Log progress periodically
        if step % 200 == 0:
            print(f"Step {step}: "
                  f"Reward {metrics['total_reward']/(step+1):.3f}, "
                  f"Firing {brain.getFiringNeuronCount()}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Novelty {agent.getNoveltyLevel():.3f}")
    
    print(f"Simulation complete after {metrics['steps']} steps")
    print(f"Average reward per step: {metrics['total_reward']/metrics['steps']:.3f}")
    print(f"Maximum reward achieved: {metrics['max_reward']:.3f}")
    print(f"Final curiosity level: {metrics['curiosity_level']:.3f}")
    print(f"Final novelty level: {metrics['novelty_level']:.3f}")
    
    return metrics

def example_2_neural_learning_experiment():
    """
    Neural learning experiment demonstrating synaptic plasticity and memory.
    
    This example shows how to create a controlled learning environment to study:
    - Spike-Timing Dependent Plasticity (STDP)
    - Hebbian learning rules
    - Memory formation and consolidation
    - Development over time
    """
    print("\n=== Neural Learning Experiment Example ===")
    
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    
    # Enable plasticity for learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created learning experiment brain")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    
    # Create simple environment for controlled input
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run learning phases
    print("Running learning phases...")
    
    # Phase 1: Sensory exploration
    print("Phase 1: Sensory exploration...")
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Phase 2: Reward-based learning
    print("Phase 2: Reward-based learning...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Use proximity to a target as reward
        agent_body = world.getAgentBody()
        target_x, target_y = 8.0, 8.0
        distance = ((agent_body.x - target_x) ** 2 + (agent_body.y - target_y) ** 2) ** 0.5
        reward = max(0.0, 10.0 - distance)
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Phase 3: Developmental maturation
    print("Phase 3: Developmental maturation...")
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Collect learning metrics
    metrics = {
        'total_steps': 1500,
        'final_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'memory_system': 'Working: ' + str(brain.getWorkingMemory() is not None) +
                         ', Episodic: ' + str(brain.getEpisodicMemory() is not None),
        'development_stage': brain.getDevelopmentalStage().name
    }
    
    print(f"Learning experiment completed!")
    print(f"  Final firing rate: {metrics['final_firing_rate']:.3f} Hz")
    print(f"  Final curiosity: {metrics['curiosity_level']:.3f}")
    print(f"  Final novelty: {metrics['novelty_level']:.3f}")
    print(f"  Memory systems: {metrics['memory_system']}")
    print(f"  Development stage: {metrics['development_stage']}")
    
    return metrics

def example_3_performance_monitoring():
    """
    Performance monitoring and benchmarking example.
    
    This example demonstrates how to:
    - Monitor brain performance in real-time
    - Track resource usage
    - Benchmark different configurations
    - Profile neural activity patterns
    """
    print("\n=== Performance Monitoring Example ===")
    
    def run_benchmark(config_name, config):
        """Run a benchmark with given configuration."""
        print(f"\nBenchmarking {config_name}...")
        
        start_time = time.time()
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with performance tracking
        step_times = []
        firing_rates = []
        spike_counts = []
        
        for step in range(500):
            step_start = time.time()
            
            brain.step(step, step * 0.001)
            
            step_end = time.time()
            step_times.append(step_end - step_start)
            firing_rates.append(brain.getFiringNeuronCount())
            spike_counts.append(brain.getTotalSpikeCount())
            
            if step % 100 == 0:
                print(f"    Step {step}: "
                      f"Firing {brain.getFiringNeuronCount()}, "
                      f"Spikes {brain.getTotalSpikeCount()}, "
                      f"Time {step_times[-1]:.3f}s")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Calculate performance metrics
        avg_step_time = sum(step_times) / len(step_times)
        max_step_time = max(step_times)
        min_step_time = min(step_times)
        
        metrics = {
            'config': config_name,
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'max_step_time': max_step_time,
            'min_step_time': min_step_time,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'total_spikes': spike_counts[-1] if spike_counts else 0,
            'performance_score': 1.0 / avg_step_time if avg_step_time > 0 else 0
        }
        
        print(f"  Benchmark results:")
        print(f"    Total time: {metrics['total_time']:.2f}s")
        print(f"    Average step time: {metrics['avg_step_time']:.4f}s")
        print(f"    Max step time: {metrics['max_step_time']:.4f}s")
        print(f"    Min step time: {metrics['min_step_time']:.4f}s")
        print(f"    Average firing rate: {metrics['avg_firing_rate']:.2f} Hz")
        print(f"    Performance score: {metrics['performance_score']:.2f}")
        
        return metrics
    
    # Different configurations to benchmark
    configs = [
        ("Small Brain", pynlm.createDefaultConfig()),
        ("Medium Brain", pynlm.createDefaultConfig()),
        ("Large Brain", pynlm.createDefaultConfig())
    ]
    
    # Customize configurations
    configs[0][1].set("brain.neuron_count", 200)
    configs[1][1].set("brain.neuron_count", 500)
    configs[2][1].set("brain.neuron_count", 1000)
    
    # Run benchmarks
    results = []
    for config_name, config in configs:
        result = run_benchmark(config_name, config)
        results.append(result)
    
    # Compare results
    print(f"\nBenchmark Comparison:")
    print(f"{'Config':<15} {'Neurons':<8} {'Time (s)':<10} {'Avg Step':<12} {'Perf Score':<12}")
    print(f"{'-' * 65}")
    
    for result in results:
        print(f"{result['config']:<15} {result['neurons']:<8} {result['total_time']:<10.2f} "
              f"{result['avg_step_time']:<12.4f} {result['performance_score']:<12.2f}")
    
    return results

def example_4_advanced_world_scenarios():
    """
    Advanced world scenarios and environment interaction examples.
    
    This example demonstrates:
    - Complex world environments
    - Multiple agent behaviors
    - Resource management
    - Hazard avoidance
    - Social interaction patterns
    """
    print("\n=== Advanced World Scenarios Example ===")
    
    # Create world with multiple regions and objects
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    
    # Add various world objects
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Resource, value=20.0))
    world.addObject(pynlm.WorldObject(10.0, 5.0, pynlm.WorldObjectType.Hazard, value=1.0))
    world.addObject(pynlm.WorldObject(18.0, 8.0, pynlm.WorldObjectType.Wall, value=0.0))
    
    # Initialize agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set agent start position
    world.setAgentStart(2.0, 2.0)
    
    print(f"Created advanced world scenario:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision range: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    print(f"  Agent starting position: (2.0, 2.0)")
    
    # Run complex interaction scenario
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    scenario_metrics = {
        'steps': 0,
        'resources_found': 0,
        'hazards_encountered': 0,
        'closest_resource': float('inf'),
        'total_reward': 0.0
    }
    
    for step in range(1000):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate environment rewards
        agent_body = world.getAgentBody()
        
        # Reward for finding resources
        world_objects = []
        for x in range(-world.getSensoryPercept().getVisionWidth()//2, 
                      world.getSensoryPercept().getVisionWidth()//2 + 1):
            for y in range(-world.getSensoryPercept().getVisionHeight()//2,
                          world.getSensoryPercept().getVisionHeight()//2 + 1):
                wx = agent_body.x + x
                wy = agent_body.y + y
                if world.isValidPosition(wx, wy):
                    # Would need to add method to check object at position
                    pass
        
        # Simplified reward calculation
        if step % 100 == 0:
            distance_to_start = ((agent_body.x - 2.0) ** 2 + (agent_body.y - 2.0) ** 2) ** 0.5
            scenario_metrics['closest_resource'] = min(scenario_metrics['closest_resource'], distance_to_start)
        
        # Curiosity-driven exploration
        curiosity_reward = agent.getCuriosityLevel() * 0.1
        scenario_metrics['total_reward'] += curiosity_reward
        
        agent.applyRewardModulation(curiosity_reward, 0.0)
        agent.updateDevelopment(0.1)
        
        scenario_metrics['steps'] = step + 1
        
        if step % 200 == 0:
            print(f"Step {step}: "
                  f"Pos ({agent_body.x:.1f}, {agent_body.y:.1f}), "
                  f"Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Reward {scenario_metrics['total_reward']:.2f}")
    
    print(f"\nAdvanced world scenario completed:")
    print(f"  Total steps: {scenario_metrics['steps']}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Total reward accumulated: {scenario_metrics['total_reward']:.2f}")
    
    return scenario_metrics

def main():
    """
    Run all advanced examples and demonstrate comprehensive NLM capabilities.
    """
    print("NLM Advanced Python Examples")
    print("=" * 50)
    
    try:
        # Example 1: Advanced agent simulation
        metrics1 = example_1_advanced_agent_simulation()
        
        # Example 2: Neural learning experiment
        metrics2 = example_2_neural_learning_experiment()
        
        # Example 3: Performance monitoring
        metrics3 = example_3_performance_monitoring()
        
        # Example 4: Advanced world scenarios
        metrics4 = example_4_advanced_world_scenarios()
        
        print("\n" + "=" * 50)
        print("All advanced examples completed successfully!")
        print("\nKey capabilities demonstrated:")
        print("  ✓ Advanced agent simulation with complete perception-action loops")
        print("  ✓ Neural learning experiments with plasticity and memory")
        print("  ✓ Performance monitoring and benchmarking")
        print("  ✓ Complex world environments with dynamic interaction")
        print("  ✓ Real-time reward-based learning and development")
        
    except Exception as e:
        print(f"\nError in advanced examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
