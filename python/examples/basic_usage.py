#!/usr/bin/env python3
"""
Basic Usage Examples for NLM Python Advanced API

This module provides basic usage examples for getting started with the
NLM Python Advanced API.
"""

import pynlm
from advanced_nlm import AdvancedBrainUtils, AdvancedWorldUtils
def example_1_basic_brain():
    """
    Example 1: Basic Brain Creation and Simulation
    """
    print("=" * 60)
    print("Example 1: Basic Brain Creation and Simulation")
    print("=" * 60)
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Connection probability: {brain.getConfig().getConnectionProbability()}")
    
    # Run simple simulation
    print("\nRunning simulation for 500 steps...")
    for step in range(500):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return brain
def example_2_world_simulation():
    """
    Example 2: World Simulation with Agent
    """
    print("\n" + "=" * 60)
    print("Example 2: World Simulation with Agent")
    print("=" * 60)
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created world: {world.getWidth()}x{world.getHeight()} cells")
    print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
    print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run simulation
    print("\nRunning simulation for 200 steps...")
    for step in range(200):
        world.update(0.1)  # Update world physics
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, Health: {body.health:.1f}")
    
    print(f"\nFinal world state:")
    body = world.getAgentBody()
    print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  Agent orientation: {body.orientation:.1f} degrees")
    print(f"  Agent energy: {body.energy:.1f}")
    print(f"  Agent health: {body.health:.1f}")
    print(f"  Agent age: {body.age}")
    
    return brain, world, agent
def example_3_advanced_configuration():
    """
    Example 3: Advanced Configuration and Presets
    """
    print("\n" + "=" * 60)
    print("Example 3: Advanced Configuration and Presets")
    print("=" * 60)
    
    from advanced_nlm import AdvancedBrainUtils
    
    # Create brains with different presets
    presets = ["minimal", "balanced", "learning", "fast", "high_performance"]
    
    brains = {}
    for preset in presets:
        print(f"\nCreating brain with '{preset}' preset...")
        brain = AdvancedBrainUtils.create_brain_with_preset_config(preset)
        brains[preset] = brain
        
        config = brain.getConfig()
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Connection probability: {config.getConnectionProbability()}")
        print(f"  STDP LTP weight: {config.getOr('stdp_ltp_weight', 0.0)}")
        print(f"  STDP LTD weight: {config.getOr('stdp_ltd_weight', 0.0)}")
        
        # Quick performance test
        brain.reset()
        brain.initialize()
        for step in range(100):
            brain.step(step)
        
        print(f"  Test spikes: {brain.getTotalSpikeCount()}")
    
    # Compare performance
    print("\n" + "-" * 60)
    print("Performance Comparison:")
    print("-" * 60)
    
    for preset, brain in brains.items():
        config = brain.getConfig()
        print(f"{preset:15} | Neurons: {brain.getTotalNeuronCount():4} | "
              f"Conn: {config.getConnectionProbability():.2f} | "
              f"STDP: {config.getOr('stdp_ltp_weight', 0.0):.3f}")
    
    return brains
def example_4_world_complex_environment():
    """
    Example 4: Complex World Environment
    """
    print("\n" + "=" * 60)
    print("Example 4: Complex World Environment")
    print("=" * 60)
    
    from advanced_nlm import AdvancedWorldUtils
    
    # Create rich world
    world = AdvancedWorldUtils.create_rich_world(width=30, height=30)
    
    print(f"Created complex world: {world.getWidth()}x{world.getHeight()} cells")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Show agent start
    body = world.getAgentBody()
    print(f"Agent start position: ({body.x}, {body.y})")
    
    # Add some manual objects for visualization
    print("\nAdding sample objects...")
    
    # Add resources
    world.createObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=2.0, radius=0.5))
    world.createObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Resource, value=1.5, radius=0.5))
    
    # Add hazards
    world.createObject(pynlm.WorldObject(25, 25, pynlm.WorldObjectType.Hazard, value=0.8, radius=0.5))
    
    # Add markers
    world.createObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Marker, value=1.0, radius=0.3))
    world.createObject(pynlm.WorldObject(20, 10, pynlm.WorldObjectType.Marker, value=1.0, radius=0.3))
    
    print("  - Added 2 resources")
    print("  - Added 1 hazard")
    print("  - Added 2 markers")
    
    # Run short simulation
    print("\nRunning simulation for 50 steps...")
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"  Step {step:2d}: Position ({body.x:4.1f}, {body.y:4.1f}), "
                  f"Action: {agent.decodeMotorCommand().getType()}")
    
    return brain, world, agent
def example_5_brain_analysis():
    """
    Example 5: Brain Analysis and Benchmarking
    """
    print("\n" + "=" * 60)
    print("Example 5: Brain Analysis and Benchmarking")
    print("=" * 60)
    
    import time
    from advanced_nlm import AdvancedBrainUtils
    
    # Create and test different brain configurations
    configs = [
        {"neurons": 500, "conn": 0.05, "name": "Small"},
        {"neurons": 2000, "conn": 0.1, "name": "Medium"},
        {"neurons": 5000, "conn": 0.15, "name": "Large"},
    ]
    
    results = {}
    
    for config in configs:
        print(f"\nTesting {config['name']} brain ({config['neurons']} neurons)...")
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.getConfig().setNeuronCount(config['neurons'])
        brain.getConfig().setConnectionProbability(config['conn'])
        
        brain.initialize()
        
        # Benchmark
        start_time = time.time()
        for step in range(500):
            brain.step(step)
        end_time = time.time()
        
        execution_time = end_time - start_time
        steps_per_second = 500 / execution_time
        
        results[config['name']] = {
            "brain": brain,
            "steps_per_second": steps_per_second,
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "execution_time": execution_time,
        }
        
        print(f"  Steps/second: {steps_per_second:.0f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Execution time: {execution_time:.2f}s")
    
    # Compare results
    print("\n" + "-" * 60)
    print("Performance Summary:")
    print("-" * 60)
    
    print(f"{'Size':>8} | {'Steps/s':>8} | {'Total Spikes':>12} | {'Firing Rate':>11} | {'Time (s)':>9}")
    print("-" * 60)
    
    for name, result in results.items():
        print(f"{name:>8} | {result['steps_per_second']:>8.0f} | {result['total_spikes']:>12} | "
              f"{result['avg_firing_rate']:>11.2f} | {result['execution_time']:>9.2f}")
    
    # Find best performer
    best_name = max(results.keys(), key=lambda k: results[k]['steps_per_second'])
    print(f"\nBest performer: {best_name} brain")
    
    return results
def example_6_serialization_demo():
    """
    Example 6: Brain Serialization
    """
    print("\n" + "=" * 60)
    print("Example 6: Brain Serialization")
    print("=" * 60)
    
    from advanced_nlm import AdvancedSerializationUtils
    import os
    
    # Create and run brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Creating brain and running simulation...")
    for step in range(1000):
        brain.step(step)
    
    # Get brain statistics
    print(f"Brain statistics:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Export to JSON
    filename = "brain_state.json"
    print(f"\nExporting brain to {filename}...")
    
    if AdvancedSerializationUtils.export_brain_to_json(brain, filename):
        print("✓ Brain exported successfully")
        
        # Check file size
        file_size = os.path.getsize(filename)
        print(f"  File size: {file_size / 1024:.1f} KB")
        
        # Import from JSON
        print(f"\nImporting brain from {filename}...")
        brain_data = AdvancedSerializationUtils.import_brain_from_json(filename)
        
        if brain_data:
            print("✓ Brain imported successfully")
            print(f"  Metadata: {brain_data.get('metadata', {})}")
        else:
            print("✗ Failed to import brain")
    else:
        print("✗ Failed to export brain")
    
    return brain
def example_7_agent_training():
    """
    Example 7: Agent Training
    """
    print("\n" + "=" * 60)
    print("Example 7: Agent Training")
    print("=" * 60)
    
    from advanced_nlm import AdvancedTrainingUtils
    
    # Create world and brain
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    config = pynlm.createDefaultConfig()
    config.setNeuronCount(1500)
    config.setConnectionProbability(0.12)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set start position
    world.setAgentStart(7.5, 7.5)
    
    print(f"Training agent in {world.getWidth()}x{world.getHeight()} world")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()} pixels")
    
    # Training parameters
    num_episodes = 10
    steps_per_episode = 200
    
    print(f"\nTraining parameters:")
    print(f"  Episodes: {num_episodes}")
    print(f"  Steps per episode: {steps_per_episode}")
    
    # Train agent
    print(f"\nStarting training...")
    stats = AdvancedTrainingUtils.train_agent_world(
        world, brain, num_episodes, steps_per_episode
    )
    
    # Show results
    print(f"\nTraining completed!")
    print(f"  Episodes completed: {stats['episodes_completed']}")
    print(f"  Best reward: {stats['best_reward']:.2f}")
    print(f"  Final reward: {stats['episode_rewards'][-1]:.2f}")
    print(f"  Average reward: {stats['avg_reward']:.2f}")
    print(f"  Converged: {'Yes' if stats['converged'] else 'No'}")
    
    # Get learning curve
    learning_curve = AdvancedTrainingUtils.create_learning_curve_data(stats)
    print(f"\nLearning curve analysis:")
    print(f"  Total episodes: {len(learning_curve['episodes'])}")
    print(f"  Best reward: {learning_curve['best_reward']:.2f}")
    print(f"  Final reward: {learning_curve['final_reward']:.2f}")
    print(f"  Reward improvement: {learning_curve['final_reward'] - learning_curve['rewards'][0]:.2f}")
    
    return brain, world, agent, stats
def example_8_complete_workflow():
    """
    Example 8: Complete Workflow
    """
    print("\n" + "=" * 60)
    print("Example 8: Complete Workflow")
    print("=" * 60)
    
    # Create custom brain using AdvancedBrainUtils
    from advanced_nlm import AdvancedBrainUtils
    
    print("1. Creating custom brain with 'learning' preset...")
    brain = AdvancedBrainUtils.create_brain_with_preset_config("learning")
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    print("2. Creating world with obstacles and resources...")
    world = AdvancedWorldUtils.create_rich_world(width=25, height=25)
    print(f"   World size: {world.getWidth()}x{world.getHeight()} cells")
    
    # Create agent
    print("3. Creating agent and initializing with world...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    print("   Enabled: Reward modulation, Curiosity, Development")
    
    # Run complete episode
    print("4. Running complete episode...")
    steps = 500
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            body = world.getAgentBody()
            print(f"   Step {step:3d}: Position ({body.x:5.1f}, {body.y:5.1f}), "
                  f"Energy: {body.energy:4.1f}")
    
    # Analyze results
    print("5. Analyzing results...")
    
    # Brain analysis
    brain_analysis = AdvancedBrainUtils.analyze_brain_connectivity(brain)
    print(f"   Brain analysis:")
    print(f"     Connection density: {brain_analysis['connection_density']:.4f}")
    print(f"     Firing rate: {brain_analysis['average_firing_rate']:.2f}")
    print(f"     E/I ratio: {brain_analysis['excitation_inhibition_ratio']:.2f}")
    
    # World analysis
    from advanced_nlm import AdvancedWorldUtils
    world_analysis = AdvancedWorldUtils.analyze_world_state(world)
    print(f"   World analysis:")
    print(f"     Agent position: ({world_analysis['agent_position']['x']:.1f}, "
          f"{world_analysis['agent_position']['y']:.1f})")
    print(f"     Agent energy: {world_analysis['agent_energy']:.1f}")
    print(f"     Agent health: {world_analysis['agent_health']:.1f}")
    
    # Agent stats
    agent_stats = agent.getAgentStats()
    print(f"   Agent stats:")
    print(f"     Neuromodulation level: {agent_stats['neuromodulationLevel']:.3f}")
    print(f"     Curiosity level: {agent_stats['curiosityLevel']:.3f}")
    print(f"     Novelty level: {agent_stats['noveltyLevel']:.3f}")
    
    print("\n" + "=" * 60)
    print("Complete workflow finished successfully!")
    print("=" * 60)
    
    return brain, world, agent
def run_all_examples():
    """Run all examples in sequence."""
    examples = [
        ("Basic Brain", example_1_basic_brain),
        ("World Simulation", example_2_world_simulation),
        ("Advanced Configuration", example_3_advanced_configuration),
        ("Complex World", example_4_world_complex_environment),
        ("Brain Analysis", example_5_brain_analysis),
        ("Serialization", example_6_serialization_demo),
        ("Agent Training", example_7_agent_training),
        ("Complete Workflow", example_8_complete_workflow),
    ]
    
    results = {}
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*20} {name} {'='*20}")
            result = example_func()
            results[name] = result
            print(f"✓ {name} completed successfully")
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = {"error": str(e)}
    
    return results
if __name__ == "__main__":
    print("NLM Python Advanced API Examples")
    print("===============================")
    
    # Run all examples
    all_results = run_all_examples()
    
    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    successful = sum(1 for r in all_results.values() if "error" not in r)
    total = len(all_results)
    
    print(f"Examples run: {successful}/{total} successful")
    
    if successful == total:
        print("\n🎉 All examples completed successfully!")
        print("\nThe NLM Python Advanced API is working correctly!")
    else:
        print(f"\n⚠️  {total - successful} example(s) had errors.")
        print("\nCheck error messages above for details.")
    
    print("\nFor more information, see:")
    print("  - python/advanced_nlm.py (advanced utilities)")
    print("  - python/README.md (documentation)")
    print("  - examples/ (additional examples)")

# Export example functions
__all__ = [
    "example_1_basic_brain",
    "example_2_world_simulation",
    "example_3_advanced_configuration",
    "example_4_world_complex_environment",
    "example_5_brain_analysis",
    "example_6_serialization_demo",
    "example_7_agent_training",
    "example_8_complete_workflow",
    "run_all_examples",
]