#!/usr/bin/env python3
"""
Example demonstrating the enhanced Python bindings for NLM.

This script showcases all the new Pythonic APIs and convenience methods
that have been added to the NLM Python bindings.
"""

import pynlm
import time

def example_basic_usage():
    """Example 1: Basic NLM usage (unchanged from documentation)."""
    print("=== Example 1: Basic Usage ===")
    
    # Create and initialize a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"Simulation complete. Total spikes: {brain.getTotalNeuronCount()}")
    print()

def example_config_pythonic():
    """Example 2: Pythonic configuration operations."""
    print("=== Example 2: Pythonic Configuration ===")
    
    config = pynlm.createDefaultConfig()
    
    # Use getOr for safe access
    learning_rate = config.getOr("learning.rate", 0.001)
    print(f"Learning rate: {learning_rate}")
    
    # Use setIfMissing to only set if not present
    config.setIfMissing("brain.plasticity.stdp.enable", True)
    config.setIfMissing("brain.plasticity.hebbian.enable", True)
    print("Set plasticity options (only if not already set)")
    
    # Use update for multiple settings
    updates = {
        "brain.neuron_count": 500,
        "brain.synapse_density": 0.15,
        "neuromod.dopamine.scale": 1.2
    }
    config.update(updates)
    print("Applied multiple configuration updates")
    
    # Get a section
    brain_section = config.getSection("brain")
    print(f"Brain configuration section retrieved")
    
    # Save configuration
    with open("example_config.json", "w") as f:
        config.saveToFile("example_config.json")
    print("Configuration saved to example_config.json")
    
    # Load configuration
    loaded_config = pynlm.loadConfig("example_config.json")
    print(f"Loaded configuration with {len(loaded_config.getKeys())} settings")
    
    print()

def example_brain_pythonic():
    """Example 3: Pythonic brain operations."""
    print("=== Example 3: Pythonic Brain Operations ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use stepMany for efficient batch processing
    print("Running 100 simulation steps efficiently...")
    brain.stepMany(0, 100)
    print(f"Total spikes after 100 steps: {brain.getTotalSpikeCount()}")
    
    # Get comprehensive neuron statistics
    neuron_stats = brain.getNeuronStats()
    print(f"Neuron Statistics:")
    for key, value in neuron_stats.items():
        print(f"  {key}: {value}")
    
    # Use runEpisode for clean episode management
    print("\nRunning episode with 50 steps...")
    brain.runEpisode(50)
    
    # Create regions using alias
    region1 = brain.createRegion("cortex")
    region2 = brain.createRegion("hippocampus")
    print(f"Created regions: {region1}, {region2}")
    
    # Clone brain for parallel experiments
    print("\nCreating brain copy for comparison...")
    cloned_brain = brain.clone()
    print(f"Original brain neurons: {brain.getTotalNeuronCount()}")
    print(f"Cloned brain neurons: {cloned_brain.getTotalNeuronCount()}")
    
    # Compare two brains
    comparison = brain.compare(cloned_brain)
    print(f"Brain comparison: {comparison}")
    
    print()

def example_world_pythonic():
    """Example 4: Pythonic world operations."""
    print("=== Example 4: Pythonic World Operations ===")
    
    world = pynlm.createSimpleWorld()
    world.configure(25, 25, 10, 10)
    
    # Use runEpisode with initial objects
    initial_objects = {
        "treasure1": pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 50.0),
        "treasure2": pynlm.WorldObject(20, 20, pynlm.WorldObjectType.Resource, 75.0),
        "hazard1": pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, 25.0),
    }
    
    print("Starting world episode with resources and hazards...")
    world.runEpisode(initial_objects)
    
    # Use convenience methods for common objects
    resource = world.addResource(8, 8, 30.0)
    print(f"Added resource at (8,8) with value {resource.value}")
    
    hazard = world.addHazard(18, 18, 15.0)
    print(f"Added hazard at (18,18) with value {hazard.value}")
    
    wall = world.addWall(12, 12, 1.5)
    print(f"Added wall at (12,12) with radius {wall.radius}")
    
    # Get comprehensive world statistics
    world_stats = world.getWorldStats()
    print(f"\nWorld Statistics:")
    for key, value in world_stats.items():
        print(f"  {key}: {value}")
    
    # Get agent occupancy
    occupancy = world.getOccupancy()
    print(f"\nAgent Position: ({occupancy['agent_x']}, {occupancy['agent_y']})")
    print(f"Agent Orientation: {occupancy['agent_orientation']}")
    
    print()

def example_action_pythonic():
    """Example 5: Pythonic action operations."""
    print("=== Example 5: Pythonic Action Operations ===")
    
    # Create actions using different methods
    move_action = pynlm.Action.create(pynlm.ActionType.MoveForward, [0.8, 0.2])
    print(f"Created move action: type={move_action.getType()}, params={move_action.getParameters()}")
    
    # Use fromDict for action creation
    action_dict = {
        "type": pynlm.ActionType.TurnLeft,
        "parameters": [0.3]
    }
    turn_action = pynlm.Action()
    turn_action.fromDict(action_dict)
    print(f"Created turn action from dict: {turn_action.getName()}")
    
    # Convert back to dictionary
    action_info = turn_action.toDict()
    print(f"Action as dictionary: {action_info}")
    
    # Check if action is valid
    print(f"Is valid action: {turn_action.isValid()}")
    
    # Create action variations
    eating_action = pynlm.Action.create(pynlm.ActionType.Eat, [1.0])
    print(f"Eating action: {eating_action.getName()}")
    
    drinking_action = pynlm.Action.create(pynlm.ActionType.Drink, [0.7])
    print(f"Drinking action: {drinking_action.getName()}")
    
    # Clone with new parameters
    eating_with_force = eating_action.withParameters([1.5])
    print(f"Eating action with force: params={eating_with_force.getParameters()}")
    
    print()

def example_agent_pythonic():
    """Example 6: Pythonic agent operations."""
    print("=== Example 6: Pythonic Agent Operations ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run a complete episode
    print("Running agent episode with 30 steps...")
    agent.runEpisode(world, 30)
    
    # Get comprehensive agent statistics
    agent_stats = agent.getAgentStats()
    print(f"\nAgent Statistics:")
    for key, value in agent_stats.items():
        print(f"  {key}: {value}")
    
    # Check if agent is healthy
    healthy = agent.isHealthy()
    print(f"\nAgent health status: {'Healthy' if healthy else 'Unhealthy'}")
    
    # Process multiple percepts
    percepts = []
    for i in range(5):
        world.update(0.1)
        percepts.append(world.getSensoryPercept())
    
    print(f"\nProcessed {len(percepts)} percepts in sequence")
    
    # Reset for new episode
    agent.reset()
    print("Agent reset for new episode")
    
    print()

def example_serialization():
    """Example 7: Serialization and batch operations."""
    print("=== Example 7: Serialization and Batch Operations ===")
    
    # Configure and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    brain.runEpisode(100)
    
    # Save brain state
    with open("example_brain.bin", "wb") as f:
        pass  # File will be created by saveBrain
    
    pynlm.saveBrain(brain, "example_brain.bin")
    print(f"Saved brain with {brain.getTotalNeuronCount()} neurons to example_brain.bin")
    
    # Load brain state
    loaded_brain = pynlm.loadBrain("example_brain.bin")
    print(f"Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")
    
    # Create multiple brains for batch processing
    brains = [pynlm.createBrain(config) for _ in range(5)]
    for b in brains:
        b.initialize()
    
    # Run batch simulation
    print("\nRunning batch simulation on 5 brains...")
    batch_results = pynlm.runBatchSimulation(brains, 20)
    for i, brain_results in enumerate(batch_results):
        print(f"Brain {i+1}: {len(brain_results)} steps completed, final spikes: {brain_results[-1]}")
    
    # Benchmark brains
    print("\nBenchmarking brain performance...")
    benchmark = pynlm.benchmarkBrains(brains, 50)
    print(f"Average time per brain: {benchmark['average_time_ms']:.2f} ms")
    print(f"Speed: {benchmark['steps_per_second']:.1f} steps/second")
    
    print()

def example_multiagent_system():
    """Example 8: Multi-agent system."""
    print("=== Example 8: Multi-Agent System ===")
    
    # Create multiple agents with different configurations
    configs = [
        pynlm.createDefaultConfig(),
        pynlm.createDefaultConfig(),
        pynlm.createDefaultConfig(),
    ]
    
    # Make second agent more aggressive
    configs[1].update({"brain.neuron_count": 2000, "neuromod.curiosity.enable": True})
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(40, 40, 12, 12)
    
    # Create multi-agent system
    multi_agent_result = pynlm.createMultiAgentSystem(configs, world)
    
    brains = multi_agent_result["brains"]
    agents = multi_agent_result["agents"]
    
    print(f"Created multi-agent system with {len(brains)} brains and {len(agents)} agents")
    
    # Initialize all agents
    for i, agent in enumerate(agents):
        agent.initialize(world)
        print(f"Agent {i+1} initialized")
    
    # Run multi-agent episode
    print("\nRunning multi-agent episode...")
    success = pynlm.runMultiAgentEpisode(agents, [world, pynlm.createSimpleWorld(), pynlm.createSimpleWorld()], 15)
    print(f"Multi-agent episode {'completed successfully' if success else 'failed'}")
    
    print()

def example_visualization_and_debugging():
    """Example 9: Visualization and debugging tools."""
    print("=== Example 9: Visualization and Debugging ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get visualizations
    print("Brain Visualization:")
    print(pynlm.visualizeBrain(brain))
    print()
    
    print("World Visualization:")
    print(pynlm.visualizeWorld(world))
    print()
    
    print("Agent Visualization:")
    print(pynlm.visualizeAgent(agent))
    print()
    
    # Run a step with debugging
    print("Debugging single step...")
    debug_info = pynlm.debugStep(brain, 0, 0.0)
    print(debug_info)
    
    # Debug agent step
    print("\nDebugging agent step...")
    agent_debug = pynlm.debugAgentStep(agent, world, 0)
    print(agent_debug)
    
    # Profile simulation
    print("\nProfiling simulation...")
    profile = pynlm.profileSimulation(brain, 100)
    print(f"Profile Results:")
    for key, value in profile.items():
        print(f"  {key}: {value}")
    
    print()

def example_enums_and_constants():
    """Example 10: Enums and constants."""
    print("=== Example 10: Enums and Constants ===")
    
    # Demonstrate enum usage
    print("Neuron Types:")
    print(f"  Excitatory: {pynlm.NeuronType.Excitatory}")
    print(f"  Inhibitory: {pynlm.NeuronType.Inhibitory}")
    print(f"  Sensory: {pynlm.NeuronType.Sensory}")
    print(f"  Motor: {pynlm.NeuronType.Motor}")
    print(f"  Internal: {pynlm.NeuronType.Internal}")
    
    print("\nAction Types:")
    print(f"  MoveForward: {pynlm.ActionType.MoveForward}")
    print(f"  TurnLeft: {pynlm.ActionType.TurnLeft}")
    print(f"  Eat: {pynlm.ActionType.Eat}")
    print(f"  Drink: {pynlm.ActionType.Drink}")
    
    print("\nWorld Object Types:")
    print(f"  Resource: {pynlm.WorldObjectType.Resource}")
    print(f"  Hazard: {pynlm.WorldObjectType.Hazard}")
    print(f"  Wall: {pynlm.WorldObjectType.Wall}")
    
    print("\nConstants:")
    print(f"  Invalid Neuron ID: {pynlm.INVALID_NEURON_ID}")
    print(f"  Invalid Synapse ID: {pynlm.INVALID_SYNAPSE_ID}")
    
    print()

def example_complete_simulation():
    """Example 11: Complete simulation using all new APIs."""
    print("=== Example 11: Complete Simulation with All APIs ===")
    
    # Setup configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and configure
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.setAgentStart(15, 15)
    
    # Create agent and initialize
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some initial objects
    world.addResource(10, 10, 100.0)
    world.addResource(20, 20, 200.0)
    world.addHazard(5, 5, 50.0)
    world.addWall(25, 25, 2.0)
    
    # Run simulation with visualization
    print("Starting complete simulation...")
    viz_brain = pynlm.visualizeBrain(brain)
    print("Initial Brain State:")
    print(viz_brain[:200] + "..." if len(viz_brain) > 200 else viz_brain)
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Add resource periodically
        if step % 10 == 0:
            world.addResource(step * 2, step * 2, 10.0)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if world.getAgentBody().energy > 10.0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 10 steps
        if step % 10 == 0:
            print(f"Step {step}: Energy={world.getAgentBody().energy:.1f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    # Show final state
    print("\nFinal State Visualization:")
    print(pynlm.visualizeWorld(world))
    print(pynlm.visualizeAgent(agent))
    
    # Get comprehensive results
    final_stats = agent.getAgentStats()
    final_neuron_stats = brain.getNeuronStats()
    
    print(f"\nSimulation Summary:")
    print(f"Total Steps: 50")
    print(f"Final Reward: {final_stats['neuromodulation_level']:.2f}")
    print(f"Final Curiosity: {final_stats['curiosity_level']:.2f}")
    print(f"Final Novelty: {final_stats['novelty_level']:.2f}")
    print(f"Total Spikes: {final_neuron_stats['spike_count']}")
    print(f"Average Firing Rate: {final_neuron_stats['average_firing_rate']:.2f} Hz")
    
    # Profile the episode
    profile = pynlm.profileAgentEpisode(agent, world, 50)
    print(f"\nPerformance:")
    print(f"Time per step: {profile['time_per_step_ms']:.1f} ms")
    print(f"Speed: {profile['steps_per_second']:.1f} steps/second")
    
    print()

def main():
    """Run all examples."""
    print("=" * 60)
    print("NLM Python Bindings - Enhanced Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Usage", example_basic_usage),
        ("Pythonic Configuration", example_config_pythonic),
        ("Pythonic Brain Operations", example_brain_pythonic),
        ("Pythonic World Operations", example_world_pythonic),
        ("Pythonic Action Operations", example_action_pythonic),
        ("Pythonic Agent Operations", example_agent_pythonic),
        ("Serialization and Batch Operations", example_serialization),
        ("Multi-Agent System", example_multiagent_system),
        ("Visualization and Debugging", example_visualization_and_debugging),
        ("Enums and Constants", example_enums_and_constants),
        ("Complete Simulation", example_complete_simulation),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    main()