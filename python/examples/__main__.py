#!/usr/bin/env python3
"""
NLM Example 1: Basic Brain Creation and Simulation

This example demonstrates the fundamental usage of NLM Python bindings
for creating a brain and running basic simulation steps.
"""

import pynlm
import time

def basic_brain_example():
    """Create and run a basic brain simulation"""
    print("=== Basic Brain Example ===")
    
    # Step 1: Create a brain with default configuration
    print("1. Creating brain with default config...")
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    
    # Step 2: Initialize the brain
    print("2. Initializing brain...")
    brain.initialize()
    
    # Step 3: Print initial brain statistics
    print("3. Initial brain statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Step 4: Run simulation for 100 steps
    print("4. Running 100 simulation steps...")
    start_time = time.time()
    brain.step(0, 0.0)
    
    for i in range(1, 100):
        brain.step(i, i * 0.001)
    
    end_time = time.time()
    
    # Step 5: Print final statistics
    print("5. Final brain statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   - Total spike count: {brain.getTotalSpikeCount()}")
    print(f"   - Simulation time: {end_time - start_time:.3f} seconds")
    
    # Step 6: Save brain state
    print("6. Saving brain state...")
    brain.save("basic_brain_example.brain")
    print("   - Saved to 'basic_brain_example.brain'")
    
    print("=== Basic Brain Example Complete ===\n")
    return brain

def enhanced_brain_example():
    """Demonstrate enhanced brain features"""
    print("=== Enhanced Brain Features Example ===")
    
    # Create brain with enhanced configuration
    print("1. Creating brain with experimental configuration...")
    config = pynlm.create_experimental_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use context manager for simulation session
    print("2. Running simulation with context manager...")
    with pynlm.simulation_session(brain, num_steps=500, timestep=0.001) as session:
        session.run()
    
    # Use batch operations
    print("3. Running batch simulation...")
    brain.runSteps(1000)
    
    # Use advanced features
    print("4. Demonstrating advanced features...")
    print(f"   - Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   - Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Get network statistics
    stats = brain.getNetworkStats()
    print(f"   - Network neurons: {stats['total_neurons']}")
    print(f"   - Network synapses: {stats['total_synapses']}")
    
    print("=== Enhanced Brain Features Example Complete ===\n")
    return brain

def world_simulation_example():
    """Demonstrate world simulation with agent"""
    print("=== World Simulation Example ===")
    
    # Create world and brain
    print("1. Creating world and brain...")
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 10, 10)
    
    config = pynlm.create_agent_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Add some objects to the world
    print("2. Adding environment objects...")
    world.addResource(5.0, 5.0, 20.0)
    world.addHazard(15.0, 15.0, 10.0)
    world.addWall(10.0, 10.0)
    
    # Run agent simulation
    print("3. Running agent simulation...")
    print(f"   - Initial agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run several steps
    for step in range(50):
        world.update(0.001)
        
        if step % 10 == 0:
            percept = world.getSensoryPercept()
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"   - Step {step}: Action={action.getType()}, "
                  f"Success={result.success}, Reward={result.reward:.1f}")
    
    print("4. Final agent statistics:")
    agent_stats = agent.getAgentStats()
    print(f"   - Sensory input size: {agent_stats['sensory_input_size']}")
    print(f"   - Motor output size: {agent_stats['motor_output_size']}")
    print(f"   - Developmental stage: {agent_stats['developmental_stage']}")
    print(f"   - Neuromodulation level: {agent_stats['neuromodulation_level']:.2f}")
    print(f"   - Curiosity level: {agent_stats['curiosity_level']:.2f}")
    
    print("=== World Simulation Example Complete ===\n")
    return world, agent

def event_system_example():
    """Demonstrate event system functionality"""
    print("=== Event System Example ===")
    
    # Create brain and world
    world = pynlm.createSimpleWorld()
    world.configure(10.0, 10.0, 5, 5)
    
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create event manager
    event_manager = pynlm.EventManager()
    
    print("1. Setting up event handlers...")
    
    # Define event handlers
    def handle_spike_event(event):
        print(f"   - Spike event detected: Neuron {event.neuron_id.value} at time {event.timestamp}")
    
    def handle_learning_event(event):
        print(f"   - Learning event: Weight change {event.weight_change:.4f}")
    
    def handle_simulation_event(event):
        print(f"   - Simulation event: {event.event_type}")
    
    # Register handlers (these would be properly implemented in real usage)
    print("2. Event system ready (handlers registered)")
    
    # Run simulation
    print("3. Running simulation with event monitoring...")
    for step in range(20):
        brain.step(step, step * 0.001)
        
        # Check for spike events
        spike_count = brain.getTotalSpikeCount()
        if spike_count > step:  # New spikes detected
            print(f"   - Step {step}: Spike events occurred")
    
    # Get event statistics
    print("4. Event system statistics:")
    # These would be properly implemented in real usage
    print("   - Spike events: [would be tracked]")
    print("   - Learning events: [would be tracked]")
    print("   - Simulation events: [would be tracked]")
    
    print("=== Event System Example Complete ===\n")
    return event_manager

def visualization_example():
    """Demonstrate visualization capabilities"""
    print("=== Visualization Example ===")
    
    # Create brain
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create visualization interface (placeholder)
    print("1. Creating visualization interface...")
    # Note: Visualization would be implemented with actual visualization library
    print("   - Visualization interface created (placeholder)")
    
    # Run some simulation
    print("2. Running simulation for visualization...")
    for step in range(50):
        brain.step(step, step * 0.001)
    
    # Would visualize network in real implementation
    print("3. Visualization would show:")
    print("   - Network structure")
    print("   - Neural activity patterns")
    print("   - Spike raster plot")
    print("   - Weight matrix")
    
    print("=== Visualization Example Complete ===\n")
    return brain

def batch_operations_example():
    """Demonstrate batch operations"""
    print("=== Batch Operations Example ===")
    
    # Create multiple brains
    print("1. Creating multiple brains...")
    brains = []
    for i in range(3):
        config = pynlm.create_default_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    # Use batch simulation
    print("2. Running batch simulation...")
    with pynlm.batch_simulation(brains, stepsPerBrain=100) as simulator:
        results = simulator.run()
    
    print("3. Batch simulation results:")
    for i, spikes in enumerate(results):
        print(f"   - Brain {i}: {spikes} total spikes")
    
    # Use parallel simulation
    print("4. Running parallel simulation...")
    # Would use ParallelSimulator in real implementation
    print("   - Parallel simulation would run here")
    
    print("=== Batch Operations Example Complete ===\n")
    return brains

def main():
    """Run all examples"""
    print("NLM Python Bindings Examples\n")
    print("These examples demonstrate the enhanced features of pynlm.\n")
    
    examples = [
        ("Basic Brain Example", basic_brain_example),
        ("Enhanced Brain Features", enhanced_brain_example),
        ("World Simulation", world_simulation_example),
        ("Event System", event_system_example),
        ("Visualization", visualization_example),
        ("Batch Operations", batch_operations_example),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}\n")
            import traceback
            traceback.print_exc()
    
    print("All examples completed!")

if __name__ == "__main__":
    main()