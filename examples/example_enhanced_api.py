#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.
This file shows basic usage of the enhanced bindings.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build'))

# Try to import the enhanced bindings
try:
    import pynlm_enhanced as nlm
    print("✓ Enhanced NLM Python bindings loaded successfully")
except ImportError as e:
    print(f"✗ Failed to load enhanced bindings: {e}")
    print("This is expected if the bindings are not built yet.")
    sys.exit(1)

def example_basic_usage():
    """Example 1: Basic enhanced usage"""
    print("\n" + "="*60)
    print("EXAMPLE 1: Basic Enhanced Usage")
    print("="*60)
    
    # Create advanced configuration
    print("Creating advanced configuration...")
    config = nlm.createDefaultConfig()
    
    # Modify configuration
    print("Modifying configuration...")
    config.setInt("brain.neuron_count", 1500)
    config.setFloat("brain.synapse_density", 0.12)
    config.setFloat("plasticity.stdp.learning_rate", 0.0015)
    
    # Create brain
    print("Creating brain...")
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Run simulation
    print("Running simulation...")
    step_metrics = brain.runSimulation(200)
    print(f"✓ Simulation completed: {len(step_metrics)} steps recorded")
    
    # Get statistics
    print("\nGetting statistics...")
    stats = brain.getDetailedStats()
    for stat in stats:
        print(f"  {stat}")
    
    return brain

def example_advanced_config():
    """Example 2: Advanced configuration"""
    print("\n" + "="*60)
    print("EXAMPLE 2: Advanced Configuration")
    print("="*60)
    
    # Create configuration from JSON
    print("Creating configuration from JSON string...")
    json_config = """
    {
        "brain": {
            "neuron_count": 2000,
            "synapse_density": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0
        },
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001
            },
            "hebbian": {
                "enable": true
            }
        },
        "simulation_timestep": 0.001,
        "random_seed": 42
    }
    """
    
    config = nlm.createDefaultConfig()
    config.loadFromJson(json_config)
    
    # Validate
    print("Validating configuration...")
    if config.validate():
        print("✓ Configuration is valid")
    else:
        print("✗ Configuration has issues")
    
    # Get configuration info
    print("\nConfiguration schema:")
    schema = config.getSchema()
    for key, description in schema:
        print(f"  {key}: {description}")
    
    # Convert to dictionary
    print("\nConfiguration as dictionary:")
    config_dict = config.toDict()
    for key, value in config_dict.items():
        print(f"  {key}: {value}")
    
    return config

def example_visualization():
    """Example 3: Visualization tools"""
    print("\n" + "="*60)
    print("EXAMPLE 3: Visualization Tools")
    print("="*60)
    
    # Create visualization helper
    print("Creating visualization helper...")
    viz = nlm.VisualizationHelper()
    
    # Create sample activity data
    print("Generating sample activity data...")
    activity_data = [
        0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0
    ]
    
    # Create heatmap
    print("Creating heatmap visualization...")
    heatmap = viz.createHeatmap(activity_data, 10, 2)
    print("Heatmap:")
    print(heatmap)
    
    # Create spike raster
    print("\nCreating spike raster visualization...")
    spike_times = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90]
    neuron_ids = [0, 1, 0, 1, 0, 1, 0, 1, 0, 1]
    raster = viz.createRaster(spike_times, neuron_ids)
    print("Spike Raster:")
    print(raster)
    
    return viz

def example_world_simulation():
    """Example 4: World simulation"""
    print("\n" + "="*60)
    print("EXAMPLE 4: World Simulation")
    print("="*60)
    
    # Create enhanced world
    print("Creating enhanced world...")
    world = nlm.EnhancedWorld()
    world.configure(15, 15, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Set agent start position
    print("Setting agent start position...")
    world.setAgentStart(7.5, 7.5)
    
    # Create visual field
    print("Creating visual field...")
    visual_field = world.createVisualField(
        world.getAgentBody().x,
        world.getAgentBody().y,
        world.getAgentBody().orientation
    )
    print(f"Visual field size: {len(visual_field)} pixels")
    
    # Spawn objects
    print("Spawning objects...")
    objects = world.spawnObjects(5, nlm.WorldObjectType.Resource)
    print(f"Spawned {len(objects)} objects")
    
    # Render world state
    print("\nWorld state:")
    render = world.render()
    print(render)
    
    # Run world simulation
    print("\nRunning world simulation...")
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        print(f"Step {step}: Visual field sum = {sum(percept.getVision()):.2f}")
    
    return world

def example_advanced_agent():
    """Example 5: Advanced agent learning"""
    print("\n" + "="*60)
    print("EXAMPLE 5: Advanced Agent Learning")
    print("="*60)
    
    # Create brain and advanced agent
    print("Creating brain and advanced agent...")
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    agent = nlm.AdvancedAgentBrain(brain)
    
    # Configure learning
    print("Configuring advanced learning...")
    agent.enableAdvancedLearning(True)
    agent.setLearningRate(0.01)
    agent.setExplorationRate(0.2)
    agent.setMemorySize(2000)
    
    # Create and configure world
    print("Creating and configuring world...")
    world = nlm.EnhancedWorld()
    world.configure(20, 20, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(10, 10)
    
    # Initialize agent
    print("Initializing agent...")
    agent.initialize(world)
    
    # Run learning algorithms
    print("\nRunning Policy Iteration...")
    policy_results = agent.runPolicyIteration(world, 50)
    print(policy_results)
    
    print("\nRunning Q-Learning...")
    ql_results = agent.runQLearning(world, 50)
    print(ql_results)
    
    print("\nRunning Actor-Critic...")
    ac_results = agent.runActorCritic(world, 50)
    print(ac_results)
    
    # Get policy statistics
    print("\nGetting policy statistics...")
    stats = agent.getPolicyStats()
    print(stats)
    
    return brain, agent, world

def example_performance_monitoring():
    """Example 6: Performance monitoring"""
    print("\n" + "="*60)
    print("EXAMPLE 6: Performance Monitoring")
    print("="*60)
    
    # Create brain
    print("Creating brain for performance test...")
    config = nlm.createDefaultConfig()
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Profile simulation
    print("Profiling simulation performance...")
    metrics = nlm.profileSimulation(brain, 100)
    
    print(f"Simulation completed in {metrics[0]} ms")
    print(f"Throughput: {metrics[1]:.0f} steps per second")
    print(f"Total spikes: {metrics[2]}")
    print(f"Active neurons: {metrics[3]}")
    
    # Get current performance metrics
    print("\nGetting current performance metrics...")
    current_metrics = nlm.PerformanceMonitor.getCurrentMetrics()
    print(f"CPU Usage: {current_metrics.cpuUsage:.1%}")
    print(f"Memory Usage: {current_metrics.memoryUsage:.1%}")
    print(f"Simulation Time Per Step: {current_metrics.simulationTimePerStep:.6f}s")
    print(f"Neurons Processed: {current_metrics.neuronsProcessed}")
    print(f"Throughput: {current_metrics.throughput:.0f} steps/sec")
    
    return metrics

def example_error_handling():
    """Example 7: Error handling"""
    print("\n" + "="*60)
    print("EXAMPLE 7: Error Handling")
    print("="*60)
    
    # Demonstrate error handling
    print("Testing configuration validation...")
    config = nlm.createDefaultConfig()
    
    # Test with invalid JSON
    print("Testing invalid JSON handling...")
    try:
        config.loadFromJson("invalid json { syntax")
        print("✗ Should have thrown an error")
    except nlm.NLMError as e:
        print(f"✓ Caught expected error: {e}")
    except Exception as e:
        print(f"✓ Caught error: {type(e).__name__}: {e}")
    
    # Test invalid key access
    print("\nTesting invalid key access...")
    try:
        value = config.getFloat("nonexistent.key")
        print(f"✗ Should have thrown an error (got: {value})")
    except nlm.NLMError as e:
        print(f"✓ Caught expected error: {e}")
    except Exception as e:
        print(f"✓ Caught error: {type(e).__name__}: {e}")
    
    # Test safe access
    print("\nTesting safe access...")
    safe_value = config.getWithDefault("nonexistent.key", 42.0)
    print(f"✓ Safe access returned default value: {safe_value}")
    
    # Test type conversion
    print("\nTesting type conversion...")
    try:
        neuron_type = nlm.parseNeuronType("Excitatory")
        print(f"✓ Parsed neuron type: {neuron_type}")
        
        synapse_type = nlm.parseSynapseType("Inhibitory")
        print(f"✓ Parsed synapse type: {synapse_type}")
        
        stage = nlm.parseDevelopmentalStage("Adult")
        print(f"✓ Parsed developmental stage: {stage}")
        
        action = nlm.parseActionType("MoveForward")
        print(f"✓ Parsed action type: {action}")
        
        obj_type = nlm.parseWorldObjectType("Resource")
        print(f"✓ Parsed world object type: {obj_type}")
        
    except nlm.NLMError as e:
        print(f"✗ Error in type conversion: {e}")
    
    return config

def example_full_workflow():
    """Example 8: Complete workflow"""
    print("\n" + "="*60)
    print("EXAMPLE 8: Complete Workflow")
    print("="*60)
    
    print("Setting up complete simulation workflow...")
    
    # Step 1: Create advanced configuration
    print("Step 1: Creating configuration...")
    config = nlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1000)
    config.setFloat("plasticity.stdp.learning_rate", 0.001)
    
    # Step 2: Create brain
    print("Step 2: Creating brain...")
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Step 3: Create agent
    print("Step 3: Creating agent...")
    agent = nlm.AdvancedAgentBrain(brain)
    agent.enableAdvancedLearning(True)
    
    # Step 4: Create world
    print("Step 4: Creating world...")
    world = nlm.EnhancedWorld()
    world.configure(10, 10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5, 5)
    
    # Step 5: Initialize agent
    print("Step 5: Initializing agent...")
    agent.initialize(world)
    
    # Step 6: Run simulation
    print("Step 6: Running simulation...")
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Step 7: Get results
    print("Step 7: Analyzing results...")
    episode = brain.simulateEpisode(agent, world, 50)
    
    print(f"  Total steps: {episode.steps}")
    print(f"  Total reward: {episode.totalReward:.2f}")
    print(f"  Average novelty: {sum(episode.noveltyLevels)/len(episode.noveltyLevels):.3f}")
    print(f"  Average curiosity: {sum(episode.curiosityLevels)/len(episode.curiosityLevels):.3f}")
    
    # Step 8: Visualize results
    print("Step 8: Visualizing results...")
    viz = nlm.VisualizationHelper()
    
    # Create spike raster from episode data
    spike_times = []
    neuron_ids = []
    for step in range(50):
        if brain.getFiringNeuronCount() > 0:
            spike_times.append(step)
            neuron_ids.append(brain.getFiringNeuronCount() % 100)
    
    if spike_times:
        raster = viz.createRaster(spike_times, neuron_ids)
        print("Sample Spike Raster:")
        print(raster)
    
    print("\n✓ Complete workflow executed successfully!")
    return brain, agent, world, episode

def main():
    """Run all examples"""
    print("NLM Enhanced Python API Examples")
    print("="*60)
    
    try:
        # Run all examples
        example_basic_usage()
        example_advanced_config()
        example_visualization()
        example_world_simulation()
        example_advanced_agent()
        example_performance_monitoring()
        example_error_handling()
        example_full_workflow()
        
        print("\n" + "="*60)
        print("ALL EXAMPLES COMPLETED SUCCESSFULLY!")
        print("="*60)
        
        print("\nNote: This script requires the enhanced Python bindings to be built.")
        print("To build the bindings, run:")
        print("  mkdir build && cd build")
        print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("  cmake --build . --parallel")
        print("  pip install -e .")
        
    except Exception as e:
        print(f"\n✗ Example failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())